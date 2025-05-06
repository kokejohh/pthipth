#include <stdio.h>
#include "pthipth.h"

void *task(void *arg)
{
    pthipth_t tid = pthipth_self();
    printf("tid %d\n", tid);
    pthipth_detach(tid);

    return NULL;
}

int main()
{
    int n = 10;
    pthipth_t tid[n];

    for (int i = 0; i < n; i++)
    {
	pthipth_create(&tid[i], NULL, &(pthipth_task_t){task, NULL, DEFAULT_PRIORITY});
    }

    pthipth_yield();
    
    return 0;
}
