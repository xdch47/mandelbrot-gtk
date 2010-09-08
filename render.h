
#ifndef __RENDER_H__
#define __RENDER_H__

#include <glib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "libcolor/color.h"

enum fraktal_type { MANDELBROT_SET, JULIA_SET };
enum thread_state { RUN, PAUSE, KILL };

struct iterate_param {
	GThreadFunc iterate_func;
	gint threads_count;
	gdouble cplxplane[4];
	enum fraktal_type type;
	gint itermax;
	gdouble degree;
	gdouble j[2];
	gint *xstart;
	gint *ystart;
	gint xoffset;
	gint yoffset;
	gint *row_count;
	gint xmax;
	gint ymax;
	IterationInfo *itermap;
	guchar *pixels;
	gint n_channels;
	gint rowstride;
	gint color_func_index;
	guchar color[3];
	ColorFunc colorfunc;
	void (*update_func)(gint x, gint y, const IterationInfo *iterinfo, const struct iterate_param *p);
};

struct iteration_data {
	GMutex *state_mutex;
	GCond *resume_cond;
	volatile enum thread_state *state;
	const struct iterate_param *param;
	gint number;
	gdouble b_re; 
	gdouble b_im;
	gdouble dre;
	gdouble dim;
};

typedef void (*IdleFunc)(gpointer data);
typedef void (*ThreaddestroyFunc)(gboolean succ, gpointer data);

struct render_thread {
	GMutex *control_mutex;
	GMutex *state_mutex;
	GMutex *idle_mutex;
	GCond *resume_cond;
	GThread *control_thread;
	volatile gboolean isalive;
	volatile enum thread_state state;
	struct iteration_data *it_data;
	gint idle_tag;
	IdleFunc idle_func;
	ThreaddestroyFunc destroy;
	const struct iterate_param *param;
	gpointer userdata;
};


/* implemented by render.c */
struct render_thread *render_thread_new(IdleFunc idle_func, ThreaddestroyFunc destroy, gpointer data);
void iterate_param_init(struct iterate_param *param, gint count);
void iterate_param_free(struct iterate_param *param);
void render_thread_free(struct render_thread *r);
gboolean start_render_thread(struct render_thread *r, const struct iterate_param *param);
void render_thread_pause(struct render_thread *r);
void render_thread_resume(struct render_thread *r);
void render_thread_kill(struct render_thread *r);
gboolean is_render_thread_alive(struct render_thread *r);
gboolean is_render_thread_pause(struct render_thread *r);

/* implemented by iterate.c */
gpointer mandelbrot_set(struct iteration_data *p);
gpointer mandelbrot_set_row_count(struct iteration_data *p);
gpointer mandelbrot_set_deg(struct iteration_data *p);
gpointer mandelbrot_set_deg_row_count(struct iteration_data *p);
gpointer julia_set(struct iteration_data *p);

gpointer julia_set_row_count(struct iteration_data *p);
gpointer julia_set_deg(struct iteration_data *p);
gpointer julia_set_deg_row_count(struct iteration_data *p);

#endif /* __RENDER_H__ */

