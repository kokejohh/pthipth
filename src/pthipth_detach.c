#include <sched.h>

#include "pthipth.h" 
#include "pthipth_avl.h"
#include "pthipth_internal.h"
#include "pthipth_signal.h"
#include "pthipth_queue.h"

pthipth_queue_t defunct_state;

extern futex_t global_futex;

void __pthipth_check_detach(pthipth_private_t *node)
{
    pthipth_private_t *tmp = defunct_state.head;

    if (tmp == NULL) return;

    while (tmp)
    {
	pthipth_private_t *next_tmp = tmp->next;
	
	if (node != tmp && tmp->tid_watch == 0)
	{
	    pthipth_queue_delete(&defunct_state, tmp);
	    __pthipth_free(tmp);
	}

	tmp = next_tmp;
    }
}

int pthipth_detach(pthipth_t target_thread)
{
    __PTHIPTH_SIGNAL_BLOCK();
    futex_down(&global_futex);

    pthipth_private_t *target = pthipth_avl_search(target_thread);

    if (target == NULL || target->is_detach || target->blockedForJoin)
    {
	__PTHIPTH_SIGNAL_UNBLOCK();
	futex_up(&global_futex);
	return -1;
    }

    if (target->state == DEFUNCT)
    {
	futex_up(&global_futex);

	while (target->tid_watch != 0)
	    sched_yield();

	futex_down(&global_futex);

	__pthipth_free(target);

	__PTHIPTH_SIGNAL_UNBLOCK();
	futex_up(&global_futex);
	return 0;
    }

    target->is_detach = 1;

    __PTHIPTH_SIGNAL_UNBLOCK();
    futex_up(&global_futex);

    return 0;
}
