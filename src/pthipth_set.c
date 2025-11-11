#include "pthipth_internal.h"
#include "pthipth_prio.h"

extern futex_t global_futex;

int pthipth_set_prio(int new_priority)
{
    pthipth_private_t *self = __pthipth_selfptr();
    
    if (self == NULL) return -1;

    futex_down(&global_futex);

    self->priority = self->old_priority = new_priority;

    pthipth_prio_reinsert(self);

    futex_up(&global_futex);

    return new_priority;
}
