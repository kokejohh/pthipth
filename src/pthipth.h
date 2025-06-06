/*
 * pthipth.h - interface of user threads library
 */

#ifndef PTHIPTH_H
#define PTHIPTH_H 

#include <stdint.h>
#include <stdatomic.h>

#include <sys/types.h>

#define HIGHEST_PRIORITY 1
#define DEFAULT_PRIORITY 28
#define LOWEST_PRIORITY 29
#define IDLE_PRIORITY 30
#define MAIN_PRIORITY 30

#define PTHIPTH_BARRIER_SERIAL_THREAD 1

typedef struct futex futex_t;

// pthipth_t
typedef pid_t pthipth_t;

// pthipth_barrier_t
typedef struct pthipth_barrier {
    int count;
    atomic_int waiting;
} pthipth_barrier_t;

// pthipth_cond_t
typedef struct pthipth_cond {
    char unused;
} pthipth_cond_t;

// pthipth_mutex_t
typedef struct pthipth_mutex {
    futex_t *futx;
    pthipth_t owner_tid;
} pthipth_mutex_t;

// pthipth_attr_t
typedef struct pthipth_attr {
    uint64_t stack_size;
    int time_quota_ms;
    int aging_factor;
    int aging_time;
} pthipth_attr_t;

// pthipth_task_t
typedef struct pthipth_task {
    void *(*function)(void *);
    void *arg;
    int priority;
} pthipth_task_t;

// pthipth_pool_t
typedef struct pthipth_pool {
    pthipth_mutex_t lock;
    pthipth_cond_t notify;
    pthipth_t *threads;
    pthipth_task_t *queue;
    int thread_count;
    int queue_size;
    int head;
    int tail;
    int count;
    int shutdown;
    int task_in_progess;
    int started;
} pthipth_pool_t;

// create
int pthipth_create(pthipth_t *new_thread_ID, pthipth_attr_t *attr, pthipth_task_t *task);

// join
int pthipth_join(pthipth_t target_thread, void **status);

// detach
int pthipth_detach(pthipth_t target_thread);

// yield
void pthipth_yield(void);

// yield quota
void pthipth_yield_qtime(int64_t ms);

// exit
void pthipth_exit(void *retval) __attribute__ ((noreturn));

// self
pthipth_t pthipth_self(void);

// sleep
void pthipth_sleep(int64_t millisec);

// scanf
int pthipth_scanf(const char *format, ...);

// block sigalrm
void pthipth_signal_block(void);

// unblock sigalrm
void pthipth_signal_unblock(void);

// print is sigalrm blocked?
void pthipth_signal_print(void);

// ignore sigalrm
void pthipth_signal_ignore(void);

// restore sigalrm
void pthipth_signal_restore(void);

// set priority
int pthipth_set_prio(int new_priority);

// get priority
int pthipth_get_prio(void);

// get stack size
int pthipth_get_stack_size(void);

// get time quota
int pthipth_get_time_quota(void);

// get aging factor
int pthipth_get_aging_factor(void);

// get aging time
int pthipth_get_aging_time(void);

// barrier
// barrier init
int pthipth_barrier_init(pthipth_barrier_t *barrier, int count);

// barrier wait
int pthipth_barrier_wait(pthipth_barrier_t *barrier);

// barrier destroy
int pthipth_barrier_destroy(pthipth_barrier_t *barrier);

// cond
// cond init
int pthipth_cond_init(pthipth_cond_t *cond);

// cond wait
int pthipth_cond_wait(pthipth_cond_t *cond, pthipth_mutex_t *mutex);

// cond wait for non-preemptive
int pthipth_cond_wait_non(pthipth_cond_t *cond);

// cond signal
int pthipth_cond_signal(pthipth_cond_t *cond);

// cond broadcast
int pthipth_cond_broadcast(pthipth_cond_t *cond);

// cond destroy
int pthipth_cond_destroy(pthipth_cond_t *cond);

// mutex
// mutex init
int pthipth_mutex_init(pthipth_mutex_t *mutex);

// mutex lock
int pthipth_mutex_lock(pthipth_mutex_t *mutex);

// mutex trylock
int pthipth_mutex_trylock(pthipth_mutex_t *mutex);

// mutex unlock
int pthipth_mutex_unlock(pthipth_mutex_t *mutex);

// mutex destroy
int pthipth_mutex_destroy(pthipth_mutex_t *mutex);

// pool
// pool create
int pthipth_pool_create(pthipth_pool_t *pool, pthipth_attr_t *attr, int thread_count, int queue_size);

// pool add
int pthipth_pool_add(pthipth_pool_t *pool, pthipth_task_t *task);

// pool destroy
int pthipth_pool_destroy(pthipth_pool_t *pool);

#endif
