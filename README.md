*This project has been created as part of the 42 curriculum by danda-si.*

## Description

Codexion simulates coders sharing USB dongles around a circular workspace.
Each coder needs two adjacent dongles to compile, then debugs and refactors.
The project explores POSIX threads, resource synchronization, FIFO/EDF
scheduling with a custom heap, and timing constraints.

The argument parser, initial memory allocation, coder metadata setup, and
array cleanup are implemented, following subject version 1.5. The executable
validates its arguments, allocates the arrays, initializes coder IDs and
neighbor indices, then frees the arrays. It does not create threads yet.
Sources are grouped under `src/`. Each module keeps its headers in a
`headers/` subdirectory. Shared helpers live in `src/utils/`, and
`include/codexion.h` includes the module headers.

## Instructions

Run `make` to build with `cc` and `-Wall -Wextra -Werror -pthread`.
The Makefile also provides `clean`, `fclean` and `re`.

Run the current parser with:

```text
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

All eight arguments are mandatory. Times are in milliseconds. The scheduler
must be exactly `fifo` or `edf`. Libft is not authorized.

Successful initialization exits with status 0 and no output. Allocation
failure prints `Error: failed to initialize simulation` and exits with status 1. Invalid input prints
`Error: invalid arguments` to standard error and exits with status 1.
Numeric arguments accept decimal digits only, from 0 to `INT_MAX`; the coder
count must be positive. Zero is accepted for durations, cooldown and the
compile target. The numeric limit and zero handling are local parser choices.

Example:

```sh
./codexion 5 800 200 100 50 3 20 fifo
echo $?
```

## Resources

- Codexion subject, version 1.5, chapters III–VIII: project requirements.
- POSIX manual entries for `pthread_create`, `pthread_join`,
  `pthread_mutex_lock`, `pthread_cond_wait` and `pthread_cond_timedwait`:
  threading and synchronization references to study during implementation.
- System manual entries for `gettimeofday` and `clock_gettime`: time APIs.
- AI assistance: used to interpret the subject, organize the architecture,
  prepare the Makefile and headers, review and correct numeric conversion,
  implement argument-parser corrections, integrate initialization and cleanup
  in `main`, correct allocation handling, complete coder initialization,
  explain the code, and run validation checks. The student is responsible for understanding
  and reviewing these contributions.

## Blocking cases handled

No concurrency cases have been implemented or validated yet. The implementation
must address deadlocks and Coffman's conditions, starvation under feasible EDF
parameters, dongle cooldown, burnout detection within 10 ms, serialized logs,
single-coder behavior, and clean shutdown after partial initialization.
The final README must explain the actual solutions and their validation.

## Thread synchronization mechanisms

The planned design uses one thread per coder, a separate monitor thread,
one mutex protecting each dongle's state, and mutex protection for shared
simulation state and output. A shared condition variable is planned for
startup, timed waits, and shutdown notification. The two shared mutexes and the condition variable are now initialized.
Lifecycle flags record each successful initialization so cleanup also handles
partial failures and repeated calls before threads are started.

The initial contracts specify ownership of shared fields, lock order, waiting
predicates, and a proposed protocol for granting two adjacent dongles together.
Runtime correctness, EDF liveness, and timing requirements still need validation.
The final README must include concrete race-prevention and coder/monitor
communication examples drawn from the implemented code.

