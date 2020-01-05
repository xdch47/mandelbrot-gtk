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

#ifndef __VALIDATE_H__
#define __VALIDATE_H__

#include <gtk/gtk.h>

gboolean validatecplx(GtkWidget *txtcplx[4], gdouble cplx[4], GtkWidget *win);
gboolean validateitermax(GtkWidget *txtitermax, gint *itermax, GtkWidget *win);
void errdialog(GtkWindow *win, gchar *msg);

#endif /* __VALIDATE_H__ */

