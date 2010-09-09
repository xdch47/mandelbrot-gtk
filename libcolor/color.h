
#ifndef __COLOR_H__
#define __COLOR_H__

#include <glib.h>

typedef struct _IterationInfo {
	gint iter;
/*	gdouble sqr_abs_z; */ 
} IterationInfo;

typedef void (*ColorFunc)(const IterationInfo *iterinfo, guchar *pixel);

typedef struct _ColorFuncDescriptor {
	gchar *name;
	void (*initialize_func)();
	void (*finalize_func)();
	ColorFunc colorfunc;
} ColorFuncDescriptor;

/* Maximum of iteration, defined in color.c */ 
extern gint IterMax;

/* Functions to access the color-algorithmens */
int getColorFunc_count();
const char *getColorFunc_name(int index);
ColorFunc getColorFunc(int index);

void initialize_func(int index);
void finalize_func(int index);

void setDivergentColor(const guchar *color);
void setIterMax(const gint itermax);
int getDivConv_idx();
void cpRGB(guchar *dest, const guchar *src);

#endif /* __COLOR_H__ */
