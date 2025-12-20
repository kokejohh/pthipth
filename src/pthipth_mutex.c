#include <stdlib.h>
#include <errno.h>

#include "pthipth.h"
#include "pthipth_internal.h"
#include "pthipth_avl.h"
#include "pthipth_bq.h"
#include "pthipth_queue.h"

extern futex_t global_futex;

pthipth_t init_owner_tid = 0;

// pthipth_mutex_init
// returns:
// 0 - success
// -1 - error
int pthipth_mutex_init(pthipth_mutex_t *mutex)
{
    if (mutex == NULL || mutex->futx != NULL) return -1;

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
    if (mutex == NULL || mutex->futx == NULL) return -1;

    pthipth_private_t *self = __pthipth_selfptr();

    if (self == NULL) return -1;

    // current thread already owns the mutex
    if (mutex->owner_tid == self->tid) return -1;
    while (__futex_down(&mutex->futx->count) != 0)
    {
	pthipth_yield();
    }

    futex_down(&global_futex);

    // special HIGHEST_PRIORITY for mutex.
    self->priority = HIGHEST_PRIORITY - 1;
    pthipth_bq_reinsert(self);

    mutex->owner_tid = __pthipth_gettid();

    self->mutex_count++;

    futex_up(&global_futex);

    return 0;
}

// pthipth_mutex_trylock
// returns:
// 0 - can lock
// EBUSY - already lock
// -1 - error
int pthipth_mutex_trylock(pthipth_mutex_t *mutex)
{
    if (mutex == NULL || mutex->futx) return -1;

    pthipth_private_t *self = __pthipth_selfptr();

    int expected = 1;
    int desired = 0;
    if (atomic_compare_exchange_strong(&mutex->futx->count, &expected, desired))
    {
	mutex->owner_tid = self->tid;
	return 0;
    }
    else
    {
	return EBUSY;
    }
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

    futex_down(&global_futex);

    pthipth_private_t *owner = pthipth_avl_search(mutex->owner_tid);

    owner->mutex_count--;
    // set old priority before boost priority
    if (owner->mutex_count == 0)
    {
	owner->priority = owner->old_priority;
	pthipth_bq_reinsert(owner);
    }

    // set default mutex value
    mutex->owner_tid = init_owner_tid;
    // set mutex is available
    futex_init(mutex->futx, 1);

    futex_up(&global_futex);

    return 0;
}

int pthipth_mutex_destroy(pthipth_mutex_t *mutex)
{
    if (mutex == NULL || mutex->futx == NULL) return -1;

    if (mutex->owner_tid != init_owner_tid) return EBUSY;

    free(mutex->futx);
    mutex->futx = NULL;

    return 0;
}
