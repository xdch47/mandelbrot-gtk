
#include "defs.h"

const gchar *LCPLX[4] = {
		"Min-Realteil:",
		"Max-Realteil:",
		"Min-Imag.teil:",
		"Max-Imag.teil:"
		};

const gchar *LPJMENU[6] = {
	"Dendrit",
	"San-Marco-Drache",
	"Julia-Drachen",
	"Fatou-Staub",
	"Douady Kanichen",
	"Misiurewicz-Punkt"
};

const gdouble JDEFINES[12] = { 

        0.0,    1.0,    // dendrit
        -1.0,   0.0,    // san-marco-dragon
        0.286,  0.0115, // jula-dragon
        0,      0.666,  // fatou
        -0.127, 0.775,  // douady
        -0.562, 0.643   // misiurewic-point

};

const gchar *LCOLOR[COLORFUNC] = {
	"Blaukonstanten",
	"Konvergent / Divergente Menge",
	"Dixius-Algo_1",
	"Dixius-Algo_2"
};

// default complex plane
const gdouble CPLXPLANE[4] = {
	-1.167,
	1.167,
	-1.33,
	1.33
};

// special default mandelbrotset(2):
const gdouble MCPLXPLANE[4] = {
	-2.167,
	1.167,
	-1.33,
	1.33
};

