#ifndef PTHIPTH_MUTEX_H
#define PTHIPTH_MUTEX_H

#include "pthipth.h"

typedef struct pthipth_mutex {
    futex_t futx;
    pthipth_t owner;
} pthipth_mutex_t;

int pthipth_mutex_init(pthipth_mutex_t *mutex);

int pthipth_mutex_lock(pthipth_mutex_t *mutex);

int pthipth_mutex_trylock(pthipth_mutex_t *mutex);

int pthipth_mutex_unlock(pthipth_mutex_t *mutex);

#endif
