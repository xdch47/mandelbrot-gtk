#include <glib.h>
#include "render.h"
#include "math.h"

#define sqr(x) (x) * (x)
/* using dirty define/include-hacks for fast-inline templates... */
/* look at iterate_template.h */

/* mandelbrot_set function: */
#undef IT_FUNC_NAME
#define IT_FUNC_NAME mandelbrot_set
#undef IT_VAR
#define IT_VAR register gdouble zre, zim, tmpzre, tmpzim;
#undef IT_INIT
#define IT_INIT
#undef IT_INLINE_FUNC
#define IT_INLINE_FUNC \
	zre = itre; \
	zim = itim; \
	for (;;) { \
		tmpzre = sqr(zre); \
		tmpzim = sqr(zim); \
		if (tmpzre + tmpzim < 4.0) { \
			zim = 2.0 * zre * zim + itim; \
			zre = tmpzre - tmpzim + itre; \
		} else { \
			break; \
		} \
		++iter; \
		if (iter > itermax) \
			break; \
	} 
#undef IT_FIRST_FOR
#include "iterate_template.h"

/* mandelbrot_set function with status: */
#undef IT_FUNC_NAME
#define IT_FUNC_NAME mandelbrot_set_row_count
#undef IT_VAR
#define IT_VAR register gdouble zre, zim, tmpzre, tmpzim; \
	guint *col_count;
#undef IT_INIT
#define IT_INIT col_count = param->row_count + id;
#undef IT_FIRST_FOR
#define IT_FIRST_FOR ++*col_count;
#include "iterate_template.h"

/* julia_set function: */
#undef IT_FUNC_NAME
#define IT_FUNC_NAME julia_set
#undef IT_VAR
#define IT_VAR register gdouble zre, zim, tmpzre, tmpzim, jre, jim; 
#undef IT_INIT
#define IT_INIT jre = param->j[0]; \
	jim = param->j[1]; 
#undef IT_INLINE_FUNC
#define IT_INLINE_FUNC \
	zre = itre; \
	zim = itim; \
	for (;;) { \
		tmpzre = sqr(zre); \
		tmpzim = sqr(zim); \
		if (tmpzre + tmpzim < 4.0) { \
			zim = 2.0 * zre * zim + jim; \
			zre = tmpzre - tmpzim + jre; \
		} else { \
			break; \
		} \
		++iter; \
		if (iter > itermax) \
			break; \
	} 
#undef IT_FIRST_FOR
#include "iterate_template.h"

/* julia_set function with status: */
#undef IT_FUNC_NAME
#define IT_FUNC_NAME julia_set_row_count
#undef IT_VAR
#define IT_VAR register gdouble zre, zim, tmpzre, tmpzim, jre, jim; \
	guint *col_count;
#undef IT_INIT
#define IT_INIT jre = param->j[0]; \
	jim = param->j[1]; \
	col_count = param->row_count + id;
#undef IT_FIRST_FOR
#define IT_FIRST_FOR ++*col_count;
#include "iterate_template.h"

/* mandelbrot_set_deg: */
#undef IT_FUNC_NAME
#define IT_FUNC_NAME mandelbrot_set_deg
#undef IT_VAR
#define IT_VAR register gdouble zre, zim, r, phi; \
	register gdouble degree;
#undef IT_INIT
#define IT_INIT degree = param->degree;
#undef IT_INLINE_FUNC
#define IT_INLINE_FUNC \
	zre = itre; \
	zim = itim; \
	for (;;) { \
		r = sqr(zre) + sqr(zim); \
		if (r < 4.0) { \
			r = pow(r, degree / 2.0); \
			phi = atan2(zim, zre); \
			zre = r * cos(degree  * phi) + itre; \
			zim = r * sin(degree * phi) + itim; \
		} else { \
			break; \
		} \
		++iter; \
		if (iter > itermax) { \
			break; \
		} \
	} 
#undef IT_FIRST_FOR
#include "iterate_template.h"
#undef IT_FUNC_NAME
#define IT_FUNC_NAME mandelbrot_set_deg_row_count
#undef IT_VAR
#define IT_VAR register gdouble zre, zim, r, phi; \
	register gdouble degree; \
	guint *col_count;
#undef IT_INIT
#define IT_INIT degree = param->degree; \
	col_count = param->row_count + id;
#undef IT_FIRST_FOR
#define IT_FIRST_FOR ++*col_count;
#include "iterate_template.h"

/* julia_set_deg function: */
#undef IT_FUNC_NAME
#define IT_FUNC_NAME julia_set_deg
#undef IT_VAR
#define IT_VAR register gdouble zre, zim, jre, jim, r, phi; \
	gdouble degree;
#undef IT_INIT
#define IT_INIT degree = param->degree; \
	jre = param->j[0]; \
	jim = param->j[1];
#undef IT_INLINE_FUNC
#define IT_INLINE_FUNC \
	zre = itre; \
	zim = itim; \
	for (;;) { \
		r = sqr(zre) + sqr(zim); \
		if (r < 4.0) { \
			r = pow(r, degree / 2.0); \
			phi = atan2(zim, zre); \
			zre = r * cos(degree * phi) + jre; \
			zim = r * sin(degree * phi) + jim; \
		} else { \
			break; \
		} \
		++iter; \
		if (iter > itermax) { \
			break; \
		} \
	}
#undef IT_FIRST_FOR
#include "iterate_template.h"

/* julia_set function with status */
#undef IT_FUNC_NAME
#define IT_FUNC_NAME julia_set_deg_row_count
#undef IT_VAR
#define IT_VAR register gdouble zre, zim, jre, jim, r, phi; \
	register gdouble degree; \
	guint *col_count;
#undef IT_INIT 
#define IT_INIT degree = param->degree; \
	jre = param->j[0]; \
	jim = param->j[1]; \
	col_count = param->row_count + id;
#define IT_FIRST_FOR ++(*col_count);
#include "iterate_template.h"

inline static void put_pixel(guchar *pixels, guint x, guint y, guint n_channels, guint rowstride, const guchar *color)
{
	guint i;
	guchar *p;

	p = pixels + y * rowstride + x * n_channels;
	for (i = 0; i < 4; ++i) 
		*p++ = *color++;
}
