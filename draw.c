
#include "draw.h"
#include "color.h"
#include "defs.h"

void (*color_func[COLORFUNC])(guchar *pixels, guint x, guint y, guint n_channels, guint rowstride, guint iter, guint itermax, guchar *convdivcol) = {
	clblue,
	divconv
};

void redraw_drawing(struct winctl *w, gint x, gint y, gint width, gint height)
{
	g_assert(w->pixbufshow != NULL);
	gdk_draw_pixbuf(w->drawing->window, NULL, w->pixbufshow, x, y, x, y, width, height, GDK_RGB_DITHER_NORMAL, 0, 0);
}

void resize_drawing(struct winctl *w)
{
	gint width = w->drawing->allocation.width;
	gint height = w->drawing->allocation.height;
	if (!w->pixbufcalc)  {
		// Create blank screen on startup:
		w->pixbufcalc = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, width, height);
		clearpixbuf(w->pixbufcalc);
		w->pixbufshow = g_object_ref(w->pixbufcalc);
	} else {
		g_assert(w->pixbufshow != NULL);
		g_object_unref(w->pixbufshow);

		if (width == gdk_pixbuf_get_width(w->pixbufcalc) && height == gdk_pixbuf_get_height(w->pixbufcalc)) {
			w->pixbufshow = g_object_ref(w->pixbufcalc);
		} else {
			w->pixbufshow = gdk_pixbuf_scale_simple(w->pixbufcalc, width, height, INTERPOLATION);
		}
	}
}


static void put_pixel(guchar *pixels, guint x, guint y, guint n_channels, guint rowstride, const guchar *color);

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

void clblue(guchar *pixels, guint x, guint y, guint n_channels, guint rowstride, guint iter, guint itermax, guchar *convdivcolor)
{
	if (iter > itermax) {
		put_pixel(pixels, x, y, n_channels, rowstride, convdivcolor);
	} else {
		put_pixel(pixels, x, y, n_channels, rowstride, (clbluedef + (iter % 48) * 4));
	}
}

void divconv(guchar *pixels, guint x, guint y, guint n_channels, guint rowstride, guint iter, guint itermax, guchar *convdivcol)
{
	if (iter > itermax) {
		put_pixel(pixels, x, y, n_channels, rowstride, convdivcol);
	} else {
		put_pixel(pixels, x, y, n_channels, rowstride, convdivcol + 4);
	}
}

static void put_pixel(guchar *pixels, guint x, guint y, guint n_channels, guint rowstride, const guchar *color)
{
	guint i;
	guchar *p;

	p = pixels + y * rowstride + x * n_channels;
	for (i = 0; i < 4; ++i) 
		*p++ = *color++;
}
