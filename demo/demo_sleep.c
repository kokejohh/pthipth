#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "pthipth.h"

void *child_function(void *arg)
{
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);
    double start_sec = (double)start.tv_sec + (double)start.tv_nsec / 1e9;
    printf("before sleep tid %d, time %lf\n", pthipth_self(), 0.0);

    pthipth_sleep(1000);

    clock_gettime(CLOCK_MONOTONIC, &end);
    double end_sec = (double)end.tv_sec + (double)end.tv_nsec / 1e9;
    double taken = end_sec - start_sec;
    printf("after sleep tid %d, time %lf\n", pthipth_self(), taken);

    return NULL;
}

int main()
{
    pthipth_init();
    int n = 5;

    pthipth_t threads[n];

    for (int i = 0; i < n; i++)
    {
	pthipth_create(&threads[i], NULL,
		&(pthipth_task_t){child_function, NULL, DEFAULT_PRIORITY});
    }

    for (int i = 0; i < n; i++)
    {
	pthipth_join(threads[i], NULL);
    }

    return (0);
}
