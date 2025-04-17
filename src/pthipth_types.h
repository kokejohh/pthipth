#ifndef PTHIPTH_TYPES_H
#define PTHIPTH_TYPES_H

#include <stdint.h>

#include "futex.h"

typedef struct pthipth_mutex pthipth_mutex_t;
typedef struct pthipth_cond pthipth_cond_t;
typedef struct pthipth_barrier pthipth_barrier_t;

typedef struct pthipth_attr {
    uint64_t stackSize;
    int time_quota_ms;
    int aging_factor;
    int aging_time;

} pthipth_attr_t;

typedef struct pthipth_task {
    void *(*function)(void *);
    void *arg;
    int priority;
} pthipth_task_t;

typedef pid_t pthipth_t;

typedef enum pthipth_state {
    RUNNING, READY, BLOCKED, SLEEPING, DEFUNCT
} pthipth_state_t;

typedef struct pthipth_private {
    pid_t tid;
    pthipth_state_t state;
    void *(*start_func)(void *);
    void *arg;
    void *return_value;
    struct pthipth_private *blockedForJoin;
    futex_t sched_futex;
    uint8_t priority, init_priority, old_priority;
    uint64_t wake_time;
    uint64_t last_selected;
    int time_quota;
    int aging_factor;
    int aging_time;
    pthipth_mutex_t *current_mutex;
    pthipth_cond_t *current_cond;
    pthipth_barrier_t *current_barrier;
    // Bucket queue and queue
    struct pthipth_private *prev, *next, *inside_prev, *inside_next;
    // AVL tree
    struct pthipth_private *parent, *left, *right;
    int height;
} pthipth_private_t;

#endif
