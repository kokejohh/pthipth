#ifndef FUTEX_INLINE_H
#define FUTEX_INLINE_H

#define __GNU_SOURCE
#include <stdatomic.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>

// atomic decrement: return new value. 0, -1
static inline int __futex_down(atomic_int *counter)
{
    int value = atomic_load(counter);
    if (value < 0) return value;

    do
    {
	value = atomic_load(counter);
	if (value < 0) return value;
    } while (!(atomic_compare_exchange_strong(counter, &value, value - 1)));
    return value - 1;
}

// atomic increment: return 1 if incremented 0 to 1
static inline int __futex_up(atomic_int *counter)
{
    int value = atomic_load(counter);

    // 1 on succuss, 0 other cases
    return (atomic_compare_exchange_strong(counter, &value, value + 1) && value == 0);
}

#endif
