
// this is a template-file for iteration.c.
// the follwing symbols should be defined before
// including this file:
// 	IT_FUNC_NAME --> name of the iteration fucntion
// 	IT_INLINE_FUNC --> function which is inline
// 	IT_VAR --> used variables + initialization
// 	IT_FIRST_FOR --> code after first for-statement
// the variables itre, itim hold the real-/imaginary part of the current point.

gpointer IT_FUNC_NAME(struct threaddata *p)
{
	gdouble b_im, itre, itim, dre, dim;
	guint ystart, x, y, xmax, ymax, xo, yo;
	guint iter, itermax;
	gboolean *retval;
	IT_VAR
	
	retval = (gboolean *)g_malloc(sizeof(gboolean));
	*retval = TRUE;
	b_im = p->b_im;
	ystart = p->ystart;
	xmax = p->xmax;
	ymax = p->ymax;
	xo = p->xoffset;
	yo = p->yoffset;
	dre = p->dre;
	dim = p->dim;
	itermax = p->itermax;

	itre = p->b_re;
	for (x = p->xstart; x < xmax; x += xo, itre += dre) {
#ifdef IT_FIRST_FOR
		IT_FIRST_FOR
#endif
		if (g_mutex_trylock(p->state_mutex)) {
			if (*p->pause) {
				// pause thread:
				if (*p->pause == THREAD_PAUSE) {
					g_cond_wait(p->resume_cond, p->state_mutex);
				}
				if (*p->pause == THREAD_TERMINATE) {
					g_mutex_unlock(p->state_mutex);
					*retval = FALSE;
					break;
				}
			}
			g_mutex_unlock(p->state_mutex);
		}

		itim = b_im;
		for (y = ystart; y < ymax; y += yo, itim += dim) {
			iter = 0;

			IT_INLINE_FUNC

			p->setcolor(p->pixels, x, y, p->n_channels, p->rowstride, iter, itermax, p->convdivcol);
		}
	}
	g_free(p->convdivcol);
	return (gpointer)retval;
}

#undef IT_FUNC_NAME
#undef IT_VAR
#undef IT_INLINE_FUNC
#undef IT_FIRST_FOR
