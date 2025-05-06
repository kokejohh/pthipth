#include "pthipth.h"
#include "pthipth_internal.h"
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
    pthipth_private_t *self = __pthipth_selfptr();

    if (target == NULL || target->blockedForJoin ||
	    target->tid == self->tid)
    {
	__PTHIPTH_SIGNAL_UNBLOCK();
	return -1;
    }

    if (target->state == DEFUNCT)
    {
	if (status == NULL)
	{
	    __pthipth_free(target);
	    __PTHIPTH_SIGNAL_UNBLOCK();
	    return 0;
	}
	*status = target->return_value;
	__pthipth_free(target);
	__PTHIPTH_SIGNAL_UNBLOCK();
	return 0;
    }

    target->blockedForJoin = self;

    __pthipth_change_to_state(self, BLOCKED);

    pthipth_yield();

    __PTHIPTH_SIGNAL_BLOCK();

    if (status == NULL)
    {
	__pthipth_free(target);
	__PTHIPTH_SIGNAL_UNBLOCK();
	return 0;
    }

    *status = target->return_value;
    __pthipth_free(target);

    __PTHIPTH_SIGNAL_UNBLOCK();

    return 0;
}
