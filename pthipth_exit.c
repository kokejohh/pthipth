#include "pthipth_prio.h"

static void __pthipth_do_exit()
{
    syscall(SYS_exit, 0);
}

void pthipth_exit(void *return_val)
{
    pthipth_private_t *self_ptr = __pthipth_selfptr();

    self_ptr->state = DEFUNCT;
    self_ptr->return_value = return_val;

    if (self_ptr->blockedForJoin != NULL)
	self_ptr->blockedForJoin->state = READY;

    pthipth_prio_delete(self_ptr);

    __pthipth_dispatcher(self_ptr, 1);

    __pthipth_do_exit();
}
