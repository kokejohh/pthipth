/*
 * Userspace Semaphore
 */
#ifndef _FUTEX_H
#define _FUTEX_H

#include <stdatomic.h>
#include <time.h>

#include <sys/syscall.h>

#include <unistd.h>

#include "futex_inline.h"

// don't touch this sturcture directly
typedef struct futex
{
    atomic_int count;
} futex_t;

// down and up operations
int sys_futex(atomic_int *, int, int, const struct timespec *);
int __futex_down_slow(futex_t *, int, struct timespec *);
int __futex_up_slow(futex_t *);

// futex_down_timeout: blocks the thread; retries if EWOULDBLOCK occurs.
// returns:
// 0 - success
// -1 - error
static inline int futex_down_timeout(futex_t *futx, struct timespec *relative)
{
    int value, woken = 0;
    while ((value = __futex_down(&futx->count)) != 0)
    {
	switch (__futex_down_slow(futx, value, relative))
	{
	    case -1: return -1;
	    case 0: woken = 1; break;
	    case 1: continue;
	}
    }

    // If we were woken, other threads might still be sleeping, so set to -1
    if (woken) atomic_store(&futx->count, -1);

    return 0;
}

// futex_down: calls futex_down_timeout without a timeout
static inline int futex_down(futex_t *futx)
{
    return futex_down_timeout(futx, NULL);
}

// futex_up: wake up thread
static inline int futex_up(futex_t *futx)
{
    return (__futex_up(&futx->count)) ? 0 :
	    __futex_up_slow(futx);
}

// futex_init: initializes the futex with the given value.
static inline void futex_init(futex_t *futx, int value)
{
    atomic_store(&futx->count, value);
}

#endif
