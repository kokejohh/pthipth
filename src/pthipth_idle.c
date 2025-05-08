#include "pthipth.h"
#include "pthipth_queue.h"
#include "pthipth_signal.h"

void *__pthipth_idle(void *phony)
{
    (void)phony;

    while (1) pthipth_yield();

    return NULL;
}
