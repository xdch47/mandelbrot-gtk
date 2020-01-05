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

#ifndef __ITERATE_H__
#define __ITERATE_H__

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

struct threaddata {
	gdouble b_re;
	gdouble b_im;
	gdouble dre;
	gdouble dim;
	gdouble jre;
	gdouble jim;
	gint xstart;
	gint ystart;
	gint xoffset;
	gint yoffset;
	gint xmax;
	gint ymax;
	gint itermax;
	double degree;
	guchar *pixels;
	guchar *convdivcol;
	void (*setcolor)(guchar *, gint , gint , gint , gint , gint, gint,  guchar *);
	GMutex *state_mutex;
	GCond *resume_cond;
	volatile gint *pause;
};

gpointer mandelbrot_set(struct threaddata *p);
gpointer mandelbrot_set_deg(struct threaddata *p);
gpointer julia_set(struct threaddata *p);
gpointer julia_set_deg(struct threaddata *p);

#endif /* __ITERATE_H__ */

