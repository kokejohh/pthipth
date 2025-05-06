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
then calls pthipth_yield_qtime, which checks if the thread has run longer than the specified time (11 ms) and yields if it has.

#### demo_sleep

#### demo_scanf

#### demo_mutex

#### demo_cond

#### demo_barrier

#### demo_pool

#### demo_quota_time

#### demo_compare_create

#### demo_working

#### demo_chat_server

#### demo_chat_client          
