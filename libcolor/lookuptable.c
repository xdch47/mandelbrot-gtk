
#include <glib.h>
#include "lookuptable.h"


static guint reader_count = 0;
static guint write_request = 0;
static GMutex *rw_mutex;
static GCond *write_done_cond;
static GCond *write_ready_cond;

void initialize_lookuptable_mutexes() {
	rw_mutex = g_mutex_new();
	write_done_cond = g_cond_new();
	write_ready_cond = g_cond_new();
}

void finalize_lookuptable_mutexes() {
	g_mutex_free(rw_mutex);
	g_cond_free(write_done_cond);
	g_cond_free(write_ready_cond);
}

LookUpDescriptor *ltb_new(cmpfunc cmp)
{
	LookUpDescriptor *desc = (LookUpDescriptor *)malloc(sizeof(LookUpDescriptor));
	desc->tree = avl_create();
	desc->cmp = cmp;
	return desc;
}

void ltb_free(LookUpDescriptor *desc)
{
	avl_free_data(desc->tree);
	free(desc);
}

LookUpItem *ltb_lookup(LookUpDescriptor *desc, LookUpItem *key, LookUpFunc func)
{
	Node *node;
	/* Shared mutex between "readers" and "writers". */

	g_mutex_lock(rw_mutex);
	if (write_request) {
		g_cond_wait(write_done_cond, rw_mutex);
	}
	reader_count++;
	g_mutex_unlock(rw_mutex);

	node = avl_find(desc->tree, &key, desc->cmp);

	g_mutex_lock(rw_mutex);
	reader_count--;
	if (reader_count == 0) {
		g_cond_signal(write_ready_cond);
	}
	g_mutex_unlock(rw_mutex);
	if (node) {
		return (LookUpItem *)node->data;
	} else {
		LookUpItem *new_entry;
		new_entry = (LookUpItem *)malloc(sizeof(LookUpItem));
		new_entry->key = key->key;
		new_entry->value = (*func)(key->key);

		g_mutex_lock(rw_mutex);
		write_request++;
		if (reader_count) {
			g_cond_wait(write_ready_cond, rw_mutex);
		}
		avl_insert(desc->tree, new_entry, desc->cmp);
		write_request--;
		if (write_request) {
			g_cond_signal(write_ready_cond);
		} else {
			g_cond_broadcast(write_done_cond);
		}
		g_mutex_unlock(rw_mutex);

		return new_entry;
	}
}

