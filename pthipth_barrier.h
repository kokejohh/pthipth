#ifndef PTHIPTH_BARRIER_H
#define PTHIPTH_BARRIER_H

typedef struct pthipth_barrier {
    int count;
} pthipth_barrier_t;

void pthipth_barrier_init(pthipth_barrier_t *barrier);

int pthipth_barrier_wait(pthipth_barrier_t *barrier);

#endif
