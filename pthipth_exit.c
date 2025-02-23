#include <unistd.h>
#include <sys/syscall.h>

#include "pthipth.h"

extern void change_to_state(pthipth_private_t *node, int state);

static void __pthipth_do_exit()
{
    syscall(SYS_exit, 0);
}

void pthipth_exit(void *return_val)
{
    pthipth_private_t *self = __pthipth_selfptr();

    if (self->blockedForJoin != NULL)
	change_to_state(self->blockedForJoin, READY);

    self->return_value = return_val;

    change_to_state(self, DEFUNCT);

    __pthipth_dispatcher(self);

    __pthipth_do_exit();
}
