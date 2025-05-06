#include "pthipth.h" 
#include "pthipth_avl.h"
#include "pthipth_internal.h"
#include "pthipth_signal.h"
#include "pthipth_queue.h"

pthipth_queue_t defunct_state;

void __pthipth_check_detach()
{
    pthipth_private_t *tmp = defunct_state.head;

    if (tmp == NULL) return;

    while (tmp)
    {
	pthipth_private_t *next_tmp = tmp->next;
	
	pthipth_queue_delete(&defunct_state, tmp);
	__pthipth_free(tmp);

	tmp = next_tmp;
    }
}

int pthipth_detach(pthipth_t target_thread)
{
    __PTHIPTH_SIGNAL_BLOCK();

    pthipth_private_t *target = pthipth_avl_search(target_thread);

    if (target == NULL || target->is_detach || target->blockedForJoin)
    {
	__PTHIPTH_SIGNAL_UNBLOCK();
	return -1;
    }

    if (target->state == DEFUNCT)
    {
	__pthipth_free(target);
	__PTHIPTH_SIGNAL_UNBLOCK();
	return 0;
    }

    target->is_detach = 1;

    __PTHIPTH_SIGNAL_UNBLOCK();

    pthipth_yield();

    return 0;
}
