#include <unistd.h>
#include <syscall.h>

#include "pthipth.h"
#include "pthipth_internal.h"
#include "pthipth_signal.h"
#include "pthipth_avl.h"

extern futex_t global_futex;

__attribute__ ((noreturn))
static void __pthipth_exit()
{
    syscall(SYS_exit, 0);
    __builtin_unreachable();
}

// pthipth_exit:
__attribute__ ((noreturn))
void pthipth_exit(void *return_val)
{
    // sync 
    futex_down(&global_futex);

    __PTHIPTH_SIGNAL_BLOCK();

    pthipth_private_t *self = __pthipth_selfptr();

    // change state of thread join to ready
    if (self->blockedForJoin != NULL)
	__pthipth_change_to_state(self->blockedForJoin, READY);

    self->return_value = return_val;

    __pthipth_change_to_state(self, DEFUNCT);

    __pthipth_dispatcher(self);

    futex_up(&global_futex);

    __PTHIPTH_SIGNAL_UNBLOCK();

    __pthipth_exit();
}
