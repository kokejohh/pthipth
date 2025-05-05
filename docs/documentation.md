# pthipth's Documentation

#### Basic Workflow:
when threads are created, they block using futex_wait. To run a thread,
you must call pthipth_join or pthipth_detach, which uses futex_wake.
Calling pthipth_yield() can also wake it, but is not recommended,
as it does not handle thread termination.

#### Thread states
- RUNNING -  the thread is currently executing on the CPU.
- READY -  the thread is ready to run and waiting for CPU time.
- BLOCKED- the thread is waiting for a resource or event
- SLEEPING - the thread is paused for a set duration.
- DEFUNCT - the thread has finished execution and is awaiting cleanup.

## Functions

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
##### pthipth_create(pthipth_t *new_thread_ID, pthipth_attr_t *attr, pthipth_task_t *task);
execute clone system call to create a new process with shared address space. 
The (optional) argument passed to pthipth_create specifies the attr
be used by the created thread. We invoke a __pthipth_wrapper function upon 
completion of clone since we dont want the newly created thread to be scheduled 
right-away. This wrapper does a down on the futex corresponsding to this thread 
making the thread sleep.

##### pthipth_join(pthipth_t target_thread, void **status);
Our Thread Control Block(tcb) contains a blockedForJoin parameter which 
specifies a thread that has done a join on this thread. We set the state of 
the thread that has done a join on this thread to be DEFUNCT. This means that 
when this thread exits, it will wake up the thread that was waiting on this 
thread completion via join. If more than one thread try to do a mythread_join() 
on the same target thread, only one will succeed and all others will return error.

##### pthipth_detach(pthipth_t target_thread);

##### pthipth_yield(void); and pthipth_yield_qtime(int64_t ms);

##### pthipth_exit(void *retval);

##### pthipth_self(void);

##### pthipth_sleep(int64_t millisec);

##### pthipth_scanf(const char *format, ...);

##### pthipth_set_prio(int new_priority);

##### pthipth_get_prio(void); pthipth_get_stack_size(void);  pthipth_get_time_quota(void); pthipth_get_aging_factor(void);  int pthipth_get_aging_time(void);

##### pthipth_barrier_init(pthipth_barrier_t *barrier, int count);
##### pthipth_barrier_destroy(pthipth_barrier_t *barrier);

#### pthipth_barrier_wait(pthipth_barrier_t *barrier);

#### pthipth_cond_init(pthipth_cond_t *cond);
#### pthipth_cond_destroy(pthipth_cond_t *cond);

#### pthipth_cond_wait(pthipth_cond_t *cond, pthipth_mutex_t *mutex);
#### pthipth_cond_wait_non(pthipth_cond_t *cond);

#### pthipth_cond_signal(pthipth_cond_t *cond);
#### pthipth_cond_broadcast(pthipth_cond_t *cond);

#### pthipth_mutex_init(pthipth_mutex_t *mutex);
#### pthipth_mutex_destroy(pthipth_mutex_t *mutex);

#### pthipth_mutex_lock(pthipth_mutex_t *mutex);

#### pthipth_mutex_trylock(pthipth_mutex_t *mutex);

#### pthipth_mutex_unlock(pthipth_mutex_t *mutex);

#### pthipth_pool_create(pthipth_pool_t *pool, pthipth_attr_t *attr, int thread_count, int queue_size);
create pool of thread can assign number of thread and queue size
use pthipth_create create thread has ability receive task and then if task finish will receiv
new task form queue instead destroy themself use pthipth_cond_wait_non for wait thread not have
task to do something when task finish will give themself highest priority


#### pthipth_pool_add(pthipth_pool_t *pool, pthipth_task_t *task);
add task to pool specifly and then use pthipth_cond_signal to wake thread in pool

#### pthipth_pool_destroy(pthipth_pool_t *pool);
pthipth_join all thread created with pthipth_pool_create and free pool if all thread terminate

#### Idle thread:
We have implemented an idle thread which infinitely loopes over and repeatedly 
yields the processor. have priority same with main thread

## After this is internal function for handle thread
### Data Structures:
to manage and schedule threads efficiently, this library uses the following core data structures:
1. Bucket Queue - used by the scheduler to select the next runnable thread.
2. AVL Tree - each node stores a Thread Control Block (TCB)
3. Queue - maintain lists of threads in special states : sleeping, blocked, defunct states

  * #### Bucket Queue

    * ##### void pthipth_prio_init(pthipth_private_t \*node);
      - initialize the first node in the priority bucket queue.
    * ##### void pthipth_prio_insert(pthipth_private_t \*node);
      - insert a node into the bucket queue.
      - if a node with the same priority already exists, insert into its bucket.
      - otherwise, insert the node in order based on its priority.
    * ##### void pthipth_prio_reinsert(pthipth_private_t \*node);
      - remove the node and insert it again (for update its priority).
    * ##### void pthipth_prio_delete(pthipth_private_t \*node);
      - delete the specified node from the priority bucket queue.
    * ##### void pthipth_prio_rotate();
      - rotate the current bucket so the next node with the highest priority becomes the head.
    * ##### pthipth_private_t \*pthipth_private_t \*pthipth_prio_peek();
      - return the current head node with the highest priority without removing it.
    * ##### pthipth_private_t \*pthipth_prio_extract();
      - return and remove the current head node with the highest priority from the queue.
      - the next highest-priority node (or the next in the same bucket) will become the new head.
    * ##### pthipth_private_t \*pthipth_prio_search(pid_t tid);
      - search and return the node with the specified thread ID (tid).
    * ##### void pthipth_prio_display();
      - display information about all nodes in the bucket queue.

  * #### AVL Tree
    * ##### void pthipth_avl_init(pthipth_private_t *node);
      - initialize the first node in the avl tree.
    * ##### void pthipth_avl_insert(pthipth_private_t *node);
      - insert a node into the avl tree.
      - the tree will automatically balance itself.
    * ##### void pthipth_avl_delete(pthipth_private_t *node);
      - delete a node in the avl tree.
      - after deletion, the tree rebalances if necessary to preserve avl properties
    * ##### pthipth_private_t *pthipth_avl_search(pid_t tid);
      - earch for a node with the specified thread ID (tid) in the avl tree.
      - returns a pointer to the node if found, or NULL if not.
    * ##### void pthipth_avl_display();
      - display information about all nodes in the avl tree.

  * #### Queue
    * ##### void pthipth_queue_init(pthipth_queue_t *q, pthipth_private_t *node);
      - initialize the first node in the queue.
    * ##### void pthipth_queue_add(pthipth_queue_t *q, pthipth_private_t *node);
      - add a node to the end of the queue.
    * ##### void pthipth_queue_delete(pthipth_queue_t *q, pthipth_private_t *node);
      - delete the specified node from the queue.
    * ##### void pthipth_queue_display(pthipth_queue_t *q);
      - display information about all nodes in the queue.
     
    ```pthipth_private_t``` uses the same next and prev pointers for both the **bucket queue** and the **generic queue.**
    This means a **node can only exist in one data structure at a time.**
    In other words, a thread can either be in the bucket queue (e.g., in the ready or running state),
    or in the generic queue (e.g., in the sleeping, blocked, or defunct states).

#### Internal functions
int futex_down(futex_t *futx);
int futex_up(futex_t *futx);
int __pthipth_wrapper(void *thread_tcb)
void __pthipth_change_to_state(pthipth_private_t *node, pthipth_state_t to_state);
void __pthipth_set_thread_time_quota(int ms);
int __pthipth_dispatcher(pthipth_private_t *node);
pthipth_private_t *__pthipth_selfptr();
void __pthipth_check_sleeping();
void __pthipth_check_detach(void);
void __pthipth_aging();
uint64_t __pthipth_gettime_ms();
__PTHIPTH_SIGNAL_BLOCK();
__PTHIPTH_SIGNAL_UNBLOCK();
