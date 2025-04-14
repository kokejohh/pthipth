#ifndef PTHIPTH_POOL_H
#define PTHIPTH_POOL_H

#include "pthipth_types.h"
#include "pthipth_mutex.h"
#include "pthipth_cond.h"

#define MIN_THREAD_COUNT 1
#define MAX_THREAD_COUNT 9999
#define MIN_QUEUE_SIZE 1
#define MAX_QUEUE_SIZE 9999
#define SHUTDOWN 1

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

int pthipth_pool_create(pthipth_pool_t *pool, pthipth_attr_t *attr, int thread_count, int queue_size);

int pthipth_pool_add(pthipth_pool_t *pool, pthipth_task_t *task);

int pthipth_pool_destroy(pthipth_pool_t *pool);

#endif
