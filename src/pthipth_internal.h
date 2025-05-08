#ifndef PTHIPTH_INTERNAL_H
#define PTHIPTH_INTERNAL_H

#include "pthipth_internal_type.h"

pid_t __pthipth_gettid();

uint64_t __pthipth_gettime_ms();

void __pthipth_set_thread_time_quota(int ms);

void __pthipth_change_to_state(pthipth_private_t *node, pthipth_state_t to_state);

pthipth_private_t *__pthipth_selfptr();

int __pthipth_dispatcher(pthipth_private_t *node);

void __pthipth_check_sleeping();

void __pthipth_aging();

void *__pthipth_idle(void *phony);

int __pthipth_wrapper(void *arg);

void __pthipth_free(pthipth_private_t *thread);

void __pthipth_check_detach(pthipth_private_t *node);

#endif
