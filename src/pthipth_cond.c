#include "pthipth.h"
#include "pthipth_internal.h"
#include "pthipth_internal_type.h"
#include "pthipth_queue.h"

extern pthipth_queue_t blocked_state;

extern futex_t global_futex;

// pthipth_cond_init
// returns:
// 0 - success
// -1 - error
int pthipth_cond_init(pthipth_cond_t *cond)
{
    if (cond == NULL) return -1;

    return 0;
}

// pthipth_cond_signal
// returns:
// 0 - success
// -1 - error
int pthipth_cond_signal(pthipth_cond_t *cond)
{
    if (cond == NULL) return -1;

    futex_down(&global_futex);

    pthipth_private_t *tmp = blocked_state.head;
    pthipth_private_t *selected = NULL;

    while (tmp)
    {
	pthipth_private_t *next_tmp = tmp->next;

	if (tmp->state == BLOCKED && tmp->current_cond == cond &&
		(selected == NULL || tmp->priority < selected->priority))
	{
	    selected = tmp;
	    break;
	}
	tmp = next_tmp;
    }
    if (selected)
    {
	selected->current_cond = NULL;
	__pthipth_change_to_state(selected, READY);
    }

    futex_up(&global_futex);

    return 0;
}

// pthipth_cond_broadcast
// returns:
// 0 - success
// -1 - error
int pthipth_cond_broadcast(pthipth_cond_t *cond)
{
    if (cond == NULL) return -1;

    futex_down(&global_futex);

    pthipth_private_t *tmp = blocked_state.head;

    while (tmp)
    {
	pthipth_private_t *next_tmp = tmp->next;

	if (tmp->state == BLOCKED && tmp->current_cond == cond)
	{
	    tmp->current_cond = NULL;
	    __pthipth_change_to_state(tmp, READY);
	}
	tmp = next_tmp;
    }

    futex_up(&global_futex);

    return 0;
}

// pthipth_cond_wait
// returns:
// 0 - success
// -1 - error
int pthipth_cond_wait(pthipth_cond_t *cond, pthipth_mutex_t *mutex)
{
    if (cond == NULL || mutex == NULL) return -1;

    futex_down(&global_futex);

    pthipth_private_t *self = __pthipth_selfptr();

    self->current_cond = cond;

    __pthipth_change_to_state(self, BLOCKED);
    futex_up(&global_futex);

    pthipth_mutex_unlock(mutex);

    pthipth_yield();

    pthipth_mutex_lock(mutex);

    return 0;
}

// pthipth_cond_wait_non
// non-preemptive only and not use mutex
// returns:
// 0 - success
// -1 - error
int pthipth_cond_wait_non(pthipth_cond_t *cond)
{
    if (cond == NULL) return -1;

    pthipth_private_t *self = __pthipth_selfptr();

    futex_down(&global_futex);

    __pthipth_change_to_state(self, BLOCKED);

    self->current_cond = cond;

    futex_up(&global_futex);

    pthipth_yield();

    return 0;
}

// pthipth_cond_destroy
// actually nothing to do
// returns:
// 0 - success
// -1 - error
int pthipth_cond_destroy(pthipth_cond_t *cond)
{
    if (cond == NULL) return -1;

    return 0;
}
