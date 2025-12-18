#define _GNU_SOURCE
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <errno.h>
#include <sched.h>
#include <time.h>

#include <sys/mman.h>

#include "pthipth.h"
#include "pthipth_internal.h"
#include "pthipth_bq.h"
#include "pthipth_avl.h"

extern int __g_pthipth_init;
extern int __g_pthipth_idle;

extern __thread pthipth_private_t *cur_pthipth;

extern futex_t global_futex;

int pthipth_create(pthipth_t *new_thread_ID, pthipth_attr_t *attr, pthipth_task_t *task)
{
    if (__g_pthipth_init == 0 || new_thread_ID == NULL || task == NULL  ) return -1;

    int clone_flags = (CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD | CLONE_PARENT_SETTID | CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID);

    pthipth_private_t *new_node = (pthipth_private_t *)calloc(1, sizeof(pthipth_private_t));
    if (new_node == NULL) return -1;

    uint64_t stack_size = (attr == NULL || attr->stack_size <= 0) ? SIGSTKSZ : attr->stack_size;
    int time_quota = (attr == NULL || attr->time_quota_ms <= 0) ? 0 : attr->time_quota_ms;
    int aging_factor = (attr == NULL || attr->aging_factor <= 0) ? 0 : attr->aging_factor;
    int aging_time = (attr == NULL || attr->aging_time <= 0) ? 1 : attr->aging_time;
    int priority = (task->priority) ? task->priority : DEFAULT_PRIORITY;
    if (task->priority < HIGHEST_PRIORITY) priority = HIGHEST_PRIORITY;
    else if (__g_pthipth_idle && task->priority > LOWEST_PRIORITY) priority = LOWEST_PRIORITY;

    char *child_stack = mmap(NULL, stack_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    if (child_stack == MAP_FAILED)
    {
	free(new_node);
	return -1;
    }

    child_stack = child_stack + stack_size;
    new_node->start_func = task->function;
    new_node->arg = task->arg;
    new_node->state = READY;
    new_node->priority = new_node->init_priority = new_node->old_priority = priority;
    new_node->last_selected = __pthipth_gettime_ms();
    new_node->time_quota = time_quota;
    new_node->aging_factor = aging_factor;
    new_node->aging_time = aging_time;
    new_node->stack_size = stack_size;
    new_node->mutex_count = 0;

    futex_init(&new_node->sched_futex, 0);

    if (clone(__pthipth_wrapper, child_stack, clone_flags, new_node, &new_node->tid, NULL, &new_node->tid_watch) == -1)
    {
	*new_thread_ID = -1;
	munmap(child_stack - stack_size, stack_size);
	free(new_node);
	return (-errno);
    }
    *new_thread_ID = new_node->tid;

    futex_down(&global_futex);

    pthipth_bq_insert(new_node);
    pthipth_avl_insert(new_node);

    futex_up(&global_futex);

    return 0;
}
