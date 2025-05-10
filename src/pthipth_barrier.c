#include "pthipth.h"
#include "pthipth_internal.h"
#include "pthipth_queue.h"
#include "pthipth_signal.h"

extern pthipth_queue_t blocked_state;

extern futex_t global_futex;

// pthipth_barrier_init
// returns:
// 0 - success
// -1 - error
int pthipth_barrier_init(pthipth_barrier_t *barrier, int count)
{
    if (barrier == NULL) return -1;

    barrier->count = count < 0 ? 1 : count;
    atomic_store(&barrier->waiting, 0);

    return 0;
}

// pthipth_barrier_wait:
// returns:
// 0 - thread blocked
// 1 - PTHIPTH_BARRIER_SERIAL_THREAD (last thread into barrier)
// -1 - error
int pthipth_barrier_wait(pthipth_barrier_t *barrier)
{
    if (barrier == NULL) return -1;

    pthipth_private_t *self = __pthipth_selfptr();

    if (self == NULL) return -1;

    atomic_fetch_add(&barrier->waiting, 1);

    // barrier not yet full: put this thread into BLOCKED state and yield
    if (atomic_load(&barrier->waiting) < barrier->count)
    {
	self->current_barrier = barrier;

	__PTHIPTH_SIGNAL_BLOCK();
	futex_down(&global_futex);

	__pthipth_change_to_state(self, BLOCKED);

	futex_up(&global_futex);
	__PTHIPTH_SIGNAL_UNBLOCK();

	pthipth_yield();
	return 0;
    }

    __PTHIPTH_SIGNAL_BLOCK();
    futex_down(&global_futex);

    // wake all thread in this barrier
    pthipth_private_t *tmp = blocked_state.head;
    while (tmp)
    {
	pthipth_private_t *next_tmp = tmp->next;
	if (tmp->state == BLOCKED && tmp->current_barrier == barrier)
	    __pthipth_change_to_state(tmp, READY);
	tmp = next_tmp;
    }

    futex_up(&global_futex);
    __PTHIPTH_SIGNAL_UNBLOCK();

    atomic_store(&barrier->waiting, 0);

    return PTHIPTH_BARRIER_SERIAL_THREAD;
}

// pthipth_barrier_destroy
// returns:
// 0 - success
// -1 - error
int pthipth_barrier_destroy(pthipth_barrier_t *barrier)
{
    if (barrier == NULL) return -1;

    if (atomic_load(&barrier->waiting) > 0)
	return -1;

    barrier->count = -1;
    atomic_store(&barrier->waiting, -1);

    return 0;
}
