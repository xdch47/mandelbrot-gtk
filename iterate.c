#include <glib.h>
#include "render.h"
#include "math.h"
#include "libcolor/color.h"

#define sqr(x) (x) * (x)
/* using dirty define/include-hacks for fast-inline templates... */
/* look at iterate_template.h */

static int outcircle(gdouble cx, gdouble cy, gdouble sqrr, double x, double y)
{
	x -= cx;
	y -= cy;
	return (x * x + y * y > sqrr) ? 1 : 0;
}


/* mandelbrot_set function: */
#undef IT_FUNC_NAME
#define IT_FUNC_NAME mandelbrot_set
#undef IT_VAR
#define IT_VAR register gdouble zre, zim, tmpzre, tmpzim;
#undef IT_INIT
#define IT_INIT
#undef IT_INLINE_FUNC
#define IT_INLINE_FUNC                                                \
        zre = itre;                                                   \
        zim = itim;                                                   \
        tmpzre = sqr(zre);                                            \
        tmpzim = sqr(zim);                                            \
        iterinfo = param->itermap + x + y * xmax;                     \
        /* iterinfo->sqr_abs_z = tmpzre + tmpzim; */                  \
        if ((outcircle(-0.11, 0.0, sqr(0.63), zre, zim) || zre > 0.1) \
            && outcircle(-1.0, 0.0, sqr(0.25), zre, zim)              \
            && outcircle(-0.125, 0.744, sqr(0.092), zre, zim)         \
            && outcircle(-1.308, 0.0, sqr(0.058), zre, zim)           \
            && outcircle(0.0, 0.25, sqr(0.35), zre, zim)) {           \
                for (;;) {                                            \
                        if (tmpzre + tmpzim < 4.0) {                  \
                                zim = 2.0 * zre * zim + itim;         \
                                zre = tmpzre - tmpzim + itre;         \
                        } else {                                      \
                                iterinfo->iter = iter;                \
                                break;                                \
                        }                                             \
                        ++iter;                                       \
                        if (iter > itermax) {                         \
                                iterinfo->iter = -1;                  \
                                break;                                \
                        }                                             \
                        tmpzre = sqr(zre);                            \
                        tmpzim = sqr(zim);                            \
                }                                                     \
        } else {                                                      \
                iterinfo->iter = -1;                                  \
        }
#undef IT_FIRST_FOR
#define USE_UPDATE_FUNC
#include "iterate_template.h"

#undef IT_FUNC_NAME
#define IT_FUNC_NAME mandelbrot_set_row_count
#undef IT_VAR
#define IT_VAR register gdouble zre, zim, tmpzre, tmpzim; \
	gint *col_count;
#undef IT_INIT
#define IT_INIT col_count = param->row_count + id;
#undef IT_FIRST_FOR
#define IT_FIRST_FOR ++*col_count;
#undef USE_UPDATE_FUNC
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
#define IT_INLINE_FUNC                                \
        zre = itre;                                   \
        zim = itim;                                   \
        tmpzre = sqr(zre);                            \
        tmpzim = sqr(zim);                            \
        iterinfo = param->itermap + x + y * xmax;     \
        /* iterinfo->sqr_abs_z = tmpzre + tmpzim; */  \
        for (;;) {                                    \
                if (tmpzre + tmpzim < 4.0) {          \
                        zim = 2.0 * zre * zim + jim;  \
                        zre = tmpzre - tmpzim + jre;  \
                } else {                              \
			iterinfo->iter = iter;        \
                        break;                        \
                }                                     \
                ++iter;                               \
                if (iter > itermax) {                 \
			iterinfo->iter = -1;          \
                        break;                        \
		}                                     \
                tmpzre = sqr(zre);                    \
                tmpzim = sqr(zim);                    \
        }
#undef IT_FIRST_FOR
#define USE_UPDATE_FUNC
#include "iterate_template.h"

/* julia_set function with status: */
#undef IT_FUNC_NAME
#define IT_FUNC_NAME julia_set_row_count
#undef IT_VAR
#define IT_VAR register gdouble zre, zim, tmpzre, tmpzim, jre, jim; \
	gint *col_count;
#undef IT_INIT
#define IT_INIT jre = param->j[0]; \
	jim = param->j[1]; \
	col_count = param->row_count + id;
#undef IT_FIRST_FOR
#define IT_FIRST_FOR ++*col_count;
#undef USE_UPDATE_FUNC
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
#define IT_INLINE_FUNC                                      \
        zre = itre;                                         \
        zim = itim;                                         \
        r = sqr(zre) + sqr(zim);                            \
        iterinfo = param->itermap + x + y * xmax;           \
        /* iterinfo->sqr_abs_z = r; */                      \
        for (;;) {                                          \
                if (r < 4.0) {                              \
                        r = pow(r, degree / 2.0);           \
                        phi = atan2(zim, zre);              \
                        zre = r * cos(degree * phi) + itre; \
                        zim = r * sin(degree * phi) + itim; \
                } else {                                    \
                        iterinfo->iter = iter;              \
                        break;                              \
                }                                           \
                ++iter;                                     \
                if (iter > itermax) {                       \
                        iterinfo->iter = -1;                \
                        break;                              \
                }                                           \
                r = sqr(zre) + sqr(zim);                    \
        }
#undef IT_FIRST_FOR
#define USE_UPDATE_FUNC
#include "iterate_template.h"

/* mandelbrot_set_deg function with status */
#undef IT_FUNC_NAME
#define IT_FUNC_NAME mandelbrot_set_deg_row_count
#undef IT_VAR
#define IT_VAR register gdouble zre, zim, r, phi; \
	register gdouble degree; \
	gint *col_count;
#undef IT_INIT
#define IT_INIT degree = param->degree; \
	col_count = param->row_count + id;
#undef IT_FIRST_FOR
#define IT_FIRST_FOR ++*col_count;
#undef USE_UPDATE_FUNC
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
#define IT_INLINE_FUNC                                     \
        zre = itre;                                        \
        zim = itim;                                        \
        r = sqr(zre) + sqr(zim);                           \
        iterinfo = param->itermap + x + y * xmax;          \
        /* iterinfo->sqr_abs_z = r; */                     \
        for (;;) {                                         \
                if (r < 4.0) {                             \
                        r = pow(r, degree / 2.0);          \
                        phi = atan2(zim, zre);             \
                        zre = r * cos(degree * phi) + jre; \
                        zim = r * sin(degree * phi) + jim; \
                } else {                                   \
                        iterinfo->iter = iter;             \
                        break;                             \
                }                                          \
                ++iter;                                    \
                if (iter > itermax) {                      \
                        iterinfo->iter = -1;               \
                        break;                             \
                }                                          \
                r = sqr(zre) + sqr(zim);                   \
        }
#undef IT_FIRST_FOR
#define USE_UPDATE_FUNC
#include "iterate_template.h"

/* julia_set_deg function with status */
#undef IT_FUNC_NAME
#define IT_FUNC_NAME julia_set_deg_row_count
#undef IT_VAR
#define IT_VAR register gdouble zre, zim, jre, jim, r, phi; \
	register gdouble degree; \
	gint *col_count;
#undef IT_INIT 
#define IT_INIT degree = param->degree; \
	jre = param->j[0]; \
	jim = param->j[1]; \
	col_count = param->row_count + id;
#define IT_FIRST_FOR ++(*col_count);
#undef USE_UPDATE_FUNC
#include "iterate_template.h"

