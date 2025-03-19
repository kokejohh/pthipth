#include <stdlib.h>
#include "pthipth_cond.h"
#include "pthipth_mutex.h"
#include "pthipth_pool.h"
#include "pthipth_prio.h"

static int pthipth_pool_free(pthipth_pool_t *pool);
static void *pthipth_thread(void *arg);

int pthipth_pool_create(pthipth_pool_t *pool, pthipth_attr_t *attr, int thread_count, int queue_size)
{
    if (thread_count <= MIN_THREAD_COUNT || thread_count > MAX_THREAD_COUNT ||
	    queue_size <= MIN_QUEUE_SIZE || queue_size > MAX_QUEUE_SIZE ||
	    pool == NULL)
    {
	return -1;
    }

    // Initialize
    pool->thread_count = 0;
    pool->queue_size = queue_size;
    pool->head = pool->tail = pool->count = 0;
    pool->shutdown = pool->started = 0;

    // Allocate thread and task queue
    pool->threads = (pthipth_t *)malloc(sizeof(pthipth_t) * thread_count);
    pool->queue = (pthipth_task_t *)malloc(sizeof(pthipth_task_t) * queue_size);

    // Init
    if ((pthipth_mutex_init(&pool->lock) != 0) || pthipth_cond_init(&pool->notify) != 0 ||
	    (pool->threads == NULL || pool->queue == NULL))
    {
	pthipth_pool_free(pool);
	return -1;
    }

    for (int i = 0; i < thread_count; i++)
    {
	if (pthipth_create(&pool->threads[i], attr, &(pthipth_task_t){ pthipth_thread, (void *)pool, HIGHEST_PRIORITY}) != 0)
	{
	    pthipth_pool_destroy(pool);
	    return -1;
	}
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

    pthipth_mutex_lock(&pool->lock);

    int next = (pool->tail + 1) % pool->queue_size;

    if (pool->count == pool->queue_size) return -1;

    pool->queue[pool->tail].function = task->function;
    pool->queue[pool->tail].arg = task->arg;
    pool->queue[pool->tail].priority = task->priority;
    pool->tail = next;
    pool->count += 1;

    pthipth_cond_signal(&pool->notify);

    pthipth_mutex_unlock(&pool->lock);

    return 0;
}

int pthipth_pool_destroy(pthipth_pool_t *pool)
{
    if (pool == NULL || pool->threads == NULL || pool->queue == NULL) return -1;

    pthipth_mutex_lock(&pool->lock);

    if (pool->shutdown) 
    {
	pthipth_mutex_unlock(&pool->lock);
	return -1;
    }

    pool->shutdown = SHUTDOWN;

    pthipth_cond_broadcast(&pool->notify);

    pthipth_mutex_unlock(&pool->lock);

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
	pthipth_mutex_lock(&pool->lock);

	while (!pool->shutdown && pool->count == 0)
	    pthipth_cond_wait(&(pool->notify), &(pool->lock));

	if (pool->shutdown && pool->count == 0) break;

	task.function = pool->queue[pool->head].function;
	task.arg = pool->queue[pool->head].arg;
	task.priority = pool->queue[pool->head].priority ? pool->queue[pool->head].priority : DEFAULT_PRIORITY;
	if (pool->queue[pool->head].priority < HIGHEST_PRIORITY) task.priority = HIGHEST_PRIORITY;
	else if (task.priority > LOWEST_PRIORITY && task.priority != IDLE_PRIORITY) task.priority = LOWEST_PRIORITY;

	pool->head = (pool->head + 1) % pool->queue_size;
	pool->count -= 1;

	thread->priority = thread->init_priority = thread->old_priority = task.priority;

	pthipth_prio_reinsert(thread);

	pthipth_mutex_unlock(&pool->lock);

	pthipth_yield();

	(*(task.function))(task.arg);
    }

    pool->started--;

    pthipth_mutex_unlock(&pool->lock);

    pthipth_exit(NULL);

    return NULL;
}
