#include <stdio.h>
#include "pthipth.h"

pthipth_mutex_t mutex;
pthipth_cond_t cond;

int num = 0;

void *child_function(void *arg)
{
    num++;

    if (num >= 5)
    {
	pthipth_cond_broadcast(&cond);
	printf("broadcast\n");
    }

    return NULL;
}

void *child_function2(void *arg)
{
    pthipth_mutex_lock(&mutex);

    while (num < 5)
    {
	printf("num < 5\n");
	pthipth_cond_wait(&cond, &mutex);
    }

    pthipth_mutex_unlock(&mutex);
    
    printf("I am woken\n");

    return NULL;
}

int main()
{
    pthipth_init();

    pthipth_mutex_init(&mutex);
    pthipth_cond_init(&cond);

    int n = 10;

    pthipth_t threads[n];

    for (int i = 0; i < n / 2; i++)
    {
	pthipth_create(&threads[i], NULL, &(pthipth_task_t){child_function2, NULL, DEFAULT_PRIORITY});
    }
    for (int i = n / 2; i < n; i++)
    {
	pthipth_create(&threads[i], NULL, &(pthipth_task_t){child_function, NULL, DEFAULT_PRIORITY});
    }

    for (int i = 0; i < n; i++)
    {
	pthipth_join(threads[i], NULL);
    }

    return (0);
}
