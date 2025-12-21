// doubly linked list

#include <stdio.h>

#include "pthipth_queue.h"

pthipth_queue_t blocked_state = {.head = NULL, .tail = NULL};
pthipth_queue_t sleeping_state = {.head = NULL, .tail = NULL};

void pthipth_queue_init(pthipth_queue_t *q, pthipth_private_t *node)
{
    node->next2 = node->prev2 = NULL;

    q->head = q->tail = node;
}

void pthipth_queue_add(pthipth_queue_t *q, pthipth_private_t *node)
{
    if (q->head == NULL || q->tail == NULL)
    {
	pthipth_queue_init(q, node);
	return;
    }

    node->next2 = NULL;
    node->prev2 = q->tail;

    q->tail->next2 = node;
    q->tail = node;
}

void pthipth_queue_delete(pthipth_queue_t *q, pthipth_private_t *node)
{
    if (q->head == NULL || q->tail == NULL) return;

    if (q->head == node && q->tail == node)
	q->head = q->tail = NULL;
    else if (q->head == node)
    {
	q->head = q->head->next2;
	q->head->prev2 = NULL;
    }
    else if (q->tail == node)
    {
	q->tail = q->tail->prev2;
	q->tail->next2 = NULL;
    }
    else
    {
	// (node->prve and node->next2) prev2ent deletion of unqueued nodes
	if (node->prev2) node->prev2->next2 = node->next2;
	if (node->next2) node->next2->prev2 = node->prev2;
    }
    node->next2 = node->prev2 = NULL;
}

void pthipth_queue_display(pthipth_queue_t *q)
{
    printf("display queue ->\n");
    if (q == NULL) return;

    pthipth_private_t *cur = q->head;
    while (cur)
    {
	printf("tid %d prio: %d\n", cur->tid, cur->priority);
	cur = cur->next2;
    }

    printf("end display queue\n");
}
