#ifndef PTHIPTH_BARRIER_H
#define PTHIPTH_BARRIER_H

#include <stdatomic.h>

typedef struct pthipth_barrier {
    int count;
    atomic_int waiting;
} pthipth_barrier_t;

void pthipth_barrier_init(pthipth_barrier_t *barrier, int count);

int pthipth_barrier_wait(pthipth_barrier_t *barrier);

#endif
