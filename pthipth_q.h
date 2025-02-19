#ifndef PTHIPTH_Q_H
#define PTHIPTH_Q_H

#include "pthipth.h"

void pthipth_q_init(pthipth_private_t *node);

void pthipth_q_add(pthipth_private_t *node);

void pthipth_q_delete(pthipth_private_t *node);

void pthipth_q_display();

pthipth_private_t *pthipth_q_search(unsigned long);

#endif
