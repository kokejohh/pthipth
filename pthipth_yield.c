#include "pthipth_prio.h"

extern futex_t global_futex;

extern void __pthipth_aging();

extern time_t getcurrenttime_millisec();

int __pthipth_dispatcher(pthipth_private_t *node, int killed)
{

    pthipth_private_t *tmp = pthipth_prio_extract();
    tmp->last_selected = getcurrenttime_millisec();

    if (tmp == node) return -1;
    else
    {
	// pre-selection aging
	printf("start -------- start\n");
	__pthipth_aging();
	printf("end -------- end\n");
    }

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
