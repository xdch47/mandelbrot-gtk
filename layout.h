
#ifndef __LAYOUT_H__
#define __LAYOUT_H__

#include "interface.h"

GtkWidget *createcplxplane(GtkWidget *txtcplx[4]);
struct winctl *buildinterface(void);
void restoredefaults(struct winctl *w);
void statejctl(struct winctl *w);
void setcplxplane(GtkWidget *txtcplx[4], const gdouble value[4], gdouble width, gdouble height);

#endif /* __LAYOUT_H__ */

