
#include "avltree.h"

static void init_node(Node *node, void *data, char descend)
{
	node->left = NULL;
	node->right = NULL;
	node->data = data;
	node->balance = 0;
	node->descend = descend;
}

static Node *rotateleft(Node *node)
{
	int ntmpbal;
	int descend;
	Node *newhead;
	Node *chnode; /* Node that change the parent */
	Node *parent;

	newhead = node->right;
	descend = newhead->descend = node->descend;
	parent = newhead->parent = node->parent;
	if (descend == -1)
		parent->left = newhead;
	else if (descend == 1)
		parent->right = newhead;
	node->parent = newhead;
	node->descend = -1;
	
	chnode = newhead->left;
	node->right = chnode;
	if (chnode) {
		chnode->parent = node;
		chnode->descend = 1;
	}
	
	newhead->left = node; 

	ntmpbal = newhead->balance;
	newhead->balance = node->balance-- - 2;
	if (ntmpbal <= 0)
		newhead->balance += ntmpbal - node->balance;
	else
		node->balance -= ntmpbal;

	return newhead;
}

static Node *rotateright(Node *node)
{
 	int ntmpbal;
	int descend;
  	Node *newhead;
	Node *chnode; /* Element that change the parent */
	Node *parent;
	
	newhead = node->left;
	descend = newhead->descend = node->descend;
	parent = newhead->parent = node->parent;
	if (descend == -1)
		parent->left = newhead;
	else if (descend == 1)
		parent->right = newhead;

	node->parent = newhead;
	node->descend = 1;

	chnode = newhead->right;
  	node->left = chnode;
	if (chnode) {
		chnode->parent = node;
		chnode->descend = -1;
	}

  	newhead->right = node;
  
  	ntmpbal = newhead->balance;
  	newhead->balance = node->balance++ + 2;
  	if (ntmpbal >= 0)
  		 newhead->balance += ntmpbal - node->balance;
	else 
  		node->balance -= ntmpbal;
 
  	return newhead;
}

static Node *rotate(Node *node)
{
	if (node->balance == -2) {
		if (node->left->balance == 1) 
			node->left = rotateleft(node->left);
		return rotateright(node);
	} else {
		if (node->right->balance == -1) 
			node->right = rotateright(node->right);
		return rotateleft(node);
	}
}

static void update_balance_insert(AVLtree tree, Node *itparent, int descend)
{
	int balance;
	Node *rothead;

	while (itparent) {
		balance = (itparent->balance += descend);
		if (balance == 0) {
			return;
		} else if (balance == 2 || balance == -2) {
			rothead = rotate(itparent);

			if (rothead->descend == 0)
				*tree = rothead;
			return;
		} else {
			descend = itparent->descend;
			itparent = itparent->parent;
		}
	}
}

static void update_balance_delete(AVLtree tree, Node *itparent, int descend)
{
	int balance;
	Node *rothead;

	while (itparent) {
		balance = (itparent->balance -= descend);
		if (balance == -1 || balance == 1) {
			return;
		} else if (balance == -2 || balance == 2) {
			rothead = rotate(itparent);

			if (rothead->descend == 0) {
				*tree = rothead;
				return;
			}
			itparent = rothead;
			if (rothead->balance == -1 || rothead->balance == 1)
				return;
		}
		descend = itparent->descend;
		itparent = itparent->parent;
	}
}

void *avl_insert(AVLtree tree, void *data, cmpfunc cmp)
{
	Node *it;
	Node *newnode;

	if (*tree == NULL) {
		*tree = (Node *)malloc(sizeof(Node));
		init_node(*tree, data, 0);
		(*tree)->parent = NULL;
		return NULL;
	}

	it = *tree;
	for (;;) {
		switch ((*cmp)(data, it->data)) {
			case -1:
				if (it->left) {
					it = it->left;
				} else {
					newnode = it->left = (Node *)malloc(sizeof(Node));
					init_node(newnode, data, -1);
					newnode->parent = it;
					update_balance_insert(tree, it, -1);
					return NULL;
				}
				break;
			case 1:
				if (it->right) {
					it = it->right;
				} else {
					newnode = it->right = (Node *)malloc(sizeof(Node));
					init_node(newnode, data, 1);
					newnode->parent = it;
					update_balance_insert(tree, it, 1);
					return NULL;
				}
				break;
			default: /* case 0: Element already exists. */
				return it;
		}
	}

}

Node *avl_find(AVLtree tree, void *data, cmpfunc cmp)
{
	Node *retval = *tree;
	while (retval) {
		switch ((*cmp)(data, retval->data)) {
			case -1:
				retval = retval->left;
				break;
			case 1:
				retval = retval->right;
				break;
			default: /* case 0: */
				return retval;
		}
	}

	/* Element not found */
	return NULL;
}

void *avl_delete(AVLtree tree, Node *node)
{
	Node *leaf; /* The leaf node will take the place of the delete node */
	void *data;

	data = node->data;
	/* Find leaf node and replace node with leaf node */
	if (node->balance <= 0) {
		if (node->left) {
			leaf = node->left;
			if (leaf->right) {
				do {
					leaf = leaf->right;
				} while (leaf->right);
				leaf->parent->right = leaf->left;
				if (leaf->left) {
					leaf->left->descend = 1;
					leaf->left->parent = leaf->parent;
				}
			} else {
				node->left = leaf->left;
				if (leaf->left) {
					leaf->left->parent = node;
				}
			}
		} else {
			/* The node which should be delete is a leaf node */
			if (node->descend == 0) {
				*tree = NULL;
			} else {
				if (node->descend == -1)
					node->parent->left = NULL;
				else
					node->parent->right = NULL;

				update_balance_delete(tree, node->parent, node->descend);
			}
			free(node);
			return data;
		}
	} else {
		leaf = node->right;
		if (leaf->left) {
			do {
				leaf = leaf->left;
			} while (leaf->left);
			leaf->parent->left = leaf->right;
			if (leaf->right) {
				leaf->right->descend = -1;
				leaf->right->parent = leaf->parent;
			}
		} else {
			node->right = leaf->right;
			if (leaf->right) {
				leaf->right->parent = node;
			}
		}
	}

	node->data = leaf->data;
	update_balance_delete(tree, leaf->parent, leaf->descend);
	free(leaf);
	return data;
}

AVLtree avl_create() 
{
	AVLtree tree = (AVLtree)malloc(sizeof(Node *));
	*tree = NULL;
	return tree;
}


static void free_all_nodes(Node *node)
{
	if (node) {
		if (node->left)
			free_all_nodes(node->left);
		if (node->right)
			free_all_nodes(node->right);
		free(node);
	}
}

void avl_free(AVLtree tree)
{
	if (tree) {
		free_all_nodes(*tree);
		free(tree);
	}
}

static void free_all_nodes_data(Node *node)
{
	if (node) {
		if (node->left)
			free_all_nodes_data(node->left);
		if (node->right)
			free_all_nodes_data(node->right);

		free(node->data);
		free(node);
	}
}

void avl_free_data(AVLtree tree)
{
	if (tree) {
		free_all_nodes_data(*tree);
		free(tree);
	}
}
