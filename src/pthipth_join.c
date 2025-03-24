#include "pthipth_avl.h"

extern void change_to_state(pthipth_private_t *node, enum pthipth_state state);

int pthipth_join(pthipth_t target_thread, void **status)
{
    pthipth_private_t *target, *self_ptr;

    target = pthipth_avl_search(target_thread);
    self_ptr = __pthipth_selfptr();

    if (target->state == DEFUNCT)
    {
	if (status == NULL) return 0;
	*status = target->return_value;
	return 0;
    }
    if (target->blockedForJoin != NULL) return -1;

    target->blockedForJoin = self_ptr;

    change_to_state(self_ptr, BLOCKED);

    pthipth_yield();

    if (status == NULL) return 0;
    *status = target->return_value;

    return 0;
}
