#ifndef PTHIPTH_POOL_H
#define PTHIPTH_POOL_H

#include "pthipth_mutex.h"
#include "pthipth_cond.h"

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
    int started;
} pthipth_pool_t;

pthipth_pool_t *pthipth_pool_create(pthipth_attr_t *attr, int thread_count, int queue_size);

int pthipth_pool_add(pthipth_pool_t *pool, pthipth_task_t *task);

int pthipth_pool_destroy(pthipth_pool_t *pool, int flags);

#endif
