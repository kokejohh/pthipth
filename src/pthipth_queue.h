// doubly linked list

#ifndef PTHIPTH_QUEUE_H
#define PTHIPTH_QUEUE_H

#include "pthipth_types.h"

typedef struct pthipth_queue
{
    pthipth_private_t *head, *tail;
} pthipth_queue_t;

void pthipth_queue_init(pthipth_queue_t *q, pthipth_private_t *node);

void pthipth_queue_add(pthipth_queue_t *q, pthipth_private_t *node);

void pthipth_queue_delete(pthipth_queue_t *q, pthipth_private_t *node);

void pthipth_queue_display(pthipth_queue_t *q);

#endif
