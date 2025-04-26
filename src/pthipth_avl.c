// avl tree (self-balancing binary search tree)

#include <stdio.h>
#include <stdlib.h>

#include "pthipth_avl.h"

pthipth_private_t *pthipth_avl_root; 

static int getHeight(pthipth_private_t *node)
{
    return (node == NULL) ? 0 : node->height;
}

static void updateHeight(pthipth_private_t *node)
{
    if (node == NULL) return;

    int heightL = getHeight(node->left);
    int heightR = getHeight(node->right);

    node->height = 1 + ((heightL > heightR) ? heightL : heightR);
}

static pthipth_private_t *rightRotate(pthipth_private_t *y)
{
    pthipth_private_t *x = y->left;
    pthipth_private_t *T2 = x->right;

    x->right = y;
    y->left = T2;

    if (T2) T2->parent = y;

    if (y->parent)
    {
	if (y->parent->left == y)
	    y->parent->left = x;
	else
	    y->parent->right = x;
    }

    x->parent = y->parent; 
    y->parent = x;

    updateHeight(y);
    updateHeight(x);

    return x;
}

static pthipth_private_t *leftRotate(pthipth_private_t *x)
{
    pthipth_private_t *y = x->right;
    pthipth_private_t *T2 = y->left;

    y->left = x;
    x->right = T2;

    if (T2) T2->parent = x;

    if (x->parent)
    {
	if (x->parent->right == x)
	    x->parent->right = y;
	else 
	    x->parent->left = y;
    }

    y->parent = x->parent;
    x->parent = y;

    updateHeight(x);
    updateHeight(y);

    return y;
}

static int getBalance(pthipth_private_t *node)
{
    return (node == NULL) ? 0 : getHeight(node->left) - getHeight(node->right);
}

static pthipth_private_t *balance(pthipth_private_t *node)
{
    updateHeight(node);
    int balance = getBalance(node);

    // Left heavy
    if (balance > 1)
    {
	if (getBalance(node->left) < 0) leftRotate(node->left);
	return rightRotate(node);
    }
    // Right heavy
    else if (balance < -1)
    {
	if (getBalance(node->right) > 0) rightRotate(node->right);
	return leftRotate(node);
    }
    return node;
}

void pthipth_avl_init(pthipth_private_t *node)
{
    pthipth_avl_root = node;

    node->parent = node->left = node->right = NULL;
    node->height = 1;
}


void pthipth_avl_insert(pthipth_private_t *node)
{
    if (pthipth_avl_root == NULL) 
    {
	pthipth_avl_init(node);
	return;
    }

    pthipth_private_t *parent, *cur = pthipth_avl_root;

    while (cur)
    {
	parent = cur;
	if (node->tid < cur->tid) cur = cur->left; 
	else if (node->tid > cur->tid) cur = cur->right;
	else return;
    }

    node->parent = parent;
    node->height = 1;
    node->left = node->right = NULL;
    if (node->tid < parent->tid) parent->left = node;
    else parent->right = node;

    while (parent)
    {
	parent = balance(parent);
	if (parent->parent == NULL) pthipth_avl_root = parent;
	parent = parent->parent;
    }
}

void pthipth_avl_delete(pthipth_private_t *node)
{
    if (pthipth_avl_root  == NULL || node == NULL) return;

    pthipth_private_t *cur = node;
    pthipth_private_t *tmp = cur;

    if (node->left && node->right)
    {
	//int right = 1;
	cur = cur->right;
	while (cur->left)
	{
	    cur = cur->left;
	    //right = 0;
	}
	//if (right)
	    //tmp = cur;
	//else
	if (node->right == cur)
	    tmp = cur;
	else
	    tmp = cur->parent;

	if (cur->parent->left == cur)
	    cur->parent->left = cur->right;
	else
	    cur->parent->right = cur->right;


	if (cur->right)
	    cur->right->parent = cur->parent;

	cur->left = node->left;
	if (cur->left) cur->left->parent = cur;

	cur->right = node->right;
	if (cur->right) cur->right->parent = cur;

	cur->parent = node->parent;
	if (node->parent == NULL)
	{
	    pthipth_avl_root = cur;
	}
	else if (node->parent->left == node)
	{
	    node->parent->left = cur;
	}
	else
	{
	    node->parent->right = cur;
	}
    }
    else
    {
	cur = node->left ? node->left : node->right;
	if (cur) cur->parent = node->parent;

	if (node->parent == NULL)
	    pthipth_avl_root = cur;
	else if (node->parent->left == node)
	    node->parent->left = cur;
	else
	    node->parent->right = cur;

	tmp = node->parent;
    }

    node->parent = node->left = node->right = NULL;

    pthipth_private_t *parent = tmp;
    while (parent)
    {
	parent = balance(parent);
	if (parent->parent == NULL) pthipth_avl_root = parent;
	parent = parent->parent;
    }
    free(node);
}

pthipth_private_t *pthipth_avl_search(pid_t tid)
{
    pthipth_private_t *cur = pthipth_avl_root;

    while (cur)
    {
	if (tid == cur->tid) return cur;
	cur = tid < cur->tid ? cur->left : cur->right;
    }
    return cur;
}

void traversal_inorder(pthipth_private_t *node)
{
    if (node == NULL) return;

    traversal_inorder(node->left);
    printf("tid(%d) prio: %d h: %d state: %d\n", node->tid, node->priority, node->height, node->state);
    traversal_inorder(node->right);
}

void pthipth_avl_display()
{
    printf("display avl tree\n");
    traversal_inorder(pthipth_avl_root);
    printf("end display avl tree\n");
}
