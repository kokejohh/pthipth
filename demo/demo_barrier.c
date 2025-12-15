#include <stdio.h>
#include "pthipth.h"

pthipth_barrier_t barrier;

void *child_function(void *arg)
{
    printf("before barrier tid %d, prio %d\n", pthipth_self(), pthipth_get_prio());

    if (pthipth_barrier_wait(&barrier) == PTHIPTH_BARRIER_SERIAL_THREAD)
    {
	printf("wake other threads\n");
    }

    printf("after barrier tid %d, prio %d\n", pthipth_self(), pthipth_get_prio());

    return NULL;
}

int main()
{
    pthipth_init();
    pthipth_barrier_init(&barrier, 5);

    int n = 10;

    pthipth_t threads[n];

    for (int i = 0; i < n; i++)
    {
	pthipth_create(&threads[i], NULL, &(pthipth_task_t){child_function, NULL, DEFAULT_PRIORITY});
    }

    for (int i = 0; i < n; i++)
    {
	pthipth_join(threads[i], NULL);
    }

    return (0);
}
