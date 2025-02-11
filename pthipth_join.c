//#include "pthipth_bst.h"
#include "pthipth_avl.h"

int pthipth_join(pthipth_t target_thread, void **status)
{
    pthipth_private_t *target, *self_ptr;

    //target = pthipth_bst_search(target_thread.tid);
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

    pthipth_yield();

    if (status == NULL) return 0;
    *status = target->return_value;

    return 0;
}
