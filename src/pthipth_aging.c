#include <stdint.h>

#include "pthipth.h"
#include "pthipth_types.h"
#include "pthipth_prio.h"

extern pthipth_private_t *pthipth_prio_head;

void __pthipth_aging(int aging_factor)
{
    pthipth_private_t *tmp = pthipth_prio_head;

    uint64_t current_time = __pthipth_gettime_ms();

    while (tmp)
    {
	if (tmp->state == RUNNING) continue;
	// use old_priority because current priority may be temporarily promoted (priority inheritance)
	int priority = tmp->old_priority;
	if (priority == MAIN_PRIORITY || priority == IDLE_PRIORITY) break;

	uint64_t waiting_time = current_time - tmp->last_selected;
	// calcuate priority
	priority -= (waiting_time / 1000) * aging_factor;
	if (priority < 1) priority = 1;

	pthipth_private_t *next_tmp = (tmp->inside_next == tmp) ? tmp->next : tmp->inside_next;

	// set last_selected to current time after calculation is complete
	tmp->last_selected = current_time;
	// update only old_priority because current priority may be temporarily promoted
	if (priority < tmp->old_priority)
	    tmp->old_priority = priority;
	// will update current_priority only if new priority greater than current priority
	if (priority < tmp->priority)
	{
	    tmp->priority = priority;
	    pthipth_prio_reinsert(tmp);
	}
	tmp = next_tmp;
    }
}
