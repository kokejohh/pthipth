#include "pthipth.h"

int __pthipth_wrapper(void *thread_tcb)
{
    pthipth_private_t *new_tcb = (pthipth_private_t *)thread_tcb;

    futex_down(&new_tcb->sched_futex);

    if (new_tcb->start_func != NULL)
	new_tcb->start_func(new_tcb->arg);

    pthipth_exit(NULL);

    return 0;
}

