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

    pthipth_mutex_unlock(&hate);
    pthipth_mutex_lock(&hate);
    pthipth_yield();
    num++;
    pthipth_mutex_unlock(&hate);

    printf("num is %d\n", num);
    //pthipth_yield();
    printf("after tid %d\n", __pthipth_gettid());
    //LOG_PRINTF("start child %d\n", __pthipth_gettid());
    return NULL;
}

void *child_function2(void *arg)
{
    pid_t tid = __pthipth_gettid();
    printf("before tid %d\n", __pthipth_gettid());
    pthipth_yield();
    printf("middle tid %d\n", __pthipth_gettid());
    pthipth_yield();
    printf("after tid %d\n", __pthipth_gettid());
    //LOG_PRINTF("start child %d\n", __pthipth_gettid());
    return NULL;
}

int main()
{
    pthipth_mutex_init(&hate);
    int n = 4;
    pthipth_t threads[n];
    pthipth_attr_t thread_attr[n];

    for (int i = 0; i < n; i++)
    {
	pthipth_attr_t attr;
//	attr.stackSize = 1024 * 8;
	attr.priority = i % 2 + 1;
	if (i == 3 ) {
//	    sleep(1);
	    attr.priority = 1;
	}
	//thread_attr[i].stackSize = 1024 * 1024;
	//thread_attr[i].priority = i % 2;// + 1;
	pthipth_create(&threads[i], &attr, child_function, NULL);
	//pthipth_join(threads[i], NULL);
	//pthipth_create(&threads[i], &thread_attr[i], child_function, NULL);
    }

    pthipth_prio_display();
//    pthipth_avl_display();
    for (int i = 0; i < n; i++)
    {
	pthipth_join(threads[i], NULL);
    }
    pthipth_avl_display();
    pthipth_prio_display();
    sleep(2);
}
