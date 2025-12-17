// bucket queue

#include <stdio.h>

#include "pthipth.h"
#include "pthipth_bq.h"

pthipth_private_t *pthipth_bq_head;

pthipth_private_t *pthipth_bq_table[IDLE_PRIORITY + 1] = {0}; //idle is largest value.
pthipth_private_t *pthipth_bq_table_tail[IDLE_PRIORITY + 1] = {0}; //idle is largest value.

void pthipth_bq_insert(pthipth_private_t *node)
{
    int p = node->priority;

    node->cur_priority = p;

    if (pthipth_bq_table[p] == NULL)
    {
	node->prev = node->next = node;

	pthipth_bq_table[p] = node;
	pthipth_bq_table_tail[p] = node;

	if (pthipth_bq_head == NULL || p < pthipth_bq_head->priority)
	{
	    pthipth_bq_head = node;
	}
    }
    else
    {
	pthipth_bq_table[p]->prev = node;
	node->next = pthipth_bq_table[p];
	node->prev = pthipth_bq_table_tail[p];

	pthipth_bq_table_tail[p]->next = node;
	pthipth_bq_table_tail[p] = node;
    }
}

void pthipth_bq_rotate()
{
    pthipth_private_t *cur = pthipth_bq_head;

    if (cur == NULL || cur == cur->next || cur->cur_priority == -1)
	return;

    int p = cur->cur_priority;

    pthipth_bq_table_tail[p] = cur;
    pthipth_bq_head = pthipth_bq_table[p] = cur->next;
}

pthipth_private_t *pthipth_bq_peek()
{
    return pthipth_bq_head;
}

void pthipth_bq_delete(pthipth_private_t *node)
{
    if (node == NULL || node->cur_priority == -1 || pthipth_bq_head == NULL) return;

    node->next->prev = node->prev;
    node->prev->next = node->next;

    int p = node->cur_priority;

    if (node == pthipth_bq_table[p])
    {
	pthipth_bq_table[p] = (pthipth_bq_table[p] == node->next) ? NULL : node->next;
	pthipth_bq_table_tail[p] = (pthipth_bq_table[p] == NULL) ? NULL : pthipth_bq_table_tail[p];

	if (node == pthipth_bq_head)
	{
	    int tmp = 0;
	    for (int i = pthipth_bq_head->cur_priority; i <= IDLE_PRIORITY; i++)
	    {
		if (pthipth_bq_table[i])
		{
		    tmp = i;
		    break;
		}
	    }
	    pthipth_bq_head = (pthipth_bq_table[p] == NULL) ?
				    pthipth_bq_table[tmp] : node->next;
	}
    }
    else if (node == pthipth_bq_table_tail[p])
    {
	pthipth_bq_table_tail[p] = pthipth_bq_table_tail[p]->prev;
    }

    node->next = NULL;
    node->prev = NULL;

    node->cur_priority = -1;
}

void pthipth_bq_reinsert(pthipth_private_t *node)
{
    // delete node
    pthipth_bq_delete(node);
    // insert node again
    pthipth_bq_insert(node);
}

pthipth_private_t *pthipth_bq_search(pid_t tid)
{
    int p = pthipth_bq_head ? pthipth_bq_head->cur_priority : 0;

    for (int i = p; i <= IDLE_PRIORITY; i++)
    {
	pthipth_private_t *head = pthipth_bq_table[i];
	pthipth_private_t *cur = head;
	if (cur == NULL) continue;
	do
	{
	    if (cur->tid == tid) return cur;
	    cur = cur->next;
	} while (cur != head);
    }
    return NULL;
}

void pthipth_bq_display()
{
    printf("display bucket queue ->\n");

    for (int i = 0; i <= IDLE_PRIORITY; i++)
    {
	pthipth_private_t *head = pthipth_bq_table[i];
	pthipth_private_t *cur = head;
	if (cur == NULL) continue;
	printf("priority %d\n", cur->cur_priority);
	do
	{
	    printf("tid %d prio: %d state: %d\n", cur->tid, cur->priority, cur->state);
	    cur = cur->next;
	} while (cur != head);
	printf("--------------\n");
    }

    printf("end display bucket queue\n");
}
