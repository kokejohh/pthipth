#include "pthipth_prio.h"

// Bucket Queue
pthipth_private_t *pthipth_prio_head;

typedef enum actions { insert, inside, addLast } action_t;

void pthipth_prio_init(pthipth_private_t *node)
{
    pthipth_prio_head = node;

    node->next = NULL;
    node->prev = NULL;

    node->inside_next = node;
    node->inside_prev = node;
}

void pthipth_prio_insert(pthipth_private_t *node)
{
    if (pthipth_prio_head == NULL)
    {
	pthipth_prio_init(node);
	return;
    }

    action_t action;

    pthipth_private_t *tmp = pthipth_prio_head;
    while (1)
    {
	if (node->priority == tmp->priority)
	{
	    action = inside;
	    break;
	}
	else if (node->priority < tmp->priority)
	{
	    action = insert;
	    break;
	}
	if (tmp->next == NULL)
	{
	    action = addLast;
	    break;
	}
	tmp = tmp->next;
    }

    if (action == inside)
    {
	node->next = NULL;
	node->prev = NULL;

	node->inside_next = tmp;
	node->inside_prev = tmp->inside_prev;

	tmp->inside_prev->inside_next = node;
	tmp->inside_prev = node;
    }
    else if (action == insert)
    {
	node->inside_next = node;
	node->inside_prev = node;

	if (node->priority < pthipth_prio_head->priority)
	{
	    node->prev = NULL;

	    node->next = pthipth_prio_head;
	    pthipth_prio_head->prev = node;
	    pthipth_prio_head = node;

	    return;
	}

	node->next = tmp;
	node->prev = tmp->prev;

	tmp->prev->next = node;
	tmp->prev = node;
    }
    else if (action == addLast)
    {
	node->inside_next = node;
	node->inside_prev = node;

	tmp->next = node;
	node->prev = tmp;

	node->next = NULL;
    }
}

// unused
pthipth_private_t *pthipth_prio_extract_remove()
{
    pthipth_private_t *tmp = pthipth_prio_head;
    if (tmp->inside_next == tmp)
    {
	pthipth_prio_head = tmp->next;
	if (pthipth_prio_head == NULL)
	{
	    pthipth_prio_head = tmp;
	    return pthipth_prio_head;
	}
	pthipth_prio_head->prev = NULL;
    }
    else
    {
	tmp->inside_next->inside_prev = tmp->inside_prev;
	tmp->inside_prev->inside_next = tmp->inside_next;

	pthipth_prio_head = tmp->inside_next;

	pthipth_prio_head->next = tmp->next;
	pthipth_prio_head->prev = NULL;
	tmp->next = tmp->prev = NULL;
    }
    return tmp;
}

pthipth_private_t *pthipth_prio_extract()
{
    pthipth_private_t *tmp = pthipth_prio_head;

    if (tmp->inside_next == tmp) return tmp;

    pthipth_prio_head = tmp->inside_next;

    pthipth_prio_head->next = tmp->next;
    pthipth_prio_head->next->prev = tmp->inside_next;
    pthipth_prio_head->prev = NULL;
    tmp->next = tmp->prev = NULL;

    return tmp;
}

void pthipth_prio_delete(pthipth_private_t *node)
{
    node->inside_next->inside_prev = node->inside_prev;
    node->inside_prev->inside_next = node->inside_next;

    if (node->next)
    {
	node->inside_next->next = node->next;
	node->inside_next->prev = node->prev;

	node->next->prev = (node->inside_next == node) ?
			    node->prev : node->inside_next;
    }

    if (node->prev)
	node->prev->next = (node->inside_next == node) ?
			    node->next : node->inside_next;

    if (pthipth_prio_head == node)
	pthipth_prio_head = (pthipth_prio_head == node->inside_next) ?
	    node->next : node->inside_next;
}

void pthipth_prio_reinsert(pthipth_private_t *node)
{
    // delete node
    pthipth_prio_delete(node);
    // insert node 
    pthipth_prio_insert(node);
}

pthipth_private_t *pthipth_prio_search(unsigned long tid)
{
    pthipth_private_t *tmp = pthipth_prio_head;

    do
    {
	pthipth_private_t *inside_tmp = tmp;
	do
	{
	    if (inside_tmp->tid == tid)
		return inside_tmp;
	    inside_tmp = inside_tmp->inside_next;
	} while (inside_tmp != tmp);
	tmp = tmp->next;
    } while (tmp != NULL);

    return NULL;
}

void pthipth_prio_display()
{
    if (pthipth_prio_head == NULL) return;

    printf("Display Bucket Queue ->\n");

    pthipth_private_t *tmp = pthipth_prio_head;
    do
    {
	printf("priority %d\n", tmp->priority);
	pthipth_private_t *inside_tmp = tmp;
	do
	{
	    printf("tid %d prio: %d futex: %d state: %d\n", inside_tmp->tid, inside_tmp->priority, inside_tmp->sched_futex.count, inside_tmp->state); 
	    inside_tmp = inside_tmp->inside_next;    
	} while (inside_tmp != tmp);
	tmp = tmp->next;
	printf("--------------\n");
    } while (tmp != NULL);
}
