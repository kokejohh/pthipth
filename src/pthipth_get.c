#include "pthipth_internal.h"

int pthipth_get_prio()
{
    pthipth_private_t *self = __pthipth_selfptr();

    if (self == NULL) return -1;

    return self->priority;
}

uint64_t pthipth_get_stack_size()
{
    pthipth_private_t *self = __pthipth_selfptr();

    if (self == NULL) return -1;

    return self->stack_size;
}

int pthipth_get_time_quota()
{
    pthipth_private_t *self = __pthipth_selfptr();

    if (self == NULL) return -1;

    return self->time_quota;
}

int pthipth_get_aging_factor()
{
    pthipth_private_t *self = __pthipth_selfptr();

    if (self == NULL) return -1;

    return self->aging_factor;
}

int pthipth_get_aging_time()
{
    pthipth_private_t *self = __pthipth_selfptr();

    if (self == NULL) return -1;

    return self->aging_time;
}
