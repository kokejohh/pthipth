#include <stdio.h>
#include <string.h>
#include "pthipth.h"

pthipth_mutex_t mutex;

int num = 0;
void *task(void *arg)
{
    for (int i = 0; i < 10000; i++)
    {
	printf("num is %d\n", num);
	//pthipth_mutex_lock(&mutex);
	num++;
	//pthipth_mutex_unlock(&mutex);
    }

    return NULL;
}
int main()
{
    pthipth_mutex_init(&mutex);

    int n = 10;
    pthipth_t t[n];

    pthipth_attr_t attr;
    memset(&attr, 0, sizeof(pthipth_attr_t));
    attr.time_quota_ms = 1;
    for (int i = 0; i < n; i++)
    {
	pthipth_create(&t[i], &attr, &(pthipth_task_t){task, NULL, DEFAULT_PRIORITY});
    }

    for (int i = 0; i < n; i++)
    {
	pthipth_join(t[i], NULL);
    }

    printf("num is %d\n", num);
    return 0;
}
