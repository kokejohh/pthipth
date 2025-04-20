#include <stdio.h>
#include <time.h>
#include "pthipth.h"

void *child_function(void *arg)
{
    printf("before yield hello from tid %d, prio %d\n", pthipth_self(), pthipth_getprio());
    
    struct timespec ts;

    ts.tv_nsec = 10000000;
    ts.tv_sec = 0;

    nanosleep(&ts, NULL);

    pthipth_yield_qtime(11);

    printf("after yield hello from tid %d, prio %d\n", pthipth_self(), pthipth_getprio());

    return NULL;
}

int main()
{
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
