
#include "lookuptable.h"

static int cmpLookUpItem(LookUpItem *it1, LookUpItem *it2)
{
	if (it1->key < it2->key) 
		return -1;
	else if (it1->key == it2->key)
		return 0;
	else
		return 1;
}

void *lookup(AVLtree desc, gint key, LookUpFunc func)
{
	static GStaticRWLock rwlock = G_STATIC_RW_LOCK_INIT;
	Node *node;
	LookUpItem lookupitem;

	lookupitem.key = key;
	g_static_rw_lock_reader_lock(&rwlock);
	node = avl_find(desc, &lookupitem, (cmpfunc)cmpLookUpItem);
	g_static_rw_lock_reader_unlock(&rwlock);

	if (node) {
		return ((LookUpItem *)node->data)->value;
	} else {
		LookUpItem *new_entry;
		new_entry = (LookUpItem *)malloc(sizeof(LookUpItem));
		new_entry->key = key;
		new_entry->value = (*func)(key);

		g_static_rw_lock_writer_lock(&rwlock);
		avl_insert(desc, new_entry, (cmpfunc)cmpLookUpItem);
		g_static_rw_lock_writer_unlock(&rwlock);

		return new_entry->value;
	}
}
