#include "pthipth_internal.h"
#include "pthipth_prio.h"
#include "pthipth_signal.h"

int pthipth_set_prio(int new_priority)
{
    __PTHIPTH_SIGNAL_BLOCK();

    pthipth_private_t *self = __pthipth_selfptr();
    
    if (self == NULL) return -1;

    self->priority = self->old_priority = new_priority;

    pthipth_prio_reinsert(self);

    __PTHIPTH_SIGNAL_UNBLOCK();

    return 0;
}
