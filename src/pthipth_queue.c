#include "pthipth_queue.h"

pthipth_queue_t blocked_state;
pthipth_queue_t sleeping_state;

void pthipth_queue_init(pthipth_queue_t *q, pthipth_private_t *node)
{
    node->next = node->prev = NULL;

    q->head = q->tail = node;
}

void pthipth_queue_add(pthipth_queue_t *q, pthipth_private_t *node)
{
    if (q->head == NULL)
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
    if (q->head == NULL || q->tail == NULL)
	return;

    if (q->head == node && q->tail == node)
    {
	q->head = q->tail = NULL;
    }
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
	node->prev->next = node->next;
	node->next->prev = node->prev;
    }
}

void pthipth_queue_display(pthipth_queue_t *q)
{
    if (q->head == NULL)
    {
	printf("queue nothing\n");
	return;
    }

    printf("\n The Queue contents are => \n");

    pthipth_private_t *tmp = q->head;
    while (tmp)
    {
	printf(" %d prio: %d\n", tmp->tid, tmp->priority);
	tmp = tmp->next;
    }
}
