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

    int n = 10;
    pthipth_t threads[n];

    pthipth_task_t task = {
	.function=child_function,
	.arg=NULL,
	.priority=DEFAULT_PRIORITY
    };

    for (int i = 0; i < n; i++)
    {
	if (i < 2) task.priority = DEFAULT_PRIORITY + 1;
	else if (i < 4) task.priority = DEFAULT_PRIORITY + 2;
	else if (i < 6) task.priority = DEFAULT_PRIORITY + 3;
	else task.priority = DEFAULT_PRIORITY;
	pthipth_create(&threads[i], NULL, &task);
    }

    for (int i = 0; i < n; i++)
	pthipth_join(threads[i], NULL);

    return 0;
}
