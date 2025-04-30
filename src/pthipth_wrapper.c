#include "pthipth.h"
#include "pthipth_internal.h"
#include "pthipth_signal.h"

int __pthipth_wrapper(void *thread_tcb)
{
    pthipth_private_t *new_tcb = (pthipth_private_t *)thread_tcb;

    futex_down(&new_tcb->sched_futex);

    __pthipth_set_thread_time_quota(new_tcb->time_quota);

    __PTHIPTH_SIGNAL_UNBLOCK();

    void *return_val = NULL;

    if (new_tcb->start_func != NULL)
	return_val = new_tcb->start_func(new_tcb->arg);

    pthipth_exit(return_val);

    return 0;
}

