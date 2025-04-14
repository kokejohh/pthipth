#include <stdatomic.h>
#include <errno.h>
#include <time.h>

#include <unistd.h>

#include <linux/futex.h>
#include <syscall.h>

#include "futex.h"

// sys_futex: wrapper function for the futex syscall
// returns:
// 0 - success
// -1 - error
int sys_futex(atomic_int *uaddr, int operation, int value, const struct timespec *timeout)
{
    return syscall(SYS_futex, uaddr, operation, value, timeout, NULL);
}

// __futex_down_slow: calls futex(FUTEX_WAIT) to block the thread
// if the futex->count is equal to the value
// returns:
// 0 - woken up,
// 1 - value changed before sleeping (EWOULDBLOCK)
// -1 - error (interrupt, etc)
int __futex_down_slow(futex_t *futx, int value, struct timespec *relative)
{
    if(sys_futex(&futx->count, FUTEX_WAIT, value, relative) == 0) return 0;

    if (errno == EWOULDBLOCK) return 1;
    return -1;
}

// __futex_up_slow: calls futex(FUTEX_WAKE) for wake up the thread
// returns:
// 0 - successfully woke up the thread
// -1 - error
int __futex_up_slow(futex_t *futx)
{
    atomic_store(&futx->count, 1);
    return sys_futex(&futx->count, FUTEX_WAKE, 1, NULL);
}
