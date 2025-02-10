#ifndef PTHIPTH_MUTEX_H
#define PTHIPTH_MUTEX_H

#include "pthipth.h"

typedef struct pthipth_mutex {
    futex_t futx;
    pthipth_t owner;
} pthipth_mutex_t;

void pthipth_mutex_init(pthipth_mutex_t *mutex);

void pthipth_mutex_lock(pthipth_mutex_t *mutex);

void pthipth_mutex_unlock(pthipth_mutex_t *mutex);

#endif
