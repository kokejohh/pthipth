#define _GNU_SOURCE
#include <signal.h>
#include <errno.h>
#include <sched.h>
#include <stdlib.h>
#include <time.h>

#include "futex.h"
#include "pthipth.h"
#include "pthipth_prio.h"
#include "pthipth_avl.h"

#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/types.h>

extern void *pthipth_idle(void *);

extern int pthipth_wrapper(void *);

extern time_t gettime_ms();

extern pthipth_private_t *pthipth_prio_head;

futex_t global_futex;

static int __pthipth_add_main_tcb()
{
    pthipth_private_t *main_tcb = (pthipth_private_t *)malloc(sizeof(pthipth_private_t));

    if (main_tcb == NULL)
    {
	return -ENOMEM;
    }

    main_tcb->start_func = NULL;
    main_tcb->arg = NULL;
    main_tcb->state = READY;
    main_tcb->return_value = NULL;
    main_tcb->blockedForJoin = NULL;
    main_tcb->tid = __pthipth_gettid();
    main_tcb->priority = main_tcb->init_priority = main_tcb->old_priority = MAIN_PRIORITY;
    main_tcb->last_selected = gettime_ms();
    main_tcb->current_mutex = NULL;
    main_tcb->current_cond = NULL;

    futex_init(&main_tcb->sched_futex, 1);

    pthipth_prio_insert(main_tcb);
    pthipth_avl_insert(main_tcb);

    return 0;
}

extern void set_time_slice(int ms);

void time_slice()
{
    printf("timer interrupt %d\n", __pthipth_gettid());
    pthipth_yield();
}

int pthipth_create(pthipth_t *new_thread_ID, pthipth_attr_t *attr, pthipth_task_t *task)
{
    //int clone_flags = (CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGNAL | CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID |  CLONE_PARENT_SETTID );

    int clone_flags = (CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD);
    if (pthipth_prio_head == NULL)
    {
	//signal(SIGALRM, time_slice);
	signal(SIGALRM, SIG_IGN);

	set_time_slice(TIME_SLICE);

	int return_value = __pthipth_add_main_tcb();
	if (return_value != 0) return return_value;

	futex_init(&global_futex, 1);

	pthipth_t idle_u_tcb;
	pthipth_task_t task_idle = { .function = pthipth_idle, .arg = NULL, .priority = IDLE_PRIORITY };
	pthipth_create(&idle_u_tcb, NULL, &task_idle);
    }

    pthipth_private_t *new_node = (pthipth_private_t *)malloc(sizeof(pthipth_private_t));
    if (new_node == NULL) return -ENOMEM;

    unsigned long stackSize;
    unsigned priority;
    if (attr == NULL)
    {
	stackSize = SIGSTKSZ;
    }
    else
    {
	//dont forget fix bug
	stackSize = (attr->stackSize) ? attr->stackSize : SIGSTKSZ;
    }

    priority = (task->priority) ? task->priority : DEFAULT_PRIORITY;
    if (task->priority < HIGHEST_PRIORITY) priority = HIGHEST_PRIORITY;
    else if (task->priority > LOWEST_PRIORITY && task->priority != IDLE_PRIORITY) priority = LOWEST_PRIORITY;

    char *child_stack = mmap(NULL, stackSize, PROT_READ | PROT_WRITE,
	    MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);

    if (child_stack == NULL) return -ENOMEM;

    child_stack = child_stack + stackSize - sizeof(sigset_t);

    new_node->start_func = task->function;
    new_node->arg = task->arg;
    new_node->state = READY;
    new_node->return_value = NULL;
    new_node->blockedForJoin = NULL;
    new_node->priority = new_node->init_priority = new_node->old_priority = priority;
    new_node->last_selected = gettime_ms();
    new_node->current_mutex = NULL;
    new_node->current_cond = NULL;

    futex_init(&new_node->sched_futex, 0);

    pthipth_prio_insert(new_node);

    pid_t tid;
    if ((tid = clone(pthipth_wrapper, child_stack, clone_flags, new_node)) == -1)
    {
	return (-errno);
    }

    *new_thread_ID = new_node->tid = tid;

    pthipth_avl_insert(new_node);

    // in the future
    //futex_up(&new_node->sched_futex);

    return 0;
}
