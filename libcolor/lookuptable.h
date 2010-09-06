
#ifndef __LOOKUPTABLE_H__
#define __LOOKUPTABLE_H__

#include "avltree.h"


typedef struct _LookUpItem {
	void *key;
	void *value;
} LookUpItem;

typedef void *(*LookUpFunc)(void *key);

typedef struct _LookUpDescriptor {
	AVLtree tree;
	cmpfunc cmp;
} LookUpDescriptor;

void initialize_lookuptable_mutexes();
void finalize_lookuptable_mutexes();
LookUpDescriptor *ltb_new(cmpfunc cmp);
LookUpItem *ltb_lookup(LookUpDescriptor *desc, LookUpItem *key, LookUpFunc func);
void ltb_free(LookUpDescriptor *desc);

#endif /* __LOOKUPTABLE_H__ */

