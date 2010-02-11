
#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "defs.h"
#include "iterate.h"


struct winctl {
	// widgets:
	GtkWidget *win;
	GtkWidget *drawing;
	GtkWidget *txtcplx[4];
	GtkWidget *txtdegree;
	GtkWidget *txtitermax;
	GtkWidget *chkjulia;
	GtkWidget *lbljre;
	GtkWidget *lbljim;
	GtkWidget *txtjre;
	GtkWidget *txtjim;
	GtkWidget *btnj;
	GtkWidget *btncalc;
	GtkWidget *btnreset;
	GtkWidget *btnclose;
	GtkWidget *drawmenu;
	GtkWidget *pjmenu;
	GtkWidget *mchkxyscale;
	GtkWidget *pmchkxyscale;
	GtkWidget *mcolalgo[COLORFUNC];
	GdkPixbuf *pixbufcalc;
	GdkPixbuf *pixbufshow;
	GdkColor convcol;
	GdkColor divcol;
	// zooming:
	gint focus_x;
	gint focus_y;
	GdkColor focus_color;
	GdkRectangle focus_area;
	gdouble scalefactor;
	gdouble cplx[4];
	gboolean xyscale;
	// threads:
	GThread *render_thread;
	GMutex *thread_control_mutex;
	GMutex *threadstate_mutex;
	GMutex *redraw_mutex;
	GCond *resume_cond;
	volatile gboolean runthread;
	volatile gint pausethread;
	gint redraw_idle;
	struct threaddata data[CALCTHREADS];
	gint color_func_index;
	// config:
	gchar *configfile;
	// misc:
	gboolean mandelbrot;
	gboolean get_j;
};

void run_interface(gchar *file_name);
void calc(GtkWidget *widget, struct winctl *w);
void reset(GtkWidget *widget, struct winctl *w);

#endif /* __INTERFACE_H__ */

