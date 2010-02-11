#include <stdlib.h>
#include <math.h>
#include <gtk/gtk.h>

#include "mbcolor.h"

MBColor *mb_color_new (
			guchar (*cfr) (unsigned int iter, unsigned int maxiter, unsigned int period, char *params), 
			guchar (*cfg) (unsigned int iter, unsigned int maxiter, unsigned int period, char *params),
			guchar (*cfb) (unsigned int iter, unsigned int maxiter, unsigned int period, char *params)
			)
{
	MBColor *tempmbc = (MBColor*) malloc (sizeof (MBColor));
	tempmbc->Colors  = NULL;
	tempmbc->MaxIter = 0;
	tempmbc->Period  = 1;

	if (cfr) tempmbc->color_func_r = cfr; else tempmbc->color_func_r = &mb_color_standard_sw;
	if (cfg) tempmbc->color_func_g = cfg; else tempmbc->color_func_g = &mb_color_standard_sw;
	if (cfb) tempmbc->color_func_b = cfb; else tempmbc->color_func_b = &mb_color_standard_sw;

	return tempmbc;
}


void mb_color_free (MBColor **mbc)
{
	if (*mbc) {
		if ((*mbc)->Colors) free ((*mbc)->Colors);
		free (*mbc);
		*mbc = NULL;
	}
}



void mb_color_calc (MBColor *mbc, unsigned int maxiter, unsigned int period, char *params)
{
	unsigned int i;
	guchar       *tempcolor;

	if (!mbc) return;
	if ((maxiter == mbc->MaxIter) && (period == mbc->Period) && mbc->Colors) return;

	if (maxiter > mbc->MaxIter) {
		if (mbc->Colors) free (mbc->Colors);
		mbc->Colors = malloc (sizeof (guchar) * 4 * (maxiter + 1));
	}

	mbc->MaxIter = maxiter;
	tempcolor    = mbc->Colors;

	for (i = 0; i <= maxiter; i++) {
		*(tempcolor++) = mbc->color_func_r (i, maxiter, period, params);
		*(tempcolor++) = mbc->color_func_g (i, maxiter, period, params);
		*(tempcolor++) = mbc->color_func_b (i, maxiter, period, params);
		*(tempcolor++) = 255;
	}
}




guchar mb_color_standard_sw (unsigned int iter, unsigned int maxiter, unsigned int period, char *params)
{
	guchar result;

	if (iter == maxiter) result = 255;
	else {
		long double x;
		x = sqrt ((long double) iter) / sqrt ((long double) period);
		x = 0.5 + 0.5 * cos (x);
		result = (guchar) (255 * x);
	}

	return result;
}



guchar mb_color_sample_1 (unsigned int iter, unsigned int maxiter, unsigned int period, char *params)
{
	guchar      result;
	long double x;

	if (iter >= maxiter) return 0;

	x = 0.5 - 0.5 * cos (sqrt ((long double) iter) / sqrt ((long double) period));
	if (x < 0.35) result = (guchar) ((exp (-0.5 * pow ((x - 0.35) / 0.1, 2)) + exp (-0.5 * pow ((x + 0.65) / 0.4, 2))) * 244);
	else          result = (guchar) (exp (-0.5 * pow ((x - 0.35) / 0.4, 2)) * 255);

	return result;
}


guchar mb_color_sample_2 (unsigned int iter, unsigned int maxiter, unsigned int period, char *params)
{
	guchar      result;
	long double x;

	if (iter >= maxiter) return 0;

	x = 0.5 - 0.5 * cos (sqrt ((long double) iter) / sqrt ((long double) period));
	if (x < 0.35) result = (guchar) ((exp (-0.5 * pow ((x - 0.35) / 0.1, 2)) + exp (-0.5 * pow ((x + 0.65) / 0.2, 2))) * 254);
	else          result = (guchar) (exp (-0.5 * pow ((x - 0.35) / 0.2, 2)) * 255);

	return result;
}


guchar mb_color_sample_3 (unsigned int iter, unsigned int maxiter, unsigned int period, char *params)
{
	guchar      result;
	long double x;

	if (iter >= maxiter) return 0;

	x = 0.5 - 0.5 * cos (sqrt ((long double) iter) / sqrt ((long double) period));
	if (x < 0.35) result = (guchar) (exp (-0.5 * pow ((x - 0.35) / 0.3, 2)) * 255);
	else          result = (guchar) ((exp (-0.5 * pow ((x - 0.35) / 0.1, 2)) + exp (-0.5 * pow ((x + 1.35) / 0.3, 2))) * 254);

	return result;
}



guchar mb_color_sample_4 (unsigned int iter, unsigned int maxiter, unsigned int period, char *params)
{
	guchar      result;
	long double x;

	if (iter == maxiter) return 0;

	x = sqrt ((long double) iter) / sqrt ((long double) period);
	x = exp (-2 * (cos (x + 3) + 1)) + 0.3 * exp (-2 * (cos (x + 0.5) + 1));
	result = (guchar) (220 * x);

	return result;
}


guchar mb_color_sample_5 (unsigned int iter, unsigned int maxiter, unsigned int period, char *params)
{
	guchar      result;
	long double x;

	if (iter == maxiter) return 0;

	x = sqrt ((long double) iter) / sqrt ((long double) period);
	x = exp (-2 * (cos (x + 3) + 1))  + 0.75 * exp (-2 * (cos (x + 0.5) + 1));
	result = (guchar) (120 * x);

	return result;
}


guchar mb_color_sample_6 (unsigned int iter, unsigned int maxiter, unsigned int period, char *params)
{
	guchar      result;
	long double x;

	if (iter == maxiter) return 0;

	x = sqrt ((long double) iter) / sqrt ((long double) period);
	x = exp (cos (x + 3) - 1);
	result = (guchar) (180 * x);

	return result;
}

