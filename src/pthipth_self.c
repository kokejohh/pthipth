#include "pthipth.h"
#include "pthipth_avl.h"

pthipth_t pthipth_self()
{
    return __pthipth_gettid();
}

pthipth_private_t *__pthipth_selfptr()
{
    return pthipth_avl_search(__pthipth_gettid());
}
