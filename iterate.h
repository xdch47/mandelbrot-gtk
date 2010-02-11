
#ifndef __ITERATE_H__
#define __ITERATE_H__

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

struct threaddata {
	gdouble b_re; 
	gdouble b_im;
	gdouble dre;
	gdouble dim;
	gdouble jre;
	gdouble jim;
	guint xstart;
	guint ystart;
	guint xoffset;
	guint yoffset;
	guint xmax;
	guint ymax;
	guint itermax;
	double degree;
	guchar *pixels;
	guint n_channels;
	guint rowstride;
	guchar *convdivcol;
	void (*setcolor)(guchar *, guint , guint , guint , guint , guint, guint,  guchar *);
	GMutex *state_mutex;
	GCond *resume_cond;
	volatile gint *pause;
};

gpointer mandelbrot_set(struct threaddata *p);
gpointer mandelbrot_set_deg(struct threaddata *p);
gpointer julia_set(struct threaddata *p);
gpointer julia_set_deg(struct threaddata *p);

#endif /* __ITERATE_H__ */

