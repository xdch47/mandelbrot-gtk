#ifndef __mbcolor_h__
#define __mbcolor_h__

#include <gtk/gtk.h>


typedef struct _MBColor {
	unsigned int MaxIter;
	unsigned int Period;
	guchar *Colors;
	guchar (*color_func_r) (unsigned int iter, unsigned int maxiter, unsigned int period, char *params);
	guchar (*color_func_g) (unsigned int iter, unsigned int maxiter, unsigned int period, char *params);
	guchar (*color_func_b) (unsigned int iter, unsigned int maxiter, unsigned int period, char *params);
} MBColor;


MBColor *mb_color_new (
			guchar (*cfr) (unsigned int iter, unsigned int maxiter, unsigned int period, char *params), 
			guchar (*cfg) (unsigned int iter, unsigned int maxiter, unsigned int period, char *params),
			guchar (*cfb) (unsigned int iter, unsigned int maxiter, unsigned int period, char *params)
			);

void mb_color_free (MBColor **mbc);

void mb_color_calc (MBColor *mbc, unsigned int maxiter, unsigned int period, char *params);



guchar mb_color_standard_sw (unsigned int iter, unsigned int maxiter, unsigned int period, char *params);

guchar mb_color_sample_1 (unsigned int iter, unsigned int maxiter, unsigned int period, char *params);
guchar mb_color_sample_2 (unsigned int iter, unsigned int maxiter, unsigned int period, char *params);
guchar mb_color_sample_3 (unsigned int iter, unsigned int maxiter, unsigned int period, char *params);

guchar mb_color_sample_4 (unsigned int iter, unsigned int maxiter, unsigned int period, char *params);
guchar mb_color_sample_5 (unsigned int iter, unsigned int maxiter, unsigned int period, char *params);
guchar mb_color_sample_6 (unsigned int iter, unsigned int maxiter, unsigned int period, char *params);

#endif

