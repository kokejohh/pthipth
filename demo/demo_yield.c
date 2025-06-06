#include <stdio.h>
#include "pthipth.h"

void *child_function(void *arg)
{
    printf("before yield hello from tid %d, prio %d\n", pthipth_self(), pthipth_get_prio());
    
    pthipth_yield();

    printf("after yield hello from tid %d, prio %d\n", pthipth_self(), pthipth_get_prio());

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
