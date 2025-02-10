#include "pthipth_bst.h"

pthipth_private_t *pthipth_bst_root;


void pthipth_bst_init(pthipth_private_t *node)
{
    pthipth_bst_root = node;
    node->left = node->right = NULL;
}

void pthipth_bst_insert(pthipth_private_t *node)
{
    if (pthipth_bst_root == NULL)
    {
	pthipth_bst_init(node);
	return;
    }

    pthipth_private_t *tmp = pthipth_bst_root;
    while (1)
    {
	if (node->tid < tmp->tid)
	{
	    if (tmp->left == NULL)
	    {
		tmp->left = node;
		break;
	    }
	    else
	    {
		tmp = tmp->left;
	    }
	}
	else if (node->tid > tmp->tid)
	{
	    if (tmp->right == NULL)
	    {
		tmp->right = node;
		break;
	    }
	    else
	    {
		tmp = tmp->right;
	    }
	}
	else
	{
	    break;
	}
    }
}

pthipth_private_t *pthipth_bst_search(unsigned long tid)
{
    if (pthipth_bst_root == NULL) return NULL;

    pthipth_private_t *tmp = pthipth_bst_root;

    while (1)
    {
	if (tmp->tid == tid) return tmp;

	if (tmp->tid > tid) tmp = tmp->left;
	else if (tmp->tid < tid) tmp = tmp->right;
	if (tmp == NULL) break;
    }
    return NULL;
}
void traversal(pthipth_private_t *node)
{
    if (node == NULL) return;
    traversal(node->left);
    printf("tid(%d) prio: %d\n", node->tid, node->priority);
    traversal(node->right);
}

void pthipth_bst_display()
{
    printf("display binary search\n");
    traversal(pthipth_bst_root);
}
