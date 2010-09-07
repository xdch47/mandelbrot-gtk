
#ifndef __VALIDATE_H__
#define __VALIDATE_H__

#include <gtk/gtk.h>

gboolean validatecplx(GtkWidget *txtcplx[4], gdouble cplx[4], GtkWidget *win);
gboolean validateitermax(GtkWidget *txtitermax, gint *itermax, GtkWidget *win);
void errdialog(GtkWindow *win, gchar *msg);

#endif /* __VALIDATE_H__ */

