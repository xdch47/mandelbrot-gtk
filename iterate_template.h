
/* this is a template-file for iteration.c. */
/* the follwing symbols should be defined before */
/* including this file: */
/* 	IT_FUNC_NAME --> name of the iteration fucntion */
/* 	IT_INLINE_FUNC --> function which is inline */
/* 	IT_VAR --> used variables + initialization */
/* 	IT_FIRST_FOR --> code after first for-statement */
/* the variables itre, itim hold the real-/imaginary part of the current point. */

inline static void put_pixel(guchar *pixels, guint x, guint y, guint n_channels, guint rowstride, const guchar *color);

gpointer IT_FUNC_NAME(struct iteration_data *p)
{
	guint y, yo;
	gdouble dim, b_re;
	register guint x, xo, iter;
	register gdouble itre, dre, itim;
	guint xstart, xmax, ymax;
	guint itermax;
	guint id;
	const struct iterate_param *param;
	gboolean *retval;
	guchar *pixels;
	guint n_channels, rowstride;
	const guchar *color;

	IT_VAR
	
	retval = (gboolean *)g_malloc(sizeof(gboolean));
	*retval = TRUE;
	id = p->number;
	b_re = p->b_re;
	dre = p->dre;
	dim = p->dim;
	param = p->param;
	xstart = param->xstart[id];
	ymax = param->ymax;
	xmax = param->xmax;
	yo = param->yoffset;
	xo = param->xoffset;
	itermax = param->itermax;
	pixels = param->pixels;
	n_channels = param->n_channels;
	rowstride = param->rowstride;

	IT_INIT

	itim = p->b_im;
	for (y = param->ystart[id]; y < ymax; y += yo, itim += dim) {
#ifdef IT_FIRST_FOR
		IT_FIRST_FOR
#endif
		if (g_mutex_trylock(p->state_mutex)) {
			if (*p->state) {
				/* pause thread: */
				if (*p->state == PAUSE) {
					g_cond_wait(p->resume_cond, p->state_mutex);
				}
				if (*p->state == KILL) {
					g_mutex_unlock(p->state_mutex);
					*retval = FALSE;
					break;
				}
			}
			g_mutex_unlock(p->state_mutex);
		}

		itre = b_re;
		for (x = xstart; x < xmax; x += xo, itre += dre) {
			iter = 0;

			IT_INLINE_FUNC

			if (iter < param->itermax) {
				color = param->setcolor(iter, itermax);
				put_pixel(pixels, x, y, n_channels, rowstride, color);
			} else {
				put_pixel(pixels, x, y, n_channels, rowstride, param->color);
			}
		}
	}
	return (gpointer)retval;
}

