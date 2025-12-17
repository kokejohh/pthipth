// bucket queue

#ifndef PTHIPTH_PRIO_H
#define PTHIPTH_PRIO_H

#include "pthipth_internal_type.h"

void pthipth_bq_insert(pthipth_private_t *node);

void pthipth_bq_reinsert(pthipth_private_t *node);

void pthipth_bq_delete(pthipth_private_t *node);

void pthipth_bq_rotate();

pthipth_private_t *pthipth_bq_peek();

// unused
pthipth_private_t *pthipth_bq_extract();

pthipth_private_t *pthipth_bq_search(pid_t tid);

void pthipth_bq_display();

#endif
