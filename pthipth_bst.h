#ifndef PTHIPTH_BST_H
#define PTHIPTH_BST_H

#include "pthipth.h"

void pthipth_bst_init(pthipth_private_t *node);

void pthipth_bst_insert(pthipth_private_t *node);

pthipth_private_t *pthipth_bst_search(unsigned long tid);

void pthipth_bst_display();

#endif
