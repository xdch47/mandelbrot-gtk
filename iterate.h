
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
	gint xstart;
	gint ystart;
	gint xoffset;
	gint yoffset;
	gint xmax;
	gint ymax;
	gint itermax;
	double degree;
	guchar *pixels;
	gint n_channels;
	gint rowstride;
	guchar *convdivcol;
	void (*setcolor)(guchar *, gint , gint , gint , gint , gint, gint,  guchar *);
	GMutex *state_mutex;
	GCond *resume_cond;
	volatile gint *pause;
};

gpointer mandelbrot_set(struct threaddata *p);
gpointer mandelbrot_set_deg(struct threaddata *p);
gpointer julia_set(struct threaddata *p);
gpointer julia_set_deg(struct threaddata *p);

#endif /* __ITERATE_H__ */

