// bucket queue

#include <stdio.h>

#include "pthipth_types.h"
#include "pthipth_prio.h"

pthipth_private_t *pthipth_prio_head;

void pthipth_prio_init(pthipth_private_t *node)
{
    pthipth_prio_head = node;

    node->next = NULL;
    node->prev = NULL;

    node->inside_next = node;
    node->inside_prev = node;
}

static void prio_insert_new_bucket(pthipth_private_t *node, pthipth_private_t *cur)
{
    node->inside_next = node->inside_prev = node;

    node->next = cur;
    node->prev = cur->prev;

    // link the previous node back if it exists
    if (cur->prev)
	cur->prev->next = node;

    cur->prev = node;

    if (cur == pthipth_prio_head)
	pthipth_prio_head = node;
}

static void prio_insert_inside_bucket(pthipth_private_t *node, pthipth_private_t *cur)
{
    node->next = node->prev = NULL;

    node->inside_next = cur;
    node->inside_prev = cur->inside_prev;

    cur->inside_prev->inside_next = node;
    cur->inside_prev = node;
}

static void prio_insert_last_bucket(pthipth_private_t *node, pthipth_private_t *cur)
{
    node->inside_next = node->inside_prev = node;

    cur->next = node;

    node->prev = cur;
    node->next = NULL;
}

void pthipth_prio_insert(pthipth_private_t *node)
{
    if (pthipth_prio_head == NULL)
    {
	pthipth_prio_init(node);
	return;
    }

    pthipth_private_t *cur = pthipth_prio_head;

    while (cur)
    {
	if (node->priority == cur->priority)
	{
	    prio_insert_inside_bucket(node, cur);
	    return;
	}
	else if (node->priority < cur->priority)
	{
	    prio_insert_new_bucket(node, cur);
	    return;
	}
	if (cur->next == NULL)
	{
	    prio_insert_last_bucket(node, cur);
	    return;
	}
	cur = cur->next;
    }
}

// unused
pthipth_private_t *pthipth_prio_extract_remove()
{
    pthipth_private_t *cur = pthipth_prio_head;
    if (cur->inside_next == cur)
    {
	pthipth_prio_head = cur->next;
	if (pthipth_prio_head == NULL)
	{
	    pthipth_prio_head = cur;
	    return pthipth_prio_head;
	}
	pthipth_prio_head->prev = NULL;
    }
    else
    {
	cur->inside_next->inside_prev = cur->inside_prev;
	cur->inside_prev->inside_next = cur->inside_next;

	pthipth_prio_head = cur->inside_next;

	pthipth_prio_head->next = cur->next;
	pthipth_prio_head->prev = NULL;
	cur->next = cur->prev = NULL;
	cur->inside_next = cur->inside_prev = cur;
    }
    return cur;
}

pthipth_private_t *pthipth_prio_extract()
{
    pthipth_private_t *cur = pthipth_prio_head;

    if (cur == NULL || cur == cur->inside_next)
	return cur;

    pthipth_prio_head = cur->inside_next;
    pthipth_prio_head->next = cur->next;

    // link the next node back if it exists
    if (pthipth_prio_head->next)
	pthipth_prio_head->next->prev = pthipth_prio_head;

    pthipth_prio_head->prev = NULL;
    cur->next = cur->prev = NULL;

    return cur;
}

void pthipth_prio_delete(pthipth_private_t *node)
{
    node->inside_next->inside_prev = node->inside_prev;
    node->inside_prev->inside_next = node->inside_next;

    // has next node
    if (node->next)
    {
	node->inside_next->next = node->next;
	node->inside_next->prev = node->prev;

	node->next->prev = (node == node->inside_next) ?
			    node->prev : node->inside_next;
    }

    // has previous node
    if (node->prev)
	// link previous node next
	node->prev->next = (node == node->inside_next) ?
			    node->next : node->inside_next;

    if (node == pthipth_prio_head)
	pthipth_prio_head = (pthipth_prio_head == node->inside_next) ?
	    node->next : node->inside_next;
}

void pthipth_prio_reinsert(pthipth_private_t *node)
{
    // delete node
    pthipth_prio_delete(node);
    // insert node again
    pthipth_prio_insert(node);
}

pthipth_private_t *pthipth_prio_search(pid_t tid)
{
    pthipth_private_t *cur = pthipth_prio_head;

    while (cur)
    {
	pthipth_private_t *inside_cur = cur;
	do
	{
	    if (inside_cur->tid == tid)
		return inside_cur;
	    inside_cur = inside_cur->inside_next;
	} while (inside_cur != cur);
	cur = cur->next;
    }

    return NULL;
}

void pthipth_prio_display()
{
    printf("display bucket queue ->\n");

    pthipth_private_t *cur = pthipth_prio_head;
    while (cur)
    {
	printf("priority %d\n", cur->priority);
	pthipth_private_t *inside_cur = cur;
	do
	{
	    printf("tid %d prio: %d state: %d\n", inside_cur->tid, inside_cur->priority, inside_cur->state); 
	    inside_cur = inside_cur->inside_next;    
	} while (inside_cur != cur);
	cur = cur->next;
	printf("--------------\n");
    }
    printf("end display bucket queue\n");
}
