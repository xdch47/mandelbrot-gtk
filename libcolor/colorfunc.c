
#include <math.h>
#include "color.h"

#define COLOR_PERIOD_DEFAULT 40.0

/* color value for the divergent color */
static guchar divcol[4];

static const guchar clbluedef[4 * 48] = {
	  0,   0,   0, 255,
	  0,   0,  31, 255,
	  0,   0,  63, 255,
	  0,   0,  95, 255,
	  0,   0, 127, 255,
	  0,   0, 159, 255,
	  0,   0, 191, 255,
	  0,   0, 223, 255,
	  0,   0, 255, 255,
	  0,  31, 255, 255,
	  0,  63, 255, 255,
	  0,  95, 255, 255,
	  0, 127, 255, 255,
	  0, 159, 255, 255,
	  0, 191, 255, 255,
	  0, 223, 255, 255,
	  0, 255, 255, 255,
	 31, 255, 255, 255,
	 63, 255, 255, 255,
	 95, 255, 255, 255,
	127, 255, 255, 255,
	159, 255, 255, 255,
	191, 255, 255, 255,
	223, 255, 255, 255,
	255, 255, 255, 255,
	223, 255, 255, 255,
	191, 255, 255, 255,
	159, 255, 255, 255,
	127, 255, 255, 255,
	 95, 255, 255, 255,
	 63, 255, 255, 255,
	 31, 255, 255, 255,
	  0, 255, 255, 255,
	  0, 223, 255, 255,
	  0, 191, 255, 255,
	  0, 159, 255, 255,
	  0, 127, 255, 255,
	  0,  95, 255, 255,
	  0,  63, 255, 255,
	  0,  31, 255, 255,
	  0,   0, 255, 255,
	  0,   0, 223, 255,
	  0,   0, 191, 255,
	  0,   0, 159, 255,
	  0,   0, 127, 255,
	  0,   0,  95, 255,
	  0,   0,  63, 255,
	  0,   0,  31, 255
};

const guchar *clblue(const guint iter, const guint itermax)
{
	return (clbluedef + (iter % 48) * 4);
}

void setdivcol(const guchar *color)
{
	divcol[0] = color[0];
	divcol[1] = color[1];
	divcol[2] = color[2];
	divcol[3] = color[3];
}

const guchar *divconv(const guint iter, const guint itermax)
{
	return divcol;
}

const guchar *mb_color_standard_sw(const guint iter, const guint itermax)
{
	guchar res;
	guchar *result;
	long double x;
	x = sqrt((long double)iter) / sqrt(COLOR_PERIOD_DEFAULT);
	x = 0.5 + 0.5 * cos(x);
	res = (guchar)(255 * x);
	result = (guchar *)g_malloc(sizeof(guchar) * 4);
	result[0] = res;
	result[1] = res;
	result[2] = res;
	result[3] = 0xff;
	return result;
}
