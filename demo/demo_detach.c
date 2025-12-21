#include <stdio.h>
#include "pthipth.h"

void *child_function(void *arg)
{
    printf("tid %d, prio %d\n", pthipth_self(), pthipth_get_prio());

    return NULL;
}

int main()
{
    pthipth_init();

    int n = 5;
    pthipth_t threads[n];

    pthipth_task_t task = {
	.function=child_function,
	.arg=NULL,
	.priority=DEFAULT_PRIORITY
    };

    for (int i = 0; i < n; i++)
	pthipth_create(&threads[i], NULL, &task);

    pthipth_yield();

    for (int i = 0; i < n; i++)
	pthipth_detach(threads[i]);

    return 0;
}
