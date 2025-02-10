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

    if (y->parent) y->parent->left = x;

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

    if (x->parent) x->parent->right = y;

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

void pthipth_avl_init(pthipth_private_t *node)
{
    pthipth_avl_root = node;

    node->parent = node->left = node->right = NULL;
    node->height = 1;
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

void pthipth_avl_insert(pthipth_private_t *node)
{
    if (pthipth_avl_root == NULL)
    {
	pthipth_avl_init(node);
	return;
    }

    pthipth_private_t *parent, *tmp = pthipth_avl_root;

    while (tmp)
    {
	parent = tmp;
	if (node->tid < tmp->tid) tmp = tmp->left; 
	else if (node->tid > tmp->tid) tmp = tmp->right;
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

pthipth_private_t *pthipth_avl_search(unsigned long tid)
{
    if (pthipth_avl_root == NULL) return NULL;

    pthipth_private_t *tmp = pthipth_avl_root;

    while (tmp)
    {
	if (tid == tmp->tid) return tmp;
	tmp = tid < tmp->tid ? tmp->left : tmp->right;
    }
    return tmp;
}

void traversal_inorder(pthipth_private_t *node)
{
    if (node == NULL) return;

    traversal_inorder(node->left);
    printf("tid(%d) h: %d state: %d\n", node->tid, node->height, node->state);
    traversal_inorder(node->right);
}

void pthipth_avl_display()
{
    printf("display AVL tree\n");
    traversal_inorder(pthipth_avl_root);
}
