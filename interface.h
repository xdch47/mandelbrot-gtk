
#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib/gi18n.h>
#include "defs.h"
#include "render.h"

struct winctl {
	/* widgets: */
	GtkWidget *win;
	GtkWidget *drawing;
	GtkWidget *lbldraw;
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
	GtkWidget *mchkzoomprop;
	GtkWidget *pmchkzoomprop;
	GtkWidget **mcolalgo;
	GdkPixbuf *pixbufcalc;
	GdkPixbuf *pixbufshow;
	IterationInfo *itermap;
	/* render thread */
	struct render_thread *render_thread;
	struct iterate_param it_param;
	/* zooming: */
	gint focus_x;
	gint focus_y;
	GdkColor focus_color;
	GdkRectangle focus_area;
	gdouble zoomfactor;
	gboolean zoomprop;
	gint get_jitermax;
	/* config: */
	gchar *configfile;
	double default_cplxplane[4];
	double default_mcplxplane[4];
	/* misc: */
	gboolean get_j;
	gboolean succ_render;
	gboolean redraw;
	/* color: */
	GdkColor convcol;
	GdkColor divcol;
	/* Timer:  */
	GTimer *timer;
};

/* implemented by layout.c: */
GtkWidget *createcplxplane(GtkWidget *txtcplx[4]);
struct winctl *buildinterface(void);
void setcplxplane(GtkWidget *txtcplx[4], const gdouble value[4], gint width, gint height);
void restoredefaults(struct winctl *w);
void statejctl(struct winctl *w);

/* implemented by interface.c: */
void run_interface(gchar *file_name);
void calc(GtkWidget *widget, struct winctl *w);
void reset(GtkWidget *widget, struct winctl *w);
void update_pixbuf(gint x, gint y, const IterationInfo *iterinfo,  const struct iterate_param *it_param);
void redraw_pixbuf(struct winctl *w);

/* implemented by storedrawing.c */
void store_drawing_show(struct winctl *w);

/* implemented by validate.c */
gboolean validate_cplx(GtkWidget *txtcplx[4], gdouble cplx[4], GtkWidget *win);
gboolean validate_itermax(GtkWidget *txtitermax, gint *itermax, GtkWidget *win);
gboolean validate_degree(GtkWidget *txtdegree, gdouble *degree, GtkWidget *win);
void errdialog(GtkWindow *win, gchar *msg);

/* implemented by config.c */
enum configtype { LOAD_CONFIG, STORE_CONFIG, LOAD_RENDER_CONFIG };
gboolean configure_interface(struct winctl *w, const gchar *configfile, enum configtype type);
void save_rendersettings_xml(const gchar *filename, struct winctl *w);

/* implemented by pref.c */
void preference_show(struct winctl *w);

#endif /* __INTERFACE_H__ */

