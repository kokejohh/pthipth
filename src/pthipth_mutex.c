#include <stdlib.h>
#include <errno.h>

#include "pthipth.h"
#include "pthipth_internal.h"
#include "pthipth_avl.h"
#include "pthipth_prio.h"
#include "pthipth_queue.h"
#include "pthipth_signal.h"

extern pthipth_queue_t blocked_state;

extern futex_t global_futex;

pthipth_t init_owner_tid = 0;

// pthipth_mutex_init
// returns:
// 0 - success
// -1 - error
int pthipth_mutex_init(pthipth_mutex_t *mutex)
{
    if (mutex == NULL) return -1;

    mutex->futx = malloc(sizeof(futex_t));
    if (mutex->futx == NULL) return -1;

    futex_init(mutex->futx, 1);

    mutex->owner_tid = init_owner_tid;

    return 0;
}
    
// pthipth_mutex_lock
// returns:
// 0 - success
// -1 - error
int pthipth_mutex_lock(pthipth_mutex_t *mutex)
{
    if (mutex == NULL) return -1;

    pthipth_private_t *self = __pthipth_selfptr();

    if (self == NULL) return -1;

    // current thread already owns the mutex
    if (mutex->owner_tid == self->tid) return -1;
    // another thread owns the mutex: priority inheritance
    else if (mutex->owner_tid != 0)
    {
	__PTHIPTH_SIGNAL_BLOCK();
	futex_down(&global_futex);

	pthipth_private_t *owner_tid = pthipth_avl_search(mutex->owner_tid);

	// donate priority to prevent priority inversion
	if (self->priority < owner_tid->priority)
	    owner_tid->priority = self->priority;

	// update priority in bucket
	if (owner_tid->state == READY)
	    pthipth_prio_reinsert(owner_tid);

	futex_up(&global_futex);
	__PTHIPTH_SIGNAL_UNBLOCK();
    }

    while (__futex_down(&mutex->futx->count) != 0)
    {
	self->current_mutex = mutex;

	__PTHIPTH_SIGNAL_BLOCK();
	futex_down(&global_futex);

	__pthipth_change_to_state(self, BLOCKED);

	futex_up(&global_futex);
	__PTHIPTH_SIGNAL_UNBLOCK();

	pthipth_yield();

    }

    mutex->owner_tid = __pthipth_gettid();

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

    __PTHIPTH_SIGNAL_BLOCK();
    futex_down(&global_futex);

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
    if (selected)
    {
	selected->current_mutex = NULL;
	__pthipth_change_to_state(selected, READY);
    }

    pthipth_private_t *owner = pthipth_avl_search(mutex->owner_tid);

    // reset priority after priority inheritance ends
    int tmp_priority = owner->priority;
    owner->priority = owner->old_priority;
    if (tmp_priority != owner->priority)
	pthipth_prio_reinsert(owner);

    // set default mutex value
    pthipth_mutex_init(mutex);

    futex_up(&global_futex);
    __PTHIPTH_SIGNAL_UNBLOCK();

    return 0;
}

int pthipth_mutex_destroy(pthipth_mutex_t *mutex)
{
    if (mutex == NULL) return -1;

    free(mutex->futx);

    mutex->futx = NULL;

    return 0;
}
