
#ifndef __DRAW_H__
#define __DRAW_H__

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "render.h"

void clearpixbuf(GdkPixbuf *pixbuf);
void clblue(const struct iterate_param *param, guint x, guint y, guint iter);
void divconv(const struct iterate_param *param, guint x, guint y, guint iter);
void cldixius_1(const struct iterate_param *param, guint x, guint y, guint iter);
void cldixius_2(const struct iterate_param *param, guint x, guint y, guint iter);

#endif /* __DRAW_H__ */
