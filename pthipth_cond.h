#ifndef PTHIPTH_COND_H
#define PTHIPTH_COND_H

#include "pthipth_mutex.h"

typedef struct pthipth_cond {} pthipth_cond_t;

int pthipth_cond_init(pthipth_cond_t *cond);

int pthipth_cond_wait(pthipth_cond_t *cond, pthipth_mutex_t *mutex);

int pthipth_cond_signal(pthipth_cond_t *cond);

int pthipth_cond_broadcast(pthipth_cond_t *cond);

#endif
