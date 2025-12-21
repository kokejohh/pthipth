#include <stdio.h>
#include <stdlib.h>

#include "pthipth.h"

void *child_function(void *arg)
{
    int *num = (int *)arg;
    int *result = malloc(sizeof(int));
    *result = (*num) * 2;

    printf("tid %d, arg %d, return %d\n", pthipth_self(), *(int*)arg, *result);

    pthipth_exit(result);
}

int main()
{
    pthipth_init();

    pthipth_t thread;
    int value = 10;
    void *ret;

    pthipth_task_t task = {
	.function=child_function,
	.arg=&value,
	.priority=DEFAULT_PRIORITY
    };

    pthipth_create(&thread, NULL, &task);
    pthipth_join(thread, &ret);

    int *result = (int *)ret;
    printf("main thread receives %d\n", *result);

    return 0;
}
