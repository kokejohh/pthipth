#include <errno.h>
#include <stdio.h>

#include "pthipth.h"
#include "pthipth_avl.h"
#include "pthipth_mutex.h"
#include "pthipth_prio.h"
#include "pthipth_queue.h"

extern void change_to_state(pthipth_private_t *node, enum pthipth_state state);

extern pthipth_queue_t blocked_state;

pthipth_t init_owner_tid = 0;

int pthipth_mutex_init(pthipth_mutex_t *mutex)
{
    if (mutex == NULL) return -1;

    futex_init(&mutex->futx, 1);

    mutex->owner_tid = init_owner_tid;
    return 0;
}
    
int pthipth_mutex_lock(pthipth_mutex_t *mutex)
{
    if (mutex == NULL) return -1;
    // If have mutex owner_tid
    pthipth_private_t *self = __pthipth_selfptr();
    if (mutex->owner_tid == self->tid) return 0;
    else if (mutex->owner_tid != 0)
    {
	pthipth_private_t *owner_tid = pthipth_avl_search(mutex->owner_tid);

	if (self->priority < owner_tid->priority)
	    owner_tid->priority = self->priority;

	if (owner_tid->state == READY)
	    pthipth_prio_reinsert(owner_tid);
    }

    while (__futex_down(&mutex->futx.count) != 0)
    {
	self->current_mutex = mutex;

	change_to_state(self, BLOCKED);

	pthipth_yield();
    }

    mutex->owner_tid = pthipth_self();
    return 0;
}

int pthipth_mutex_trylock(pthipth_mutex_t *mutex)
{
    if (mutex == NULL) return -1;
    else if (mutex->owner_tid) return -EBUSY;
    return pthipth_mutex_lock(mutex);
}

int pthipth_mutex_unlock(pthipth_mutex_t *mutex)
{
    if (mutex == NULL) return -1;
    else if (mutex->owner_tid == 0) return 0;
    else if (mutex->owner_tid != __pthipth_gettid())
    {
	printf("mutex unlock error: not owner_tid unlock!\n");
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

    pthipth_private_t *owner_tid = pthipth_avl_search(mutex->owner_tid);

    owner_tid->priority = owner_tid->old_priority;
    owner_tid->current_mutex = NULL;

    pthipth_mutex_init(mutex);

    return 0;
}
