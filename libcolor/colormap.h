
#ifndef __COLORMAP_H__
#define __COLORMAP_H__

#include "color.h"
#include "avltree.h"

void initialize_colormap_mutexes();
void finalize_colormap_mutexes();
const guchar *getColor(AVLtree tree, ColorFunc colorfunc, const guint iter, const guint itermax);

#endif /* __COLORMAP_H__ */

