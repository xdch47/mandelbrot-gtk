
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

