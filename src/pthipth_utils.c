#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include <syscall.h>
#include <sys/time.h>

#include "pthipth_queue.h"
#include "pthipth_prio.h"

extern pthipth_private_t *pthipth_prio_head;

extern pthipth_queue_t blocked_state;
extern pthipth_queue_t sleeping_state;

pid_t __pthipth_gettid()
{
    return (pid_t)syscall(SYS_gettid);
}

uint64_t __pthipth_gettime_ms()
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

// set time quota per thread
void __pthipth_set_thread_time_quota(int ms)
{
    if (ms <= 0)
    {
	struct itimerval timer = {0};
	setitimer(ITIMER_REAL, &timer, NULL);
	return;
    }
    struct itimerval timer;

    timer.it_value.tv_sec = ms / 1000;
    timer.it_value.tv_usec = (ms % 1000) * 1000;
    timer.it_interval.tv_sec = ms / 1000;
    timer.it_interval.tv_usec = (ms % 1000) * 1000;

    setitimer(ITIMER_REAL, &timer, NULL);
}

// safe change state
void __pthipth_change_to_state(pthipth_private_t *node, pthipth_state_t to_state)
{
    if (node->state == to_state) return;

    switch (node->state)
    {
	case BLOCKED:
	    pthipth_queue_delete(&blocked_state, node);
	    break;
	case SLEEPING:
	    pthipth_queue_delete(&sleeping_state, node);
	    break;
	default: break;
    }

    switch (to_state)
    {
	case BLOCKED:
	    pthipth_prio_delete(node);
	    pthipth_queue_add(&blocked_state, node);
	    break;
	case SLEEPING:
	    pthipth_prio_delete(node);
	    pthipth_queue_add(&sleeping_state, node);
	    break;
	case READY:
	    if (node->state == RUNNING) break;
	    pthipth_prio_insert(node);
	    break;
	case DEFUNCT:
	    pthipth_prio_delete(node);
	    break;
	case RUNNING: break;
    }
    node->state = to_state;
}
