#include <stdio.h>
#include "pthipth.h"

pthipth_mutex_t mutex;
pthipth_cond_t cond;

int num = 0;

void *child_broadcast(void *arg)
{
    num++;

    printf("tid %d increment to %d\n", pthipth_self(), num);
    if (num >= 5)
    {
	pthipth_cond_broadcast(&cond);
	printf("tid %d broadcast\n", pthipth_self());
    }
    return NULL;
}

void *child_function(void *arg)
{
    int tid = pthipth_self();

    pthipth_mutex_lock(&mutex);
    while (num < 5) 
    {
	printf("tid %d wait until 5\n", tid);
	pthipth_cond_wait(&cond, &mutex);
    }
    pthipth_mutex_unlock(&mutex);
    
    printf("tid %d, num %d woken\n", tid, num);

    return NULL;
}

int main()
{
    pthipth_init();

    pthipth_mutex_init(&mutex);
    pthipth_cond_init(&cond);

    int n = 6;
    pthipth_t threads[n];

    pthipth_create(&threads[0], NULL, &(pthipth_task_t){child_function, NULL, DEFAULT_PRIORITY});
    for (int i = 1; i < 6; i++)
	pthipth_create(&threads[i], NULL, &(pthipth_task_t){child_broadcast, NULL, DEFAULT_PRIORITY});

    for (int i = 0; i < n; i++)
	pthipth_join(threads[i], NULL);

    return (0);
}
