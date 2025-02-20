#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "pthipth.h"
#include "pthipth_debug.h"
#include "pthipth_mutex.h"

pthipth_mutex_t hate;

int num = 0;
void *child_function(void *arg)
{
    pid_t tid = __pthipth_gettid();
    printf("before tid %d\n", __pthipth_gettid());

    pthipth_mutex_lock(&hate);
    pthipth_yield();
    num++;
    pthipth_mutex_unlock(&hate);

    printf("num is %d\n", num);
    printf("after tid %d\n", __pthipth_gettid());
    //LOG_PRINTF("start child %d\n", __pthipth_gettid());
    return NULL;
}

void *child_function2(void *arg)
{

    pthipth_t thread1;
    pthipth_attr_t thread_attr1 = {
	.priority = 3
    };
    pthipth_mutex_lock(&hate);
    printf("koke %d\n", __pthipth_gettid());

    pthipth_create(&thread1, &thread_attr1, child_function, NULL);
    printf("middle\n");
    //pthipth_prio_display();
    pthipth_join(thread1, NULL);
    printf("joh %d\n", __pthipth_gettid());

    pthipth_mutex_unlock(&hate);

    return NULL;
}

int main()
{
    pthipth_mutex_init(&hate);
    int n = 1;
    pthipth_t threads[n], thread1;
    pthipth_attr_t thread_attr[n];

    for (int i = 0; i < n; i++)
    {
	pthipth_attr_t attr;
//	attr.stackSize = 1024 * 8;
	attr.priority = 5;// i % 2 + 1;
	//thread_attr[i].stackSize = 1024 * 1024;
	//thread_attr[i].priority = i % 2;// + 1;
	if (i == 0)
	{
	    pthipth_create(&threads[i], &attr, child_function2, NULL);
	}
	else
	{
	    pthipth_create(&threads[i], &attr, child_function, NULL);
	}
	//pthipth_join(threads[i], NULL);
	//pthipth_create(&threads[i], &thread_attr[i], child_function, NULL);
    }

//    pthipth_prio_display();
//    pthipth_avl_display();
    for (int i = 0; i < n; i++)
    {
	pthipth_join(threads[i], NULL);
    }
 //   pthipth_prio_display();

    pthipth_avl_display();
    sleep(1);
}
