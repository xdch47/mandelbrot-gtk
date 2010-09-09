
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gprintf.h>
#include "interface.h"
#include "defs.h"
#include "libcolor/color.h"

struct savectl {
	struct winctl *w;
	gdouble cplx[4];
	GtkWidget *win;
	GtkWidget *txtfilename;
	GtkWidget *txtcplx[4];
	GtkWidget *txtitermax;
	GtkWidget *sbtwidth;
	GtkWidget *sbtheight;
	GtkWidget *cbocolor;
	GtkWidget *progresswin;
	GtkWidget *progresslbl;
	GtkWidget *progressbar;
	GtkWidget *progressbtn;
	GdkPixbuf *pixbuf;
	gchar *filename;
	gchar format[5];
	struct render_thread *render_thread;
	struct iterate_param it_param;
};

static void create_progress_dialog(struct savectl *s);
static void progressbtn_clicked(GtkWidget *widget, struct savectl *s);
static gboolean progresswin_close(GtkWidget *widget, GdkEvent *event, struct savectl *s);
static void save_pixbuf_to_stream(struct savectl *s);
static void btnsave_clicked(GtkWidget *widget, struct savectl *s);
static void destroy(GtkWidget *widget, struct savectl *s);
static void savedialog(GtkWidget *widget, struct savectl *s);
static void restore_pictopt(GtkWidget *widget, struct savectl *s);
static void ssetcplxplane(GtkWidget *widget, struct savectl *s);

/* progress dialog: */
static void create_progress_dialog(struct savectl *s)
{
	GtkWidget *vbox, *align;

	s->progresswin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_resizable(GTK_WINDOW(s->progresswin), FALSE);
	gtk_window_set_title(GTK_WINDOW(s->progresswin), LSAVECAP);
	gtk_container_set_border_width(GTK_CONTAINER(s->progresswin), 10);
	vbox = gtk_vbox_new(FALSE, 0);
	s->progresslbl = gtk_label_new(LSAVELABEL);
	gtk_misc_set_alignment(GTK_MISC(s->progresslbl), 0.0, 0.0);
	gtk_box_pack_start(GTK_BOX(vbox), s->progresslbl, FALSE, FALSE, 0);
	s->progressbar = gtk_progress_bar_new();
	gtk_widget_set_size_request(s->progressbar, 300, -1);
	gtk_box_pack_start(GTK_BOX(vbox), s->progressbar, FALSE, FALSE, 10);
	align = gtk_alignment_new(0.5, 0.5, 0.0, 0.0);
	s->progressbtn = gtk_button_new_with_mnemonic(LCANCEL);
	gtk_widget_set_size_request(s->progressbtn, 100, -1);
	gtk_container_add(GTK_CONTAINER(align), s->progressbtn);
	gtk_box_pack_start(GTK_BOX(vbox), align, FALSE, FALSE, 0);
	gtk_container_add(GTK_CONTAINER(s->progresswin), vbox);
	g_signal_connect(G_OBJECT(s->progresswin), "delete_event", G_CALLBACK(progresswin_close), s);
	g_signal_connect(G_OBJECT(s->progressbtn), "clicked", G_CALLBACK(progressbtn_clicked), s);
}

static void progressbtn_clicked(GtkWidget *widget, struct savectl *s)
{
	if (s->render_thread && is_render_thread_alive(s->render_thread)) {
		GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(s->progresswin), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, LSAVECANCEL);
		if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES) {
			render_thread_kill(s->render_thread);
		}
		gtk_widget_destroy(dialog);
		return;
	}
	if (s->render_thread) {
		render_thread_kill(s->render_thread);
		render_thread_free(s->render_thread);
		s->render_thread = NULL;
	}

	g_free(s->filename);
	gtk_widget_destroy(s->win);
}

static gboolean progresswin_close(GtkWidget *widget, GdkEvent *event, struct savectl *s)
{
	progressbtn_clicked(NULL, s);
	return TRUE;
}

static void set_progress(struct savectl *s)
{
	gint row_count, i;
	gdouble p;
	gchar buf[20];

	gdk_threads_enter();
	row_count = 0;
	for (i = 0; i < s->it_param.threads_count; ++i) {
		row_count += s->it_param.row_count[i];
	}
	p = (gdouble)row_count / (gdouble)(s->it_param.ymax * s->it_param.xoffset);
	g_snprintf(buf, 20, "%.0f %%", p * 100);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(s->progressbar), p);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(s->progressbar), buf);
	gdk_threads_leave();

}

static void render_thread_done(gboolean succ, struct savectl *s)
{
	GdkPixbuf *pixbuf;
	IterationInfo *itermap;
	ColorFunc colorfunc;
	gint width, height, rowstride, n_channels;
	guchar *itp, *endp;

	itermap = s->it_param.itermap;
	gdk_threads_enter();
	if (succ) {
		setIterMax(s->it_param.itermax);
		pixbuf = s->pixbuf;
		initialize_func(s->it_param.color_func_index);
		colorfunc = getColorFunc(s->it_param.color_func_index);
		n_channels = gdk_pixbuf_get_n_channels(pixbuf);
		width = gdk_pixbuf_get_width(pixbuf) - 1;
		height = gdk_pixbuf_get_height(pixbuf) - 1;
		rowstride = gdk_pixbuf_get_rowstride(pixbuf);
		itp = s->it_param.pixels;
		endp = itp + height * rowstride + width * n_channels;

		for (; itp < endp; itp += 4) {
			if (itermap->iter != -1) {
				colorfunc(itermap++, itp);
			} else {
				itp[0] = s->it_param.color[0];
				itp[1] = s->it_param.color[1];
				itp[2] = s->it_param.color[2];
				itermap++;
			}
			itp[3] = 0xff;
		}
		save_pixbuf_to_stream(s);
		setIterMax(s->w->it_param.itermax);
	} else {
		gtk_label_set_text(GTK_LABEL(s->progresslbl), LSAVELABELCANCEL);
		gtk_button_set_label(GTK_BUTTON(s->progressbtn), LSAVEDONE);
	}
	g_free(s->it_param.itermap);
	gdk_threads_leave();
}

static void save_pixbuf_to_stream(struct savectl *s)
{
	GFile *f = g_file_new_for_path(s->filename);
	GFileOutputStream *stream = g_file_replace(f, NULL, TRUE, G_FILE_CREATE_NONE, NULL, NULL);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(s->progressbar), 1.0);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(s->progressbar), "100 %");
	if (stream) {
		if (strcmp(s->format, "jpeg") == 0) {
			gdk_pixbuf_save_to_stream(s->pixbuf, G_OUTPUT_STREAM(stream), s->format, NULL, NULL, "quality", "100", NULL);
		} else {
			gdk_pixbuf_save_to_stream(s->pixbuf, G_OUTPUT_STREAM(stream), s->format, NULL, NULL, NULL);
		}
		g_output_stream_close(G_OUTPUT_STREAM(stream), NULL, NULL);
		g_object_unref(stream);
		gtk_label_set_text(GTK_LABEL(s->progresslbl), LSAVELABELDONE);
	} else {
		gchar buf[FILENAME_MAX + 100];
		g_snprintf(buf, FILENAME_MAX + 100, LSAVEERROR, s->filename);
		gtk_label_set_text(GTK_LABEL(s->progresslbl), buf);
	}
	gtk_button_set_label(GTK_BUTTON(s->progressbtn), LSAVEDONE);
	g_object_unref(s->pixbuf);
	g_object_unref(f);
}

/* store drawing dialog: */
void store_drawing_show(struct winctl *w)
{
	GtkWidget *vbox, *hbox, *vbox2, *hbox2, *table, *align;
	GtkWidget *frame, *frame2, *lbl, *btn;
	gint i;
	struct savectl *s;

	s = (struct savectl *)g_malloc(sizeof(struct savectl));
	s->w = w;

	/* initialize threads: */
	s->render_thread = NULL;
	iterate_param_init(&s->it_param, s->w->it_param.threads_count);

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
	/* width: */
	table = gtk_table_new(3, 4, FALSE);
	gtk_table_set_col_spacing(GTK_TABLE(table), 0, 5);
	gtk_table_set_col_spacing(GTK_TABLE(table), 1, 10);
	gtk_table_set_row_spacing(GTK_TABLE(table), 1, 5);
	lbl = gtk_label_new(LWIDTH);
	gtk_table_attach(GTK_TABLE(table), lbl, 0, 1, 0, 1, GTK_SHRINK, GTK_SHRINK, FALSE, FALSE);
	s->sbtwidth = gtk_spin_button_new_with_range(1.0, 5000.0, 1.0);
	gtk_table_attach(GTK_TABLE(table), s->sbtwidth, 1, 2, 0, 1, GTK_SHRINK | GTK_FILL, GTK_SHRINK, FALSE, FALSE);
	/* height: */
	lbl = gtk_label_new(LHEIGTH);
	gtk_table_attach(GTK_TABLE(table), lbl, 0, 1, 1, 2, GTK_SHRINK, GTK_SHRINK, 0, 0);
	s->sbtheight = gtk_spin_button_new_with_range(1.0, 5000.0, 1.0);
	gtk_table_attach(GTK_TABLE(table), s->sbtheight, 1, 2, 1, 2, GTK_SHRINK | GTK_FILL, GTK_SHRINK, 0, 0);
	/* itermax:	 */
	lbl = gtk_label_new_with_mnemonic(LITERMAX);
	gtk_misc_set_alignment(GTK_MISC(lbl), 0.0, 1.0);
	s->txtitermax = gtk_entry_new();
	gtk_widget_set_size_request(s->txtitermax, 70, -1);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl), s->txtitermax);
	gtk_table_attach(GTK_TABLE(table), lbl, 2, 3, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	gtk_table_attach(GTK_TABLE(table), s->txtitermax, 2, 3, 1, 2, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	/* combo-coloralgo: */
	lbl = gtk_label_new_with_mnemonic(LCOLORALGO);
	gtk_misc_set_alignment(GTK_MISC(lbl), 0.0, 1.0);
	s->cbocolor = gtk_combo_box_new_text();
	for (i = 0; i < getColorFunc_count(); ++i) {
		gtk_combo_box_append_text(GTK_COMBO_BOX(s->cbocolor), getColorFunc_name(i));
	}
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl), s->cbocolor);
	gtk_combo_box_set_active(GTK_COMBO_BOX(s->cbocolor), w->it_param.color_func_index);
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
	g_signal_connect(G_OBJECT(btn), "clicked", G_CALLBACK(btnsave_clicked), s);
	gtk_box_pack_start(GTK_BOX(hbox2), btn, TRUE, TRUE, 0);
	btn = gtk_button_new_with_mnemonic(LSAVECLOSE);
	g_signal_connect_swapped(G_OBJECT(btn), "clicked", G_CALLBACK(gtk_widget_destroy), s->win);
	gtk_box_pack_start(GTK_BOX(hbox2), btn, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(align), hbox2);
	gtk_box_pack_start(GTK_BOX(hbox), align, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(s->win), vbox);
	g_signal_connect(G_OBJECT(s->win), "destroy", G_CALLBACK(destroy), (gpointer)s);

	gtk_window_set_transient_for(GTK_WINDOW(s->win), GTK_WINDOW(w->win));
	gtk_window_set_modal(GTK_WINDOW(s->win), TRUE);
	restore_pictopt(NULL, s);
	create_progress_dialog(s);
	gtk_widget_show_all(s->win);
	
}

static void savedialog(GtkWidget *widget, struct savectl *s)
{
	GtkWidget *filechooser = gtk_file_chooser_dialog_new(LSAVECAP, GTK_WINDOW(s->win), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_OK, GTK_RESPONSE_OK,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(filechooser), gtk_entry_get_text(GTK_ENTRY(s->txtfilename)));
	if (gtk_dialog_run(GTK_DIALOG(filechooser)) == GTK_RESPONSE_OK) {
		gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooser));
		gtk_entry_set_text(GTK_ENTRY(s->txtfilename),  filename);
		g_free(filename);
	}
	gtk_widget_destroy(filechooser);
}

static void restore_pictopt(GtkWidget *widget, struct savectl *s)
{
	gint i;
	char buf[30];
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(s->sbtwidth), gdk_pixbuf_get_width(s->w->pixbufcalc));
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(s->sbtheight), gdk_pixbuf_get_height(s->w->pixbufcalc));
	for (i = 0; i <= 3; ++i) {
		g_sprintf(buf, NUMFORMAT, s->w->it_param.cplxplane[i]);
		s->cplx[i] = s->w->it_param.cplxplane[i];
		gtk_entry_set_text(GTK_ENTRY(s->txtcplx[i]), buf);
	}
	g_sprintf(buf, "%d", s->w->it_param.itermax);
	gtk_entry_set_text(GTK_ENTRY(s->txtitermax), buf);

}

static void ssetcplxplane(GtkWidget *widget, struct savectl *s)
{
	if (validate_cplx(s->txtcplx, s->cplx, s->win)) {
		setcplxplane(s->txtcplx, s->cplx, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(s->sbtwidth)),
				gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(s->sbtheight)));
	}
}

static void btnsave_clicked(GtkWidget *widget, struct savectl *s)
{
	gchar *dirname;
	gchar *basename;
	gchar *ext;
	gboolean chkdir;
	gint width, height;
	gint i;
	gboolean equal;

	s->filename = (gchar *)g_malloc(sizeof(gchar) * strlen(gtk_entry_get_text(GTK_ENTRY(s->txtfilename))) + 5);
	g_stpcpy(s->filename, gtk_entry_get_text(GTK_ENTRY(s->txtfilename)));
	dirname = g_path_get_dirname(s->filename);
	/* check the directory for existing and read/write permission: */
	chkdir = dirname ? g_file_test(dirname, G_FILE_TEST_IS_DIR) : 1;

	if (!chkdir) {
		errdialog(GTK_WINDOW(s->win), LDIRERR);
		g_free(s->filename);
		g_free(dirname);
		return;
	}
#ifdef G_OS_LINUX
	if(g_access(dirname, 0600) != 0) {
		errdialog(GTK_WINDOW(s->win), LPERMERR);
		g_free(s->filename);
		g_free(dirname);
		return;
	}
#endif
	g_free(dirname);
	basename = g_path_get_basename(s->filename);
	if (basename == NULL) {
		errdialog(GTK_WINDOW(s->win), LFILEERR);
		g_free(basename);
		g_free(s->filename);
	}

	ext = g_strrstr(basename, ".");
	if (ext && strcmp(ext, ".jpg") == 0) {
		g_stpcpy(ext, ".jpeg");
	}
	if (ext && (strcmp(ext, ".png") == 0 || strcmp(ext, ".jpeg") == 0 || strcmp(ext, ".bmp") == 0)) {
		g_stpcpy(s->format, ++ext);
	} else {
		g_stpcpy(s->format, "png");
		strcat(s->filename, ".png");
	}
	g_free(basename);

	if (g_file_test(s->filename, G_FILE_TEST_IS_REGULAR)) {
		GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(s->win), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, LFILEEXISTS, s->filename);
		if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_NO) {
			g_free(s->filename);
			gtk_widget_destroy(dialog);
			return;
		}
		gtk_widget_destroy(dialog);
	}

	width = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(s->sbtwidth));
	height = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(s->sbtheight));
	equal = TRUE;
	
	if (!(validate_cplx(s->txtcplx, s->it_param.cplxplane, s->win) && validate_itermax(s->txtitermax, &s->it_param.itermax, s->win))) {
		return;
	}
	for (i = 0; i < 4; ++i) {
		if (s->w->succ_render && s->w->it_param.cplxplane[i] != s->it_param.cplxplane[i]) {
			equal = FALSE;
			break;
		}
	}
	s->it_param.color_func_index = gtk_combo_box_get_active(GTK_COMBO_BOX(s->cbocolor));
	if (s->w->succ_render && equal && gdk_pixbuf_get_width(s->w->pixbufcalc) == width && gdk_pixbuf_get_height(s->w->pixbufcalc) == height
			&& s->w->it_param.itermax == s->it_param.itermax && s->w->it_param.color_func_index == s->it_param.color_func_index) {
		s->pixbuf = g_object_ref(s->w->pixbufcalc);
		gtk_widget_hide(s->win);
		save_pixbuf_to_stream(s);
		gtk_window_set_modal(GTK_WINDOW(s->progresswin), TRUE);
		gtk_widget_show_all(s->progresswin);
		return;
	} else {
		s->render_thread = render_thread_new((IdleFunc)set_progress, (ThreaddestroyFunc)render_thread_done, (gpointer)s);
		s->pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, width, height);

		s->it_param.xmax = width;
		s->it_param.ymax = height;
		s->it_param.j[0] = s->w->it_param.j[0];
		s->it_param.j[1] = s->w->it_param.j[1];
		s->it_param.pixels = gdk_pixbuf_get_pixels(s->pixbuf);
		s->it_param.n_channels = gdk_pixbuf_get_n_channels (s->pixbuf);
		s->it_param.rowstride = gdk_pixbuf_get_rowstride (s->pixbuf);
		s->it_param.itermap = (IterationInfo *)g_malloc(sizeof(IterationInfo) * width * height);

		s->it_param.color[0] = (guchar)(s->w->convcol.red >> 8);
		s->it_param.color[1] = (guchar)(s->w->convcol.green >> 8);
		s->it_param.color[2] = (guchar)(s->w->convcol.blue >> 8);
		s->it_param.degree = s->w->it_param.degree;
		if (s->w->it_param.type == MANDELBROT_SET) {
			s->it_param.iterate_func = (s->w->it_param.degree == 2.0) ? (GThreadFunc)mandelbrot_set_row_count : (GThreadFunc)mandelbrot_set_deg_row_count;
		} else {
			s->it_param.iterate_func = (s->w->it_param.degree == 2.0) ? (GThreadFunc)julia_set_row_count : (GThreadFunc)julia_set_deg_row_count;
		}

		gtk_button_set_label(GTK_BUTTON(s->progressbtn), LCANCEL);
		gtk_label_set_text(GTK_LABEL(s->progresslbl), LSAVELABEL);
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(s->progressbar), 0.0);
		gtk_widget_hide(s->win);
		gtk_window_set_modal(GTK_WINDOW(s->progresswin), TRUE);
		gtk_widget_show_all(s->progresswin);
		start_render_thread(s->render_thread, &s->it_param);
	}
}

static void destroy(GtkWidget *widgets, struct savectl *s)
{
	iterate_param_free(&s->it_param);
	gtk_widget_destroy(s->progresswin);
	g_free(s);
}

