
#ifndef __LOOKUPTABLE_H__
#define __LOOKUPTABLE_H__

#include "avltree.h"
#include <glib.h>

typedef struct _LookUpItem {
	gint key;
	void *value;
} LookUpItem;

typedef void *(*LookUpFunc)(const gint key);

void *lookup(AVLtree desc, gint key, LookUpFunc func);

#endif /* __LOOKUPTABLE_H__ */

