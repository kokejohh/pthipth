/*
 * pthipth.h - interface of user threads library
 */

#ifndef PTHIPTH_H
#define PTHIPTH_H 

#include <stdio.h>
#include <time.h>

#include "futex.h"
#include "pthipth_debug.h"

#define HIGHEST_PRIORITY 1
#define DEFAULT_PRIORITY 28
#define LOWEST_PRIORITY 29
#define IDLE_PRIORITY 30
#define MAIN_PRIORITY 31

#define TIME_SLICE 500

enum pthipth_state {
    RUNNING, READY, BLOCKED, SLEEPING, DEFUNCT
};

typedef struct pthipth_attr {
    unsigned long stackSize;
    unsigned priority;
} pthipth_attr_t;

typedef struct pthipth {
    pid_t tid;
} pthipth_t;

typedef struct pthitph_task {
    void *(*function)(void *);
    void *arg;
    unsigned priority;
} pthipth_task_t;

typedef struct pthipth_mutex pthipth_mutex_t;
typedef struct pthipth_cond pthipth_cond_t;
typedef struct pthipth_barrier pthipth_barrier_t;

typedef struct pthipth_private {
    pid_t tid;
    enum pthipth_state state;
    void *(*start_func)(void *);
    void *arg;
    void *return_value;
    struct pthipth_private *blockedForJoin;
    futex_t sched_futex;
    int priority, init_priority, old_priority;
    time_t wake_time;
    time_t last_selected;
    pthipth_mutex_t *current_mutex;
    pthipth_cond_t *current_cond;
    pthipth_barrier_t *current_barrier;
    // Bucket queue
    struct pthipth_private *prev, *next, *inside_prev, *inside_next;
    // AVL tree
    struct pthipth_private *parent, *left, *right;
    int height;
} pthipth_private_t;


pthipth_t pthipth_self(void);

int pthipth_create(pthipth_t *new_thread_ID, pthipth_attr_t *attr, pthipth_task_t *task);

int pthipth_yield(void);

int pthipth_join(pthipth_t target_thread, void **status);

void pthipth_exit(void *retval);

void pthipth_sleep(time_t millisec);

pid_t __pthipth_gettid();

pthipth_private_t *__pthipth_selfptr();

int __pthipth_dispatcher(pthipth_private_t *);

#endif
