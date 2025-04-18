// doubly linked list

#include <stdio.h>

#include "pthipth_types.h"
#include "pthipth_queue.h"

pthipth_queue_t blocked_state = {.head = NULL, .tail = NULL};
pthipth_queue_t sleeping_state = {.head = NULL, .tail = NULL};

void pthipth_queue_init(pthipth_queue_t *q, pthipth_private_t *node)
{
    node->next = node->prev = NULL;

    q->head = q->tail = node;
}

void pthipth_queue_add(pthipth_queue_t *q, pthipth_private_t *node)
{
    if (q->head == NULL || q->tail == NULL)
    {
	pthipth_queue_init(q, node);
	return;
    }

    node->next = NULL;
    node->prev = q->tail;

    q->tail->next = node;
    q->tail = node;
}

void pthipth_queue_delete(pthipth_queue_t *q, pthipth_private_t *node)
{
    if (q->head == NULL || q->tail == NULL) return;

    if (q->head == node && q->tail == node)
	q->head = q->tail = NULL;
    else if (q->head == node)
    {
	q->head = q->head->next;
	q->head->prev = NULL;
    }
    else if (q->tail == node)
    {
	q->tail = q->tail->prev;
	q->tail->next = NULL;
    }
    else
    {
	// (node->prve and node->next) prevent deletion of unqueued nodes
	if (node->prev) node->prev->next = node->next;
	if (node->next) node->next->prev = node->prev;
    }
    node->next = node->prev = NULL;
}

void pthipth_queue_display(pthipth_queue_t *q)
{
    printf("display queue ->\n");
    if (q == NULL) return;

    pthipth_private_t *cur = q->head;
    while (cur)
    {
	printf("tid %d prio: %d\n", cur->tid, cur->priority);
	cur = cur->next;
    }

    printf("end display queue\n");
}
