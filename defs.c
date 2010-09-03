
#include <glib/gi18n.h>
#include "defs.h"
#include "render.h"

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

/* default complex plane */
const gdouble CPLXPLANE[4] = {
	-1.5,
	1.5,
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

