#include <stdio.h>
#include "pthipth.h"
#include "pthipth_pool.h"
#include "pthipth_queue.h"
#include "pthipth_prio.h"
#include "pthipth_types.h"

pthipth_pool_t pool1;
void *child_function2(void *arg)
{
    printf("new child_function %d\n", pthipth_self());
  //  pthipth_yield();

    return NULL;
}
void *child_function(void *arg)
{
    printf("child function %d\n", pthipth_self());
//    pthipth_yield();
    printf("mid child function %d\n", pthipth_self());
 //   pthipth_yield();
    pthipth_pool_add(&pool1, &(pthipth_task_t){.arg=NULL,.function=child_function2,.priority=DEFAULT_PRIORITY - 1});
    pthipth_pool_add(&pool1, &(pthipth_task_t){.arg=NULL,.function=child_function2,.priority=DEFAULT_PRIORITY - 1});
    pthipth_pool_add(&pool1, &(pthipth_task_t){.arg=NULL,.function=child_function2,.priority=DEFAULT_PRIORITY - 1});
    pthipth_pool_add(&pool1, &(pthipth_task_t){.arg=NULL,.function=child_function2,.priority=DEFAULT_PRIORITY - 1});
    pthipth_pool_add(&pool1, &(pthipth_task_t){.arg=NULL,.function=child_function2,.priority=DEFAULT_PRIORITY - 1});
    printf("end child function %d\n", pthipth_self());
    return NULL;
}
int main()
{
    /*
    pthipth_private_t t1, t2, t3;

    t1.tid = 1;
    t2.tid = 2;
    t3.tid = 3;
    t1.priority = 31;
    t2.priority = 28;
    t3.priority = 28;
    pthipth_prio_insert(&t1);
    pthipth_prio_insert(&t2);
    pthipth_prio_insert(&t3);

    pthipth_prio_delete(&t1);
    pthipth_private_t *tmp = pthipth_prio_extract();

    printf("tmp %d\n", tmp->tid);

    pthipth_prio_display();
    */

    int n = 5;
    pthipth_pool_create(&pool1, NULL, 10, 10);

    pthipth_pool_add(&pool1, &(pthipth_task_t){.arg=NULL,.function=child_function,.priority=DEFAULT_PRIORITY});
//    pthipth_pool_add(&pool1, &(pthipth_task_t){.arg=NULL,.function=child_function,.priority=DEFAULT_PRIORITY});

    pthipth_pool_destroy(&pool1);


    /*
    int n = 5;
    pthipth_t threads[n];
    for (int i = 0; i < n; i++)
    {
	pthipth_create(&threads[i], NULL, &(pthipth_task_t){ .arg = NULL, .function = child_function, .priority = (n - i) % 2 + 1DEFAULT_PRIORITY });
    }

    for (int i = 0; i < n; i++)
    {
	pthipth_join(threads[i], NULL);
    }
    */
    pthipth_avl_display();
    printf("main end\n");

    return 0;
}
