#include <unistd.h>
#include <sys/syscall.h>
#include <time.h>

#include "futex.h"
#include "pthipth_prio.h"


futex_t debug_futex;
char msg[1000];
static int debug_futex_init_already = 0;

extern pthipth_private_t *pthipth_prio_head;

pid_t __pthipth_gettid()
{
    return (pid_t)syscall(SYS_gettid);
}

void __pthipth_debug_futex_init()
{
    if (!debug_futex_init_already)
    {
	futex_init(&debug_futex, 1);
	debug_futex_init_already = 0;
    }
}

time_t getcurrenttime_millisec()
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    time_t current_time = ts.tv_sec * 1000 + ts.tv_nsec / 1000;

    return current_time;
}

void __pthipth_aging()
{
//    pthipth_prio_display();

    pthipth_private_t *tmp = pthipth_prio_head;

    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    do
    {
	pthipth_private_t *inside_tmp = tmp;
	do
	{
	    int new_priority = inside_tmp->priority;
	    time_t current_time = getcurrenttime_millisec();
	    time_t waiting_time = current_time - inside_tmp->last_selected;
	    //printf("tid %d, inside_tmp->last_selected %ld\n", inside_tmp->tid, inside_tmp->last_selected);
	    //printf("waiting_time %ld\n", waiting_time);
	    int aging_factor = 1;
	    new_priority += waiting_time / 1000 * aging_factor;
	    if (new_priority < 1) new_priority = 1;
	    inside_tmp->priority = new_priority;
	    pthipth_private_t *next_tmp = inside_tmp->inside_next;
	    if (inside_tmp->priority < inside_tmp->old_priority)
	    {
		inside_tmp->old_priority = inside_tmp->priority;
		pthipth_prio_reinsert(inside_tmp);
	    }

	    inside_tmp = next_tmp;
	} while (inside_tmp != tmp);
	tmp = tmp->next;
    } while (tmp != NULL);
}

