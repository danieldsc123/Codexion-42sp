#!/usr/bin/env python3
"""Compile temporary C probes; no test code is linked into codexion."""
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FLAGS = ['cc', '-Wall', '-Wextra', '-Werror', '-pthread', '-Iinclude', '-Isrc',
         '-fsanitize=undefined', '-fno-sanitize-recover=all']

HEAP = r'''
#include "codexion.h"
#include <assert.h>
#include <limits.h>

int main(void)
{
    t_heap heap;
    t_request requests[257];
    t_request previous;
    t_request current;
    t_sim sim;
    t_config config = {0};
    int policy;
    int i;
    int j;

    assert(cx_heap_init(&heap, 0, CX_FIFO) == -1);
    cx_heap_destroy(&heap);
    assert(cx_heap_init(&heap, SIZE_MAX, CX_FIFO) == -1);
    assert(cx_heap_init(&heap, 2, (t_policy)42) == -1);
    for (policy = CX_FIFO; policy <= CX_EDF; ++policy)
    {
        assert(cx_heap_init(&heap, 257, policy) == 0);
        for (i = 0; i < 257; ++i)
        {
            requests[i].coder_id = i + 1;
            requests[i].sequence = (i * 73) % 257;
            requests[i].deadline = (i * 19) % 11;
            assert(cx_heap_push(&heap, requests[i]) == 0);
        }
        assert(cx_heap_push(&heap, requests[0]) == -1);
        for (i = 1; i <= 257; i += 3)
            assert(cx_heap_remove(&heap, i) == 0);
        assert(cx_heap_remove(&heap, 999) == -1);
        i = 0;
        while (heap.size)
        {
            current = *cx_heap_peek(&heap);
            assert(cx_heap_pop(&heap, &requests[0]) == 0);
            assert(current.coder_id == requests[0].coder_id);
            assert((current.coder_id - 1) % 3 != 0);
            if (i++)
                assert(!cx_request_before(&current, &previous, policy));
            previous = current;
        }
        assert(cx_heap_peek(&heap) == NULL);
        assert(cx_heap_pop(&heap, &current) == -1);
        cx_heap_destroy(&heap);
        cx_heap_destroy(&heap);
    }
    previous = (t_request){1, ULLONG_MAX, LLONG_MAX};
    current = (t_request){2, 0, 0};
    assert(cx_request_before(&current, &previous, CX_FIFO));
    assert(cx_request_before(&current, &previous, CX_EDF));
    previous = (t_request){1, 5, 100};
    current = (t_request){2, 5, 100};
    assert(cx_request_before(&previous, &current, CX_EDF));
    assert(!cx_request_before(&previous, &previous, CX_EDF));
    for (policy = CX_FIFO; policy <= CX_EDF; ++policy)
    {
        config.number_of_coders = 2;
        config.scheduler = policy;
        config.time_to_burnout = 100;
        assert(cx_sim_init(&sim, &config) == 0);
        pthread_mutex_lock(&sim.state_mutex);
        sim.coders[0].last_compile_start = 100;
        sim.coders[1].last_compile_start = 0;
        assert(cx_schedule_submit_locked(&sim.coders[0]) == 0);
        assert(cx_schedule_submit_locked(&sim.coders[1]) == 0);
        j = (policy == CX_EDF);
        assert(cx_schedule_ready_locked(&sim.coders[j], 0));
        assert(!cx_schedule_ready_locked(&sim.coders[1 - j], 0));
        pthread_mutex_lock(&sim.dongles[0].mutex);
        sim.dongles[0].available_at = 10;
        pthread_mutex_unlock(&sim.dongles[0].mutex);
        assert(!cx_schedule_ready_locked(&sim.coders[j], 9));
        assert(cx_schedule_ready_locked(&sim.coders[j], 10));
        cx_schedule_cancel_locked(&sim.coders[j]);
        assert(cx_schedule_ready_locked(&sim.coders[1 - j], 10));
        cx_schedule_cancel_locked(&sim.coders[1 - j]);
        cx_schedule_cancel_locked(&sim.coders[1 - j]);
        pthread_mutex_unlock(&sim.state_mutex);
        cx_sim_destroy(&sim);
        cx_sim_destroy(&sim);
    }
    return 0;
}
'''

FAULTS = r'''
#include "codexion.h"
#include <assert.h>
#include <errno.h>
#include <unistd.h>
int cx_test_main(int argc, char **argv);
static int allocations, mutexes, conditions, threads;
static int malloc_calls, mutex_calls, cond_calls, create_calls;
static int fail_malloc, fail_mutex, fail_cond, fail_create, fail_write;
void *__real_malloc(size_t n);
void __real_free(void *p);
int __real_pthread_mutex_init(pthread_mutex_t *m, const pthread_mutexattr_t *a);
int __real_pthread_mutex_destroy(pthread_mutex_t *m);
int __real_pthread_cond_init(pthread_cond_t *c, const pthread_condattr_t *a);
int __real_pthread_cond_destroy(pthread_cond_t *c);
int __real_pthread_create(pthread_t *t, const pthread_attr_t *a, void *(*f)(void *), void *arg);
int __real_pthread_join(pthread_t t, void **result);
ssize_t __real_write(int fd, const void *buf, size_t n);
void *__wrap_malloc(size_t n) {
    void *p;
    if (++malloc_calls == fail_malloc) return NULL;
    p = __real_malloc(n); if (p) ++allocations; return p;
}
void __wrap_free(void *p) {
    if (p) { assert(allocations > 0); --allocations; } __real_free(p);
}
int __wrap_pthread_mutex_init(pthread_mutex_t *m, const pthread_mutexattr_t *a) {
    int r; if (++mutex_calls == fail_mutex) return EAGAIN;
    r = __real_pthread_mutex_init(m, a); if (!r) ++mutexes; return r;
}
int __wrap_pthread_mutex_destroy(pthread_mutex_t *m) {
    int r; assert(mutexes > 0); r = __real_pthread_mutex_destroy(m);
    assert(!r); --mutexes; return r;
}
int __wrap_pthread_cond_init(pthread_cond_t *c, const pthread_condattr_t *a) {
    int r; if (++cond_calls == fail_cond) return EAGAIN;
    r = __real_pthread_cond_init(c, a); if (!r) ++conditions; return r;
}
int __wrap_pthread_cond_destroy(pthread_cond_t *c) {
    int r; assert(conditions > 0); r = __real_pthread_cond_destroy(c);
    assert(!r); --conditions; return r;
}
int __wrap_pthread_create(pthread_t *t, const pthread_attr_t *a, void *(*f)(void *), void *arg) {
    int r; if (++create_calls == fail_create) return EAGAIN;
    r = __real_pthread_create(t, a, f, arg); if (!r) ++threads; return r;
}
int __wrap_pthread_join(pthread_t t, void **result) {
    int r = __real_pthread_join(t, result); assert(!r); --threads; return r;
}
ssize_t __wrap_write(int fd, const void *buf, size_t n) {
    if (fail_write) { errno = EIO; return -1; }
    return __real_write(fd, buf, n);
}
static void run_case(int expected) {
    char *args[] = {"codexion", "3", "2000", "2", "1", "1", "2", "1", "edf", NULL};
    malloc_calls = mutex_calls = cond_calls = create_calls = 0;
    assert(cx_test_main(9, args) == expected);
    assert(!allocations && !mutexes && !conditions && !threads);
}
int main(void) {
    int i;
    run_case(0);
    for (i = 1; i <= 5; ++i) { fail_malloc = i; run_case(1); }
    fail_malloc = 0;
    for (i = 1; i <= 5; ++i) { fail_mutex = i; run_case(1); }
    fail_mutex = 0;
    fail_cond = 1; run_case(1); fail_cond = 0;
    for (i = 1; i <= 4; ++i) { fail_create = i; run_case(1); }
    fail_create = 0;
    fail_write = 1; run_case(1); fail_write = 0;
    return 0;
}
'''


class InternalTests(unittest.TestCase):
    def compile_probe(self, source, wrappers=(), rename_main=False):
        sources = sorted(str(p.relative_to(ROOT)) for p in (ROOT / 'src').rglob('*.c')
                         if p.name != 'main.c')
        with tempfile.TemporaryDirectory(prefix='codexion-test-') as directory:
            tmp = Path(directory)
            probe = tmp / 'probe.c'
            probe.write_text(source)
            if rename_main:
                subprocess.run(FLAGS + ['-Dmain=cx_test_main', '-c', 'src/core/main.c',
                                       '-o', str(tmp / 'main.o')], cwd=ROOT, check=True)
                sources.append(str(tmp / 'main.o'))
            command = FLAGS + [str(probe), *sources]
            command += [f'-Wl,--wrap={name}' for name in wrappers]
            subprocess.run(command + ['-o', str(tmp / 'probe')], cwd=ROOT, check=True)
            result = subprocess.run([str(tmp / 'probe')], cwd=ROOT, capture_output=True,
                                    text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stderr)

    def test_heap_and_arbitration(self):
        self.compile_probe(HEAP)

    def test_partial_failures(self):
        self.compile_probe(FAULTS, ('malloc', 'free', 'pthread_mutex_init',
                                  'pthread_mutex_destroy', 'pthread_cond_init',
                                  'pthread_cond_destroy', 'pthread_create',
                                  'pthread_join', 'write'), rename_main=True)


if __name__ == '__main__':
    unittest.main(verbosity=2)
