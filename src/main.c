
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include "pthipth.h"
#include "pthipth_debug.h"
#include "pthipth_mutex.h"
#include "pthipth_avl.h"
#include "pthipth_cond.h"
#include "pthipth_barrier.h"
#include "pthipth_pool.h"

pthipth_mutex_t mutex;

void *test(void *arg)
{
    printf("koke\n");
    return NULL;
}

pthipth_pool_t pool;// = pthipth_pool_create(NULL, 5, 10);
void *child_function(void *arg)
{
    pthipth_private_t *t = __pthipth_selfptr();
    printf("I love you\n");
    printf("tid %d prio %d arg %ld\n", __pthipth_gettid(), t->priority, (long)arg);

    pthipth_pool_add(&pool, &(pthipth_task_t){ .function = test, .arg = NULL, .priority = DEFAULT_PRIORITY });

    return NULL;
}

int main()
{
    pthipth_pool_create(&pool, NULL, 5, 100);

    for (long i = 0; i < 12; i++)
    {
	pthipth_task_t task = {
	    .function = child_function,
	    .arg = (void *)(i * 10),
	    i % 2
	};
	pthipth_pool_add(&pool, &task);
    }
    printf("love\n");

    pthipth_pool_destroy(&pool);
    pthipth_task_t task = {
	.function = child_function,
	.arg = (void *)99,
	99 % 2// + 1
    };
//    pthipth_avl_display();
    pthipth_pool_create(&pool, NULL, 5, 100);
    pthipth_pool_add(&pool, &task);
    pthipth_pool_destroy(&pool);
//    pthipth_avl_display();
}

