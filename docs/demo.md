# pthipth's Demo

#### demo_create
creates 5 threads using `pthipth_create`. The threads are created with default priority and no arguments.
Each thread runs child_function, which prints its thread ID and priority.
The main thread waits for all threads to finish using `pthipth_join` before exiting.

#### demo_yield
creates 5 threads using pthipth_create. Each thread voluntarily yields the CPU once by calling `pthipth_yield`,
allowing other threads to run. After creation, the main thread waits for all threads to finish using pthipth_join.

#### demo_yield_qtime
creates 10 threads. Each thread simulates doing work for ~10 ms using nanosleep(),
then calls `pthipth_yield_qtime`, which checks if the thread has run longer than the specified time (11 ms) and yields if it has.

#### demo_detach
creates 5 threads. detaches itself with `pthipth_detach`, meaning it won't be joined later.
 
#### demo_sleep
creates 5 threads. Each thread sleeps for 1000 ms (1 second) using `pthipth_sleep`, a non-blocking sleep function provided by the pthipth library. This simulates pausing the thread without blocking the entire system. After sleeping, the thread resumes and prints a message. Using sleep() or nanosleep() would block the thread, as only one thread is running at a time.

#### demo_mutex
creates 5 threads. Each thread simulates mutex usage by locking a mutex `pthipth_mutex_lock`, modifying a shared variable num,
and yielding while still holding the lock to test mutual exclusion. After yielding, the thread unlocks the mutex `pthipth_mutex_unlock`.

#### demo_cond
creates 5 threads that wait until num >= 5 and 5 threads that increment num. When num reaches 5, a broadcast wakes the waiting threads.


#### demo_pool
creates a thread pool with a maximum of 10 concurrent `pthipth_pool_create` threads and submits 20 tasks `pthipth_pool_add` to it.
Each task prints its thread ID and priority. The pool is destroyed `pthipth_pool_destroy` after all tasks are complete.

#### demo_compare_create
this program benchmarks the performance of three threading methods: ```pthread```, ```pthipth```, and ```pthipth_pool(thread pool)```.
It tests with different thread counts, measuring the time taken to create and execute threads.

#### demo_working
this program shows how pthread and pthipth differ in thread scheduling. pthread relies on the kernel’s scheduler,
so the execution order is non-deterministic. In contrast, pthipth uses a user-level bucket priority queue, providing a more predictable.

<p align="center">
  <img src="https://media.tenor.com/1UoL-HJFGDAAAAAi/pentol-stiker-pentol.gif" alt="้help me please" />
</p>
