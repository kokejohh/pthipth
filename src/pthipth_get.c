#include "pthipth_internal.h"

int pthipth_get_prio()
{
    pthipth_private_t *self = __pthipth_selfptr();

    if (self == NULL) return -1;

    return self->priority;
}
