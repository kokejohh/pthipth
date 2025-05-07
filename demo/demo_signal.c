#include <stdio.h>
#include "pthipth.h"

int num = 0;

void *child_function(void *arg)
{
    for (int i = 0; i < 10000; i++)
    {

	pthipth_signal_block();
	num++;
	printf("num is %d\n", num);
	pthipth_signal_unblock();
    }

    return NULL;
}

int main()
{
    int n = 10;

    pthipth_t threads[n];

    pthipth_attr_t attr = {0};
    attr.time_quota_ms = 1;
    for (int i = 0; i < n; i++)
    {
	pthipth_create(&threads[i], &attr, &(pthipth_task_t){child_function, NULL, DEFAULT_PRIORITY});
    }

    for (int i = 0; i < n; i++)
    {
	pthipth_join(threads[i], NULL);
    }

    printf("num %d\n", num);

    return (0);
}
