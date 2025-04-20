#include <stdio.h>
#include "pthipth.h"

int num = 0;

pthipth_mutex_t mutex;

void *child_function(void *arg)
{
    printf("before mutex lock tid %d, prio %d\n", pthipth_self(), pthipth_getprio());

    pthipth_mutex_lock(&mutex);

    num++;
    printf("between mutex lock tid %d, prio %d\n", pthipth_self(), pthipth_getprio());
    pthipth_yield();

    pthipth_mutex_unlock(&mutex);

    printf("after mutex unlock tid %d, prio %d\n", pthipth_self(), pthipth_getprio());

    return NULL;
}

int main()
{
    pthipth_mutex_init(&mutex);

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

    printf("num %d\n", num);

    return (0);
}
