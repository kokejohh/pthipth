#include <stdint.h>

#include "pthipth.h"
#include "pthipth_prio.h"
#include "pthipth_queue.h"
#include "pthipth_avl.h"

extern pthipth_queue_t sleeping_state;

void pthipth_sleep(uint64_t millisec)
{
    pthipth_private_t *self = __pthipth_selfptr();

    self->wake_time = __pthipth_gettime_ms() + millisec;

    __pthipth_change_to_state(self, SLEEPING);

    pthipth_yield();
}

void __pthipth_check_sleeping()
{
    pthipth_private_t *tmp = sleeping_state.head;

    if (tmp == NULL) return;

    uint64_t current_time = __pthipth_gettime_ms();

    while (tmp)
    {
	pthipth_private_t *next_tmp = tmp->next;
	if (tmp->state == SLEEPING && current_time >= tmp->wake_time)
	{
	    tmp->last_selected = current_time;
	    __pthipth_change_to_state(tmp, READY);
	}
	tmp = next_tmp;
    }
}
