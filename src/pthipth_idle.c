#include "pthipth.h"
#include "pthipth_queue.h"
#include "pthipth_signal.h"

extern pthipth_queue_t blocked_state;
extern pthipth_queue_t sleeping_state;

void *__pthipth_idle(void *phony)
{
    (void)phony;

    while (1) pthipth_yield();

    return NULL;
}
