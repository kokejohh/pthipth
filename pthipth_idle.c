#include <unistd.h>
#include "pthipth.h"
#include "pthipth_debug.h"

// The thread checks whether it is the only one alive,
// if yes, exit() else keep scheduling someone.
void *pthipth_idle(void *phony)
{
    pthipth_private_t *traverse_tcb;
    pid_t idle_tcb_tid;

    while (1)
    {
//	LOG_PRINTF("idle\n");
	traverse_tcb = __pthipth_selfptr();
	idle_tcb_tid = traverse_tcb->tid;
	traverse_tcb = traverse_tcb->next;

	while (traverse_tcb->tid != idle_tcb_tid)
	{
	    if (traverse_tcb->state != DEFUNCT) break;

	    traverse_tcb = traverse_tcb->next;
	}

	if (traverse_tcb->tid == idle_tcb_tid) exit(0);

	pthipth_yield();
    }
}
