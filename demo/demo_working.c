#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "pthipth.h"

pthread_mutex_t mutex;
int newline = 0;
void *task(void *arg)
{
    pthread_mutex_lock(&mutex);

    printf("%ld", (long)arg);
    fflush(stdout);
    if ((newline % 10) == 9) printf("\n");
    newline++;

    pthread_mutex_unlock(&mutex);

    int *result = malloc(sizeof(int));
    *result = (long)arg;
    return result;
}

void *task2(void *arg)
{
    printf("%ld", (long)arg);
    fflush(stdout);
    if ((newline % 10) == 9) printf("\n");
    newline++;

    int *result = malloc(sizeof(int));
    *result = (long)arg;
    return result;
}



int main()
{
    pthread_mutex_init(&mutex, NULL);
    int n = 1000;
    pthread_t pthreads[n];
    pthipth_t pthipths[n];
    void *num[n];

    for (long i = 0; i < n; i++)
    {
	pthread_create(&pthreads[i], NULL, &task, (void *)(i % 10));
    }

    for (int i = 0; i < n; i++)
    {
	pthread_join(pthreads[i], &num[i]);
    }

    printf("--------------------------\n");

    for (long i = 0; i < n; i++)
    {
	pthipth_create(&pthipths[i], NULL, &(pthipth_task_t){task2, (void *)(i % 10), DEFAULT_PRIORITY});
    }
    
    for (int i = 0; i < n; i++)
    {
	pthipth_join(pthipths[i], &num[i]);
    }

    
    return 0;
}
