
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
	guint itermax;
	gdouble degree;
	gdouble j[2];
	guint *xstart;
	guint *ystart;
	guint xoffset;
	guint yoffset;
	guint *row_count;
	guint xmax;
	guint ymax;
	IterationInfo *itermap;
	guchar *pixels;
	guint n_channels;
	guint rowstride;
	guint color_func_index;
	guchar *color;
	ColorFunc colorfunc;
	void (*update_func)(guint x, guint y, const IterationInfo *iterinfo, const struct iterate_param *p);
};

struct iteration_data {
	GMutex *state_mutex;
	GCond *resume_cond;
	volatile enum thread_state *state;
	const struct iterate_param *param;
	guint number;
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
void iterate_param_init(struct iterate_param *param, guint count);
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

