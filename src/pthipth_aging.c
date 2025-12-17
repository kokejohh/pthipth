#include "pthipth.h"
#include "pthipth_internal.h"
#include "pthipth_bq.h"

extern pthipth_private_t *pthipth_bq_table[IDLE_PRIORITY + 1];
extern pthipth_private_t *pthipth_bq_head;

void __pthipth_aging()
{
    pthipth_private_t *tmp = pthipth_bq_head;

    if (tmp == NULL) return;

    uint64_t current_time = __pthipth_gettime_ms();

    for (int i = tmp->cur_priority; i <= IDLE_PRIORITY; i++)
    {
	pthipth_private_t *head = pthipth_bq_table[i];
	pthipth_private_t *cur = head;
	if (cur == NULL ||
		cur->init_priority == MAIN_PRIORITY ||
		cur->init_priority == IDLE_PRIORITY)
	    continue;
	do
	{
	    pthipth_private_t *cur_next = cur->next;

	    if (cur->state == RUNNING || cur->aging_factor == 0)
	    {
		cur = cur_next;
		if (cur == head) break;
		continue;
	    }

	    int priority = cur->old_priority;

	    uint64_t waiting_time = current_time - cur->last_selected;
	    priority -= (waiting_time / cur->aging_time) * cur->aging_factor;
	    if (priority < HIGHEST_PRIORITY) priority = HIGHEST_PRIORITY;

	    // update only old_priority because current priority may be temporarily promoted
	    if (priority < cur->old_priority)
	    {
		cur->old_priority = priority;
		// update last_selected only if can aging
		cur->last_selected = current_time;
	    }

	    // will update current_priority only if new priority greater than current priority
	    if (priority < cur->priority)
	    {
		pthipth_private_t *old_head = head;
		if (cur == head) head = cur->next;

		cur->priority = priority;
		pthipth_bq_reinsert(cur);

		if (old_head != head)
		{
		    cur = cur_next;
		    continue;
		}
	    }
	    if (cur == head) break;
	} while (cur != head);
    }
}
