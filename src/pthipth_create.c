#define _GNU_SOURCE
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <errno.h>
#include <sched.h>
#include <time.h>

#include <sys/mman.h>

#include "pthipth.h"
#include "pthipth_prio.h"
#include "pthipth_avl.h"

extern pthipth_private_t *pthipth_prio_head;

extern futex_t global_futex;

static int __pthipth_add_main_tcb()
{
    pthipth_private_t *main_tcb = (pthipth_private_t *)malloc(sizeof(pthipth_private_t));

    if (main_tcb == NULL) return -1;

    main_tcb->start_func = NULL;
    main_tcb->arg = NULL;
    main_tcb->state = READY;
    main_tcb->return_value = NULL;
    main_tcb->blockedForJoin = NULL;
    main_tcb->tid = __pthipth_gettid();
    main_tcb->priority = main_tcb->init_priority = main_tcb->old_priority = MAIN_PRIORITY;
    main_tcb->last_selected = __pthipth_gettime_ms();
    main_tcb->current_mutex = NULL;
    main_tcb->current_cond = NULL;
    main_tcb->current_barrier = NULL;
    main_tcb->wake_time = 0;

    futex_init(&main_tcb->sched_futex, 1);

    pthipth_prio_insert(main_tcb);
    pthipth_avl_insert(main_tcb);

    return 0;
}

void signal_time_slice()
{
    //printf("timer interrupt %d\n", __pthipth_gettid());
    pthipth_yield();
}

int pthipth_create(pthipth_t *new_thread_ID, pthipth_attr_t *attr, pthipth_task_t *task)
{
    int clone_flags = (CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD);
    if (pthipth_prio_head == NULL)
    {
	//signal(SIGALRM, signal_time_slice);
	signal(SIGALRM, SIG_IGN);

	__pthipth_set_thread_time_quota(TIME_SLICE);

	// add main thread
	int ret = __pthipth_add_main_tcb();
	if (ret != 0) return ret;

	futex_init(&global_futex, 1);

	// add idle thread
	pthipth_t idle_u_tcb;
	pthipth_task_t task_idle = { .function = __pthipth_idle, .arg = NULL, .priority = IDLE_PRIORITY };
	pthipth_create(&idle_u_tcb, NULL, &task_idle);
    }

    pthipth_private_t *new_node = (pthipth_private_t *)malloc(sizeof(pthipth_private_t));
    if (new_node == NULL) return -1;

    uint64_t stackSize = (attr == NULL) ? SIGSTKSZ : attr->stackSize;
    int priority = (task->priority) ? task->priority : DEFAULT_PRIORITY;

    if (task->priority < HIGHEST_PRIORITY) priority = HIGHEST_PRIORITY;
    else if (task->priority > LOWEST_PRIORITY && task->priority != IDLE_PRIORITY) priority = LOWEST_PRIORITY;

    // allocate memory
    char *child_stack = mmap(NULL, stackSize, PROT_READ | PROT_WRITE,
	    MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    if (child_stack == NULL) return -1;

    child_stack = child_stack + stackSize - sizeof(sigset_t);

    new_node->start_func = task->function;
    new_node->arg = task->arg;
    new_node->state = READY;
    new_node->return_value = NULL;
    new_node->blockedForJoin = NULL;
    new_node->priority = new_node->init_priority = new_node->old_priority = priority;
    new_node->last_selected = __pthipth_gettime_ms();
    new_node->current_mutex = NULL;
    new_node->current_cond = NULL;
    new_node->current_barrier = NULL;
    new_node->wake_time = 0;

    futex_init(&new_node->sched_futex, 0);

    pthipth_prio_insert(new_node);

    pid_t tid;
    if ((tid = clone(__pthipth_wrapper, child_stack, clone_flags, new_node)) == -1)
	return (-errno);

    *new_thread_ID = new_node->tid = tid;

    pthipth_avl_insert(new_node);

    return 0;
}
