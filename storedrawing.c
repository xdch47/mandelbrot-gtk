
#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gprintf.h>
#include "storedrawing.h"
#include "defs.h"
#include "layout.h"
#include "validate.h"
#include "gio/gio.h"

struct savectl {
	struct winctl *w;
	gdouble cplx[4];
	GtkWidget *win;
	GtkWidget *txtfilename;
	GtkWidget *txtcplx[4];
	GtkWidget *txtitermax;
	GtkWidget *spinx;
	GtkWidget *spiny;
	GtkWidget *cbocolor;
	GdkPixbuf *pixbuf;
};

static void destroy(GtkWidget *widget, struct savectl *s);
static void savedialog(GtkWidget *widget, struct savectl *s);
static void restore_pictopt(GtkWidget *widget, struct savectl *s);
static void ssetcplxplane(GtkWidget *widget, struct savectl *s);
static void save_pixbuf(GtkWidget *widget, struct savectl *s);

void store_drawing_show(struct winctl *w)
{
	GtkWidget *vbox, *hbox, *vbox2, *hbox2, *table, *align;
	GtkWidget *frame, *frame2, *lbl, *btn;
	gint i;

	struct savectl *s = (struct savectl *)g_malloc(sizeof(struct savectl));
	s->w = w;
	
	s->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(s->win), LSAVECAP);
	gtk_container_set_border_width(GTK_CONTAINER(s->win), 5);
	gtk_window_set_resizable(GTK_WINDOW(s->win), FALSE);
	
	vbox = gtk_vbox_new(FALSE, 0);
	
	frame = gtk_frame_new(LSAVEPATH);
	gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 5);
	lbl = gtk_label_new_with_mnemonic(LFILENAME);
	s->txtfilename = gtk_entry_new();
	gtk_widget_set_size_request(s->txtfilename, 400, -1);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl), s->txtfilename);
	btn = gtk_button_new_with_label("...");
	g_signal_connect(G_OBJECT(btn), "clicked", G_CALLBACK(savedialog), s);
	gtk_box_pack_start(GTK_BOX(hbox), lbl, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), s->txtfilename, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), btn, FALSE, FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), 4);
	gtk_container_add(GTK_CONTAINER(frame), hbox);

	frame = gtk_frame_new(LRENDEROPT);
	hbox = gtk_hbox_new(FALSE, 4);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);
	frame2 = gtk_frame_new(LPICTOPT);
	vbox2 = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox2), 5);
	// width:
	table = gtk_table_new(3, 4, FALSE);
	gtk_table_set_col_spacing(GTK_TABLE(table), 0, 5);
	gtk_table_set_col_spacing(GTK_TABLE(table), 1, 10);
	gtk_table_set_row_spacing(GTK_TABLE(table), 1, 5);
	lbl = gtk_label_new(LWIDTH);
	gtk_table_attach(GTK_TABLE(table), lbl, 0, 1, 0, 1, GTK_SHRINK, GTK_SHRINK, FALSE, FALSE);
	s->spinx = gtk_spin_button_new_with_range(1.0, 5000.0, 1.0);
	gtk_table_attach(GTK_TABLE(table), s->spinx, 1, 2, 0, 1, GTK_SHRINK | GTK_FILL, GTK_SHRINK, FALSE, FALSE);
	// height:
	lbl = gtk_label_new(LHEIGTH);
	gtk_table_attach(GTK_TABLE(table), lbl, 0, 1, 1, 2, GTK_SHRINK, GTK_SHRINK, 0, 0);
	s->spiny = gtk_spin_button_new_with_range(1.0, 5000.0, 1.0);
	gtk_table_attach(GTK_TABLE(table), s->spiny, 1, 2, 1, 2, GTK_SHRINK | GTK_FILL, GTK_SHRINK, 0, 0);
	// itermax:	
	lbl = gtk_label_new_with_mnemonic(LITERMAX);
	gtk_misc_set_alignment(GTK_MISC(lbl), 0.0, 1.0);
	s->txtitermax = gtk_entry_new();
	gtk_widget_set_size_request(s->txtitermax, 70, -1);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl), s->txtitermax);
	gtk_table_attach(GTK_TABLE(table), lbl, 2, 3, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	gtk_table_attach(GTK_TABLE(table), s->txtitermax, 2, 3, 1, 2, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	// combo-coloralgo:
	lbl = gtk_label_new_with_mnemonic(LCOLORALGO);
	gtk_misc_set_alignment(GTK_MISC(lbl), 0.0, 1.0);
	s->cbocolor = gtk_combo_box_new_text();
	for (i = 0; i < COLORFUNC; ++i) {
		gtk_combo_box_append_text(GTK_COMBO_BOX(s->cbocolor), LCOLOR[i]);
	}
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl), s->cbocolor);
	gtk_combo_box_set_active(GTK_COMBO_BOX(s->cbocolor), w->color_func_index);
	gtk_table_attach(GTK_TABLE(table), lbl, 0, 3, 2, 3, GTK_SHRINK | GTK_FILL, GTK_SHRINK, 0, 0);
	gtk_table_attach(GTK_TABLE(table), s->cbocolor, 0, 3, 3, 4, GTK_SHRINK | GTK_FILL, GTK_SHRINK, 0, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), table, FALSE, FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame2), vbox2);
	gtk_box_pack_start(GTK_BOX(hbox), frame2, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(hbox), createcplxplane(s->txtcplx), FALSE, FALSE, 0);
	gtk_container_add(GTK_CONTAINER(frame), hbox);
	gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 5);
	align = gtk_alignment_new(1.0, 0.0, 0.0, 0.0);
	hbox = gtk_hbox_new(FALSE, 4);
	btn = gtk_button_new_with_mnemonic(LRESTOREVAL);
	gtk_box_pack_start(GTK_BOX(hbox), btn, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(btn), "clicked", G_CALLBACK(restore_pictopt), s);
	btn = gtk_button_new_with_mnemonic(LSSETCPLX);
	gtk_box_pack_start(GTK_BOX(hbox), btn, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(btn), "clicked", G_CALLBACK(ssetcplxplane), s);
	hbox2 = gtk_hbox_new(TRUE, 4);
	btn = gtk_button_new_with_mnemonic(LSAVEBTN);
	g_signal_connect(G_OBJECT(btn), "clicked", G_CALLBACK(save_pixbuf), s);
	gtk_box_pack_start(GTK_BOX(hbox2), btn, TRUE, TRUE, 0);
	btn = gtk_button_new_with_mnemonic(LCANCEL);
	g_signal_connect_swapped(G_OBJECT(btn), "clicked", G_CALLBACK(gtk_widget_destroy), s->win);
	gtk_box_pack_start(GTK_BOX(hbox2), btn, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(align), hbox2);
	gtk_box_pack_start(GTK_BOX(hbox), align, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(s->win), vbox);
	g_signal_connect(G_OBJECT(s->win), "destroy", G_CALLBACK(destroy), (gpointer)s);

	gtk_window_set_modal(GTK_WINDOW(s->win), TRUE);
	restore_pictopt(NULL, s);
	gtk_widget_show_all(s->win);
}

static void savedialog(GtkWidget *widget, struct savectl *s)
{
	GtkWidget *filechooser = gtk_file_chooser_dialog_new(LSAVECAP, GTK_WINDOW(s->win), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_OK, GTK_RESPONSE_OK,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(filechooser), gtk_entry_get_text(GTK_ENTRY(s->txtfilename)));
	if (gtk_dialog_run(GTK_DIALOG(filechooser)) == GTK_RESPONSE_OK) {
		gtk_entry_set_text(GTK_ENTRY(s->txtfilename),  gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooser)));
	}
	gtk_widget_destroy(filechooser);
}

static void destroy(GtkWidget *widgets, struct savectl *s)
{
	gtk_widget_destroy(s->win);
	g_free(s);
}

static void restore_pictopt(GtkWidget *widget, struct savectl *s)
{
	gint i;
	char buf[30];
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(s->spinx), gdk_pixbuf_get_width(s->w->pixbufcalc));
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(s->spiny), gdk_pixbuf_get_height(s->w->pixbufcalc));
	for (i = 0; i <= 3; ++i) {
		g_sprintf(buf, NUMFORMAT, s->w->cplx[i]);
		s->cplx[i] = s->w->cplx[i];
		gtk_entry_set_text(GTK_ENTRY(s->txtcplx[i]), buf);
	}
	g_sprintf(buf, "%d", s->w->data->itermax);
	gtk_entry_set_text(GTK_ENTRY(s->txtitermax), buf);

}

static void ssetcplxplane(GtkWidget *widget, struct savectl *s)
{
	if (validatecplx(s->txtcplx, s->cplx, s->win)) {
		setcplxplane(s->txtcplx, s->cplx, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(s->spinx)),
				gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(s->spiny)));
	}
}

static gboolean render_save_pixbuf(struct savectl *s)
{
	s->pixbuf = g_object_ref(s->w->pixbufcalc);
	return TRUE;
}

static void save_pixbuf(GtkWidget *widget, struct savectl *s)
{
	const gchar *filename = gtk_entry_get_text(GTK_ENTRY(s->txtfilename));
	gchar *dirname = g_path_get_dirname(filename);
	// check the directory for existing and read/write permission:
	gboolean chkdir = g_file_test(dirname, G_FILE_TEST_IS_DIR) && g_access(dirname, 0600);
	g_free(dirname);

	if (!chkdir)
		return;

	if (render_save_pixbuf(s)) {
		GFile *f = g_file_new_for_path(filename);
		GFileOutputStream *stream = g_file_replace(f, NULL, TRUE, G_FILE_CREATE_NONE, NULL, NULL);
		gdk_pixbuf_save_to_stream(s->pixbuf, G_OUTPUT_STREAM(stream), "png", NULL, NULL, NULL);
		g_object_unref(s->pixbuf);
		g_output_stream_close(G_OUTPUT_STREAM(stream), NULL, NULL);
		g_object_unref(stream);
		g_free(dirname);
		gtk_widget_destroy(s->win);
	}
}

