
#include <stdlib.h>
#include <errno.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <glib/gstdio.h>
#include <gdk/gdk.h>
#include "interface.h"
#include "libcolor/color.h"

#define SIGN(x) (x >= 0) ? 1 : -1;

static void destroy(GtkWidget *widget, struct winctl *w);
static void get_julia_set(GtkWidget *widget, struct winctl *w);
static gboolean expose_event(GtkWidget *widget, GdkEventExpose *event, struct winctl *w);
static gboolean configure_event(GtkWidget *widget, GdkEventConfigure *event, struct winctl *w);
static gboolean scroll_event(GtkWidget *widget, GdkEventScroll *event, struct winctl *w);
static gboolean button_press_event(GtkWidget *widget, GdkEventButton *event, struct winctl *w);
static gboolean motion_notify_event(GtkWidget *widget, GdkEventMotion *event, struct winctl *w);
static gboolean button_release_event(GtkWidget *widget, GdkEventButton *event, struct winctl *w);
static void render_thread_done(gboolean succ, struct winctl *w);
static void redraw_idle(struct winctl *w);
static gboolean start_calc(struct winctl *w);
static void redraw_drawing(struct winctl *w);
static void clearpixbuf(GdkPixbuf *pixbuf);

void run_interface(gchar *file_name)
{
	struct winctl *w = buildinterface();
	gboolean succ_load_config;
	gchar *dirname;
	GtkAllocation drawing_alloc;

	g_signal_connect(G_OBJECT(w->win), "destroy", G_CALLBACK(destroy), w);
	g_signal_connect(G_OBJECT(w->drawing), "configure_event", G_CALLBACK(configure_event), w);
	g_signal_connect(G_OBJECT(w->drawing), "expose_event", G_CALLBACK(expose_event), w);
	g_signal_connect(G_OBJECT(w->drawing), "scroll_event", G_CALLBACK(scroll_event), w);
	g_signal_connect(G_OBJECT(w->drawing), "button_press_event", G_CALLBACK(button_press_event), w);
	g_signal_connect(G_OBJECT(w->drawing), "motion_notify_event", G_CALLBACK(motion_notify_event), w);
	g_signal_connect(G_OBJECT(w->drawing), "button_release_event", G_CALLBACK(button_release_event), w);
	g_signal_connect(G_OBJECT(w->btncalc), "clicked", G_CALLBACK(calc), w);
	g_signal_connect(G_OBJECT(w->btnreset), "clicked", G_CALLBACK(reset), w);
	g_signal_connect(G_OBJECT(w->btnj), "clicked", G_CALLBACK(get_julia_set), w);
	g_signal_connect_swapped(G_OBJECT(w->btnclose), "clicked", G_CALLBACK(gtk_widget_destroy), w->win);

	gtk_widget_set_events(w->drawing,
	    GDK_EXPOSURE_MASK
	    | GDK_LEAVE_NOTIFY_MASK
	    | GDK_BUTTON_PRESS_MASK
	    | GDK_KEY_PRESS_MASK
	    | GDK_BUTTON_RELEASE_MASK
	    | GDK_SCROLL_MASK
	    | GDK_POINTER_MOTION_MASK
	    | GDK_POINTER_MOTION_HINT_MASK);

	w->render_thread = render_thread_new((IdleFunc)redraw_idle, (ThreaddestroyFunc)render_thread_done, (gpointer)w);

	dirname = CONFIGDIR;

	if (!g_file_test(dirname, G_FILE_TEST_IS_DIR)) {
		g_mkdir(dirname, 0777);
	}
	w->configfile = g_build_filename(dirname, CONFIGFILE, NULL);
	g_free(dirname);

	w->succ_render = FALSE;
	w->redraw = FALSE;
	succ_load_config = configure_interface(w, w->configfile, LOAD_CONFIG);

	gtk_widget_show_all(w->win);

	while (gtk_events_pending())
		gtk_main_iteration();

	if (!succ_load_config) {
		gtk_widget_get_allocation(w->drawing, &drawing_alloc);
		setcplxplane(w->txtcplx, w->it_param.cplxplane, drawing_alloc.width, drawing_alloc.height);
	}

	start_calc(w);
}

static void destroy(GtkWidget *widget, struct winctl *w)
{
	render_thread_kill(w->render_thread);
	configure_interface(w, w->configfile, STORE_CONFIG);
	render_thread_free(w->render_thread);
	iterate_param_free(&w->it_param);
	g_free(w->itermap);
	g_free(w->mcolalgo);

	gtk_widget_destroy(w->drawmenu);
	gtk_widget_destroy(w->pjmenu);
	g_free(w->configfile);
	g_free(w);

	gtk_main_quit();
}

static gboolean start_calc(struct winctl *w)
{
	gdouble degree;
	gint itermax;
	gdouble jre, jim;
	int errno;
	gchar *endptr;
	gint i;
	gdouble cplxplane[4];
	guchar divcolor[3];
	GtkAllocation drawing_alloc;

	/* validate complex plane */
	if (!validate_cplx(w->txtcplx, cplxplane, w->win)) {
		return FALSE;
	}
	/* max. iterations: */
	if (!validate_itermax(w->txtitermax, &itermax, w->win)) {
		return FALSE;
	}
	if (w->get_j) {
		itermax = w->get_jitermax;
	}
	/* degree of mandelbrot-/julia-set: */
	if (!validate_degree(w->txtdegree, &degree, w->win)) {
		return FALSE;
	}
	/* julia const: */
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w->chkjulia))) {
		jre = strtod(gtk_entry_get_text(GTK_ENTRY(w->txtjre)), &endptr);
		if (*endptr != '\0' || errno == ERANGE ) {
			gchar msg[BUFSIZE];
			g_snprintf(msg, BUFSIZE, ERRVAL, gtk_entry_get_text(GTK_ENTRY(w->txtjre)));
			errdialog(GTK_WINDOW(w->win), msg);
			gtk_widget_grab_focus(w->txtjre);
			return FALSE;
		}
		jim = strtod(gtk_entry_get_text(GTK_ENTRY(w->txtjim)), &endptr);
		if (*endptr != '\0' || errno == ERANGE ) {
			gchar msg[BUFSIZE];
			g_snprintf(msg, BUFSIZE, ERRVAL, gtk_entry_get_text(GTK_ENTRY(w->txtjim)));
			errdialog(GTK_WINDOW(w->win), msg);
			gtk_widget_grab_focus(w->txtjim);
			return FALSE;
		}
		w->it_param.type = JULIA_SET;
	} else {
		jre = w->it_param.j[0];
		jim = w->it_param.j[1];
		w->it_param.type = MANDELBROT_SET;
	}
	render_thread_kill(w->render_thread);
	if (w->it_param.type == MANDELBROT_SET) {
		gtk_label_set_text(GTK_LABEL(w->lbldraw), LCAPMRENDER);
	} else {
		gtk_label_set_text(GTK_LABEL(w->lbldraw), LCAPJRENDER);
	}

	w->succ_render = FALSE;
	if (w->pixbufcalc)
		g_object_unref(w->pixbufcalc);
	if (w->pixbufshow)
		g_object_unref(w->pixbufshow);

	g_free(w->itermap);

	gtk_widget_get_allocation(w->drawing, &drawing_alloc);
	w->itermap = (IterationInfo *)g_malloc(sizeof(IterationInfo) * drawing_alloc.width * drawing_alloc.height);
	w->pixbufcalc = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, drawing_alloc.width, drawing_alloc.height);

	clearpixbuf(w->pixbufcalc);
	w->pixbufshow = g_object_ref(w->pixbufcalc);
	gdk_window_invalidate_rect(gtk_widget_get_window(w->drawing), NULL, FALSE);

	cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(w->drawing));
	gdk_cairo_set_source_pixbuf(cr, w->pixbufcalc, 0, 0);
	cairo_paint(cr);
	cairo_destroy(cr);

	gtk_button_set_label(GTK_BUTTON(w->btncalc), LSTOP);

	w->timer = g_timer_new();
	for (i = 0; i < 4; ++i) {
		w->it_param.cplxplane[i] = cplxplane[i];
	}
	w->it_param.itermap = w->itermap;
	w->it_param.xmax = drawing_alloc.width;
	w->it_param.ymax = drawing_alloc.height;
	w->it_param.j[0] = jre;
	w->it_param.j[1] = jim;
	w->it_param.itermax = itermax;
	w->it_param.pixels = gdk_pixbuf_get_pixels(w->pixbufcalc);
	w->it_param.rowstride = gdk_pixbuf_get_rowstride(w->pixbufcalc);
	w->it_param.n_channels = gdk_pixbuf_get_n_channels(w->pixbufcalc);
	w->it_param.colorfunc = getColorFunc(w->it_param.color_func_index);

	w->it_param.color[0] = (guchar)(w->convcol.red >> 8);
	w->it_param.color[1] = (guchar)(w->convcol.green >> 8);
	w->it_param.color[2] = (guchar)(w->convcol.blue >> 8);
	divcolor[0] = (guchar)(w->divcol.red >> 8);
	divcolor[1] = (guchar)(w->divcol.green >> 8);
	divcolor[2] = (guchar)(w->divcol.blue >> 8);
	setDivergentColor(divcolor);
	setIterMax(w->it_param.itermax);
	w->it_param.degree = degree;
	if (w->it_param.type == MANDELBROT_SET) {
		w->it_param.iterate_func = (degree == 2.0) ? (GThreadFunc)mandelbrot_set : (GThreadFunc)mandelbrot_set_deg;
	} else {
		w->it_param.iterate_func = (degree == 2.0) ? (GThreadFunc)julia_set : (GThreadFunc)julia_set_deg;
	}

	start_render_thread(w->render_thread, &w->it_param);
	return TRUE;
}

static void render_thread_done(gboolean succ, struct winctl *w)
{
	gchar buf[50];
	GtkAllocation drawing_alloc;

	g_timer_stop(w->timer);
	if (succ) {
		gdk_threads_enter();
		if (w->pixbufshow)
			g_object_unref(w->pixbufshow);
		gtk_widget_get_allocation(w->drawing, &drawing_alloc);
		w->pixbufshow = gdk_pixbuf_scale_simple(w->pixbufcalc, drawing_alloc.width, drawing_alloc.height, INTERPOLATION);

		cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(w->drawing));
		gdk_cairo_set_source_pixbuf(cr, w->pixbufshow, 0, 0);
		cairo_paint(cr);
		cairo_destroy(cr);

		gtk_button_set_label(GTK_BUTTON(w->btncalc), LCALC);
		gtk_window_set_resizable(GTK_WINDOW(w->win), TRUE);
		if (w->redraw) {
			redraw_pixbuf(w);
			w->redraw = FALSE;
		}
		gdk_threads_leave();
	}

	gdk_threads_enter();
	w->succ_render = succ;
	gtk_button_set_label(GTK_BUTTON(w->btncalc), LCALC);
	if (w->it_param.type == MANDELBROT_SET) {
		g_snprintf(buf, 50, LCAPM, g_timer_elapsed(w->timer, NULL));
	} else {
		g_snprintf(buf, 50, LCAPJ, g_timer_elapsed(w->timer, NULL));
	}
	gtk_label_set_text(GTK_LABEL(w->lbldraw), buf);
	gdk_threads_leave();
	g_timer_destroy(w->timer);
}

void calc(GtkWidget *widget, struct winctl *w)
{
	if (!is_render_thread_alive(w->render_thread)) {
		start_calc(w);
	} else {
		if (is_render_thread_pause(w->render_thread)) {
			GtkWidget *vbox;
			GtkWidget *lbl;
			GtkWidget *dialog;
			gint response;

			lbl = gtk_label_new(LTHREADPAUSE);
			vbox = gtk_vbox_new(FALSE, 0);
			gtk_box_pack_start(GTK_BOX(vbox), lbl, FALSE, FALSE, 0);
			gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
			dialog = gtk_dialog_new_with_buttons(LWCAP, GTK_WINDOW(w->win), GTK_DIALOG_MODAL, LCONTDRAW, RESPONSE_CONTDRAW, LREDRAW,
			    RESPONSE_REDRAW, LCANCEL, RESPONSE_CANCEL, NULL);
			gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), vbox, FALSE, FALSE, 10);
			gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
			gtk_widget_show_all(vbox);
			response = gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);

			switch (response) {
			case RESPONSE_CONTDRAW:
				gtk_button_set_label(GTK_BUTTON(w->btncalc), LSTOP);
				g_timer_continue(w->timer);
				if (w->it_param.type == MANDELBROT_SET) {
					gtk_label_set_text(GTK_LABEL(w->lbldraw), LCAPMRENDER);
				} else {
					gtk_label_set_text(GTK_LABEL(w->lbldraw), LCAPJRENDER);
				}
				render_thread_resume(w->render_thread);
				break;
			case RESPONSE_REDRAW:
				render_thread_kill(w->render_thread);
				start_calc(w);
				break;
			default:
				break;
			}
		} else {
			gchar buf[50];
			g_timer_stop(w->timer);
			if (w->it_param.type == MANDELBROT_SET) {
				g_snprintf(buf, 50, LCAPM, g_timer_elapsed(w->timer, NULL));
			} else {
				g_snprintf(buf, 50, LCAPJ, g_timer_elapsed(w->timer, NULL));
			}
			gtk_label_set_text(GTK_LABEL(w->lbldraw), buf);
			render_thread_pause(w->render_thread);
			gtk_button_set_label(GTK_BUTTON(w->btncalc), LCALC);
		}

	}
}

void reset(GtkWidget *widget, struct winctl *w)
{
	restoredefaults(w);
}

static void get_julia_set(GtkWidget *widget, struct winctl *w)
{
	GdkCursor* c;

	c = gdk_cursor_new(GDK_TCROSS);
	gdk_window_set_cursor(gtk_widget_get_window(w->win), c);
	gdk_cursor_unref(c);
	w->get_j = TRUE;
	if (!w->it_param.type == MANDELBROT_SET) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w->chkjulia), FALSE);
		restoredefaults(w);
		start_calc(w);
	}
}

static gboolean expose_event(GtkWidget *widget, GdkEventExpose *event, struct winctl *w)
{
	cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widget));
	gdk_cairo_set_source_pixbuf(cr, w->pixbufshow, 0, 0);
	cairo_paint(cr);
	cairo_destroy(cr);
	return FALSE;
}

static gboolean configure_event(GtkWidget *widget, GdkEventConfigure *event, struct winctl *w)
{
	GtkAllocation drawing_alloc;

	gtk_widget_get_allocation(w->drawing, &drawing_alloc);
	if (!w->pixbufcalc) {
		/* Create blank screen on startup: */
		w->pixbufcalc = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, drawing_alloc.width, drawing_alloc.height);
		clearpixbuf(w->pixbufcalc);
		w->pixbufshow = g_object_ref(w->pixbufcalc);
	} else {
		g_object_unref(w->pixbufshow);

		if (drawing_alloc.width == gdk_pixbuf_get_width(w->pixbufcalc) && drawing_alloc.height == gdk_pixbuf_get_height(w->pixbufcalc)) {
			w->pixbufshow = g_object_ref(w->pixbufcalc);
		} else {
			w->pixbufshow = gdk_pixbuf_scale_simple(w->pixbufcalc, drawing_alloc.width, drawing_alloc.height, INTERPOLATION);
		}
	}
	return TRUE;
}

void zoom(struct winctl *w, enum zoom_mode mode)
{
	gdouble b_re, b_im, dim, dre;
	gdouble val[4];
	gint px, py;
	gint pwx, pwy;
	gdouble sf;
	GdkDisplay *display;
	GdkScreen *s;
	GtkAllocation drawing_alloc;

	sf = (mode == ZOOM_IN) ? w->zoomfactor: 1.0 / w->zoomfactor;

	gtk_widget_get_allocation(w->drawing, &drawing_alloc);
	gtk_widget_get_pointer(w->drawing, &px, &py);
	dre = (w->it_param.cplxplane[1] - w->it_param.cplxplane[0]) / (gdouble)drawing_alloc.width;
	dim = (w->it_param.cplxplane[3] - w->it_param.cplxplane[2]) / (gdouble)drawing_alloc.height;
	b_re = w->it_param.cplxplane[0] + dre * (px - sf * (gdouble)drawing_alloc.width / 2.0);
	b_im = w->it_param.cplxplane[3] - dim * (py - sf * (gdouble)drawing_alloc.height / 2.0);
	val[0] = b_re;
	val[1] = b_re + sf * dre * drawing_alloc.width;
	val[2] = b_im;
	val[3] = b_im - sf * dim * drawing_alloc.height;
	setcplxplane(w->txtcplx, val, drawing_alloc.width, drawing_alloc.height);
	display = gdk_display_get_default();
	s = gdk_display_get_default_screen(display);
	gdk_display_get_pointer(display, NULL, &pwx, &pwy, NULL);
	gdk_display_warp_pointer(display, s, pwx - px + drawing_alloc.width / 2, pwy - py + drawing_alloc.height / 2);
	start_calc(w);
}

static void setj(struct winctl *w)
{
	gchar buf[30];
	gdouble dim, dre;
	gint px, py;
	GtkAllocation drawing_alloc;

	gtk_widget_get_pointer(w->drawing, &px, &py);

	gtk_widget_get_allocation(w->drawing, &drawing_alloc);

	dre = (w->it_param.cplxplane[1] - w->it_param.cplxplane[0]) / (gdouble)drawing_alloc.width;
	dim = (w->it_param.cplxplane[3] - w->it_param.cplxplane[2]) / (gdouble)drawing_alloc.height;
	g_sprintf(buf, NUMFORMAT, w->it_param.cplxplane[0] + dre * px);
	gtk_entry_set_text(GTK_ENTRY(w->txtjre), buf);
	g_sprintf(buf, NUMFORMAT, w->it_param.cplxplane[3] - dim * py);
	gtk_entry_set_text(GTK_ENTRY(w->txtjim), buf);
}

static gboolean scroll_event(GtkWidget *widget, GdkEventScroll *event, struct winctl *w)
{
	if (event->direction == GDK_SCROLL_UP) {
		zoom(w, ZOOM_IN);
	} else if (event->direction == GDK_SCROLL_DOWN) {
		zoom(w, ZOOM_OUT);
	}
	return FALSE;
}

static gboolean button_press_event(GtkWidget *widget, GdkEventButton *event, struct winctl *w)
{
	if (w->get_j) {
		setj(w);
		return TRUE;
	}

	if (event->button == 3) {
		gtk_menu_popup(GTK_MENU(w->drawmenu), NULL, NULL, NULL, NULL, event->button, event->time);
		return TRUE;
	}
	w->focus_x = event->x;
	w->focus_y = event->y;
	w->focus_area.x = event->x;
	w->focus_area.y = event->y;
	w->focus_area.width = 0;
	w->focus_area.height = 0;
	return TRUE;
}

static gboolean motion_notify_event(GtkWidget *widget, GdkEventMotion *event, struct winctl *w)
{
	static const gint cursortype[4] = { GDK_TOP_LEFT_CORNER, GDK_TOP_RIGHT_CORNER, GDK_BOTTOM_LEFT_CORNER, GDK_BOTTOM_RIGHT_CORNER };

	if (w->get_j) {
		setj(w);
		return TRUE;
	}


	if (event->state & GDK_BUTTON1_MASK) {
		GdkCursor *c;
		gint x, y, px, py, width, height, sw, sh;
		gint delta;
		gint pos = 0;

		gtk_widget_get_pointer(w->drawing, &px, &py);
		width = w->focus_x - px;
		sw = SIGN(width);
		width *= sw;
		height = w->focus_y - py;
		sh = SIGN(height);
		height *= sh;

		if (sw < 0)
			pos += 1;
		if (sh < 0)
			pos += 2;

		c = gdk_cursor_new(cursortype[pos]);
		gdk_window_set_cursor(gtk_widget_get_window(w->win), c);
		gdk_cursor_unref(c);
		if (w->zoomprop) {
			GtkAllocation drawing_alloc;

			gtk_widget_get_allocation(w->drawing, &drawing_alloc);
			delta = height;
			height = width * drawing_alloc.height / drawing_alloc.width;
			delta -= height;
			if (delta) {
				GdkDisplay *display;
				GdkScreen *screen;

				display = gdk_display_get_default();
				screen = gdk_display_get_default_screen(display);
				gdk_display_get_pointer(display, NULL, &px, &py, NULL);
				py += sh * delta;
				gdk_display_warp_pointer(display, screen, px, py);
				return TRUE;
			}
		}

		x = MIN(w->focus_x, px);
		y = MIN(w->focus_y, py);
		w->focus_area.x = x;
		w->focus_area.y = y;
		w->focus_area.width = width;
		w->focus_area.height = height;
		redraw_drawing(w);

		gtk_paint_focus(gtk_widget_get_style(widget), gtk_widget_get_window(widget), GTK_STATE_NORMAL, NULL, widget, NULL, x, y, width, height);

	}

	return TRUE;
}

static gboolean button_release_event(GtkWidget *widget, GdkEventButton *event, struct winctl *w)
{
	gchar buf[30];
	gdouble b_re, b_im, dim, dre;
	gdouble val[4];
	GtkAllocation drawing_alloc;

	gdk_window_set_cursor(gtk_widget_get_window(w->win), NULL);
	if (w->get_j) {
		w->get_j = FALSE;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w->chkjulia), TRUE);
		restoredefaults(w);
		start_calc(w);
		return TRUE;
	}
	if (w->pixbufshow) {
		cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widget));
		gdk_cairo_set_source_pixbuf(cr, w->pixbufshow, 0, 0);
		cairo_paint(cr);
		cairo_destroy(cr);
	}

	if (!w->focus_area.width) {
		return TRUE;
	}

	gtk_widget_get_allocation(w->drawing, &drawing_alloc);
	dre = (w->it_param.cplxplane[1] - w->it_param.cplxplane[0]) / (gdouble)drawing_alloc.width;
	dim = (w->it_param.cplxplane[3] - w->it_param.cplxplane[2]) / (gdouble)drawing_alloc.height;
	b_re = w->it_param.cplxplane[0] + w->focus_area.x * dre;
	b_im = w->it_param.cplxplane[3] - w->focus_area.y * dim;

	if (w->zoomprop) {
		val[0] = b_re;
		val[1] = b_re + w->focus_area.width * dre;
		val[2] = b_im;
		val[3] = b_im - dim * w->focus_area.height;
		setcplxplane(w->txtcplx, val, drawing_alloc.width, drawing_alloc.height);
	} else {
		g_sprintf(buf, NUMFORMAT, b_re);
		gtk_entry_set_text(GTK_ENTRY(w->txtcplx[0]), buf);
		g_sprintf(buf, NUMFORMAT, b_re + w->focus_area.width * dre);
		gtk_entry_set_text(GTK_ENTRY(w->txtcplx[1]), buf);
		g_sprintf(buf, NUMFORMAT, b_im - w->focus_area.height * dim);
		gtk_entry_set_text(GTK_ENTRY(w->txtcplx[2]), buf);
		g_sprintf(buf, NUMFORMAT, b_im);
		gtk_entry_set_text(GTK_ENTRY(w->txtcplx[3]), buf);
	}

	return TRUE;
}

static void redraw_idle(struct winctl *w)
{
	GtkAllocation drawing_alloc;

	gdk_threads_enter();
	gtk_widget_get_allocation(w->drawing, &drawing_alloc);


	if (drawing_alloc.width != gdk_pixbuf_get_width(w->pixbufcalc) || drawing_alloc.height != gdk_pixbuf_get_height(w->pixbufcalc)) {
		g_object_unref(w->pixbufshow);
		w->pixbufshow = gdk_pixbuf_scale_simple(w->pixbufcalc, drawing_alloc.width, drawing_alloc.height, INTERPOLATION);
	}
	redraw_drawing(w);
	gdk_threads_leave();
}

static void redraw_drawing(struct winctl *w)
{
	cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(w->drawing));
	gdk_cairo_set_source_pixbuf(cr, w->pixbufshow, 0, 0);
	cairo_paint(cr);
	cairo_destroy(cr);
}

void clearpixbuf(GdkPixbuf *pixbuf)
{
	gint width, height, rowstride, n_channels;
	guchar *itp, *endp;

	n_channels = gdk_pixbuf_get_n_channels(pixbuf);
	width = gdk_pixbuf_get_width(pixbuf) - 1;
	height = gdk_pixbuf_get_height(pixbuf) - 1;
	rowstride = gdk_pixbuf_get_rowstride(pixbuf);
	itp = gdk_pixbuf_get_pixels(pixbuf);
	endp = itp + height * rowstride + width * n_channels;

	for (; itp < endp; itp += 4) {
		itp[0] = 0xff;
		itp[1] = 0xff;
		itp[2] = 0xff;
		itp[3] = 0xff;
	}
}

void update_pixbuf(gint x, gint y, const IterationInfo *iterinfo, const struct iterate_param *it_param)
{
	guchar *p;

	p = it_param->pixels + y * it_param->rowstride + x * it_param->n_channels;
	if (iterinfo->iter != -1) {
		it_param->colorfunc(iterinfo, p);
	} else {
		p[0] = it_param->color[0];
		p[1] = it_param->color[1];
		p[2] = it_param->color[2];
	}

}

void redraw_pixbuf(struct winctl *w)
{
	GdkPixbuf *pixbuf;
	IterationInfo *itermap;
	ColorFunc colorfunc;
	gint width, height, rowstride, n_channels;
	guchar *itp, *endp;
	guchar divcolor[3];

	itermap = w->itermap;
	pixbuf = w->pixbufcalc;
	initialize_func(w->it_param.color_func_index);
	colorfunc = getColorFunc(w->it_param.color_func_index);
	n_channels = gdk_pixbuf_get_n_channels(pixbuf);
	width = gdk_pixbuf_get_width(pixbuf) - 1;
	height = gdk_pixbuf_get_height(pixbuf) - 1;
	rowstride = gdk_pixbuf_get_rowstride(pixbuf);
	itp = gdk_pixbuf_get_pixels(pixbuf);
	endp = itp + height * rowstride + width * n_channels;

	w->it_param.color[0] = (guchar)(w->convcol.red >> 8);
	w->it_param.color[1] = (guchar)(w->convcol.green >> 8);
	w->it_param.color[2] = (guchar)(w->convcol.blue >> 8);
	divcolor[0] = (guchar)(w->divcol.red >> 8);
	divcolor[1] = (guchar)(w->divcol.green >> 8);
	divcolor[2] = (guchar)(w->divcol.blue >> 8);
	setDivergentColor(divcolor);

	for (; itp < endp; itp += 4) {
		if (itermap->iter != -1) {
			colorfunc(itermap++, itp);
		} else {
			itp[0] = w->it_param.color[0];
			itp[1] = w->it_param.color[1];
			itp[2] = w->it_param.color[2];
			itermap++;
		}
	}

	configure_event(NULL, NULL, w);
	redraw_drawing(w);
}
