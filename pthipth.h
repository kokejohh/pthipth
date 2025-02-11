/*
 * pthipth.h - interface of user threads library
 */

#ifndef PTHIPTH_H
#define PTHIPTH_H 

#include <stdio.h>
#include <stdlib.h>
#include "futex.h"
#include "pthipth_debug.h"

enum student {
    RUNNING, READY, BLOCKED, DEFUNCT
};

typedef struct pthipth_attr {
    unsigned long stackSize;
    unsigned priority;
} pthipth_attr_t;

typedef struct pthipth {
    pid_t tid;
} pthipth_t;

typedef struct pthipth_private {
    pid_t tid;
    int state;
    void *(*start_func) (void *);
    void *args;
    void *return_value;
    struct pthipth_private *blockedForJoin;
    futex_t sched_futex;
    int priority, init_priority, tmp_priority;
    // Bucket queue
    struct pthipth_private *prev, *next, *inside_prev, *inside_next;
    // AVL tree
    struct pthipth_private *parent, *left, *right;
    int height;
} pthipth_private_t;

pthipth_t pthipth_self(void);

int pthipth_create(pthipth_t *new_thread_ID, pthipth_attr_t *attr, void *(start_func)(void *), void *arg);

int pthipth_yield(void);

int pthipth_join(pthipth_t target_thread, void **status);

void pthipth_exit(void *retval);

pid_t __pthipth_gettid();

pthipth_private_t *__pthipth_selfptr();

int __pthipth_dispatcher(pthipth_private_t *, int killed);

#endif
