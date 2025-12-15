#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>

int num1 = 0;
pthread_mutex_t pmutex;

void *child_function(void *arg)
{
    pthread_mutex_lock(&pmutex);
    num1++;
    pthread_mutex_unlock(&pmutex);
    return NULL;
}

int main()
{
    double elapsed;

    struct timespec start, end;

    int n = 9000;
    int m = 10;
    int numOfthreads[] = {1, 10, 50, 100, 1000, 2000, 3000, 6000, 9000};
    int o = sizeof(numOfthreads) / sizeof(numOfthreads[0]);
    float totalOfthread[9] = {0};
    float avg[9] = {0};

    pthread_t thread[n];

    pthread_mutex_init(&pmutex, NULL);

    int round = 100;
    for (int k = 0; k < o; k++)
    {
	printf("numOfthreads %d\n", numOfthreads[k]);
	num1 = 0;
	totalOfthread[k] = 0;
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

	    //printf("pthread elapsed time: %.5f ms, avg: %.5f, ans: %d\n", elapsed, elapsed / numOfthreads[k], num1);
	    totalOfthread[k] += elapsed;
	}
	avg[k] = totalOfthread[k] / numOfthreads[k];
	printf("pthread elapsed time: %.5f ms, avg: %.5f, ans: %d\n", totalOfthread[k], avg[k], num1);

	printf("\n");
    }

    return (0);
}
