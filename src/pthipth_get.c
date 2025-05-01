#include "pthipth_internal.h"
#include "pthipth_signal.h"

int pthipth_get_prio()
{
    __PTHIPTH_SIGNAL_BLOCK();

    pthipth_private_t *self = __pthipth_selfptr();

    if (self == NULL) return -1;

    __PTHIPTH_SIGNAL_UNBLOCK();

    return self->priority;
}

uint64_t pthipth_get_stack_size()
{
    pthipth_private_t *self = __pthipth_selfptr();

    if (self == NULL) return -1;

    __PTHIPTH_SIGNAL_UNBLOCK();

    return self->stack_size;
}

int pthipth_get_time_quota()
{
    __PTHIPTH_SIGNAL_BLOCK();

    pthipth_private_t *self = __pthipth_selfptr();

    if (self == NULL) return -1;

    __PTHIPTH_SIGNAL_UNBLOCK();

    return self->time_quota;
}

int pthipth_get_aging_factor()
{
    __PTHIPTH_SIGNAL_BLOCK();

    pthipth_private_t *self = __pthipth_selfptr();

    if (self == NULL) return -1;

    __PTHIPTH_SIGNAL_UNBLOCK();

    return self->aging_factor;
}

int pthipth_get_aging_time()
{
    __PTHIPTH_SIGNAL_BLOCK();

    pthipth_private_t *self = __pthipth_selfptr();

    if (self == NULL) return -1;

    __PTHIPTH_SIGNAL_UNBLOCK();

    return self->aging_time;
}
