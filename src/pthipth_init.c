#define _GNU_SOURCE
#include <stdlib.h>

#include "pthipth.h"
#include "pthipth_internal.h"
#include "pthipth_prio.h"
#include "pthipth_avl.h"

extern __thread pthipth_private_t *cur_pthipth;

extern pthipth_private_t *pthipth_prio_head;

extern futex_t global_futex;

int __g_pthipth_init = 0;
int __g_pthipth_idle = 0;

static int __pthipth_add_main_tcb()
{
    pthipth_private_t *main_tcb = (pthipth_private_t *)calloc(1, sizeof(pthipth_private_t));

    if (!main_tcb) return -1;

    cur_pthipth = main_tcb;

    main_tcb->state = READY;
    main_tcb->tid = __pthipth_gettid();
    main_tcb->priority = main_tcb->init_priority = main_tcb->old_priority = MAIN_PRIORITY;
    main_tcb->last_selected = __pthipth_gettime_ms();
    main_tcb->aging_time = 1;

    futex_init(&main_tcb->sched_futex, 1);

    pthipth_prio_insert(main_tcb);
    pthipth_avl_insert(main_tcb);

    return 0;
}

int pthipth_init()
{
    if (__g_pthipth_init && __g_pthipth_idle) return 0;

    // add main thread
    int ret = __pthipth_add_main_tcb();
    if (ret != 0) return ret;

    __g_pthipth_init = 1;

    futex_init(&global_futex, 1);

    pthipth_t idle_u_tcb;
    pthipth_task_t task_idle = { .function = __pthipth_idle, .arg = NULL, .priority = IDLE_PRIORITY };

    int ret_idle = pthipth_create(&idle_u_tcb, NULL, &task_idle);

    if (ret_idle) return ret_idle;

    __g_pthipth_idle = 1;

    return 0;
}
