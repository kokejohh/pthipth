#include "pthipth_mutex.h"
#include "pthipth_prio.h"
#include "pthipth_avl.h"
//#include "pthipth.h"

pthipth_t init_owner = {
    .tid = 0
};

void pthipth_mutex_init(pthipth_mutex_t *mutex)
{
    futex_init(&mutex->futx, 1);

    mutex->owner = init_owner;
}
    
void pthipth_mutex_lock(pthipth_mutex_t *mutex)
{
    //pthipth_private_t *self = __pthipth_selfptr();
    //pthipth_private_t *owner = pthipth_avl_search(mutex->owner.tid);
    //printf("koke %d\n", self->tid);
    while (__futex_down(&mutex->futx.count) != 0)
    {
	pthipth_yield();
    }
    mutex->owner = pthipth_self();

    // koke don't forget to fix
    //pthipth_private_t *owner = pthipth_avl_search(mutex->owner.tid);

    //if (owner->sched_futex.priority > 


}

void pthipth_mutex_unlock(pthipth_mutex_t *mutex)
{
    if (mutex->owner.tid != __pthipth_gettid())
    {
	printf("unlock error\n");
	//exit(1);
	return;
    }

    mutex->owner = init_owner;

    // assign futex is 1 to futex up
    futex_init(&mutex->futx, 1);
}

