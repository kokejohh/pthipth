#ifndef PTHIPTH_INTERNAL_TYPE
#define PTHIPTH_INTERNAL_TYPE

#include <stdint.h>

#include "futex.h"

typedef struct pthipth_barrier pthipth_barrier_t;
typedef struct pthipth_cond pthipth_cond_t;
typedef struct pthipth_mutex pthipth_mutex_t;

typedef enum pthipth_state {
    READY, RUNNING, BLOCKED, SLEEPING, DEFUNCT
} pthipth_state_t;

// pthipth_private_t
typedef struct pthipth_private {
    pid_t tid;
    pid_t tid_watch;
    pthipth_state_t state;
    void *(*start_func)(void *);
    void *arg;
    void *return_value;
    struct pthipth_private *blockedForJoin;
    futex_t sched_futex;
    int priority, init_priority, old_priority;
    uint64_t wake_time;
    uint64_t last_selected;
    int time_quota;
    int aging_factor;
    int aging_time;
    int mutex_count;
    pthipth_cond_t *current_cond;
    pthipth_barrier_t *current_barrier;
    char *child_stack;
    int stack_size;
    int is_detach;
    // Bucket queue and queue
    struct pthipth_private *prev, *next;
    struct pthipth_private *prev2, *next2;
    int cur_priority;
    // AVL tree
    struct pthipth_private *parent, *left, *right;
    int height;
} pthipth_private_t;

#endif
