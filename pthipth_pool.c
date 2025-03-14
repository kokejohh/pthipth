#include <stdlib.h>

#include "pthipth.h"
#include "pthipth_pool.h"

static int pthipth_pool_free(pthipth_pool_t *pool);
static void *pthipth_thread(void *arg);

pthipth_pool_t *pthipth_pool_create(pthipth_attr_t *attr, int thread_count, int queue_size)
{
    pthipth_pool_t *pool;

    if (thread_count <= 0 || thread_count > 100 || queue_size <= 0 || queue_size > 100)
    {
	return NULL;
    }

    if ((pool = (pthipth_pool_t *)malloc(sizeof(pthipth_pool_t))) == NULL)
    {
	goto err;
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
	goto err;
    }

    for (int i = 0; i < thread_count; i++)
    {
	if (pthipth_create(&pool->threads[i], attr, &(pthipth_task_t){ pthipth_thread, (void *)pool, DEFAULT_PRIORITY}) != 0)
	{
	    // hi don't forget focus this
	    pthipth_pool_destroy(pool, 0);
	    return NULL;
	}
	pool->thread_count++;
	pool->started++;
    }
    /*
    for (int i = 0; i < thread_count; i++)
    {
	printf("northanear\n");
	if (pthipth_join(pool->threads[i], NULL) != 0)
	{
	    return NULL;
	}
	printf("northanear 2\n");
    }
    */

    return pool;

err:
    if (pool)
    {
	pthipth_pool_free(pool);
    }
    return NULL;
}

int pthipth_pool_add(pthipth_pool_t *pool, pthipth_task_t *task) 
{
    int err = 0;
    int next;

    if (pool == NULL || task == NULL || task->function == NULL)
	return -1;

    if (pthipth_mutex_lock(&pool->lock) != 0)
    {
	return -1;
    }

    next = (pool->tail + 1) % pool->queue_size;

    do
    {
	if (pool->count == pool->queue_size)
	{
	    err = -1;
	    break;
	}
	
	if (pool->shutdown)
	{
	    err = -1;
	    break;
	}

	pool->queue[pool->tail].function = task->function;
	pool->queue[pool->tail].arg = task->arg;
	pool->queue[pool->tail].priority = task->priority;
	pool->tail = next;
	pool->count += 1;

	if (pthipth_cond_signal(&pool->notify) != 0)
	{
	    err = -1;
	    break;
	}
    } while (0);

    if (pthipth_mutex_unlock(&pool->lock) != 0)
    {
	err = - 1;
    }
    return err;
}

int pthipth_pool_destroy(pthipth_pool_t *pool, int flags)
{
    int i, err = 0;

    if (pool == NULL)
    {
	return -1;
    }

    if (pthipth_mutex_lock(&pool->lock) != 0)
    {
	return -1;
    }

    do
    {
	if (pool->shutdown)
	{
	    err = -1;
	    break;
	}

	pool->shutdown = (flags & 2) ? -1 : -2;

	if (pthipth_cond_broadcast(&pool->notify) != 0 || pthipth_mutex_unlock(&pool->lock) != 0)
	{
	    err = -1;
	    break;
	}

	for (int i = 0; i < pool->thread_count; i++)
	{
	    if (pthipth_join(pool->threads[i], NULL) != 0)
	    {
		err = -1;
	    }
	}
    } while (0);

    if (!err)
    {
	pthipth_pool_free(pool);
    }

    return 0;
}

static int pthipth_pool_free(pthipth_pool_t *pool)
{
    if (pool == NULL || pool->started == 0)
    {
	return -1;
    }

    if (pool->threads)
    {
	free(pool->threads);
	free(pool->queue);

	pthipth_mutex_lock(&pool->lock);
    }
    free(pool);
    return 0;
}

static void *pthipth_thread(void *arg)
{
    pthipth_pool_t *pool = (pthipth_pool_t *)arg;
    pthipth_task_t task;

    while (1)
    {
	pthipth_mutex_lock(&pool->lock);

	while ((pool->count == 0) && (!pool->shutdown))
	{
	    pthipth_cond_wait(&(pool->notify), &(pool->lock));
	}

	if (pool->shutdown == -1 || (pool->shutdown == -2 && pool->count == 0)) break;

	task.function = pool->queue[pool->head].function;
	task.arg = pool->queue[pool->head].arg;
	task.priority = pool->queue[pool->head].priority;
	pool->head = (pool->head + 1) % pool->queue_size;
	pool->count -= 1;

	pthipth_mutex_unlock(&pool->lock);

	(*(task.function))(task.arg);

	pthipth_yield();

    }

    pool->started--;

    pthipth_mutex_unlock(&pool->lock);

    pthipth_exit(NULL);

    return NULL;
}
