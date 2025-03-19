#include "pthipth_avl.h"

pthipth_t pthipth_self()
{
    pthipth_t self_tcb;

    self_tcb.tid = __pthipth_gettid();

    return self_tcb;
}

pthipth_private_t *__pthipth_selfptr()
{
    return pthipth_avl_search(__pthipth_gettid());
}
