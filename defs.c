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

