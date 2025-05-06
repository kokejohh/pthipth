# pthipth's Demo

#### demo_create
creates 5 threads using ```pthipth_create```. the threads are created with default priority and no arguments.
Each thread runs child_function, which prints its thread ID and priority.
The main thread waits for all threads to finish using ```pthipth_join``` before exiting.

#### demo_yield
creates 10 threads using pthipth_create. Each thread voluntarily yields the CPU once by calling ```pthipth_yield```,
allowing other threads to run. After creation, the main thread waits for all threads to finish using pthipth_join.

#### demo_yield_qtime
creates 10 threads. Each thread simulates doing work for ~10 ms using nanosleep(),
then calls ```pthipth_yield_qtime```, which checks if the thread has run longer than the specified time (11 ms) and yields if it has.

#### demo_sleep
creates 10 threads. Each thread sleeps for 1000 ms (1 second) using ```pthipth_sleep```,
a non-blocking sleep function provided by the pthipth library. This simulates pausing the thread without blocking the entire system.
After sleeping, the thread resumes and prints a message. Using sleep() or nanosleep() would block the thread, as only one thread is running at a time.

#### demo_scanf
Creates 10 threads. Each thread reads an integer using ```pthipth_scanf```, a non-blocking alternative to scanf, then prints the value.
Using blocking I/O like scanf would halt the entire system, since only one thread is running at a time.

#### demo_mutex
creates 10 threads. Each thread simulates mutex usage by locking a mutex ```pthipth_mutex_lock```, modifying a shared variable num,
and yielding while still holding the lock to test mutual exclusion. After yielding, the thread unlocks the mutex ```pthipth_mutex_unlock```.

#### demo_barrier
creates 10 threads. Each thread waits at a barrier that releases in groups of 5 ```pthipth_barrier_wait```.
One thread in each group prints a message to wake the others. This simulates thread synchronization using a barrier.

#### demo_pool
creates a thread pool with a maximum of 10 concurrent ```pthipth_pool_create``` threads and submits 20 tasks ```pthipth_pool_add``` to it.
Each task prints its thread ID and priority. The pool is destroyed ```pthipth_pool_destroy``` after all tasks complete.

#### demo_quota_time
creates 10 threads that increment a shared variable num 10,000 times each without using a mutex, which may lead to race conditions.
Each thread is limited to a 1 ms time quota; if it exceeds this, it yields to allow other threads to run.
The final value of num is printed, but it may be incorrect due to concurrent access. 
Because of this time slicing and concurrent access, a mutex is necessary to ensure correct results.

#### demo_compare_create
this program benchmarks the performance of three threading methods: ```pthread```, ```pthipth```, and ```pthipth_pool(thread pool)```.
It tests with different thread counts, measuring the time taken to create and execute threads.

#### demo_working
this program shows how pthread and pthipth differ in thread scheduling. pthread relies on the kernel’s scheduler,
so the execution order is non-deterministic. In contrast, pthipth uses a user-level bucket priority queue, providing a more predictable.

#### demo_chat_server
this program shows an example of a chat server created using pthipth

#### demo_chat_client
connect to the chat server and chat with others using demo_chat_client. You have to run demo_chat_server first.

<p align="center">
  <img src="https://media.tenor.com/1UoL-HJFGDAAAAAi/pentol-stiker-pentol.gif" alt="้help me please" />
</p>
