
#ifndef __DRAW_H__
#define __DRAW_H__

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "interface.h"

void (*color_func[COLORFUNC])(guchar *pixels, guint x, guint y, guint n_channels, guint rowstride, guint iter, guint itermax, guchar *convdivcol);
void clearpixbuf(GdkPixbuf *pixbuf);
void clblue(guchar *pixels, guint x, guint y, guint n_channels, guint rowstride, guint iter, guint itermax, guchar *convdivcol);
void divconv(guchar *pixels, guint x, guint y, guint n_channels, guint rowstride, guint iter, guint itermax, guchar *convdivcol);
void redraw_drawing(struct winctl *w, gint x, gint y, gint width, gint height);
void resize_drawing(struct winctl *w);

#endif /* __DRAW_H__ */
