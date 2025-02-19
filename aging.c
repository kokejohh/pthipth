#include "pthipth.h"
#include "pthipth_prio.h"

extern pthipth_private_t *pthipth_prio_head;

extern time_t gettime_ms();

void pthipth_aging(int aging_factor)
{
    pthipth_private_t *head_on_prio = pthipth_prio_head;
    pthipth_private_t *tmp = pthipth_prio_head;

    time_t current_time = gettime_ms();
    while (tmp)
    {
	int priority = tmp->old_priority;
	if (priority == LOWEST_PRIORITY || priority == IDLE_PRIORITY) break;

	time_t waiting_time = current_time - tmp->last_selected;
	priority -= (waiting_time / 1000) * aging_factor;

	if (priority < 1) priority = 1;

	//printf("koke %d , calling tid %d, current tid %d waiting_time %ld\n", priority, __pthipth_gettid(), tmp->tid, waiting_time);

	if (tmp->next) head_on_prio = tmp;

	pthipth_private_t *next_tmp = (tmp->inside_next == head_on_prio) ?
					head_on_prio->next : tmp->inside_next;

	if (priority < tmp->old_priority)
	    tmp->old_priority = priority;
	if (priority < tmp->priority)
	{
	    tmp->priority = priority;
	    tmp->last_selected = gettime_ms();
	    pthipth_prio_reinsert(tmp);
	}
	tmp = next_tmp;
    }
}
