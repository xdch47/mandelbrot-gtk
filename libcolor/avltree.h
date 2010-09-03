
#ifndef __AVLTREE_H__
#define __AVLTREE_H__

#include <stdlib.h>

typedef struct _Node {
	void *data;
	struct _Node *left;
	struct _Node *right;
	struct _Node *parent;
	char balance; 
	char descend; /* -1 => left node, 1 => right node, 0 => root node */
} Node;

typedef Node **AVLtree;

typedef int (*cmpfunc)(const void *, const void *);

AVLtree avl_create();

void *avl_insert(AVLtree tree, void *data, cmpfunc cmp);
Node *avl_find(AVLtree tree, void *data, cmpfunc cmp);
void *avl_delete(AVLtree tree, Node *node);

void avl_free(AVLtree tree);
void avl_free_data(AVLtree tree);
#endif

