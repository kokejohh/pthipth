#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>

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

time_t gettime_ms()
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ts.tv_sec * 1000 + ts.tv_nsec / 1000;
}

// time_slice
void set_time_slice(int ms)
{
    struct itimerval timer;

    timer.it_value.tv_sec = ms / 1000;
    timer.it_value.tv_usec = (ms % 1000) * 1000;
    timer.it_interval.tv_sec = ms / 1000;
    timer.it_interval.tv_usec = (ms % 1000) * 1000;

    setitimer(ITIMER_REAL, &timer, NULL);
}
