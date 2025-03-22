#include "pthipth.h"
#include "pthipth_prio.h"
#include "pthipth_queue.h"
#include "pthipth_avl.h"

extern time_t gettime_ms();

extern void change_to_state(pthipth_private_t *node, enum pthipth_state state);

extern pthipth_queue_t sleeping_state;

void pthipth_sleep(time_t millisec)
{
    pthipth_private_t *self = __pthipth_selfptr();

    self->wake_time = gettime_ms() + millisec;

    //printf("set sleep %ld + %ld = %ld\n", millisec, self->wake_time - millisec, self->wake_time);
    change_to_state(self, SLEEPING);

    pthipth_yield();
}

void check_sleeping()
{
    pthipth_private_t *tmp = sleeping_state.head;

    if (tmp == NULL) return;

    time_t current_time = gettime_ms();

    while (tmp)
    {
	pthipth_private_t *next_tmp = tmp->next;
	if (tmp->state == SLEEPING && current_time >= tmp->wake_time)
	{
	    tmp->last_selected = current_time;
	    change_to_state(tmp, READY);
	}
	tmp = next_tmp;
    }
}
