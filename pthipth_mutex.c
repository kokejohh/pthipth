// Don't forget cas
#include <errno.h>

#include "pthipth_mutex.h"
#include "pthipth.h"
#include "pthipth_prio.h"
#include "pthipth_avl.h"

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
    if (mutex->owner.tid != 0) 
    {
	pthipth_private_t *self = __pthipth_selfptr();
	pthipth_private_t *owner = pthipth_avl_search(mutex->owner.tid);

	if (self->priority < owner->priority)
	{
	    owner->priority = self->priority;
	    pthipth_prio_reinsert(owner);
	}
    }

    while (__futex_down(&mutex->futx.count) != 0)
    {
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
    if (mutex->owner.tid != __pthipth_gettid())
    {
	printf("mutex unlock error: not owner unlock!\n");
	return -1;
    }

    pthipth_private_t *owner = pthipth_avl_search(mutex->owner.tid);

    owner->priority = owner->old_priority;

    mutex->owner = init_owner;
    // assign futex is 1 to futex up
    futex_init(&mutex->futx, 1);
    return 0;
}

