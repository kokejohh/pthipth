#include <stdio.h>
#include "pthipth.h"

int num = 0;
pthipth_mutex_t mutex;

void *child_function(void *arg)
{
    printf("tid %d, prio %d mutex_lock\n",
	    pthipth_self(), pthipth_get_prio());
    pthipth_mutex_lock(&mutex);
    printf("tid %d, prio %d  between lock\n",
	    pthipth_self(), pthipth_get_prio());
    num++;
    pthipth_mutex_unlock(&mutex);
    printf("tid %d, prio %d mutex_unlock\n",
	    pthipth_self(), pthipth_get_prio());
    return NULL;
}

int main()
{
    pthipth_init();

    pthipth_mutex_init(&mutex);

    int n = 5;
    pthipth_t threads[n];

    for (int i = 0; i < n; i++)
	pthipth_create(&threads[i], NULL,
		&(pthipth_task_t){child_function, NULL, DEFAULT_PRIORITY});

    for (int i = 0; i < n; i++)
	pthipth_join(threads[i], NULL);

    return 0;
}
