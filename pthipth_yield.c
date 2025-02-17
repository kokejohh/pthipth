#include "pthipth_prio.h"

#define TIME_SLICE 500

extern futex_t global_futex;

extern void pthipth_aging(int aging_factor);

extern time_t gettime_ms();

extern void set_time_slice(int ms);

int __pthipth_dispatcher(pthipth_private_t *node, int killed)
{
    // set last_selected of calling thread
    node->last_selected = gettime_ms();
    // pre-selection aging
    pthipth_aging(1);

    pthipth_private_t *tmp = pthipth_prio_extract();

    if (tmp == node) return -1;

    set_time_slice(TIME_SLICE);

    futex_up(&tmp->sched_futex);

    return 0;
}

int pthipth_yield()
{
    pthipth_private_t *self = __pthipth_selfptr();

    futex_down(&global_futex);
    
    // Only one thread. Nothing to do
    if (__pthipth_dispatcher(self, 0) == -1)
    {
	futex_up(&global_futex);
	return 0;
    }

    if (self->sched_futex.count > 0)
	futex_down(&self->sched_futex);

    futex_up(&global_futex);

    futex_down(&self->sched_futex);

    return 0;
}
