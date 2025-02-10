#include <errno.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <unistd.h>

#include "futex.h"

#include <stdio.h>

#define FUTEX_BITSET_MATCH_PRIORITY(prio) (1U << ((prio) % 32))

// futex system call
int sys_futex(atomic_int *uaddr, int operation, int value, const struct timespec *timeout, unsigned bitset)
{
    return syscall(SYS_futex, uaddr, operation, value, timeout, NULL, bitset);
}

// return -1 on fail, 0 on wakeup, 1 on didn't sleep
int __futex_down_slow(futex_t *futx, int value, struct timespec *relative)
{
    unsigned bitset = FUTEX_BITSET_MATCH_PRIORITY(futx->priority);
    if(sys_futex(&futx->count, FUTEX_WAIT_BITSET, value, relative, bitset) == 0) 
    {
	return 0;
    }

    if (errno == EWOULDBLOCK) return 1;
    return -1;
}

int __futex_up_slow(futex_t *futx)
{
    unsigned bitset = FUTEX_BITSET_MATCH_PRIORITY(futx->priority);
    atomic_store(&futx->count, 1);
    return sys_futex(&futx->count, FUTEX_WAKE_BITSET, 1, NULL, bitset); 
}

void futex_init(futex_t *futx, int value)
{
    atomic_store(&futx->count, value);
}
