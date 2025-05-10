#include "pthipth_internal.h"
#include "pthipth_signal.h"

void __signal_time_slice(int sig)
{
    //printf("timer interrupt %d\n", __pthipth_gettid());
    if (sig == SIGALRM) __pthipth_yield();
}

void pthipth_signal_block()
{
    __PTHIPTH_SIGNAL_BLOCK();
}

void pthipth_signal_unblock()
{
    __PTHIPTH_SIGNAL_UNBLOCK();
}

void pthipth_signal_print()
{
    __PTHIPTH_SIGNAL_PRINT();
}

void pthipth_signal_ignore()
{
    signal(SIGALRM, SIG_IGN);
}

void pthipth_signal_restore()
{
    signal(SIGALRM, __signal_time_slice);
}
