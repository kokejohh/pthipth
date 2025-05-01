#include "pthipth_internal.h"
#include "pthipth_prio.h"

int pthipth_set_prio(int new_priority)
{
    pthipth_private_t *self = __pthipth_selfptr();
    
    if (self == NULL) return -1;

    self->priority = self->old_priority = new_priority;

    pthipth_prio_reinsert(self);

    return 0;
}
