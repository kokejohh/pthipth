#include <stdint.h>

#include "pthipth.h"
#include "pthipth_types.h"
#include "pthipth_prio.h"

extern pthipth_private_t *pthipth_prio_head;

void __pthipth_aging()
{
    pthipth_private_t *tmp = pthipth_prio_head;

    if (tmp == NULL) return;

    uint64_t current_time = __pthipth_gettime_ms();

    while (tmp)
    {
	pthipth_private_t *head = tmp;
	pthipth_private_t *inside_tmp = head;
	pthipth_private_t *next_tmp = head->next;

	do
	{
	    int priority = inside_tmp->old_priority;

	    if (priority == IDLE_PRIORITY || priority == MAIN_PRIORITY) break;

	    pthipth_private_t *inside_tmp_next = inside_tmp->inside_next;

	    // running state and aging factor 0 not calculate
	    if (inside_tmp->state == RUNNING || inside_tmp->aging_factor == 0)
	    {
		inside_tmp = inside_tmp_next;
		if (inside_tmp == head) break;
		continue;
	    }

	    // calculate priority
	    uint64_t waiting_time = current_time - inside_tmp->last_selected;
	    priority -= (waiting_time / inside_tmp->aging_time) * inside_tmp->aging_factor;
	    if (priority < HIGHEST_PRIORITY) priority = HIGHEST_PRIORITY;

	    // update only old_priority because current priority may be temporarily promoted
	    if (priority < inside_tmp->old_priority)
	    {
		inside_tmp->old_priority = priority;
		// update last_selected only if can aging
		inside_tmp->last_selected = current_time;
	    }

	    // will update current_priority only if new priority greater than current priority
	    if (priority < inside_tmp->priority)
	    {
		pthipth_private_t *old_head = head;

		// change head if head deleted
		if (inside_tmp == head) head = inside_tmp->inside_next;

		inside_tmp->priority = priority;
		pthipth_prio_reinsert(inside_tmp);

		// if the head has changed, skip the check
		if (old_head != head)
		{
		    inside_tmp = inside_tmp_next;
		    continue;
		}
	    }

	    inside_tmp = inside_tmp_next;
	    if (inside_tmp == head) break;
	} while (1);
	tmp = next_tmp;
    }
}
