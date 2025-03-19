#ifndef PTHIPTH_PRIO_H
#define PTHIPTH_PRIO_H

#include "pthipth.h"

void pthipth_prio_init(pthipth_private_t *node);

void pthipth_prio_insert(pthipth_private_t *node);

void pthipth_prio_reinsert(pthipth_private_t *node);

pthipth_private_t *pthipth_prio_extract_remove();

pthipth_private_t *pthipth_prio_extract();

void pthipth_prio_delete(pthipth_private_t *node);

void pthipth_prio_display();

pthipth_private_t *pthipth_prio_search(unsigned long);

#endif
