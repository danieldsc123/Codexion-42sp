*This project has been created as part of the 42 curriculum by danda-si.*

## Description

Codexion is a C/POSIX-thread simulation based on subject version 1.5. N coders
share N USB dongles arranged in a circle. Each coder needs its two adjacent
dongles to compile, then releases them, debugs, refactors, and requests them
again. A separate monitor stops the simulation when a coder misses its burnout
deadline. Otherwise, execution ends when every coder has completed the required
number of compilations.

The implementation includes a custom binary min-heap per dongle, FIFO and EDF
arbitration, mandatory dongle cooldown, synchronized startup, serialized logs,
and cleanup for partial initialization and thread-creation failures. There are
no global variables or linked Libft library.

## Instructions

Build with `make`. The Makefile uses `cc -Wall -Wextra -Werror -pthread`, generates
header dependencies, and does not relink an unchanged executable. It provides
`all`, `codexion`, `clean`, `fclean`, and `re`.

All eight arguments are mandatory:

```text
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Meaning |
| --- | --- |
| `number_of_coders` | Number of coders and dongles; at least 1 |
| `time_to_burnout` | Maximum interval between compilation starts, in ms |
| `time_to_compile` | Time holding two dongles while compiling, in ms |
| `time_to_debug` | Debugging duration after releasing the dongles, in ms |
| `time_to_refactor` | Refactoring duration before requesting again, in ms |
| `number_of_compiles_required` | Completed compilations required from every coder |
| `dongle_cooldown` | Time each released dongle remains unavailable, in ms |
| `scheduler` | Exactly `fifo` or `edf` |

Example:

```sh
./codexion 5 800 200 100 50 3 20 edf
```

The first compilation deadline is relative to the common simulation start.
Every later deadline is relative to the beginning, not the end, of the last
compilation. A coder can therefore burn out while compiling. A coder that has
reached its personal target continues participating until the global stop;
its completed counter saturates at the target.

Input accepts decimal digits only, from 0 to `INT_MAX`. Signs, whitespace,
fractions, empty arguments and overflow are rejected. Zero coders is invalid.
The subject does not give an explicit zero-value table or numeric upper bound:
this implementation accepts zero durations/cooldown/target, and uses `INT_MAX`
as its input representation limit. A zero target completes immediately without
creating threads; zero burnout with a positive target expires at startup.

Normal completion and a simulated burnout both return status 0. Invalid input,
allocation failures, and operational errors return status 1 with a diagnostic
on standard error. Standard output contains only subject-format state logs.
Timestamps are milliseconds elapsed since the shared start. A burnout is the
last state message. There is no additional success message.

## Architecture

| Module | Responsibility |
| --- | --- |
| `src/core/` | Configuration ownership, initialization, startup barrier, joins, cleanup |
| `src/parser/` | Argument validation and checked numeric conversion |
| `src/utils/` | Project-local string comparison, conversion and memory filling |
| `src/scheduler/` | Binary heaps, request comparison and arbitration |
| `src/dongle/` | Resource ownership, pair acquisition and cooldown |
| `src/threads/` | Coder phases and independent burnout monitor |
| `src/time/` | Millisecond clock and interruptible timed waits |
| `src/log/` | Serialized, directly written state messages |

Each module has a `headers/` directory. `include/codexion.h` includes the module
interfaces. Helpers used in one translation unit are `static`; no shared state
is stored in global variables. All threads refer to a `t_sim` owned by `main`,
which remains alive until their joins.

## Blocking cases handled

### Deadlock and Coffman's conditions

The four conditions are mutual exclusion, hold-and-wait, no preemption, and
circular wait. Dongles remain mutually exclusive and are not taken away from a
compiling coder. For N > 1, admission grants both neighboring dongles together;
a waiting coder holds neither. This breaks hold-and-wait and prevents the
circular resource deadlock. The global state lock serializes the short
admission transaction, not the compilation phase: disjoint coders can compile
concurrently.

With N = 1, both neighbor indices identify the same dongle. The coder can log
one acquisition but never compile; it waits for burnout and releases the
single resource during shutdown. The same mutex is never locked twice.

### FIFO, EDF and starvation

Each dongle owns a custom binary min-heap containing copies of pending requests.
Only its two neighboring coders can request it, so capacity 2 is sufficient.
A coder has at most one request per adjacent dongle. The same request key is
inserted into both distinct queues while holding the state lock.

- FIFO compares a monotonically increasing request sequence, then coder ID.
- EDF compares `last_compile_start + time_to_burnout`, then sequence, then ID.
- Identical keys are not considered strictly earlier than themselves.

The EDF tie-break order is an implementation choice: the subject requires a
deterministic tie-breaker but does not specify its exact order. Arrival means
registration under the state lock. Initial requests are registered in alternating
index groups (0, 2, 4, ... then 1, 3, 5, ...), allowing disjoint pairs to start
without an avoidable chain of equal-priority requests. Under EDF, odd rings
also stagger initial grants: request rank `r` cannot start before
`start + floor(r * (compile + cooldown) / floor(N / 2))`. Requests remain
queued throughout this wait, and heap priority still applies. This distributes
initial starts instead of synchronizing batches that can create an avoidable
three-compilation gap. Only the initial requests are staggered; subsequent
requests are submitted immediately after refactoring.

A pair is eligible only when its coder is first in both queues, both resources
are free, and both cooldowns have expired. Older pending EDF deadlines remain
unchanged while waiting. A serviced coder removes its old request and refreshes
its deadline before submitting a new request, so repeatedly compiling coders
cannot keep overtaking an older pending deadline. All queues use the same total
priority order; contradictory priority cycles cannot arise.

A blocked head request may temporarily leave a free dongle idle while its other
resource is occupied or cooling down. The scheduler preserves per-dongle
priority rather than bypassing that request. EDF is not a proof that arbitrary
time parameters can meet every deadline: compilation, cooldown, phase lengths,
contention and OS scheduling must leave enough time. Feasible success scenarios
and actual missed-deadline scenarios are covered separately in tests.

### Cooldown and burnout

Release records `available_at = release_time + dongle_cooldown` under the
resource mutex. Waiting threads recheck eligibility after notifications and
short timed waits; cooldown expiry does not require another thread to signal.

The monitor checks the oldest deadline and waits until that deadline,
or wakes earlier after a state change to recompute it. Transitions also check for an already
expired deadline before renewing it, preventing a late acquisition from reviving
a coder. Detection uses `now >= deadline` and emits a single terminal log.
The timestamp is the observed time, not a fabricated deadline timestamp.

The target is the subject's maximum 10 ms reporting delay. This is measured in
native tests, not assumed from the waiting interval. The program uses the
subject-recommended `gettimeofday` wall clock and the default realtime clock
for timed condition waits. System clock changes, a blocked output consumer,
and severe OS scheduling delays can affect timing; these are not hard-realtime
APIs. See the measured results in `tests/VALIDATION.md`.

### Cleanup and failures

Lifecycle flags count successful mutex, condition, dongle and thread setup.
Failure paths destroy only initialized objects. A dongle initializer rolls back
its own mutex if heap allocation fails. Partial thread creation requests stop,
opens waiting threads' exit paths, and joins every successfully created thread.
Threads cancel outstanding requests and release any ownership before exiting.

Joins finish before queues, dongles, condition variables, mutexes and arrays are
destroyed. Cleared flags and null pointers make subsequent cleanup harmless.
The destructor must not be called while threads are still running. An unexpected
join failure is reported and does not destroy resources potentially still in use.

## Thread synchronization mechanisms

- `state_mutex` protects startup, stop reason, coder deadlines, completion
  counts, request sequences, and the admission/release transactions.
- Each dongle's `pthread_mutex_t` protects its owner, cooldown and heap. Runtime
  access first acquires state, then one dongle mutex at a time.
- `log_mutex` serializes writes. Logging occurs under state, then output lock,
  keeping the stop decision and final message ordered with normal messages.
- `changed` is a `pthread_cond_t` always waited on using `state_mutex`. It
  notifies startup, resource changes, deadline updates, completion and shutdown.
  Every wait is inside a predicate loop to handle spurious wakeups.

The lock order is state, then a dongle or output lock. No function acquires
state while holding a dongle/output lock. No thread sleeps or waits holding a
dongle/output lock. `pthread_cond_wait` and `pthread_cond_timedwait` release the
state lock while sleeping and reacquire it before returning.

For example, the monitor reads `last_compile_start` under the same state lock
used when acquisition writes it. A coder releases resource ownership under the
dongle lock, then broadcasts the shared condition while still holding state.
A waiter rechecks the heap, ownership and cooldown; a notification alone never
grants ownership. Coders do not inspect or directly signal other coder objects
to negotiate access: arbitration is handled through the shared resource system.

All workers and the monitor reach the startup gate before the coordinator sets
the common start time and releases them. Time spent creating threads does not
consume individual burnout budgets.

## Tests

```sh
make
norminette include src
python3 -m unittest discover -s tests -v
valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=99 \
  ./codexion 5 3000 10 5 5 3 3 edf
valgrind --tool=drd --error-exitcode=99 \
  ./codexion 5 10000 10 5 5 3 3 edf
```

Python tests cover argument rejection, phase order and timing, shared-resource
intervals, cooldown, completion, one coder, zero values, and the burnout log.
Temporary C probes check heap order/removal/ties, FIFO/EDF arbitration and
injected allocation, pthread-initialization, thread-creation and output failures.
The probes are compiled in a temporary directory and are not part of the
submitted executable. Memory/race instrumentation slows execution: use generous
burnout values for those runs and check the 10 ms timing separately without it.

## Resources

- Codexion subject, version 1.5, chapters III–VIII: mandatory requirements,
  allowed functions, logging and documentation.
- POSIX/system manuals: `man pthread_create`, `man pthread_join`,
  `man pthread_mutex_init`, `man pthread_cond_wait`,
  `man pthread_cond_timedwait`, `man gettimeofday`, and `man write`.
- *Introduction to Algorithms* (Cormen, Leiserson, Rivest, Stein): binary heaps
  and priority queues.
- *Operating System Concepts* (Silberschatz, Galvin, Gagne): synchronization,
  deadlock conditions and scheduling.
- Valgrind documentation: Memcheck and DRD usage and interpretation of reports.

AI was used to interpret the supplied subject, organize modules and headers,
review and correct the parser and initialization, implement the remaining
heap/scheduler, dongle, timing, logging, worker and monitor code, write tests,
and prepare documentation and commits. The student must understand, verify,
and be able to explain or modify these contributions during peer evaluation.
