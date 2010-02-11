
#include <glib.h>
#include <glib/gprintf.h>
#include <gdk/gdkkeysyms.h>
#include "interface.h"
#include "layout.h"
#include "config.h"
#include "render.h"
#include "iterate.h"
#include "draw.h"

#define SIGN(x) (x >= 0) ? 1 : -1;

static void get_julia_set(GtkWidget *widget, struct winctl *w);
static void destroy(GtkWidget *widget, struct winctl *w);
static gboolean expose_event(GtkWidget *widget, GdkEventExpose *event, struct winctl *w);
static gboolean configure_event(GtkWidget *widget, GdkEventConfigure *event, struct winctl *w);
static gboolean scroll_event(GtkWidget *widget, GdkEventScroll *event, struct winctl *w);
static gboolean key_press_event(GtkWidget *widget, GdkEventKey *event, struct winctl *w);
static gboolean button_press_event(GtkWidget *widget, GdkEventButton *event, struct winctl *w);
static gboolean motion_notify_event(GtkWidget *widget, GdkEventMotion *event, struct winctl *w);
static gboolean button_release_event(GtkWidget *widget, GdkEventButton *event, struct winctl *w);

void run_interface(gchar *file_name)
{
	int succ_load_config;
	struct winctl *w = buildinterface();
	gchar *dirname;

	g_signal_connect(G_OBJECT(w->win), "destroy", G_CALLBACK(destroy), w);
	g_signal_connect(G_OBJECT(w->drawing), "configure_event", G_CALLBACK(configure_event), w);
	g_signal_connect(G_OBJECT(w->drawing), "expose_event", G_CALLBACK(expose_event), w);
	g_signal_connect(G_OBJECT(w->drawing), "scroll_event", G_CALLBACK(scroll_event), w);
	g_signal_connect(G_OBJECT(w->win), "key_press_event", G_CALLBACK(key_press_event), w);
	g_signal_connect(G_OBJECT(w->drawing), "button_press_event", G_CALLBACK(button_press_event), w);
	g_signal_connect(G_OBJECT(w->drawing), "motion_notify_event", G_CALLBACK(motion_notify_event), w);
	g_signal_connect(G_OBJECT(w->drawing), "button_release_event", G_CALLBACK(button_release_event), w);
	g_signal_connect(G_OBJECT(w->btncalc), "clicked", G_CALLBACK(calc), w);
	g_signal_connect(G_OBJECT(w->btnreset), "clicked", G_CALLBACK(reset), w);
	g_signal_connect(G_OBJECT(w->btnj), "clicked", G_CALLBACK(get_julia_set), w);
	g_signal_connect_swapped(G_OBJECT(w->btnclose), "clicked", G_CALLBACK(gtk_widget_destroy), w->win);
	gtk_widget_set_events (w->drawing, GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_KEY_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_SCROLL_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK);

	init_render_thread(w);
	dirname = g_path_get_dirname(file_name);
	w->configfile = g_build_filename(dirname, CONFIGFILE, NULL);
	succ_load_config = load_config(w);
	gtk_widget_show_all(w->win);
	if (!succ_load_config)
		restoredefaults(w);

	start_render_thread(w);
}

void calc(GtkWidget *widget, struct winctl *w)
{
	if (!is_render_thread_alive(w)) {
		start_render_thread(w);
	} else {
		if (is_render_thread_pause(w)) {
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
			gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), vbox, FALSE, FALSE, 10);
			gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
			gtk_widget_show_all(vbox);
			response = gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);

			switch (response) {
				case RESPONSE_CONTDRAW:
					resume_render_thread(w);
					break;
				case RESPONSE_REDRAW: 
					kill_render_thread(w);
					start_render_thread(w);
					break;
				default:
					break;
			}
		} else {
			pause_render_thread(w);
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
	gdk_window_set_cursor(w->win->window, c);
	gdk_cursor_unref(c);
	if (!w->mandelbrot) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w->chkjulia), FALSE);
		restoredefaults(w);
		start_render_thread(w);
	}
	w->get_j = TRUE;
}

static gboolean expose_event(GtkWidget *widget, GdkEventExpose *event, struct winctl *w)
{
	redraw_drawing(w, event->area.x, event->area.y, event->area.width, event->area.height);
	return FALSE;
}

static gboolean configure_event(GtkWidget *widget, GdkEventConfigure *event, struct winctl *w)
{
	resize_drawing(w);
	return TRUE;
}

enum zoom_mode { ZOOM_IN, ZOOM_OUT };

static void zoom(struct winctl *w, enum zoom_mode mode) {
	gdouble b_re, b_im, dim, dre;
	gdouble val[4];
	gint px, py;
	double width, height;
	gdouble sf;

	
	sf = (mode == ZOOM_IN) ? w->scalefactor: 1.0 / w->scalefactor;

	gtk_widget_get_pointer(w->drawing, &px, &py);
	width = (gdouble)w->drawing->allocation.width;
	height = (gdouble)w->drawing->allocation.height;
	dre = (w->cplx[1] - w->cplx[0]) / width;
	dim = (w->cplx[3] - w->cplx[2]) / height;
	b_re = w->cplx[0] + dre * (px - sf * width / 2);
	b_im = w->cplx[3] - dim * (py - sf * height / 2);
	val[0] = b_re;
	val[1] = b_re + sf * dre * width;
	val[2] = b_im;
	val[3] = b_im - sf * dim * height;
	setcplxplane(w->txtcplx, val, width, height);
	start_render_thread(w);
}

static void setj(struct winctl *w)
{
	gchar buf[30];
	gdouble dim, dre;
	gint px, py;
	double width, height;
	gtk_widget_get_pointer(w->drawing, &px, &py);
	width = (gdouble)w->drawing->allocation.width;
	height = (gdouble)w->drawing->allocation.height;
	dre = (w->cplx[1] - w->cplx[0]) / width;
	dim = (w->cplx[3] - w->cplx[2]) / height;
	g_sprintf(buf, NUMFORMAT, w->cplx[0] + dre * px);
	gtk_entry_set_text(GTK_ENTRY(w->txtjre), buf);
	g_sprintf(buf, NUMFORMAT, w->cplx[3] - dim * py);
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

static gboolean key_press_event(GtkWidget *widget, GdkEventKey *event, struct winctl *w)
{
	if (event->keyval == GDK_i) {
		zoom(w, ZOOM_IN);
		return TRUE;
	} else if (event->keyval == GDK_o) {
		zoom(w, ZOOM_OUT);
		return TRUE;
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
		GdkDisplay *d;
		GdkScreen *s;
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
		gdk_window_set_cursor(w->win->window, c);
		gdk_cursor_unref(c);
		if (!w->xyscale) {
			delta = height;
			height = (double)width * w->drawing->allocation.height / w->drawing->allocation.width;
			delta -= height;
			if (delta) {
				d = gdk_display_get_default();
				s = gdk_display_get_default_screen(d);
				gdk_display_get_pointer(d, NULL, &px, &py, NULL); 
				py += sh * delta;
				gdk_display_warp_pointer(d, s, px, py);
				return TRUE;
			}
		}

		x = MIN(w->focus_x, px);
		y = MIN(w->focus_y, py);
		w->focus_area.x = x;
		w->focus_area.y = y;
		w->focus_area.width = width;
		w->focus_area.height = height;
		redraw_drawing(w, 0, 0, -1, -1);
		gtk_paint_focus(widget->style, widget->window, GTK_STATE_NORMAL, NULL, widget, NULL, x, y, width, height);
	}

	return TRUE;
}

static gboolean button_release_event(GtkWidget *widget, GdkEventButton *event, struct winctl *w)
{
	gchar buf[30];
	gdouble b_re, b_im, dim, dre;
	double width, height;
	gdouble val[4];

	gdk_window_set_cursor(GDK_WINDOW(w->win->window), NULL);
	if (w->get_j) {
		w->get_j = FALSE;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w->chkjulia), TRUE);
		restoredefaults(w);
		start_render_thread(w);
		return TRUE;
	}
	if (w->pixbufshow) {
		gdk_draw_pixbuf(widget->window, NULL, w->pixbufshow, 
				0, 0, 0, 0, -1, -1,
				GDK_RGB_DITHER_NORMAL, 0, 0);
	}
	
	if (!w->focus_area.width) {
		return TRUE;
	}
	
	width = (gdouble)w->drawing->allocation.width;
	height = (gdouble)w->drawing->allocation.height;
	dre = (w->cplx[1] - w->cplx[0]) / width;
	dim = (w->cplx[3] - w->cplx[2]) / height;
	b_re = w->cplx[0] + w->focus_area.x * dre;
	b_im = w->cplx[3] - w->focus_area.y * dim;

	if (w->xyscale) {
		g_sprintf(buf, NUMFORMAT, b_re);
		gtk_entry_set_text(GTK_ENTRY(w->txtcplx[0]), buf);
		g_sprintf(buf, NUMFORMAT, b_re + w->focus_area.width * dre);
		gtk_entry_set_text(GTK_ENTRY(w->txtcplx[1]), buf);
		g_sprintf(buf, NUMFORMAT, b_im - w->focus_area.height * dim);
		gtk_entry_set_text(GTK_ENTRY(w->txtcplx[2]), buf);
		g_sprintf(buf, NUMFORMAT, b_im);
		gtk_entry_set_text(GTK_ENTRY(w->txtcplx[3]), buf);
	} else {
		val[0] = b_re;
		val[1] = b_re + w->focus_area. width * dre;
		val[2] = b_im;
		val[3] = b_im - dim * w->focus_area.height;
		setcplxplane(w->txtcplx, val, width, height);
	}

	return TRUE;
}

static void destroy(GtkWidget *widget, struct winctl *w)
{
	destroy_render_thread(w);

	save_config(w);
	gtk_widget_destroy(w->drawmenu);
	gtk_widget_destroy(w->pjmenu);
	g_free(w->configfile);
	g_free(w);
	gtk_main_quit();
}
