#include "pthipth_cond.h"
#include "pthipth_mutex.h"
#include "pthipth_queue.h"

extern void change_to_state(pthipth_private_t *node, enum pthipth_state state);

extern pthipth_queue_t blocked_state;

int pthipth_cond_init(pthipth_cond_t *cond)
{
    if (cond == NULL) return -1;

    (void)cond;
    return 0;
}

int pthipth_cond_signal(pthipth_cond_t *cond)
{
    if (cond == NULL) return -1;

    pthipth_private_t *tmp = blocked_state.head;
    pthipth_private_t *selected = NULL;

    while (tmp)
    {
	pthipth_private_t *next_tmp = tmp->next;

	if (tmp->state == BLOCKED && tmp->current_cond == cond &&
		(selected == NULL || tmp->priority < selected->priority))
	    selected = tmp;
	tmp = next_tmp;
    }
    if (selected) change_to_state(selected, READY);

    return 0;
}

int pthipth_cond_broadcast(pthipth_cond_t *cond)
{
    if (cond == NULL) return -1;

    pthipth_private_t *tmp = blocked_state.head;
    while (tmp)
    {
	pthipth_private_t *next_tmp = tmp->next;

	if (tmp->state == BLOCKED && tmp->current_cond == cond)
	    change_to_state(tmp, READY);
	tmp = next_tmp;
    }

    return 0;
}

int pthipth_cond_wait(pthipth_cond_t *cond, pthipth_mutex_t *mutex)
{
    if (cond == NULL || mutex == NULL) return -1;

    pthipth_private_t *self = __pthipth_selfptr();

    pthipth_mutex_unlock(mutex);

    self->current_cond = cond;
    change_to_state(self, BLOCKED);
    pthipth_yield();

    pthipth_mutex_lock(mutex);

    return 0;
}
