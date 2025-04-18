#include <stdint.h>

#include "pthipth.h"
#include "pthipth_prio.h"
#include "pthipth_queue.h"
#include "pthipth_signal.h"

futex_t global_futex;

extern pthipth_private_t *pthipth_prio_head;
extern pthipth_queue_t blocked_state;

int __pthipth_dispatcher(pthipth_private_t *node)
{
    // check sleeping thread
    __pthipth_check_sleeping();
    // pre-selection aging (not use now)
    __pthipth_aging();

    // calling thread to ready state
    if (node->state == RUNNING)
	__pthipth_change_to_state(node, READY);

    // scheduler
    pthipth_private_t *tmp = pthipth_prio_extract();

    node->last_selected = tmp->last_selected = __pthipth_gettime_ms();

    __pthipth_change_to_state(tmp, RUNNING);

    if (tmp == node) return -1;

    futex_up(&tmp->sched_futex);

    return 0;
}

void pthipth_yield()
{
    __PTHIPTH_SIGNAL_BLOCK();

    // prevent race condition while yield
    futex_down(&global_futex);

    pthipth_private_t *self = __pthipth_selfptr();
    
    // only one thread
    if (__pthipth_dispatcher(self) == -1)
    {
	futex_up(&global_futex);
	__PTHIPTH_SIGNAL_UNBLOCK();
	return;
    }

    if (self->sched_futex.count > 0) futex_down(&self->sched_futex);

    futex_up(&global_futex);

    futex_down(&self->sched_futex);

    __pthipth_set_thread_time_quota(self->time_quota);

    __PTHIPTH_SIGNAL_UNBLOCK();

    return;
}

// pthipth_yield_qtime:
// yield if thread runs longer than time quota (ms),
// ms < 0: yield immediately
void pthipth_yield_qtime(uint64_t ms)
{
    if (ms <= 0) 
    {
	pthipth_yield();
	return;
    }

    __PTHIPTH_SIGNAL_BLOCK();

    pthipth_private_t *self = __pthipth_selfptr();
    uint64_t current_time = __pthipth_gettime_ms();
    uint64_t waiting_time = current_time - self->last_selected;

    __PTHIPTH_SIGNAL_UNBLOCK();

    if (waiting_time >= ms)
	pthipth_yield();
}
