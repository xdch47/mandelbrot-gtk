
#include <math.h>
#include "draw.h"
#include "defs.h"

/* definition is at def.c */
extern const ColorFunc color_func[];

inline static void put_pixel(guchar *pixels, guint x, guint y, guint n_channels, guint rowstride, const guchar *color);

void clblue(const struct iterate_param *param, guint x, guint y, guint iter)
{
	if (iter > param->itermax) {
		put_pixel(param->pixels, x, y, param->n_channels, param->rowstride, param->color);
	} else {
		put_pixel(param->pixels, x, y, param->n_channels, param->rowstride, (clbluedef + (iter % 48) * 4));
	}
}

void divconv(const struct iterate_param *param, guint x, guint y, guint iter)
{
	if (iter > param->itermax) {
		put_pixel(param->pixels, x, y, param->n_channels, param->rowstride, param->color);
	} else {
		put_pixel(param->pixels, x, y, param->n_channels, param->rowstride, param->color + 4);
	}
}

void cldixius_1(const struct iterate_param *param, guint x, guint y, guint iter)
{
	guchar color[4];

	if (iter == param->itermax) {
		color[0] = 255;
		color[1] = 255;
		color[2] = 255;
		color[3] = 255;
	} else {
		guchar result;
		long double x;
		x = sqrt((long double) iter); /* sqrt((long double) period); */
		x = 0.5 + 0.5 * cos(x);
		result = (guchar)(255 * x);
		color[0] = result;
		color[1] = 0;/* result; */
		color[2] = 0;/* result; */
		color[3] = 255;

	}
	put_pixel(param->pixels, x, y, param->n_channels, param->rowstride, color);

}

void cldixius_2(const struct iterate_param *param, guint x, guint y, guint iter)
{
	guchar color[4];

	if (iter == param->itermax) {
		color[0] = 0;
		color[1] = 0;
		color[2] = 0;
		color[3] = 255;
	} else {
		guchar result;
		long double x;
		x = 0.5 - 0.5 * cos(sqrt ((long double) iter));
		if (x < 0.35) {
			result = (guchar) ((exp (-0.5 * pow ((x - 0.35) / 0.1, 2)) + exp (-0.5 * pow ((x + 0.65) / 0.4, 2))) * 244);
		} else { 
			result = (guchar) (exp (-0.5 * pow ((x - 0.35) / 0.4, 2)) * 255); 
		}
		color[0] = result;
		color[1] = result;
		color[2] = result;
		color[3] = 255;

	}
	put_pixel(param->pixels, x, y, param->n_channels, param->rowstride, color);

}

void clearpixbuf(GdkPixbuf *pixbuf)
{
	guint width, height, rowstride, n_channels;
	guchar *itp, *endp;

	n_channels = gdk_pixbuf_get_n_channels (pixbuf);
	width = gdk_pixbuf_get_width (pixbuf) - 1;
	height = gdk_pixbuf_get_height (pixbuf) - 1;
	rowstride = gdk_pixbuf_get_rowstride (pixbuf);
	itp = gdk_pixbuf_get_pixels(pixbuf);
	endp =  itp + height * rowstride + width * n_channels;

	for (; itp < endp; ++itp) {
		itp[0] = 0xff;
		itp[1] = 0xff;
		itp[2] = 0xff;
		itp[3] = 0xff;
	}
}

inline static void put_pixel(guchar *pixels, guint x, guint y, guint n_channels, guint rowstride, const guchar *color)
{
	guint i;
	guchar *p;

	p = pixels + y * rowstride + x * n_channels;
	for (i = 0; i < 4; ++i) 
		*p++ = *color++;
}
