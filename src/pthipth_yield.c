#include <stdint.h>

#include "pthipth.h"
#include "pthipth_prio.h"
#include "pthipth_queue.h"

futex_t global_futex;

extern pthipth_private_t *pthipth_prio_head;
extern pthipth_queue_t blocked_state;
extern void pthipth_aging(int aging_factor);

int __pthipth_dispatcher(pthipth_private_t *node)
{
    // check sleeping thread
    __pthipth_check_sleeping();
    // pre-selection aging (not use now)
    //__pthipth_aging(1);

    // calling thread to ready state
    if (node->state == RUNNING)
	__pthipth_change_to_state(node, READY);

    // scheduler
    pthipth_private_t *tmp = pthipth_prio_extract();

    node->last_selected = tmp->last_selected = __pthipth_gettime_ms();

    __pthipth_change_to_state(tmp, RUNNING);

    if (tmp == node) return -1;

    // set time quota
    __pthipth_set_thread_time_quota(TIME_SLICE);

    futex_up(&tmp->sched_futex);

    return 0;
}

void pthipth_yield()
{
    futex_down(&global_futex);
    pthipth_private_t *self = __pthipth_selfptr();

    // prevent race condition while yield
    
    // only one thread
    if (__pthipth_dispatcher(self) == -1)
    {
	futex_up(&global_futex);
	return;
    }

    if (self->sched_futex.count > 0) futex_down(&self->sched_futex);

    futex_up(&global_futex);

    futex_down(&self->sched_futex);

    return;
}

// pthipth_yieldq: 
// yield if thread runs longer than time quota (ms),
// ms < 0: yield immediately
void pthipth_yieldq(uint64_t ms)
{
    if (ms < 0) return pthipth_yield();

    pthipth_private_t *self = __pthipth_selfptr();
    uint64_t current_time = __pthipth_gettime_ms();
    uint64_t waiting_time = current_time - self->last_selected;
    uint64_t time_quota = (ms == 0) ? TIME_SLICE : ms;

    if (waiting_time >= time_quota)
	pthipth_yield();
}
