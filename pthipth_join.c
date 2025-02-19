#include "pthipth_avl.h"
#include "pthipth_prio.h"
#include "pthipth_q.h"

int pthipth_join(pthipth_t target_thread, void **status)
{
    pthipth_private_t *target, *self_ptr;

    target = pthipth_avl_search(target_thread.tid);
    self_ptr = __pthipth_selfptr();

    if (target->state == DEFUNCT)
    {
	if (status == NULL) return 0;
	*status = target->return_value;
	return 0;
    }
    if (target->blockedForJoin != NULL) return -1;

    target->blockedForJoin = self_ptr;

    self_ptr->state = BLOCKED;

    pthipth_prio_delete(self_ptr);
    pthipth_q_add(self_ptr);

    pthipth_yield();

    if (status == NULL) return 0;
    *status = target->return_value;

    return 0;
}
