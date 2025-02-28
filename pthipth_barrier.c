#include "pthipth_barrier.h"
#include "pthipth_queue.h"

extern void change_to_state(pthipth_private_t *node, int state);

extern pthipth_queue_t blocked_state;

void pthipth_barrier_init(pthipth_barrier_t *barrier, int count)
{
    barrier->count = count;
    atomic_store(&barrier->waiting, 0);
}

int pthipth_barrier_wait(pthipth_barrier_t *barrier)
{
    pthipth_private_t *self = __pthipth_selfptr();

    atomic_fetch_add(&barrier->waiting, 1);
    if (atomic_load(&barrier->waiting) < barrier->count)
    {
	self->current_barrier = barrier;
	change_to_state(self, BLOCKED);
	pthipth_yield();
    }
    else
    {
	pthipth_private_t *tmp = blocked_state.head;
	while (tmp)
	{
	    pthipth_private_t *next_tmp = tmp->next;
	    if (tmp->state == BLOCKED && tmp->current_barrier == barrier)
		change_to_state(tmp, READY);
	    tmp = next_tmp;
	}
	atomic_store(&barrier->waiting, 0);
	return 1;
    }
    return 0;
}
