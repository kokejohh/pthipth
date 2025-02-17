#include "pthipth.h"
#include "pthipth_prio.h"
#include "pthipth_q.h"

extern time_t gettime_ms();

extern pthipth_private_t *pthipth_q_head;

void pthipth_sleep(time_t millisec)
{
    pthipth_private_t *self = __pthipth_selfptr();

    printf("sleep I love you %d\n", self->tid);
    self->state = SLEEPING;
    self->wake_time = gettime_ms() + millisec;

    pthipth_prio_delete(self);
    
    pthipth_q_add(self);

    pthipth_yield();
}

void check_sleepting()
{
    pthipth_private_t *tmp = pthipth_q_head;

    while (tmp != tmp->next)
    {
	pthipth_private_t *tmp2 = tmp->next;
	if (tmp->state == SLEEPING && gettime_ms() >= tmp->wake_time)
	{
	    tmp->state = READY;
	    pthipth_q_delete(tmp);
	    pthipth_prio_insert(tmp);
	}
	tmp = tmp2;
    }
}
