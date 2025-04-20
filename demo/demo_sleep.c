#include <stdio.h>
#include <unistd.h>
#include "pthipth.h"

void *child_function(void *arg)
{
    printf("before sleep tid %d, prio %d\n", pthipth_self(), pthipth_getprio());
    
    //sleep(1);
    pthipth_sleep(1000);

    printf("after sleep tid %d, prio %d\n", pthipth_self(), pthipth_getprio());

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
