#include "pthipth.h"
#include "pthipth_queue.h"

extern pthipth_queue_t blocked_state;
extern pthipth_queue_t sleeping_state;

void *__pthipth_idle(void *phony)
{
    while (1)
    {
	if (blocked_state.head == NULL && sleeping_state.head == NULL)
	    break;
	pthipth_yield();
    }
    return NULL;
}
