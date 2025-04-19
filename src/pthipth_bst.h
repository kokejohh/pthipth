// binary search tree
// alert: not use anymore

#ifndef PTHIPTH_BST_H
#define PTHIPTH_BST_H

#include "pthipth_internal_type.h"

void pthipth_bst_init(pthipth_private_t *node);

void pthipth_bst_insert(pthipth_private_t *node);

pthipth_private_t *pthipth_bst_search(pid_t tid);

void pthipth_bst_display();

#endif
