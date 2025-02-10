#include <unistd.h>
#include <sys/syscall.h>
#include "futex.h"

futex_t debug_futex;
char msg[1000];
static int debug_futex_init_already = 0;


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
