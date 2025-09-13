#include "pthipth_internal.h"
#include "pthipth_signal.h"

extern __thread pthipth_private_t *cur_pthipth;

extern futex_t global_futex;

int pthipth_get_prio()
{
    return cur_pthipth ? cur_pthipth->priority : - 1;
}

uint64_t pthipth_get_stack_size()
{
    return cur_pthipth ? cur_pthipth->stack_size : 0;
}

int pthipth_get_time_quota()
{
    return cur_pthipth ? cur_pthipth->time_quota : -1;
}

int pthipth_get_aging_factor()
{
    return cur_pthipth ? cur_pthipth->aging_factor : -1;
}

int pthipth_get_aging_time()
{
    return cur_pthipth ? cur_pthipth->aging_time : -1;
}
