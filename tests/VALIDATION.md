# Validation notes

Validation was performed on Linux using `cc`, pthreads, Norminette and
Valgrind 3.18.1. This records observed checks, not a guarantee about all machines.

- Full build with the subject flags and Norminette on `include/` and `src/`.
- Black-box tests: valid FIFO/EDF scenarios, exact log syntax and order, phases,
  cooldown, shared-resource lower bounds, completion, invalid input, N=1,
  zero durations and zero target.
- Heap probe: capacity rejection, overflow, invalid policy, 257 requests,
  removals followed by ordered extraction, empty operations, deterministic
  ties and comparisons at integer limits. Direct scheduler checks distinguish
  FIFO from EDF and check cooldown and cancellation.
- Fault injection: every allocation and mutex initialization for three coders,
  condition initialization, monitor/worker creation and output failure. The
  probe checks that all tracked allocations, primitives and threads are cleaned
  up. Repeated destruction is checked as well.
- Native stress runs: 200 coders with a generous deadline, 31 coders under
  repeated contention, the five-coder example with 10 compilations, and a
  two-coder case. All completed without burnout in those runs.
- Memcheck on the completed EDF simulation, single-coder burnout and burnout
  during compilation: zero bytes left allocated and zero reported errors. DRD on the threaded simulation: zero reported errors.
- Helgrind 3.18.1 emitted condition-signal mutex warnings from inside libc's
  `pthread_cond_timedwait` implementation. This run is not reported as passing.
  Application broadcasts occur with the associated state mutex held; DRD is
  included as a separate check, not as a suppression of the Helgrind report.
- Burnout timing: one initial native test observed 11 ms of reporting delay.
  Subsequent 60 repetitions (one coder, burnout during compilation, zero
  burnout) observed delays of 0–1 ms. The automated test keeps the strict
  10 ms assertion, and the complete native suite subsequently passed. Repeat
  timing tests on the evaluation hardware without instrumentation or a blocked
  stdout consumer. POSIX scheduling cannot provide an unconditional wall-time
  guarantee on a heavily loaded system.

The heap used for scheduling is a custom binary min-heap. Each dongle's queue
has capacity two because only its two neighbors can request it; the generic
heap itself is also tested at larger capacities.
