
#include <math.h>
#include <glib>
#include "color.h"
#include "colormap.h"

#define COLOR_PERIOD_DEFAULT 40.0

#define EXPORT_COLORMAP_ALGO(name)                                                \
	const guchar *name(const gint iter, const gint itermax) \
	{                                                                         \
		return getColor(colormap, name ## _algo, iter, itermax);          \
	}

/* color value for the divergent color */
static guchar divcol[3];
static AVLtree colormap = NULL;

static const guchar clbluedef[3 * 48] = {
	  0,   0,   0,
	  0,   0,  31,
	  0,   0,  63,
	  0,   0,  95,
	  0,   0, 127,
	  0,   0, 159,
	  0,   0, 191,
	  0,   0, 223,
	  0,   0, 255,
	  0,  31, 255,
	  0,  63, 255,
	  0,  95, 255,
	  0, 127, 255,
	  0, 159, 255,
	  0, 191, 255,
	  0, 223, 255,
	  0, 255, 255,
	 31, 255, 255,
	 63, 255, 255,
	 95, 255, 255,
	127, 255, 255,
	159, 255, 255,
	191, 255, 255,
	223, 255, 255,
	255, 255, 255,
	223, 255, 255,
	191, 255, 255,
	159, 255, 255,
	127, 255, 255,
	 95, 255, 255,
	 63, 255, 255,
	 31, 255, 255,
	  0, 255, 255,
	  0, 223, 255,
	  0, 191, 255,
	  0, 159, 255,
	  0, 127, 255,
	  0,  95, 255,
	  0,  63, 255,
	  0,  31, 255,
	  0,   0, 255,
	  0,   0, 223,
	  0,   0, 191,
	  0,   0, 159,
	  0,   0, 127,
	  0,   0,  95,
	  0,   0,  63,
	  0,   0,  31,
};

const guchar *clblue(const gint iter, const gint itermax)
{
	return (clbluedef + (iter % 48) * 3);
}

void setdivcol(const guchar *color)
{
	divcol[0] = color[0];
	divcol[1] = color[1];
	divcol[2] = color[2];
}

const guchar *divconv(const gint iter, const gint itermax)
{
	return divcol;
}

void initialize_colormap()
{
	colormap = avl_create();
}

void finalize_colormap()
{
	avl_free_data(colormap);
	colormap = NULL;
}

static const guchar *mb_color_standard_sw_algo(const gint iter, const gint itermax)
{
	long double x;
	guchar *retval;
	guchar res;

	retval = (guchar *)malloc(sizeof(guchar));
	x = sqrt((long double)iter) / sqrt(COLOR_PERIOD_DEFAULT);
	x = 0.5 + 0.5 * cos(x);
	res = (guchar)(255 * x);
	retval[0] = res;
	retval[1] = res;
	retval[2] = res;
	return retval;
}
EXPORT_COLORMAP_ALGO(mb_color_standard_sw)

static const guchar *clRGBS5_algo(const gint iter, const gint itermax)
{
	guchar cl;
	guchar cl_case;
	static const gint step1 = 25;
	static const gint step2 = 50;
	static const gint step3 = 100;
	static const gint step4 = 200;
	guchar *color = (guchar *)malloc(sizeof(guchar) * 3);
	gint length;

	length = 2 * step4;

	if (iter < length) {
		cl_case = iter;
	} else {
		cl_case = ((gint)(log(iter) / log(2))) % length;
	}
	if (cl_case < step1) {
		cl = (iter * (255 / step1)) % 255;
		color[0] = 0;
		color[1] = 0;
		color[2] = cl;
	} else if (cl_case < step2) {
		cl = (155 - (iter * (155 / (step2 - step1))) % 155) + 100;
		color[0] = 0;
		color[1] = cl;
		color[2] = cl;
	} else if (cl_case < step3) {
		cl = ((iter * (200 / (step3 - step2))) % 200) + 55;
		color[0] = 0;
		color[1] = cl;
		color[2] = 0;
	} else if (cl_case < step4) {
		cl = (200 - (iter * (200 / (step4 - step3))) % 200) + 55;
		color[0] = cl;
		color[1] = cl;
		color[2] = 0;
	} else {
		cl = (iter * 10) % 100 + 155;
		color[0] = 0xff;
		color[1] = cl;
		color[2] = 0;
	}
	return color;
} 
EXPORT_COLORMAP_ALGO(clRGBS5)

static void GiveRainbowColor(double position,unsigned char c[])
{
	unsigned char nmax, t;
	int n;
	double m,f;
	/* if position > 1 then we have repetition of colors
	   it maybe useful    */

	if (position>1.0){
		if (position - (int)position == 0.0)
			position=1.0; 
		else position = position - (int)position; 
	}

	nmax=6; /* number of color bars */
	m=nmax* position;

	n=(int)m; 

	f=m-n; 
	t=(int)(f*255);



	switch(n){
	case 0: {
			c[0] = 255;
			c[1] = t;
			c[2] = 0;
			break;
		};
	case 1: {
			c[0] = 255 - t;
			c[1] = 255;
			c[2] = 0;
			break;
		};
	case 2: {
			c[0] = 0;
			c[1] = 255;
			c[2] = t;
			break;
		};
	case 3: {
			c[0] = 0;
			c[1] = 255 - t;
			c[2] = 255;
			break;
		};
	case 4: {
			c[0] = t;
			c[1] = 0;
			c[2] = 255;
			break;
		};
	case 5: {
			c[0] = 255;
			c[1] = 0;
			c[2] = 255 - t;
			break;
		};
	default: {
			 c[0] = 255;
			 c[1] = 0;
			 c[2] = 0;
			 break;
		 };

	}; 


}
const guchar *mb_rainbow_algo(const gint iter, const gint itermax)
{
	guchar *color = (guchar *)malloc(sizeof(guchar) * 3);
	
	GiveRainbowColor(iter / itermax, color);
	return color;
}

EXPORT_COLORMAP_ALGO(mb_rainbow)
