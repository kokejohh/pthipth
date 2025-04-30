#include <stdlib.h>
#include <sys/mman.h>

#include "pthipth.h"
#include "pthipth_internal.h"
#include "pthipth_avl.h"
#include "pthipth_signal.h"

static void __pthipth_free(pthipth_private_t *thread)
{
    pthipth_avl_delete(thread);
    uint64_t stack_size = thread->stack_size;
    char *child_stack = thread->child_stack;
    free(thread);
    munmap(child_stack - stack_size, stack_size);
}

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

	if (status == NULL)
	{
	    __pthipth_free(target);
	    return 0;
	}
	*status = target->return_value;
	__pthipth_free(target);
	return 0;
    }
    if (target->blockedForJoin != NULL) return -1;

    pthipth_private_t *self = __pthipth_selfptr();

    target->blockedForJoin = self;

    __pthipth_change_to_state(self, BLOCKED);

    pthipth_yield();

    if (status == NULL)
    {
	__pthipth_free(target);
	return 0;
    }

    *status = target->return_value;
    __pthipth_free(target);

    __PTHIPTH_SIGNAL_UNBLOCK();

    return 0;
}
