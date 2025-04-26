// avl tree (self-balancing binary search tree)

#ifndef PTHIPTH_AVL_H
#define PTHIPTH_AVL_H

#include "pthipth_internal_type.h"

void pthipth_avl_init(pthipth_private_t *node);

void pthipth_avl_insert(pthipth_private_t *node);

void pthipth_avl_delete(pthipth_private_t *node);

pthipth_private_t *pthipth_avl_search(pid_t tid);

void pthipth_avl_display();

#endif
