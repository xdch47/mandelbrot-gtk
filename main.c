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

#include <gtk/gtk.h>
#include <locale.h>
#include <stdlib.h>
#include "defs.h"

#ifdef _WIN32
	#include <windows.h>
	#define __ENTRYPOINT__ int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
#else
	#define __ENTRYPOINT__ int main(int argc, char *argv[])
#endif

void run_interface(gchar *);

__ENTRYPOINT__
{
	#ifdef _WIN32
	/* MinGW and M$VC++ should support this: */
	char **argv = __argv;
	int argc = __argc;
	#endif

	bindtextdomain(PACKAGE, MO_DIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);

	gtk_init(&argc, &argv);

	//setlocale(LC_ALL, "");

	run_interface(*argv);
	gtk_main();

	exit(EXIT_SUCCESS);
}

