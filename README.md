# pthipth

<p align="center">
  <img src="https://media.tenor.com/km-lY7Mlqc4AAAAj/typing-pepe-the-frog.gif" alt="chil" />
</p>

### pthipth (pitchakorn thiprangsi thread) ?
It is non-preemptive priority thread library inspired by pthread

### How does pthipth differ from pthread ?
1. non-preemptive, meaning threads voluntarily yield control.
2. support thread priorities.
3. the scheduler is fully controlled in user space.
4. each thread can have a time quota.
5. each thread has its own aging mechanism to prevent starvation.
6. can use thread pool to manage thread creation and reuse efficiently.

### All functions
```
1. pthipth_create            2.pthipth_join                  3. pthipth_detach
4. pthipth_yield             5. pthipth_yield_qtime          6. pthipth_exit
7. pthipth_self              8. pthipth_sleep                9. pthipth_scanf
10. pthipth_set_prio         11. pthipth_get_prio            12. pthipth_get_stack_size
13. pthipth_get_time_quota   14. pthipth_get_aging_factor    15. pthipth_get_aging_time
16. pthipth_barrier_init     17. pthipth_barrier_wait        18. pthipth_barrier_destroy
19. pthipth_cond_init        20. pthipth_cond_wait           21. pthipth_cond_wait_non
22. pthipth_cond_signal      23. pthipth_cond_broadcast      24. pthipth_cond_destroy
25. pthipth_mutex_init       26. pthipth_mutex_lock          27. pthipth_mutex_trylock
28. pthipth_mutex_unlock     29. pthipth_mutex_destroy
30. pthipth_pool_create      31. pthipth_pool_add            32. pthipth_pool_destroy
```

Read more [Documentation](/docs/documentation.md)

### Example
```
#include "pthipth.h"

void *task_example(void *arg)
{
  printf("task_example\n");
  return NULL;
}

int main()
{
  pthipth_t tid;
  pthipth_task_t task = {task_example, NULL, DEFAULT_PRIORITY};
  pthipth_create(&tid, NULL, &task);
  pthipth_join(tid, NULL);
  return 0;
}
```
### How to build
###### Compiltion:

```
make
```
  - this is equivalent to "make all," which compiles the code, creates object files, and builds libpthipth.a and libpthipth.so, along with demo and test files.

###### Library Compiltion:
```
make lib
```
  - it will create the static (libpthipth.a) and shared (libpthipth.so) libraries. To build one, use $ make static or $ make shared.

###### Link header to demo and test:
```
make header
```
  - creates symbolic links for .h files from src to demo and test.

###### Demo:
```
make demo
```
  - compiles the demo as a sample. [Explanation Demo](/docs/demo.md)

###### Test:
```
make test
```
  - compiles the test for internal function.

###### Tags:
```
make tags
```
  - this target will create tags for vim/nvim.

###### Cleaning:
```
make clean
```
  - cleans all build files

###### Re:
```
make re
```
  - cleans all build files and builds again.

### Data Structures:
to manage and schedule threads efficiently, this library uses the following core data structures:
1. Bucket Queue - used by the scheduler to select the next runnable thread.
2. AVL Tree - each node stores a Thread Control Block (TCB)
3. Queue - maintain lists of threads in special states : sleeping, blocked, defunct states

### Basic Workflow:
when threads are created, they block using futex_wait. To run a thread,
you must call pthipth_join or pthipth_detach, which uses futex_wake.
Calling pthipth_yield() can also wake it, but is not recommended,
as it does not handle thread termination.

### Struct in library
##### pthipth_attr_t
this structure defines thread attributes and is used to pass parameters to pthipth_create. It includes:
  - stack size - specifies the size of the thread's stack
  - time quota - sets the maximum time a thread is allowed to run before it yields the processor to another thread.
  - aging - controls the aging mechanism, which includes an aging factor and aging time to adjust priority over time

##### pthipth_task_t
this structure defines the task to be executed by the thread and is also used with pthipth_create. It includes:
  - function: A pointer to the function the thread will execute.
  - argument: The argument passed to the function.
  - priority: The initial priority of the thread.

### Acknowledgements
This project includes code adapted from [Jitesh Shah and Members's](https://github.com/jitesh1337/mythread_lib) implementation.  
The pthipth_pool code includes portions inspired by the work of [Mathias Brossard](https://github.com/mbrossard/threadpool) implementation,which is licensed under the BSD 2-Clause License.

<p align="center">
  <img src="https://media.tenor.com/CH9ZL3SoZZUAAAAi/pepe-the-frog-pepe.gif" alt="chil" />
</p>
