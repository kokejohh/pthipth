// bucket queue

#ifndef PTHIPTH_PRIO_H
#define PTHIPTH_PRIO_H

#include "pthipth_types.h"

void pthipth_prio_init(pthipth_private_t *node);

void pthipth_prio_insert(pthipth_private_t *node);

void pthipth_prio_reinsert(pthipth_private_t *node);

void pthipth_prio_delete(pthipth_private_t *node);

// unused
pthipth_private_t *pthipth_prio_extract_remove();

pthipth_private_t *pthipth_prio_extract();

pthipth_private_t *pthipth_prio_search(pid_t tid);

void pthipth_prio_display();

#endif
