#include <stdlib.h>
#include <errno.h>

#include "pthipth.h"
#include "pthipth_internal.h"
#include "pthipth_avl.h"
#include "pthipth_prio.h"
#include "pthipth_queue.h"
#include "pthipth_signal.h"

extern pthipth_queue_t blocked_state;

pthipth_t init_owner_tid = 0;

// pthipth_mutex_init
// returns:
// 0 - success
// -1 - error
int pthipth_mutex_init(pthipth_mutex_t *mutex)
{
    if (mutex == NULL) return -1;

    __PTHIPTH_SIGNAL_BLOCK();

    mutex->futx = malloc(sizeof(futex_t));
    if (mutex->futx == NULL)
    {
	__PTHIPTH_SIGNAL_UNBLOCK();
	return -1;
    }

    futex_init(mutex->futx, 1);

    mutex->owner_tid = init_owner_tid;

    __PTHIPTH_SIGNAL_UNBLOCK();

    return 0;
}
    
// pthipth_mutex_lock
// returns:
// 0 - success
// -1 - error
int pthipth_mutex_lock(pthipth_mutex_t *mutex)
{
    if (mutex == NULL) return -1;

    __PTHIPTH_SIGNAL_BLOCK();

    pthipth_private_t *self = __pthipth_selfptr();

    if (self == NULL) return -1;

    // current thread already owns the mutex
    if (mutex->owner_tid == self->tid) return -1;
    // another thread owns the mutex: priority inheritance
    else if (mutex->owner_tid != 0)
    {
	pthipth_private_t *owner_tid = pthipth_avl_search(mutex->owner_tid);

	// donate priority to prevent priority inversion
	if (self->priority < owner_tid->priority)
	    owner_tid->priority = self->priority;

	// update priority in bucket
	if (owner_tid->state == READY)
	    pthipth_prio_reinsert(owner_tid);
    }

    while (__futex_down(&mutex->futx->count) != 0)
    {
	self->current_mutex = mutex;

	__pthipth_change_to_state(self, BLOCKED);

	pthipth_yield();

	__PTHIPTH_SIGNAL_BLOCK();
    }

    mutex->owner_tid = __pthipth_gettid();

    __PTHIPTH_SIGNAL_UNBLOCK();

    return 0;
}

// pthipth_mutex_trylock
// returns:
// 0 - can lock
// EBUSY - already lock
// -1 - error
int pthipth_mutex_trylock(pthipth_mutex_t *mutex)
{
    if (mutex == NULL) return -1;
    else if (mutex->owner_tid) return EBUSY;
    return pthipth_mutex_lock(mutex);
}

// pthipth_mutex_unlock
// returns:
// 0 - success
// -1 - error
int pthipth_mutex_unlock(pthipth_mutex_t *mutex)
{
    if (mutex == NULL) return -1;
    else if (mutex->owner_tid == 0) return 0; // not any lock
    else if (mutex->owner_tid != __pthipth_gettid()) return -1; //not owner unlock

    pthipth_private_t *tmp = blocked_state.head;
    pthipth_private_t *selected = NULL;

    __PTHIPTH_SIGNAL_BLOCK();

    while (tmp)
    {
	pthipth_private_t *next_tmp = tmp->next;
	if (tmp->state == BLOCKED && tmp->current_mutex == mutex &&
		(selected == NULL || tmp->priority < selected->priority))
	    selected = tmp;
	tmp = next_tmp;
    }
    if (selected)
    {
	selected->current_mutex = NULL;
	__pthipth_change_to_state(selected, READY);
    }

    pthipth_private_t *owner_tid = pthipth_avl_search(mutex->owner_tid);

    // reset priority after priority inheritance ends
    owner_tid->priority = owner_tid->old_priority;

    // set default mutex value
    pthipth_mutex_init(mutex);

    __PTHIPTH_SIGNAL_UNBLOCK();

    return 0;
}

int pthipth_mutex_destroy(pthipth_mutex_t *mutex)
{
    if (mutex == NULL) return -1;

    __PTHIPTH_SIGNAL_BLOCK();

    free(mutex->futx);

    mutex->futx = NULL;

    __PTHIPTH_SIGNAL_UNBLOCK();

    return 0;
}
