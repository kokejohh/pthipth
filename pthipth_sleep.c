#include "pthipth.h"
#include "pthipth_prio.h"
#include "pthipth_q.h"

extern time_t gettime_ms();

extern pthipth_private_t *pthipth_q_head;

void pthipth_sleep(time_t millisec)
{
    pthipth_private_t *self = __pthipth_selfptr();

    self->state = SLEEPING;
    self->wake_time = gettime_ms() + millisec;

    //printf("set sleep %ld + %ld = %ld\n", millisec, self->wake_time - millisec, self->wake_time);

    pthipth_prio_delete(self);
    pthipth_q_add(self);

    pthipth_yield();
}

void check_sleeping()
{
    pthipth_private_t *tmp = pthipth_q_head;

    if (tmp == NULL) return;

    time_t current_time = gettime_ms();
    do
    {
	pthipth_private_t *next_tmp = tmp->next;
	if (tmp->state == SLEEPING && current_time >= tmp->wake_time)
	{
	    //printf("tid %d tmp->state %d\n", tmp->tid, tmp->state);
	    //printf("tmp->time %ld and %ld\n", tmp->wake_time, current_time);
	    tmp->state = READY;
	    tmp->last_selected = current_time;

	    pthipth_q_delete(tmp);
	    pthipth_prio_insert(tmp);
	}
	tmp = next_tmp;
	if (pthipth_q_head == NULL) break;
    }
    while (tmp != pthipth_q_head);
}
