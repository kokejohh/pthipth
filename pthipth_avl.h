#ifndef PTHIPTH_AVL_H
#define PTHIPTH_AVL_H

#include "pthipth.h"

void pthipth_avl_init(pthipth_private_t *node);

void pthipth_avl_insert(pthipth_private_t *node);

pthipth_private_t *pthipth_avl_search(unsigned long tid);

void pthipth_avl_display();

#endif
