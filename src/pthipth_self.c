#include "pthipth.h"
#include "pthipth_internal.h"
#include "pthipth_avl.h"

extern __thread pthipth_private_t *cur_pthipth;

pthipth_t pthipth_self()
{
    return __pthipth_gettid();
}

pthipth_private_t *__pthipth_selfptr()
{
    return cur_pthipth ? cur_pthipth : pthipth_avl_search(__pthipth_gettid());
}
