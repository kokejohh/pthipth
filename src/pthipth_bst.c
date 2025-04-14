// binary search tree
// alert: not use anymore
#include <stdio.h>

#include "pthipth_bst.h"

pthipth_private_t *pthipth_bst_root;

void pthipth_bst_init(pthipth_private_t *node)
{
    pthipth_bst_root = node;
    node->left = node->right = NULL;
}

void pthipth_bst_insert(pthipth_private_t *node)
{
    if (pthipth_bst_root == NULL) return pthipth_bst_init(node);

    // clear leafs
    node->left = node->right = NULL;

    pthipth_private_t *cur = pthipth_bst_root;

    while (1)
    {
	// left node
	if (node->tid < cur->tid)
	{
	    if (cur->left == NULL)
	    {
		cur->left = node;
		return;
	    }
	    cur = cur->left;
	}
	// right node
	else if (node->tid > cur->tid)
	{
	    if (cur->right == NULL)
	    {
		cur->right = node;
		return;
	    }
	    cur = cur->right;
	}
	else return;
    }
}

pthipth_private_t *pthipth_bst_search(pid_t tid)
{
    if (pthipth_bst_root == NULL) return NULL;

    pthipth_private_t *cur = pthipth_bst_root;

    while (cur)
    {
	if (tid == cur->tid) return cur;
	else if (tid < cur->tid) cur = cur->left;
	else if (tid > cur->tid) cur = cur->right;
    }
    return NULL;
}

void traversal(pthipth_private_t *node)
{
    if (node == NULL) return;

    traversal(node->left);
    traversal(node->right);
    printf("tid(%d) prio: %d\n", node->tid, node->priority);
}

void pthipth_bst_display()
{
    printf("display binary search tree\n");
    traversal(pthipth_bst_root);
    printf("end display binary search tree\n");
}
