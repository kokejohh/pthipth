#include <unistd.h>
#include <sys/syscall.h>

#include "pthipth_prio.h"

static void __pthipth_do_exit()
{
    syscall(SYS_exit, 0);
}

void pthipth_exit(void *return_val)
{
    pthipth_private_t *self = __pthipth_selfptr();

    self->state = DEFUNCT;
    self->return_value = return_val;

    if (self->blockedForJoin != NULL)
	self->blockedForJoin->state = READY;

    pthipth_prio_delete(self);

    __pthipth_dispatcher(self, 1);

    __pthipth_do_exit();
}
