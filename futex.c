#include <errno.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <unistd.h>

#include "futex.h"

// futex system call
int sys_futex(atomic_int *uaddr, int operation, int value, const struct timespec *timeout)
{
    return syscall(SYS_futex, uaddr, operation, value, timeout, NULL);
}

// return -1 on fail, 0 on wakeup, 1 on didn't sleep
int __futex_down_slow(futex_t *futx, int value, struct timespec *relative)
{
    if(sys_futex(&futx->count, FUTEX_WAIT, value, relative) == 0) 
    {
	return 0;
    }

    if (errno == EWOULDBLOCK) return 1;
    return -1;
}

int __futex_up_slow(futex_t *futx)
{
    atomic_store(&futx->count, 1);
    return sys_futex(&futx->count, FUTEX_WAKE, 1, NULL);
}

void futex_init(futex_t *futx, int value)
{
    atomic_store(&futx->count, value);
}
