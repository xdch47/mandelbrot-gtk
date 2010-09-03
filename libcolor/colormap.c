
#include <gtk/gtk.h>
#include "colormap.h"

typedef struct _ColorMapItem {
	guint iter;
	const guchar *color;
} ColorMapItem;

static guint reader_count = 0;
static guint write_request = 0;
static GMutex *rw_mutex;
static GCond *write_done_cond;
static GCond *write_ready_cond;

void initialize_colormap_mutexes() {
	rw_mutex = g_mutex_new();
	write_done_cond = g_cond_new();
	write_ready_cond = g_cond_new();
}

void finalize_colormap_mutexes() {
	g_mutex_free(rw_mutex);
	g_cond_free(write_done_cond);
	g_cond_free(write_ready_cond);
}

static int cmpColorMapItem(const ColorMapItem *cmi1, const ColorMapItem *cmi2)
{
	if (cmi1->iter < cmi2->iter) {
		return -1;
	} else if (cmi1->iter == cmi2->iter) {
		return 0;
	} else {
		return 1;
	}
}

const guchar *getColor(AVLtree tree, ColorFunc colorfunc, const guint iter, const guint itermax)
{
	Node *color_node;
	ColorMapItem item;
	/* Shared mutex between "readers" and "writers". */

	item.iter = iter;
	g_mutex_lock(rw_mutex);
	if (write_request) {
		g_cond_wait(write_done_cond, rw_mutex);
	}
	reader_count++;
	g_mutex_unlock(rw_mutex);

	color_node = avl_find(tree, &item, (cmpfunc)cmpColorMapItem);

	g_mutex_lock(rw_mutex);
	reader_count--;
	if (reader_count == 0) {
		g_cond_signal(write_ready_cond);
	}
	g_mutex_unlock(rw_mutex);
	if (color_node) {
		return ((ColorMapItem *)color_node->data)->color;
	} else {
		ColorMapItem *new_entry;
		new_entry = (ColorMapItem *)malloc(sizeof(ColorMapItem));
		new_entry->iter = iter;
		new_entry->color = (*colorfunc)(iter, itermax);

		g_mutex_lock(rw_mutex);
		write_request++;
		if (reader_count) {
			g_cond_wait(write_ready_cond, rw_mutex);
		}
		avl_insert(tree, new_entry, (cmpfunc)cmpColorMapItem);
		write_request--;
		if (write_request) {
			g_cond_signal(write_ready_cond);
		} else {
			g_cond_broadcast(write_done_cond);
		}
		g_mutex_unlock(rw_mutex);

		return new_entry->color;
	}
}


