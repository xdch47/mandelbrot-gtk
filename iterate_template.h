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

/* this is a template-file for iteration.c. */
/* the follwing symbols should be defined before */
/* including this file: */
/* 	IT_FUNC_NAME --> name of the iteration fucntion */
/* 	IT_INLINE_FUNC --> function which is inline */
/* 	IT_VAR --> used variables + initialization */
/* 	IT_FIRST_FOR --> code after first for-statement */
/* the variables itre, itim hold the real-/imaginary part of the current point. */

gpointer IT_FUNC_NAME(struct iteration_data *p)
{
	gint y, yo;
	gdouble dim, b_re;
	register gint x, xo, iter;
	register gdouble itre, dre, itim;
	gint xstart, xmax, ymax;
	gint itermax;
	gint id;
	const struct iterate_param *param;
	gboolean *retval;
	IterationInfo *iterinfo;

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

			#ifdef USE_UPDATE_FUNC
			(*param->update_func)(x, y, iterinfo, param);
			#endif


		}
	}
	return (gpointer)retval;
}

