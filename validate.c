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

#include <stdlib.h>
#include <errno.h>
#include "interface.h"
#include "defs.h"

gboolean validate_cplx(GtkWidget *txtcplx[4], gdouble cplx[4], GtkWidget *win)
{
	gint i;
	gchar *endptr;
	/* check data for user input errors: */
	/* complex plane: */
	for (i = 0; i < 4; ++i) {
		errno = 0;
		cplx[i] = strtod(gtk_entry_get_text(GTK_ENTRY(txtcplx[i])), &endptr);
		if (*endptr != '\0' || errno == ERANGE) {
			gchar msg[BUFSIZE];
			g_snprintf(msg, BUFSIZE, ERRVAL, gtk_entry_get_text(GTK_ENTRY(txtcplx[i])));
			errdialog(GTK_WINDOW(win), msg);
			gtk_widget_grab_focus(txtcplx[i]);
			return FALSE;
		}
	}
	if (cplx[0] >= cplx[1]) {
		errdialog(GTK_WINDOW(win), ERRREMAXMIN);
		gtk_widget_grab_focus(txtcplx[0]);
		return FALSE;
	}
	if (cplx[2] >= cplx[3]) {
		errdialog(GTK_WINDOW(win), ERRIMMAXMIN);
		gtk_widget_grab_focus(txtcplx[2]);
		return FALSE;
	}
	return TRUE;
}

gboolean validate_itermax(GtkWidget *txtitermax, gint *itermax, GtkWidget *win)
{
	gchar *endptr;
	*itermax = strtol(gtk_entry_get_text(GTK_ENTRY(txtitermax)), &endptr, 10);
	if (*endptr != '\0' || errno == ERANGE || *itermax < LOWITERMAX || *itermax > HIITERMAX) {
		gchar msg[BUFSIZE];
		g_snprintf(msg, BUFSIZE, ERRITERMAX, LOWITERMAX, HIITERMAX);
		errdialog(GTK_WINDOW(win), msg);
		gtk_widget_grab_focus(txtitermax);
		return FALSE;
	}
	return TRUE;
}

gboolean validate_degree(GtkWidget *txtdegree, gdouble *degree, GtkWidget *win)
{
	gchar *endptr;
	*degree = strtod(gtk_entry_get_text(GTK_ENTRY(txtdegree)), &endptr);
	if (*endptr != '\0' || errno == ERANGE || *degree < LOWDEGREE || *degree > HIDEGREE) {
		gchar msg[BUFSIZE];
		g_snprintf(msg, BUFSIZE, ERRDEGREE, LOWDEGREE, HIDEGREE);
		errdialog(GTK_WINDOW(win), msg);
		gtk_widget_grab_focus(txtdegree);
		return FALSE;
	}
	return TRUE;

}

void errdialog(GtkWindow *win, gchar *msg)
{
	GtkWidget *dialog = gtk_message_dialog_new(win, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", msg);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}
