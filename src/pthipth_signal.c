#include "pthipth_signal.h"

void pthipth_signal_block()
{
    __PTHIPTH_SIGNAL_BLOCK();
}

void pthipth_signal_unblock()
{
    __PTHIPTH_SIGNAL_UNBLOCK();
}
