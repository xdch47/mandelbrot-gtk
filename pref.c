
#include "interface.h"

struct prefctl {
	struct winctl *w;
	GtkWidget *win;
	GtkWidget *txtcplx[4];
	GtkWidget *txtmcplx[4];
	GtkWidget *sbtzoom;
	GtkWidget *cbtfocus;
	GtkWidget *sbtthreads;
	GtkWidget *txtget_jiter;
};

static GtkWidget* createbtn(GtkWidget **btn1, GtkWidget **btn2);
static void prefdestroy(GtkWidget *widget, struct prefctl *p);
static void currentcplx(struct prefctl *p);
static void currentmcplx(struct prefctl *p);
static void defaultcplx(struct prefctl *p);
static void defaultmcplx(struct prefctl *p);
static void btnok_clicked(GtkWidget *widget, struct prefctl *p);

void preference_show(struct winctl *w)
{
	GtkWidget *vbox1, *vbox2, *vbox3, *hbox1, *align;
	GtkWidget *notebook, *frame, *lbl, *btn1, *btn2;
	gchar buf[30];

	struct prefctl *p = (struct prefctl *)g_malloc(sizeof(struct prefctl));
	p->w = w;

	p->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(p->win), LCPREF);
	gtk_window_set_resizable(GTK_WINDOW(p->win), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(p->win), 10);
	vbox1 = gtk_vbox_new(FALSE, 4);

	notebook = gtk_notebook_new();

	/* page default complex plane: */
	vbox2 = gtk_vbox_new(FALSE, 3);
	gtk_container_set_border_width(GTK_CONTAINER(vbox2), 10);
	lbl = gtk_label_new(LPREFLSTDCPLX);
	gtk_misc_set_alignment(GTK_MISC(lbl), 0.0, 0.0);
	gtk_box_pack_start(GTK_BOX(vbox2), lbl, FALSE, FALSE, 4);
	frame = createcplxplane(p->txtcplx);
	gtk_box_pack_start(GTK_BOX(vbox2), frame, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), createbtn(&btn1, &btn2), FALSE, FALSE, 0);
	g_signal_connect_swapped(G_OBJECT(btn1), "clicked", G_CALLBACK(currentcplx), p);
	g_signal_connect_swapped(G_OBJECT(btn2), "clicked", G_CALLBACK(defaultcplx), p);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox2, gtk_label_new(LPREFSTDCPLX));

	/* page mandelbrot complex plane: */
	vbox2 = gtk_vbox_new(FALSE, 3);
	gtk_container_set_border_width(GTK_CONTAINER(vbox2), 10);
	lbl = gtk_label_new(LPREFLSTDMCPLX);
	gtk_misc_set_alignment(GTK_MISC(lbl), 0.0, 0.0);
	gtk_box_pack_start(GTK_BOX(vbox2), lbl, FALSE, FALSE, 4);
	frame = createcplxplane(p->txtmcplx);
	gtk_box_pack_start(GTK_BOX(vbox2), frame, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), createbtn(&btn1, &btn2), FALSE, FALSE, 0);
	g_signal_connect_swapped(G_OBJECT(btn1), "clicked", G_CALLBACK(currentmcplx), p);
	g_signal_connect_swapped(G_OBJECT(btn2), "clicked", G_CALLBACK(defaultmcplx), p);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox2, gtk_label_new(LPREFSTDMCPLX));

	/* misc page: */
	vbox2 = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox2), 10);
	/* zoom: */
	frame = gtk_frame_new(LPREFZOOMFACTOR);
	hbox1 = gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox1), 4);
	lbl = gtk_label_new(LPREFLZOOM);
	gtk_box_pack_start(GTK_BOX(hbox1), lbl, FALSE, FALSE, 5);
	p->sbtzoom = gtk_spin_button_new_with_range(0.1, 100.0, 1.0);
	gtk_box_pack_start(GTK_BOX(hbox1), p->sbtzoom, FALSE, FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), hbox1);
	gtk_box_pack_start(GTK_BOX(vbox2), frame, FALSE, FALSE, 0);

	/* threads: */
	frame = gtk_frame_new(LPREFTHREADS);
	hbox1 = gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox1), 4);
	lbl = gtk_label_new(LPREFTHREADSCOUNT);
	gtk_box_pack_start(GTK_BOX(hbox1), lbl, FALSE, FALSE, 5);
	p->sbtthreads = gtk_spin_button_new_with_range(1.0, MAXTHREADS, 1.0);
	gtk_box_pack_start(GTK_BOX(hbox1), p->sbtthreads, FALSE, FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), hbox1);
	gtk_box_pack_start(GTK_BOX(vbox2), frame, FALSE, FALSE, 0);

	/* focus color: */
	frame = gtk_frame_new(LPREFFOCUSCOLOR);
	hbox1 = gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox1), 4);
	lbl = gtk_label_new(LPREFLFOCUSCOLOR);
	gtk_box_pack_start(GTK_BOX(hbox1), lbl, FALSE, FALSE, 5);
	p->cbtfocus = gtk_color_button_new_with_color(&w->focus_color);
	gtk_box_pack_start(GTK_BOX(hbox1), p->cbtfocus, FALSE, FALSE, 0);
	gtk_container_add(GTK_CONTAINER(frame), hbox1);
	gtk_box_pack_start(GTK_BOX(vbox2), frame, FALSE, FALSE, 5);

	/* get_j itermax: */
	frame = gtk_frame_new(LPREFITER);
	vbox3 = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox3), 4);
	lbl = gtk_label_new(LPREFLITER);
	gtk_misc_set_alignment(GTK_MISC(lbl), 0.0, 0.0);
	gtk_box_pack_start(GTK_BOX(vbox3), lbl, FALSE, FALSE, 5);
	p->txtget_jiter = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(vbox3), p->txtget_jiter, FALSE, FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox3);
	gtk_box_pack_start(GTK_BOX(vbox2), frame, FALSE, FALSE, 0);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox2, gtk_label_new(LPREFMISC));
	gtk_box_pack_start(GTK_BOX(vbox1), notebook, FALSE, FALSE, 0);

	align = gtk_alignment_new(1.0, 0.0, 0.0, 0.0);
	hbox1 = gtk_hbox_new(TRUE, 5);
	btn1 = gtk_button_new_with_mnemonic(LPREFOK);
	gtk_box_pack_start(GTK_BOX(hbox1), btn1, FALSE, TRUE, 0);
	g_signal_connect(G_OBJECT(btn1), "clicked", G_CALLBACK(btnok_clicked), p);
	btn1 = gtk_button_new_with_mnemonic(LPREFCANCEL);
	g_signal_connect_swapped(G_OBJECT(btn1), "clicked", G_CALLBACK(gtk_widget_destroy), p->win);
	gtk_box_pack_start(GTK_BOX(hbox1), btn1, FALSE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(align), hbox1);
	gtk_box_pack_start(GTK_BOX(vbox1), align, FALSE, FALSE, 0);
	gtk_container_add(GTK_CONTAINER(p->win), vbox1);
	g_signal_connect(G_OBJECT(p->win), "destroy", G_CALLBACK(prefdestroy), p);
	gtk_window_set_position(GTK_WINDOW(p->win), GTK_WIN_POS_CENTER);
	gtk_window_set_transient_for(GTK_WINDOW(p->win), GTK_WINDOW(w->win));
	gtk_window_set_modal(GTK_WINDOW(p->win), TRUE);
	g_snprintf(buf, 30, "%d", w->get_jitermax);
	gtk_entry_set_text(GTK_ENTRY(p->txtget_jiter), buf);
	currentcplx(p);
	currentmcplx(p);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(p->sbtzoom), w->zoomfactor * 100.0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(p->sbtthreads), w->it_param.threads_count);
	gtk_widget_show_all(p->win);
}

static void currentcplx(struct prefctl *p)
{
	gint i;
	gchar buf[30];
	for (i = 0; i < 4; ++i) {
		g_snprintf(buf, 30, NUMFORMAT, p->w->default_cplxplane[i]);
		gtk_entry_set_text(GTK_ENTRY(p->txtcplx[i]), buf);
	}
}

static void currentmcplx(struct prefctl *p)
{
	gint i;
	gchar buf[30];
	for (i = 0; i < 4; ++i) {
		g_snprintf(buf, 30, NUMFORMAT, p->w->default_mcplxplane[i]);
		gtk_entry_set_text(GTK_ENTRY(p->txtmcplx[i]), buf);
	}
}

static void defaultcplx(struct prefctl *p)
{
	gint i;
	gchar buf[30];
	for (i = 0; i < 4; ++i) {
		g_snprintf(buf, 30, NUMFORMAT, CPLXPLANE[i]);
		gtk_entry_set_text(GTK_ENTRY(p->txtcplx[i]), buf);
	}
}

static void defaultmcplx(struct prefctl *p)
{
	gint i;
	gchar buf[30];
	for (i = 0; i < 4; ++i) {
		g_snprintf(buf, 30, NUMFORMAT, MCPLXPLANE[i]);
		gtk_entry_set_text(GTK_ENTRY(p->txtmcplx[i]), buf);
	}
}

static GtkWidget *createbtn(GtkWidget **btn1, GtkWidget **btn2)
{
	GtkWidget *align, *hbox;
	align = gtk_alignment_new(1.0, 0.0, 0.0, 0.0);
	hbox = gtk_hbox_new(TRUE, 5);
	*btn1 = gtk_button_new_with_mnemonic(LPREFRESET);
	*btn2 = gtk_button_new_with_mnemonic(LPREFDEFAULT);
	gtk_box_pack_start(GTK_BOX(hbox), *btn1, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), *btn2, FALSE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(align), hbox);
	return align;
}

static void prefdestroy(GtkWidget *widget, struct prefctl *p)
{
	gtk_widget_destroy(p->win);
	g_free(p);
}

static void btnok_clicked(GtkWidget *widget, struct prefctl *p)
{
	gdouble cplx[4], mcplx[4];
	gint jitermax;
	gint threads_count;
	gint i;
	if (!validate_cplx(p->txtcplx, cplx, p->win)) {
		return;
	}
	if (!validate_cplx(p->txtmcplx, mcplx, p->win)) {
		return;
	}
	if (!validate_itermax(p->txtget_jiter, &jitermax, p->win)) {
		return;
	}

	for (i = 0; i < 4; ++i) {
		p->w->default_cplxplane[i] = cplx[i];
		p->w->default_mcplxplane[i] = mcplx[i];
	}
	gtk_color_button_get_color(GTK_COLOR_BUTTON(p->cbtfocus), &p->w->focus_color);
	p->w->get_jitermax = jitermax;
	p->w->zoomfactor = gtk_spin_button_get_value(GTK_SPIN_BUTTON(p->sbtzoom)) / 100.0;
	p->w->get_jitermax = jitermax;
	threads_count = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(p->sbtthreads));
	if (p->w->it_param.threads_count != threads_count) {
		gdouble j[2];
		render_thread_kill(p->w->render_thread);
		j[0] = p->w->it_param.j[0];
		j[1] = p->w->it_param.j[1];
		iterate_param_free(&p->w->it_param);
		iterate_param_init(&p->w->it_param, threads_count);
		p->w->it_param.j[0] = j[0];
		p->w->it_param.j[1] = j[1];
	}
	
	gtk_widget_modify_fg(p->w->drawing, GTK_STATE_NORMAL, &p->w->focus_color);
	gtk_widget_destroy(p->win);
}

