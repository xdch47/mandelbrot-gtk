
#include <stdlib.h>
#include <errno.h>
#include "render.h"
#include "iterate.h"
#include "draw.h"
#include "validate.h"

static void render_thread(struct winctl *w);
static void add_redraw_idle(struct winctl *w);
static void remove_redraw_idle(struct winctl *w);
static gboolean redraw(struct winctl *w);

gboolean is_render_thread_alive(struct winctl *w)
{
	gboolean alive;
	g_mutex_lock(w->threadstate_mutex);
	alive = w->runthread;
	g_mutex_unlock(w->threadstate_mutex);
	return alive;
}

gboolean is_render_thread_pause(struct winctl *w)
{
	gboolean pause;
	g_mutex_lock(w->threadstate_mutex);
	pause = w->pausethread;
	g_mutex_unlock(w->threadstate_mutex);
	return pause;
}

gboolean start_render_thread(struct winctl *w)
{
	guint i;
	gdouble degree;
	guint itermax;
	gdouble cplx[4];
	gdouble jre, jim;
	int errno;
	gchar *endptr;
	struct threaddata *it;

	// validate complex plane
	if (!validatecplx(w->txtcplx, cplx, w->win)) {
		return FALSE;
	}
	// degree of mandelbrot-/julia-set:
	degree = strtod(gtk_entry_get_text(GTK_ENTRY(w->txtdegree)), &endptr);
	if (*endptr != '\0' || errno == ERANGE || degree < LOWDEGREE || degree > HIDEGREE) {
		gchar msg[BUFSIZE];
		g_snprintf(msg, BUFSIZE, ERRDEGREE, LOWDEGREE, HIDEGREE);
		errdialog(GTK_WINDOW(w->win), msg);
		gtk_widget_grab_focus(w->txtdegree);
		return FALSE;
	}
	// max. iterations:
	if (!validateitermax(w->txtitermax, &itermax, w->win)) {
			return FALSE;
	}
	// julia const:
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
		w->mandelbrot = FALSE;
	} else {
		jre = 0;
		jim = 0;
		w->mandelbrot = TRUE;
	}

	// okay, no error occured, redraw, write threaddata, start thread:
	kill_render_thread(w);
	if (w->pixbufcalc)
		g_object_unref(w->pixbufcalc);
	if (w->pixbufshow) 
		g_object_unref(w->pixbufshow);

	w->pixbufcalc = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, w->drawing->allocation.width, w->drawing->allocation.height);
	clearpixbuf(w->pixbufcalc);
	w->pixbufshow = g_object_ref(w->pixbufcalc);
	gdk_window_invalidate_rect(w->drawing->window, NULL, FALSE);
	gdk_draw_pixbuf(w->drawing->window, NULL, w->pixbufcalc, 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NORMAL, 0, 0);
	
	gtk_button_set_label(GTK_BUTTON(w->btncalc), LSTOP);

	g_mutex_lock(w->thread_control_mutex);
	for (i = 0, it = w->data; i < CALCTHREADS; ++i, ++it) {
		it->xmax = gdk_pixbuf_get_width(w->pixbufcalc);
		it->ymax = gdk_pixbuf_get_height(w->pixbufcalc);
		it->dre = (cplx[1] - cplx[0]) / (gdouble)it->xmax;
		it->dim = (cplx[2] - cplx[3]) / (gdouble)it->ymax;
		it->b_re = cplx[0] + (gdouble)XSTART[i] * it->dre;
		it->b_im = cplx[3] + (gdouble)YSTART[i] * it->dim;
		it->dre *= (gdouble)XOFFSET[i];
		it->dim *= (gdouble)YOFFSET[i];
		it->jre = jre;
		it->jim = jim;
		it->xstart = XSTART[i];
		it->ystart = YSTART[i];
		it->xoffset = XOFFSET[i];
		it->yoffset = YOFFSET[i];
		it->itermax = itermax;
		it->pixels = gdk_pixbuf_get_pixels(w->pixbufcalc);
		it->n_channels = gdk_pixbuf_get_n_channels (w->pixbufcalc);
		it->rowstride = gdk_pixbuf_get_rowstride (w->pixbufcalc);
		it->state_mutex = w->threadstate_mutex;
		it->resume_cond = w->resume_cond;
		it->pause = &w->pausethread;
		if (w->color_func_index != 1) {
			it->convdivcol = (guchar *)g_malloc(sizeof(guchar) * 4);
			it->convdivcol[0] = (guchar)(w->convcol.red >> 8);
			it->convdivcol[1] = (guchar)(w->convcol.green >> 8);
			it->convdivcol[2] = (guchar)(w->convcol.blue >> 8);
			it->convdivcol[3] = 0xff;
		} else {
			it->convdivcol = (guchar *)g_malloc(sizeof(guchar) * 8);
			it->convdivcol[0] = (guchar)(w->convcol.red >> 8);
			it->convdivcol[1] = (guchar)(w->convcol.green >> 8);
			it->convdivcol[2] = (guchar)(w->convcol.blue >> 8);
			it->convdivcol[3] = 0xff;
			it->convdivcol[4] = (guchar)(w->divcol.red >> 8);
			it->convdivcol[5] = (guchar)(w->divcol.green >> 8);
			it->convdivcol[6] = (guchar)(w->divcol.blue >> 8);
			it->convdivcol[7] = 0xff;
		}
		it->setcolor = color_func[w->color_func_index];
		it->degree = degree;
		w->cplx[i] = cplx[i];
	}

	add_redraw_idle(w);

	g_mutex_lock(w->threadstate_mutex);
	w->runthread = TRUE;
	w->pausethread = FALSE;
	w->render_thread = g_thread_create((GThreadFunc)render_thread, (gpointer)w, TRUE, NULL);
	g_mutex_unlock(w->threadstate_mutex);
	g_mutex_unlock(w->thread_control_mutex);
	return TRUE;
}

void pause_render_thread(struct winctl *w)
{
	g_mutex_lock(w->thread_control_mutex);
	if (is_render_thread_alive(w)) {
		g_mutex_lock(w->threadstate_mutex);
		w->pausethread = THREAD_PAUSE;
		gtk_button_set_label(GTK_BUTTON(w->btncalc), LCALC);
		g_mutex_unlock(w->threadstate_mutex);
		remove_redraw_idle(w);
	}
	g_mutex_unlock(w->thread_control_mutex);
}

void resume_render_thread(struct winctl *w)
{
	g_mutex_lock(w->thread_control_mutex);
	g_mutex_lock(w->threadstate_mutex);
	w->pausethread = 0;
	add_redraw_idle(w);
	g_cond_broadcast(w->resume_cond);
	gtk_button_set_label(GTK_BUTTON(w->btncalc), LSTOP);
	g_mutex_unlock(w->threadstate_mutex);
	g_mutex_unlock(w->thread_control_mutex);
}


void kill_render_thread(struct winctl *w)
{
	g_mutex_lock(w->thread_control_mutex);
	remove_redraw_idle(w);
	if (w->render_thread) {
		g_mutex_lock(w->threadstate_mutex);
		w->pausethread = THREAD_TERMINATE;
		g_mutex_unlock(w->threadstate_mutex);
		g_cond_broadcast(w->resume_cond);
		gdk_threads_leave();
		g_thread_join(w->render_thread);
		gdk_threads_enter();
		w->render_thread = NULL;
	}
	g_mutex_unlock(w->thread_control_mutex);
}
void init_render_thread(struct winctl *w)
{
	w->thread_control_mutex = g_mutex_new();
	w->threadstate_mutex = g_mutex_new();
	w->redraw_mutex = g_mutex_new();
	w->resume_cond = g_cond_new();
	w->render_thread = NULL;
	w->runthread = FALSE;
	w->redraw_idle = 0;
}

void destroy_render_thread(struct winctl *w)
{
	kill_render_thread(w);

	g_mutex_free(w->threadstate_mutex);
	g_mutex_free(w->redraw_mutex);
	g_cond_free(w->resume_cond);
}

static void render_thread(struct winctl *w)
{
	guint i;
	GThread *calc_thread[CALCTHREADS];
	gboolean succ;

	for (i = 0; i < CALCTHREADS; ++i) 
		if (w->data->degree == 2.0) {
			if (w->mandelbrot)
				calc_thread[i] = g_thread_create((GThreadFunc)mandelbrot_set, (gpointer)(w->data + i), TRUE, NULL);
			else
				calc_thread[i] = g_thread_create((GThreadFunc)julia_set, (gpointer)(w->data + i), TRUE, NULL);
		} else {
			if (w->mandelbrot)
				calc_thread[i] = g_thread_create((GThreadFunc)mandelbrot_set_deg, (gpointer)(w->data + i), TRUE, NULL);
			else
				calc_thread[i] = g_thread_create((GThreadFunc)julia_set_deg, (gpointer)(w->data + i), TRUE, NULL);
		}
	
	succ = TRUE;
	for (i = 0; i < CALCTHREADS; ++i) {
		gboolean *tmp;
		tmp = g_thread_join(calc_thread[i]);
		succ = succ && *tmp;
		g_free(tmp);
	}

	remove_redraw_idle(w);

	g_mutex_lock(w->threadstate_mutex);
	succ = succ && w->pausethread != THREAD_TERMINATE;
	g_mutex_unlock(w->threadstate_mutex);
	if (succ) {
		gdk_threads_enter();
		if (w->pixbufshow)
			g_object_unref(w->pixbufshow);
		w->pixbufshow = gdk_pixbuf_scale_simple(w->pixbufcalc, w->drawing->allocation.width, w->drawing->allocation.height, INTERPOLATION);
		gdk_draw_pixbuf(w->drawing->window, NULL, w->pixbufshow, 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NORMAL, 0, 0);
		gtk_button_set_label(GTK_BUTTON(w->btncalc), LCALC);
		gtk_window_set_resizable(GTK_WINDOW(w->win), TRUE);
		gtk_widget_set_size_request(w->win, -1, -1);
		gdk_window_invalidate_rect(w->drawing->window, NULL, FALSE);
		gdk_threads_leave();
	}
	g_mutex_lock(w->threadstate_mutex);
	w->runthread = FALSE;
	w->pausethread = 0;
	g_mutex_unlock(w->threadstate_mutex);

	return;
		
}

static void add_redraw_idle(struct winctl *w) 
{
	g_mutex_lock(w->redraw_mutex);
	if (!w->redraw_idle) 
		w->redraw_idle = g_idle_add((GSourceFunc)redraw, w);
	g_mutex_unlock(w->redraw_mutex);
}

static void remove_redraw_idle(struct winctl *w)
{
	g_mutex_lock(w->redraw_mutex);
	if (w->redraw_idle) {
		w->redraw_idle = 0;
	}
	g_mutex_unlock(w->redraw_mutex);
}

static gboolean redraw(struct winctl *w)
{
	gboolean retval;
	gint width, height;
	g_mutex_lock(w->redraw_mutex);
	gdk_threads_enter();
	width = w->drawing->allocation.width;
	height = w->drawing->allocation.height;
	if (width != gdk_pixbuf_get_width(w->pixbufcalc) || height != gdk_pixbuf_get_height(w->pixbufcalc)) {
		g_assert(w->pixbufshow != NULL);
		g_object_unref(w->pixbufshow);
		w->pixbufshow = gdk_pixbuf_scale_simple(w->pixbufcalc, width, height, INTERPOLATION);
	}
	redraw_drawing(w, 0, 0, -1, -1);
	retval = w->redraw_idle ? TRUE : FALSE;
	gdk_threads_leave();
	g_mutex_unlock(w->redraw_mutex);
	return retval;
}

