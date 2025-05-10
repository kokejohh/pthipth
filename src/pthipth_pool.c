#include <stdio.h>
#include <stdlib.h>

#include "pthipth.h"
#include "pthipth_internal.h"
#include "pthipth_prio.h"
#include "pthipth_queue.h"
#include "pthipth_signal.h"

#define MIN_THREAD_COUNT 1
#define MAX_THREAD_COUNT 9999
#define MIN_QUEUE_SIZE 1
#define MAX_QUEUE_SIZE 9999
#define SHUTDOWN 1

extern pthipth_private_t *pthipth_prio_head;
extern pthipth_queue_t sleeping_state;
extern pthipth_queue_t blocked_state;

extern futex_t global_futex;

static int pthipth_pool_free(pthipth_pool_t *pool);
static void *pthipth_thread(void *arg);

int pthipth_pool_create(pthipth_pool_t *pool, pthipth_attr_t *attr, int thread_count, int queue_size)
{
    if (thread_count < MIN_THREAD_COUNT || thread_count > MAX_THREAD_COUNT ||
	    queue_size < MIN_QUEUE_SIZE || queue_size > MAX_QUEUE_SIZE ||
	    pool == NULL)
	return -1;

    // initialize
    pool->thread_count = 0;
    pool->queue_size = queue_size;
    pool->head = pool->tail = pool->count = 0;
    pool->task_in_progess = pool->shutdown = pool->started = 0;

    // allocate num of threads and task queues
    pool->threads = (pthipth_t *)malloc(sizeof(pthipth_t) * thread_count);
    pool->queue = (pthipth_task_t *)malloc(sizeof(pthipth_task_t) * queue_size);

    if (pthipth_cond_init(&pool->notify) != 0 || pool->threads == NULL || pool->queue == NULL)
    {
	pthipth_pool_free(pool);
	return -1;
    }

    pthipth_task_t task = {pthipth_thread, pool, HIGHEST_PRIORITY};
    for (int i = 0; i < thread_count; i++)
    {
	if (pthipth_create(&pool->threads[i], attr, &task) != 0)
	{
	    pthipth_pool_destroy(pool);

	    return -1;
	}
	__PTHIPTH_SIGNAL_BLOCK();

	pool->thread_count++;
	pool->started++;
    }

    return 0;
}

int pthipth_pool_add(pthipth_pool_t *pool, pthipth_task_t *task) 
{
    if (pool == NULL || task == NULL || task->function == NULL ||
	    pool->threads == NULL || pool->queue == NULL)
	return -1;

    if (pool->count == pool->queue_size) return -1;

    __PTHIPTH_SIGNAL_BLOCK();
    futex_down(&global_futex);

    int next = (pool->tail + 1) % pool->queue_size;

    pool->queue[pool->tail].function = task->function;
    pool->queue[pool->tail].arg = task->arg;
    pool->queue[pool->tail].priority = task->priority;
    pool->tail = next;
    pool->count += 1;

    futex_up(&global_futex);
    __PTHIPTH_SIGNAL_UNBLOCK();

    pthipth_cond_signal(&pool->notify);

    return 0;
}

int pthipth_pool_destroy(pthipth_pool_t *pool)
{
    if (pool == NULL || pool->threads == NULL || pool->queue == NULL)
	return -1;

    if (pool->shutdown == SHUTDOWN) return -1;

    for (int i = 0; i < pool->thread_count; i++)
	pthipth_join(pool->threads[i], NULL);

    pthipth_pool_free(pool);

    return 0; 
}

static int pthipth_pool_free(pthipth_pool_t *pool)
{
    if (pool == NULL || pool->started > 0) return -1;

    free(pool->threads);
    free(pool->queue);

    pool->threads = NULL;
    pool->queue = NULL;

    return 0;
}

static void *pthipth_thread(void *arg)
{
    pthipth_task_t task;
    pthipth_pool_t *pool = (pthipth_pool_t *)arg;

    pthipth_private_t *thread = __pthipth_selfptr();

    while (1)
    {
	if (pool->task_in_progess == 0 && pool->count == 0)
	{
	    pool->shutdown = SHUTDOWN;
	    pthipth_cond_broadcast(&pool->notify);
	}

	while (pool->count == 0 && pool->shutdown != SHUTDOWN)
	    pthipth_cond_wait_non(&pool->notify);

	__PTHIPTH_SIGNAL_BLOCK();
	futex_down(&global_futex);

	if (pool->shutdown == SHUTDOWN) break;

	task.function = pool->queue[pool->head].function;
	task.arg = pool->queue[pool->head].arg;
	task.priority = pool->queue[pool->head].priority ? pool->queue[pool->head].priority : DEFAULT_PRIORITY;
	if (pool->queue[pool->head].priority < HIGHEST_PRIORITY) task.priority = HIGHEST_PRIORITY;
	else if (task.priority > LOWEST_PRIORITY && task.priority != IDLE_PRIORITY) task.priority = LOWEST_PRIORITY;

	pool->head = (pool->head + 1) % pool->queue_size;
	pool->count -= 1;

	thread->priority = thread->init_priority = thread->old_priority = task.priority;
	pthipth_prio_reinsert(thread);

	pool->task_in_progess++;

	futex_up(&global_futex);
	__PTHIPTH_SIGNAL_UNBLOCK();

	pthipth_yield();

	(*(task.function))(task.arg);

	futex_down(&global_futex);
	__PTHIPTH_SIGNAL_BLOCK();

	pool->task_in_progess--;

	// if task complete set prio high for recieve new task
	thread->priority = thread->init_priority = thread->old_priority = HIGHEST_PRIORITY;
	pthipth_prio_reinsert(thread);

	futex_up(&global_futex);
	__PTHIPTH_SIGNAL_UNBLOCK();
    }

    pool->started--;

    futex_up(&global_futex);
    __PTHIPTH_SIGNAL_UNBLOCK();

    return NULL;
}
