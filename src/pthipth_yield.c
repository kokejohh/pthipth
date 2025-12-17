#include <stdint.h>

#include "pthipth.h"
#include "pthipth_internal.h"
#include "pthipth_bq.h"
#include "pthipth_queue.h"

extern __thread pthipth_private_t *cur_pthipth;

futex_t global_futex;

pthipth_private_t *__pthipth_scheduler(pthipth_private_t *cur)
{
    pthipth_private_t *next = pthipth_bq_peek();
    pthipth_bq_rotate();
    if (cur == next) next = pthipth_bq_peek();
    return next;
}

int __pthipth_dispatcher(pthipth_private_t *node)
{
    // check sleeping thread
    __pthipth_check_sleeping();
    // pre-selection aging
    __pthipth_aging();
    // check detach thread
    __pthipth_check_detach(node);

    // calling thread to ready state
    if (node->state == RUNNING)
	__pthipth_change_to_state(node, READY);

    // scheduler
    pthipth_private_t *tmp = __pthipth_scheduler(node);

    node->last_selected = tmp->last_selected = __pthipth_gettime_ms();

    __pthipth_change_to_state(tmp, RUNNING);

    if (tmp == node) return -1;

    cur_pthipth = tmp;

    futex_up(&tmp->sched_futex);

    return 0;
}

void pthipth_yield()
{
    // prevent race condition while yield
    futex_down(&global_futex);

    pthipth_private_t *self = __pthipth_selfptr();

    // only one thread
    if (__pthipth_dispatcher(self) == -1)
    {
	futex_up(&global_futex);
	return;
    }

    if (self->sched_futex.count > 0) futex_down(&self->sched_futex);

    futex_up(&global_futex);

    futex_down(&self->sched_futex);

    __pthipth_set_thread_time_quota(self->time_quota);
}

// pthipth_yield_qtime:
// yield if thread runs longer than time quota (ms),
// ms < 0: yield immediately
void pthipth_yield_qtime(int64_t ms)
{
    if (ms <= 0) 
    {
	pthipth_yield();
	return;
    }

    pthipth_private_t *self = __pthipth_selfptr();
    uint64_t current_time = __pthipth_gettime_ms();
    int64_t waiting_time = current_time - self->last_selected;

    if (waiting_time >= ms)
	pthipth_yield();
}
