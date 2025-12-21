#include <stdio.h>
#include "pthipth.h"

void *child_function(void *arg)
{
    printf("before scanf tid %d, prio %d\n", pthipth_self(), pthipth_get_prio());
    
    int num;

    //scanf("%d", &num);
    pthipth_scanf("%d", &num);

    printf("after scanf tid %d, prio %d\n", pthipth_self(), pthipth_get_prio());

    printf("num is %d\n", num);

    return NULL;
}

int main()
{
    pthipth_init();

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
