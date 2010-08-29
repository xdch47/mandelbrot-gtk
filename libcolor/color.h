

#ifndef __COLOR_H__
#define __COLOR_H__

#include <gtk/gtk.h>

typedef const guchar *(*ColorFunc)(const guint iter, const guint itermax);

int getcolorfunc_count();
const char *getcolorfunc_name(int index);
ColorFunc getcolorfunc(int index);
int getdivconv_idx();
void setdivcol(const guchar *color);
const guchar *clblue(const guint iter, const guint itermax);
const guchar *divconv(const guint iter, const guint itermax);

#endif /* __COLOR_H__ */
