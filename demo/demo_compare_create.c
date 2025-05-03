#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>

#include "pthipth.h"

pthread_mutex_t pmutex;
pthipth_mutex_t kmutex;
void *child_function(void *arg)
{
    int num = 0;
    num++;
    return NULL;
}

void *child_function2(void *arg)
{
    int num = 0;
    num++;
    return NULL;
}

int main()
{
    double elapsed;
    pthipth_pool_t pool;

    struct timespec start, end;

    int n = 9000;
    int m = 1;
    int numOfthreads[] = {1, 10, 100, 1000, 2000, 3000, 6000, 9000};
    int o = sizeof(numOfthreads) / sizeof(numOfthreads[0]);

    pthread_t thread[n];
    pthipth_t thipth[n];

    for (int k = 0; k < o; k++)
    {
	printf("numOfthreads %d\n", numOfthreads[k]);
	for (int j = 0; j < m; j++)
	{
	    clock_gettime(CLOCK_MONOTONIC, &start);

	    for (int i = 0; i < numOfthreads[k]; i++)
		pthread_create(&thread[i], NULL, child_function, NULL);

	    for (int i = 0; i < numOfthreads[k]; i++)
		pthread_join(thread[i], NULL);

	    clock_gettime(CLOCK_MONOTONIC, &end);

	    elapsed = (end.tv_sec - start.tv_sec) * 1e3 +
		(end.tv_nsec - start.tv_nsec) / 1e6;

	    printf("pthread elapsed time: %.3f ms, avg: %.3f\n", elapsed, elapsed / numOfthreads[k]);
	}

	for (int j = 0; j < m; j++)
	{

	    clock_gettime(CLOCK_MONOTONIC, &start);

	    for (int i = 0; i < numOfthreads[k]; i++)
		pthipth_create(&thipth[i], NULL,
			&(pthipth_task_t){child_function2, NULL, DEFAULT_PRIORITY});

	    for (int i = 0; i < numOfthreads[k]; i++)
		pthipth_join(thipth[i], NULL);

	    clock_gettime(CLOCK_MONOTONIC, &end);

	    elapsed = (end.tv_sec - start.tv_sec) * 1e3 +
		(end.tv_nsec - start.tv_nsec) / 1e6;

	    printf("pthipth elapsed time: %.3f ms, avg: %.3f\n", elapsed, elapsed / numOfthreads[k]);
	}

	for (int j = 0; j < m; j++)
	{
	    clock_gettime(CLOCK_MONOTONIC, &start);

	    pthipth_pool_create(&pool, NULL, 100, 9000);

	    for (int i = 0; i < numOfthreads[k]; i++)
		pthipth_pool_add(&pool, &(pthipth_task_t){child_function2, NULL, DEFAULT_PRIORITY});

	    pthipth_pool_destroy(&pool);

	    clock_gettime(CLOCK_MONOTONIC, &end);

	    elapsed = (end.tv_sec - start.tv_sec) * 1e3 +
		(end.tv_nsec - start.tv_nsec) / 1e6;

	    printf("pool elapsed time: %.3f ms, avg: %.3f\n", elapsed, elapsed / numOfthreads[k]);
	}
	printf("\n");
    }

    return (0);
}
