#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "pthipth.h"
#include "pthipth_debug.h"
#include "pthipth_mutex.h"
#include "pthipth_avl.h"

pthipth_mutex_t hate;
void *high_priority_task(void *arg)
{
    printf("high priority try lock\n");
    pthipth_mutex_lock(&hate);
    printf("high priority acquire resource\n");
    pthipth_mutex_unlock(&hate);

    return NULL;
}

void *low_priority_task(void *arg)
{
    printf("low priority lock\n");
    pthipth_mutex_lock(&hate);

    pthipth_t thread;
    pthipth_attr_t attr = { .priority = 1};

    pthipth_create(&thread, &attr, high_priority_task, NULL);

    pthipth_yield();

    //pthipth_join(thread, NULL);

    printf("low priority unlock\n");
    pthipth_mutex_unlock(&hate);

    return NULL;
}

void *medium_priority_task(void *arg)
{
    printf("medium\n");
    sleep(1);
    return NULL;
}

int main()
{
    pthipth_mutex_init(&hate);
    int n = 2;
    pthipth_t threads[n], thread1;
    pthipth_attr_t thread_attr[n];

    for (int i = 0; i < n; i++)
    {
	pthipth_attr_t attr;
	attr.priority = 5;
	if (i == 0)
	{
	    attr.priority = 4;
	    pthipth_create(&threads[i], &attr, low_priority_task, NULL);
	    sleep(1);
	}
	else if (i == 1)
	{
	    attr.priority = 4;
	    pthipth_create(&threads[i], &attr, medium_priority_task, NULL);
	}
    }

    for (int i = 0; i < n; i++)
    {
	pthipth_join(threads[i], NULL);
    }

    //pthipth_prio_display();

    pthipth_avl_display();

    sleep(1);
}

