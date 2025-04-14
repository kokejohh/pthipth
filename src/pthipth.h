/*
 * pthipth.h - interface of user threads library
 */

#ifndef PTHIPTH_H
#define PTHIPTH_H 

#include "pthipth_types.h"

#define HIGHEST_PRIORITY 1
#define DEFAULT_PRIORITY 28
#define LOWEST_PRIORITY 29
#define IDLE_PRIORITY 30
#define MAIN_PRIORITY 31

#define TIME_SLICE 500

// create
int pthipth_create(pthipth_t *new_thread_ID, pthipth_attr_t *attr, pthipth_task_t *task);

// yield
void pthipth_yield(void);

// yield quota
void pthipth_yieldq(uint64_t ms);

// join
int pthipth_join(pthipth_t target_thread, void **status);

// exit
void pthipth_exit(void *retval);

// sleep
void pthipth_sleep(uint64_t millisec);

// self
pthipth_t pthipth_self(void);

#include "pthipth_mutex.h"
#include "pthipth_cond.h"
#include "pthipth_barrier.h"
#include "pthipth_pool.h"

pid_t __pthipth_gettid();

uint64_t __pthipth_gettime_ms();

void __pthipth_set_thread_time_quota(int ms);

void __pthipth_change_to_state(pthipth_private_t *node, pthipth_state_t to_state);

pthipth_private_t *__pthipth_selfptr();

void __pthipth_change_to_state(pthipth_private_t *node, pthipth_state_t to_state);

int __pthipth_dispatcher(pthipth_private_t *);

void __pthipth_check_sleeping();

void __pthipth_aging(int aging_factor);

void *__pthipth_idle(void *phony);

int __pthipth_wrapper(void *arg);

#endif
