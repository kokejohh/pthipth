#include "pthipth_cond.h"

void pthipth_cond_init(pthipth_cond_t *cond)
{
    *cond = 1;
}

int pthipth_cond_signal(pthipth_cond_t *cond)
{
    return 0;
}

int pthipth_cond_broadcast(pthipth_cond_t *cond)
{
    return 0;
}

int pthipth_cond_wait(pthipth_cond_t *cond, pthipth_mutex_t * mutex)
{
    return 0;
}
