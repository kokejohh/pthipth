/*
 * Userspace Semaphore
 */
#ifndef _FUTEX_H
#define _FUTEX_H

#include <linux/futex.h>
#include <stdatomic.h>
#include <sys/time.h>

#include "futex_inline.h"

// Don't touch this sturcture directly
typedef struct futex
{
    atomic_int count;
    int priority;
} futex_t;


// Down and up operations
extern int sys_futex(atomic_int *, int, int, const struct timespec *, unsigned bitset);
extern int __futex_down_slow(futex_t *, int, struct timespec *);
extern int __futex_up_slow(futex_t *);

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
    if (woken) atomic_store(&futx->count, -1);
    return 0;
}

static inline int futex_down(futex_t *futx)
{
    return futex_down_timeout(futx, NULL);
}

static inline int futex_up(futex_t *futx)
{
    return (__futex_up(&futx->count)) ? 0 :
	    __futex_up_slow(futx);
}

void futex_init(futex_t *futx, int value);

#endif
