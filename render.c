// mandelbrot-gtk - Multithreaded GTK Application for rendering the mandelbrot and julia-set
//
// Copyright (C) 2010-2020  Felix Neumärker
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "render.h"
#include "defs.h"
#include "libcolor/color.h"

#define sqr(x) ((x) * (x))

static void render_thread(struct render_thread *r);
static void add_idle(struct render_thread *r);
static void remove_idle(struct render_thread *r);
static gboolean idle_wrapper(struct render_thread *r);

/*
static gint factor(gint num)
{
	gint i, f = 1;
	for (i = 1; sqr(i) <= num; ++i) {
		if (num % i == 0) {
	a		f = i;
		}
	}
	return f;
}
*/

struct render_thread *render_thread_new(IdleFunc idle_func, ThreaddestroyFunc destroy, gpointer data)
{
	struct render_thread *r = (struct render_thread *)g_malloc(sizeof(struct render_thread));
	g_mutex_init(&r->control_mutex);
	g_mutex_init(&r->state_mutex);
	g_mutex_init(&r->idle_mutex);
	g_cond_init(&r->resume_cond);
	r->control_thread = NULL;
	r->isalive = FALSE;
	r->state = RUN;
	r->idle_func = idle_func;
	r->idle_tag = 0;
	r->destroy = destroy;
	r->userdata = data;
	r->it_data = NULL;
	r->invoked = FALSE;
	return r;
}

void iterate_param_init(struct iterate_param *param, gint count)
{
	gint i, j;
	gint xo, yo;
	param->threads_count = count;
	param->xstart = (gint *)g_malloc(sizeof(gint) * count);
	param->ystart = (gint *)g_malloc(sizeof(gint) * count);
	param->row_count = (gint *)g_malloc(sizeof(gint) * count);

	/*
	 * yo = factor(count);
	 * xo = count / yo;
	 */
	xo = 1;
	yo = count;

	for (i = 0; i < count; i += yo) {
		for (j = 0; j < yo; ++j) {
			/*
			 * param->xstart[i + j] = i / yo;
			 * param->ystart[i + j] = j;
			 */
			param->xstart[i + j] = 0;
			param->ystart[i + j] = i + j;
		}
	}

	param->xoffset = xo;
	param->yoffset = yo;
}

void iterate_param_free(struct iterate_param *param)
{
	g_free(param->xstart);
	g_free(param->ystart);
	g_free(param->row_count);
}

void render_thread_free(struct render_thread *r)
{
	g_mutex_clear(&r->control_mutex);
	g_mutex_clear(&r->state_mutex);
	g_mutex_clear(&r->idle_mutex);
	g_cond_clear(&r->resume_cond);
	g_free(r);
}

gboolean start_render_thread(struct render_thread *r, const struct iterate_param *param)
{
	gint i;
	struct iteration_data *it;
	g_mutex_lock(&r->control_mutex);
	r->it_data = (struct iteration_data *)g_malloc(sizeof(struct iteration_data) * param->threads_count);
	for (i = 0, it = r->it_data; i < param->threads_count; ++i, ++it) {
		it->param = param;
		it->number = i;
		it->dre = (param->cplxplane[1] - param->cplxplane[0]) / (gdouble)param->xmax;
		it->dim = (param->cplxplane[2] - param->cplxplane[3]) / (gdouble)param->ymax;
		it->b_re = param->cplxplane[0] + (gdouble)param->xstart[i] * it->dre;
		it->b_im = param->cplxplane[3] + (gdouble)param->ystart[i] * it->dim;
		it->dre *= (gdouble)param->xoffset;
		it->dim *= (gdouble)param->yoffset;
		it->state_mutex = &r->state_mutex;
		it->resume_cond = &r->resume_cond;
		it->state = &r->state;
		param->row_count[i] = 0;
	}
	r->param = param;

	add_idle(r);

	g_mutex_lock(&r->state_mutex);
	r->isalive = TRUE;
	r->state = RUN;
	r->control_thread = g_thread_new("render_thread", (GThreadFunc)render_thread, (gpointer)r);
	g_mutex_unlock(&r->state_mutex);
	g_mutex_unlock(&r->control_mutex);
	return TRUE;
}

void render_thread_pause(struct render_thread *r)
{
	g_mutex_lock(&r->control_mutex);
	if (is_render_thread_alive(r)) {
		g_mutex_lock(&r->state_mutex);
		r->state = PAUSE;
		g_mutex_unlock(&r->state_mutex);
		remove_idle(r);
	}
	g_mutex_unlock(&r->control_mutex);
}

void render_thread_resume(struct render_thread *r)
{
	g_mutex_lock(&r->control_mutex);
	g_mutex_lock(&r->state_mutex);
	r->state = RUN;
	add_idle(r);
	g_cond_broadcast(&r->resume_cond);
	g_mutex_unlock(&r->state_mutex);
	g_mutex_unlock(&r->control_mutex);
}

void render_thread_kill(struct render_thread *r)
{
	g_mutex_lock(&r->control_mutex);
	remove_idle(r);
	if (r->control_thread) {
		g_mutex_lock(&r->state_mutex);
		r->state = KILL;
		g_mutex_unlock(&r->state_mutex);
		g_cond_broadcast(&r->resume_cond);
		g_thread_join(r->control_thread);
		r->control_thread = NULL;
	}
	g_mutex_unlock(&r->control_mutex);
}

gboolean is_render_thread_alive(struct render_thread *r)
{
	gboolean isalive;
	g_mutex_lock(&r->state_mutex);
	isalive = r->isalive;
	g_mutex_unlock(&r->state_mutex);
	return isalive;
}

gboolean is_render_thread_pause(struct render_thread *r)
{
	enum thread_state state;
	g_mutex_lock(&r->state_mutex);
	state = r->state;
	g_mutex_unlock(&r->state_mutex);
	return (state == PAUSE) ? TRUE : FALSE;
}

static void render_thread(struct render_thread *r)
{
	gint i, threads_count = r->param->threads_count;
	GThread **iteration_thread = (GThread **)(g_malloc(sizeof(GThread *) * threads_count));
	struct ThreaddestroyData *data;

	data = g_malloc(sizeof(struct ThreaddestroyData));
	initialize_func(r->param->color_func_index);
	for (i = 0; i < threads_count; ++i) {
		iteration_thread[i] = g_thread_new("iterate_thread", r->param->iterate_func, (gpointer)(r->it_data + i));
	}

	data->succ = TRUE;
	for (i = 0; i < threads_count; ++i) {
		gboolean *tmp;
		tmp = g_thread_join(iteration_thread[i]);
		data->succ = data->succ && *tmp;
		g_free(tmp);
	}
	g_free(r->it_data);
	g_free(iteration_thread);

	remove_idle(r);
	g_mutex_lock(&r->state_mutex);
	gboolean iskilled = r->state == KILL;
	data->succ = data->succ && !iskilled;
	r->isalive = FALSE;
	r->state = RUN;
	g_mutex_unlock(&r->state_mutex);
	data->data = r->userdata;
	if (!iskilled) {
		r->invoked = TRUE;
		g_main_context_invoke(NULL, (GSourceFunc)r->destroy, data);
	}
	return;
}

static void add_idle(struct render_thread *r)
{
	g_mutex_lock(&r->idle_mutex);
	if (!r->idle_tag)
		r->idle_tag = g_idle_add((GSourceFunc)idle_wrapper, r);
	g_mutex_unlock(&r->idle_mutex);
}

static void remove_idle(struct render_thread *r)
{
	g_mutex_lock(&r->idle_mutex);
	if (r->idle_tag) {
		r->idle_tag = 0;
	}
	g_mutex_unlock(&r->idle_mutex);
}

static gboolean idle_wrapper(struct render_thread *r)
{
	gboolean retval;

	g_main_context_invoke(NULL, (GSourceFunc)r->idle_func, r->userdata);
	g_mutex_lock(&r->idle_mutex);
	retval = (r->idle_tag) ? TRUE : FALSE;
	g_mutex_unlock(&r->idle_mutex);
	return retval;
}
