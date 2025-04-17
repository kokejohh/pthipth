#include "pthipth.h"
#include "pthipth_avl.h"
#include "pthipth_signal.h"

// pthipth_join:
// returns:
// 0 - success
// -1 - error (already join)
int pthipth_join(pthipth_t target_thread, void **status)
{
    __PTHIPTH_SIGNAL_BLOCK();

    pthipth_private_t *target = pthipth_avl_search(target_thread);
    if (target == NULL) return -1;

    if (target->state == DEFUNCT)
    {
	if (status == NULL) return 0;
	*status = target->return_value;
	return 0;
    }
    if (target->blockedForJoin != NULL) return -1;

    pthipth_private_t *self = __pthipth_selfptr();

    target->blockedForJoin = self;

    __pthipth_change_to_state(self, BLOCKED);

    pthipth_yield();

    if (status == NULL) return 0;
    *status = target->return_value;

    __PTHIPTH_SIGNAL_UNBLOCK();

    return 0;
}
