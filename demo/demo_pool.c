#include <stdio.h>
#include "pthipth.h"

void *child_function(void *arg)
{
    printf("tid %d, prio %d\n", pthipth_self(), pthipth_get_prio());

    return NULL;
}

int main()
{
    pthipth_pool_t pool;

    pthipth_pool_create(&pool, NULL, 10, 20);

    for (int i = 0; i < 20; i++)
    {
	pthipth_pool_add(&pool, &(pthipth_task_t){child_function, NULL, DEFAULT_PRIORITY});
    }

    pthipth_pool_destroy(&pool);

    return (0);
}
