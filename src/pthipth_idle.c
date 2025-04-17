#include "pthipth.h"
#include "pthipth_queue.h"
#include "pthipth_signal.h"

extern pthipth_queue_t blocked_state;
extern pthipth_queue_t sleeping_state;

void *__pthipth_idle(void *phony)
{
    while (1)
    {
	__PTHIPTH_SIGNAL_BLOCK();

	if (blocked_state.head == NULL && sleeping_state.head == NULL)
	    break;
	pthipth_yield();
    }

    __PTHIPTH_SIGNAL_UNBLOCK();

    return NULL;
}
