#include "pthipth.h"
#include "pthipth_internal.h"

int pthipth_getprio()
{
    pthipth_private_t *self = __pthipth_selfptr();

    if (self == NULL) return -1;

    return self->priority;
}
