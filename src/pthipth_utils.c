#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <syscall.h>
#include <sys/time.h>
#include <sys/mman.h>

#include "pthipth_queue.h"
#include "pthipth_bq.h"
#include "pthipth_avl.h"

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
	    pthipth_bq_delete(node);
	    pthipth_queue_add(&blocked_state, node);
	    break;
	case SLEEPING:
	    pthipth_bq_delete(node);
	    pthipth_queue_add(&sleeping_state, node);
	    break;
	case READY:
	    if (node->state == RUNNING) break;
	    pthipth_bq_insert(node);
	    break;
	case DEFUNCT:
	    pthipth_bq_delete(node);
	    break;
	case RUNNING: break;
    }
    node->state = to_state;
}

void __pthipth_free(pthipth_private_t *thread)
{
    pthipth_avl_delete(thread);
    uint64_t stack_size = thread->stack_size;
    char *child_stack = thread->child_stack;
    free(thread);
    munmap(child_stack - stack_size, stack_size);
}
