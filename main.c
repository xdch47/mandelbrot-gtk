
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <locale.h>
#include "defs.h"

void run_interface(gchar *);

int main(int argc, char *argv[])
{
	g_thread_init(NULL);
	gdk_threads_init();
	gdk_threads_enter();
	gtk_init(&argc, &argv);

	bindtextdomain(PACKAGE, MO_DIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);

	setlocale(LC_ALL, "");
	
	run_interface(*argv);
	gtk_main();
	gdk_threads_leave();

	return 0;
}

