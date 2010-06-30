
#include <glib/gi18n.h>
#include "defs.h"
#include "render.h"
#include "draw.h"

const gchar *LCPLX[4] = {
		N_("Min-Realpart:"),
		N_("Max-Realpart:"),
		N_("Min-Imag.part:"),
		N_("Max-Imag.part:")
		};

const gchar *LPJMENU[6] = {
	N_("Dendrit"),
	N_("San-Marco-Dragon"),
	N_("Julia-Dragon"),
	N_("Fatou-Set"),
	N_("Douady rabbit"),
	N_("Misiurewicz-Point")
};

const gdouble JDEFINES[12] = { 

        0.0,    1.0,    /* dendrit */
        -1.0,   0.0,    /* san-marco-dragon */
        0.286,  0.0115, /* jula-dragon */
        0,      0.666,  /* fatou */
        -0.127, 0.775,  /* douady */
        -0.562, 0.643   /* misiurewic-point */
};

const gchar *LCOLOR[COLORFUNC] = {
	N_("Blue constants"),
	N_("Convergent / Divergent Set"),
	N_("Dixius-Algo_1"),
	N_("Dixius-Algo_2")
};

/* default complex plane */
const gdouble CPLXPLANE[4] = {
	-1.167,
	1.167,
	-1.5,
	1.5
};

/* special default mandelbrotset(2): */
const gdouble MCPLXPLANE[4] = {
	-2.165,
	1.167,
	-1.33,
	1.33
};

/* color settings: */

const ColorFunc color_func[COLORFUNC] = {
	clblue,
	divconv,
	cldixius_1,
	cldixius_2
};

const guchar clbluedef[4 * 48] = {
	  0,   0,   0, 255,
	  0,   0,  31, 255,
	  0,   0,  63, 255,
	  0,   0,  95, 255,
	  0,   0, 127, 255,
	  0,   0, 159, 255,
	  0,   0, 191, 255,
	  0,   0, 223, 255,
	  0,   0, 255, 255,
	  0,  31, 255, 255,
	  0,  63, 255, 255,
	  0,  95, 255, 255,
	  0, 127, 255, 255,
	  0, 159, 255, 255,
	  0, 191, 255, 255,
	  0, 223, 255, 255,
	  0, 255, 255, 255,
	 31, 255, 255, 255,
	 63, 255, 255, 255,
	 95, 255, 255, 255,
	127, 255, 255, 255,
	159, 255, 255, 255,
	191, 255, 255, 255,
	223, 255, 255, 255,
	255, 255, 255, 255,
	223, 255, 255, 255,
	191, 255, 255, 255,
	159, 255, 255, 255,
	127, 255, 255, 255,
	 95, 255, 255, 255,
	 63, 255, 255, 255,
	 31, 255, 255, 255,
	  0, 255, 255, 255,
	  0, 223, 255, 255,
	  0, 191, 255, 255,
	  0, 159, 255, 255,
	  0, 127, 255, 255,
	  0,  95, 255, 255,
	  0,  63, 255, 255,
	  0,  31, 255, 255,
	  0,   0, 255, 255,
	  0,   0, 223, 255,
	  0,   0, 191, 255,
	  0,   0, 159, 255,
	  0,   0, 127, 255,
	  0,   0,  95, 255,
	  0,   0,  63, 255,
	  0,   0,  31, 255
};
