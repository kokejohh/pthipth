#include "pthipth.h"
#include "pthipth_avl.h"

// pthipth_join:
// returns:
// 0 - success
// -1 - error (already join)
int pthipth_join(pthipth_t target_thread, void **status)
{
    pthipth_private_t *target,*self;

    target = pthipth_avl_search(target_thread);
    self = __pthipth_selfptr();

    if (target->state == DEFUNCT)
    {
	if (status == NULL) return 0;
	*status = target->return_value;
	return 0;
    }
    if (target->blockedForJoin != NULL) return -1;

    target->blockedForJoin = self;

    __pthipth_change_to_state(self, BLOCKED);

    pthipth_yield();

    if (status == NULL) return 0;
    *status = target->return_value;

    return 0;
}
