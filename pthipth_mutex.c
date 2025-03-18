#include <errno.h>
#include <stdio.h>

#include "pthipth_avl.h"
#include "pthipth_mutex.h"
#include "pthipth_prio.h"
#include "pthipth_queue.h"

extern void change_to_state(pthipth_private_t *node, int state);

extern pthipth_queue_t blocked_state;

pthipth_t init_owner = {
    .tid = 0
};

int pthipth_mutex_init(pthipth_mutex_t *mutex)
{
    futex_init(&mutex->futx, 1);

    mutex->owner = init_owner;
    return 0;
}
    
int pthipth_mutex_lock(pthipth_mutex_t *mutex)
{
    // If have mutex owner
    pthipth_private_t *self = __pthipth_selfptr();
    if (mutex->owner.tid == self->tid) return 0;
    else if (mutex->owner.tid != 0)
    {
	pthipth_private_t *owner = pthipth_avl_search(mutex->owner.tid);

	if (self->priority < owner->priority)
	    owner->priority = self->priority;

	if (owner->state == READY)
	    pthipth_prio_reinsert(owner);
    }

    while (__futex_down(&mutex->futx.count) != 0)
    {
	self->current_mutex = mutex;

	change_to_state(self, BLOCKED);

	pthipth_yield();
    }

    mutex->owner = pthipth_self();
    return 0;
}

int pthipth_mutex_trylock(pthipth_mutex_t *mutex)
{
    if (mutex->owner.tid) return EBUSY;
    return pthipth_mutex_lock(mutex);
}

int pthipth_mutex_unlock(pthipth_mutex_t *mutex)
{
    if (mutex->owner.tid == 0) return 0;
    else if (mutex->owner.tid != __pthipth_gettid())
    {
	printf("mutex unlock error: not owner unlock!\n");
	return -1;
    }

    pthipth_private_t *tmp = blocked_state.head;
    pthipth_private_t *selected = NULL;

    while (tmp)
    {
	pthipth_private_t *next_tmp = tmp->next;
	if (tmp->state == BLOCKED && tmp->current_mutex == mutex &&
		(selected == NULL || tmp->priority < selected->priority))
	    selected = tmp;
	tmp = next_tmp;
    }
    if (selected) change_to_state(selected, READY);

    pthipth_private_t *owner = pthipth_avl_search(mutex->owner.tid);

    owner->priority = owner->old_priority;
    owner->current_mutex = NULL;

    pthipth_mutex_init(mutex);

    return 0;
}
