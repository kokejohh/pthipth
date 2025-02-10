#include "pthipth_q.h"

pthipth_private_t *pthipth_q_head;

void pthipth_q_init(pthipth_private_t *node)
{
    node->prev = node;
    node->next = node;

    pthipth_q_head = node;
}

void pthipth_q_add(pthipth_private_t *node)
{
    if (pthipth_q_head == NULL)
    {
	pthipth_q_init(node);
	return;
    }

    node->next = pthipth_q_head;
    node->prev = pthipth_q_head->prev;

    pthipth_q_head->prev->next = node;
    pthipth_q_head->prev = node;
}

void pthipth_q_delete(pthipth_private_t *node)
{
    if (node == pthipth_q_head) pthipth_q_head = pthipth_q_head->next;

    //pthipth_private_t *p = node->prev;

    node->prev->next = node->next;

    node->next->prev = node->prev;

//    free(node);
}

void pthipth_q_state_display()
{
    if (pthipth_q_head == NULL) return;

    printf("\n The Q contents are => \n");
    
    pthipth_private_t *p = pthipth_q_head;
    do
    {
	//printf(" %d\n", p->state);
	printf(" %d prio: %d\n", p->tid, p->priority);
	p = p->next;
    } while (p != pthipth_q_head);
}

pthipth_private_t *pthipth_q_search(unsigned long new_tid)
{
    pthipth_private_t *p = pthipth_q_head;
    do
    {
	if (p->tid == new_tid) return p;
	p = p->next;
    } while (p != pthipth_q_head);
    return NULL;
}
