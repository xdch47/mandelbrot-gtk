
#include <math.h>
#include <gtk/gtk.h>
#include "color.h"
#include "avltree.h"

#define COLOR_PERIOD_DEFAULT 40.0

/* color value for the divergent color */
static guchar divcol[3];
static AVLtree iter_tree;
static GMutex *read_mutex;
static GCond *write_done_cond;
static GCond *write_ready_cond;
static guint reader_count;
static guint write_request;

typedef struct _ColorMap {
	guint iter;
	guchar color[3];
} ColorMap;

static const guchar clbluedef[3 * 48] = {
	  0,   0,   0,
	  0,   0,  31,
	  0,   0,  63,
	  0,   0,  95,
	  0,   0, 127,
	  0,   0, 159,
	  0,   0, 191,
	  0,   0, 223,
	  0,   0, 255,
	  0,  31, 255,
	  0,  63, 255,
	  0,  95, 255,
	  0, 127, 255,
	  0, 159, 255,
	  0, 191, 255,
	  0, 223, 255,
	  0, 255, 255,
	 31, 255, 255,
	 63, 255, 255,
	 95, 255, 255,
	127, 255, 255,
	159, 255, 255,
	191, 255, 255,
	223, 255, 255,
	255, 255, 255,
	223, 255, 255,
	191, 255, 255,
	159, 255, 255,
	127, 255, 255,
	 95, 255, 255,
	 63, 255, 255,
	 31, 255, 255,
	  0, 255, 255,
	  0, 223, 255,
	  0, 191, 255,
	  0, 159, 255,
	  0, 127, 255,
	  0,  95, 255,
	  0,  63, 255,
	  0,  31, 255,
	  0,   0, 255,
	  0,   0, 223,
	  0,   0, 191,
	  0,   0, 159,
	  0,   0, 127,
	  0,   0,  95,
	  0,   0,  63,
	  0,   0,  31,
};

const guchar *clblue(const guint iter, const guint itermax)
{
	return (clbluedef + (iter % 48) * 3);
}

void setdivcol(const guchar *color)
{
	divcol[0] = color[0];
	divcol[1] = color[1];
	divcol[2] = color[2];
}

const guchar *divconv(const guint iter, const guint itermax)
{
	return divcol;
}

void init_mb_color_standard_sw()
{
	iter_tree = avl_create();
	read_mutex = g_mutex_new();
	write_ready_cond = g_cond_new();
	write_done_cond = g_cond_new();
	reader_count = 0;
	write_request = 0;
}

void final_mb_color_standard_sw()
{
	avl_free_data(iter_tree);
	g_mutex_free(read_mutex);
	g_cond_free(write_ready_cond);
	g_cond_free(write_done_cond);
}

int cmpColorMap(const ColorMap *cm1, const ColorMap *cm2)
{
	if (cm1->iter < cm2->iter) {
		return -1;
	} else if (cm1->iter == cm2->iter) {
		return 0;
	} else {
		return 1;
	}
}

const guchar *mb_color_standard_sw(const guint iter, const guint itermax)
{
	Node *color_node;
	ColorMap cm;
	guchar res;

	cm.iter = iter;
	g_mutex_lock(read_mutex);
	if (write_request) {
		g_cond_wait(write_done_cond, read_mutex);
	}
	reader_count++;
	g_mutex_unlock(read_mutex);

	color_node = avl_find(iter_tree, &cm, (cmpfunc)cmpColorMap);

	g_mutex_lock(read_mutex);
	reader_count--;
	if (reader_count == 0) {
		g_cond_signal(write_ready_cond);
	}
	g_mutex_unlock(read_mutex);
	if (color_node) {
		return ((ColorMap *)color_node->data)->color;
	} else {
		ColorMap *new_entry;
		long double x;

		new_entry = (ColorMap *)malloc(sizeof(ColorMap));
		new_entry->iter = iter;
		x = sqrt((long double)iter) / sqrt(COLOR_PERIOD_DEFAULT);
		x = 0.5 + 0.5 * cos(x);
		res = (guchar)(255 * x);
		new_entry->color[0] = res;
		new_entry->color[1] = res;
		new_entry->color[2] = res;

		g_mutex_lock(read_mutex);
		write_request++;
		if (reader_count) {
			g_cond_wait(write_ready_cond, read_mutex);
		}
		avl_insert(iter_tree, new_entry, (cmpfunc)cmpColorMap);
		write_request--;
		if (write_request) {
			g_cond_signal(write_ready_cond);
		} else {
			g_cond_broadcast(write_done_cond);
		}
		g_mutex_unlock(read_mutex);

		return new_entry->color;
	}
}
