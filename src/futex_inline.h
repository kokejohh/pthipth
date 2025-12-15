#ifndef FUTEX_INLINE_H
#define FUTEX_INLINE_H

#include <stdatomic.h>

// compare and swap : an atomic operation that checks
// if a value is what you expect, and if it is, it changes it to a new value

// __futex_down: atomic decrement
// returns:
// (value - 1) - if counter is non-negative and it's can compare and swap.
// negative number - other cases.
static inline int __futex_down(atomic_int *counter)
{
    int value = atomic_load(counter);

    do
    {
	if (value < 0) return value;
    } while (!atomic_compare_exchange_weak(counter, &value, value - 1));

    return value - 1;
}

// __futex_up: atomic increment
// returns:
// 1 - if counter incremented from 0 to 1,
// 0 - other cases.
static inline int __futex_up(atomic_int *counter)
{
    int value = atomic_load(counter);

    return (atomic_compare_exchange_strong(counter, &value, value + 1) && value == 0);
}

#endif
