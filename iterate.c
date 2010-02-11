#include <glib.h>
#include <complex.h>
#include "iterate.h"
#include "render.h"

#define sqr(x) (x) * (x)
// using dirty define/include-hacks for fast-inline templates...
// look at iterate_template.c

// mandelbrot_set function:
#define IT_FUNC_NAME mandelbrot_set
#define IT_VAR gdouble zre, zim, tmpzre, tmpzim;
#define IT_INLINE_FUNC \
	zre = itre; \
	zim = itim; \
	for (;;) { \
		tmpzre = zre * zre; \
		tmpzim = zim * zim; \
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
#include "iterate_template.c"

// julia_set function:
#define IT_FUNC_NAME julia_set
#define IT_VAR gdouble zre, zim, tmpzre, tmpzim, jre, jim; \
	jre = p->jre; \
	jim = p->jim; 
#define IT_INLINE_FUNC \
	zre = itre; \
	zim = itim; \
	for (;;) { \
		tmpzre = zre * zre; \
		tmpzim = zim * zim; \
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
#include "iterate_template.c"

// mandelbrot_set_deg
#define IT_FUNC_NAME mandelbrot_set_deg
#define IT_VAR complex z, c; \
	gdouble degree = p->degree;
#define IT_INLINE_FUNC \
	c = z = itre + I * itim; \
	while (cabs(z) < 2.0) { \
		z = cpow(z, degree) + c; \
		++iter; \
		if (iter > itermax) { \
			break; \
		} \
	} 
#undef IT_FIRST_FOR
#include "iterate_template.c"

//julia_set_deg
#define IT_FUNC_NAME julia_set_deg
#define IT_VAR complex z, j; \
	gdouble degree = p->degree; \
	j = p->jre + I * p->jim;
#define IT_INLINE_FUNC \
	z = itre + I * itim; \
	while (cabs(z) < 2.0) { \
		z = cpow(z, degree) + j; \
		++iter; \
		if (iter > itermax) { \
			break; \
		} \
	}
#undef IT_FIRST_FOR
#include "iterate_template.c"
