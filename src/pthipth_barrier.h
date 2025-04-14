#ifndef PTHIPTH_BARRIER_H
#define PTHIPTH_BARRIER_H

#include "pthipth_types.h"

#define PTHIPTH_BARRIER_SERIAL_THREAD 1

typedef struct pthipth_barrier {
    int count;
    atomic_int waiting;
} pthipth_barrier_t;

int pthipth_barrier_init(pthipth_barrier_t *barrier, int count);

int pthipth_barrier_wait(pthipth_barrier_t *barrier);

#endif
