

#ifndef __COLOR_H__
#define __COLOR_H__

#include <gtk/gtk.h>

typedef const guchar *(*ColorFunc)(const guint iter, const guint itermax);

int getcolorfunc_count();
const char *getcolorfunc_name(int index);
ColorFunc getcolorfunc(int index);
int getdivconv_idx();
void setdivcol(const guchar *color);
void initialize_func(int index);
void finalize_func(int index);

const guchar *clblue(const guint iter, const guint itermax);
const guchar *divconv(const guint iter, const guint itermax);
const guchar *mb_color_standard_sw(const guint iter, const guint itermax);
void init_mb_color_standard_sw();
void final_mb_color_standard_sw();


#endif /* __COLOR_H__ */
