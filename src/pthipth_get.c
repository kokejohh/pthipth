#include "pthipth_internal.h"
#include "pthipth_signal.h"

extern futex_t global_futex;

int pthipth_get_prio()
{
    pthipth_private_t *self = __pthipth_selfptr();

    return (self && self->priority) ? self->priority : -1;
}

uint64_t pthipth_get_stack_size()
{
    pthipth_private_t *self = __pthipth_selfptr();

    return (self && self->priority) ? self->stack_size : -1;
}

int pthipth_get_time_quota()
{
    pthipth_private_t *self = __pthipth_selfptr();

    return (self && self->priority) ? self->time_quota : -1;
}

int pthipth_get_aging_factor()
{
    pthipth_private_t *self = __pthipth_selfptr();

    return (self && self->aging_factor) ? self->aging_factor : -1;
}

int pthipth_get_aging_time()
{
    pthipth_private_t *self = __pthipth_selfptr();

    return (self && self->aging_factor) ? self->aging_time : -1;
}
