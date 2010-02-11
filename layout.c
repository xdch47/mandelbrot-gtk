
#include <glib/gprintf.h>
#include <gdk/gdkkeysyms.h>
#include <stdlib.h>
#include "interface.h"

static GtkWidget *convdiv_menu(struct winctl *w);
static void chkj_toggled(GtkWidget *widget, struct winctl *w);
static void toggle_zoomprop(GtkWidget *widget, struct winctl *w);
static void menu_setcplxplane(GtkWidget *widget, struct winctl *w);
static void insert_j(GtkWidget *widget, struct winctl *w);
static gboolean pjmenu(GtkWidget *widget, GdkEventButton *event, struct winctl *w);
static void change_color_algo(GtkWidget *widget, struct winctl *w);
static void change_convcol(GtkWidget *widget, struct winctl *w);
static void change_divcol(GtkWidget *widget, struct winctl *w);
static void store_drawing(GtkWidget *widget, struct winctl *w);
static void about(GtkWidget *widget, struct winctl *w);

GtkWidget *createcplxplane(GtkWidget *txtcplx[4])
{
	GtkWidget *frame, *table, *lbl;
	gint i;
	frame = gtk_frame_new(LCOORD);
	table = gtk_table_new(4, 2, FALSE);
	for (i = 0; i <= 3; ++i) {
		lbl = gtk_label_new(LCPLX[i]);
		gtk_misc_set_alignment(GTK_MISC(lbl), 0.0, 0.0);
		gtk_table_attach(GTK_TABLE(table), lbl, 0, 1, i, i + 1, GTK_SHRINK | GTK_FILL, GTK_SHRINK, 0, 0);
		txtcplx[i] = gtk_entry_new();
		gtk_entry_set_max_length(GTK_ENTRY(txtcplx[i]), TEXTLEN);
		gtk_widget_set_size_request(txtcplx[i], 160, -1);
		gtk_table_attach(GTK_TABLE(table), txtcplx[i], 1, 2, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	}
	gtk_table_set_row_spacing(GTK_TABLE(table), 0 , 1);
	gtk_table_set_row_spacing(GTK_TABLE(table), 1 , 4);
	gtk_table_set_row_spacing(GTK_TABLE(table), 2 , 1);
	gtk_table_set_col_spacing(GTK_TABLE(table), 0 , 5);
	gtk_container_set_border_width(GTK_CONTAINER(table), 5);
	gtk_container_add(GTK_CONTAINER(frame), table);
	return frame;
}

struct winctl *buildinterface(void)
{
	GtkWidget *rootbox, *vbox, *vbox2, *vbox3, *hbox, *table, *table2, *align;
	GtkWidget *frame, *frame2, *lbl;
	GtkWidget *notebook;
	GtkWidget *menubar, *menu, *menuit, *pmenuit, *submenu, *psubmenu, *psmenuit;
	GtkWidget *eventbox;
	GtkAccelGroup *accel_group;
	GSList *radio_group, *pradio_group;
	gint i;
	
	// create mainwindow:
	struct winctl *w = (struct winctl *)g_malloc(sizeof(struct winctl));
	w->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(w->win), LWCAP);
	rootbox = gtk_vbox_new(FALSE, 0);

	// menu:
	accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(w->win), accel_group);
	menubar = gtk_menu_bar_new();	
	menuit = gtk_menu_item_new_with_mnemonic(LFILE);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuit);
	menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuit) , menu);
	w->drawmenu = gtk_menu_new();
	// draw:
	menuit = gtk_menu_item_new_with_mnemonic(LCALC);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuit);
	gtk_widget_add_accelerator(menuit, "activate", accel_group, GDK_F9, 0, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(menuit), "activate", G_CALLBACK(calc), w);
	// reset:
	menuit = gtk_menu_item_new_with_mnemonic(LRESET);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuit);
	gtk_widget_add_accelerator(menuit, "activate", accel_group, GDK_F2, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(menuit), "activate", G_CALLBACK(reset), w);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), gtk_separator_menu_item_new());
	// save:
	menuit = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE, accel_group);
	pmenuit = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE, accel_group);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuit);
	gtk_menu_shell_append(GTK_MENU_SHELL(w->drawmenu), pmenuit);
	g_signal_connect(G_OBJECT(menuit), "activate", G_CALLBACK(store_drawing), w);
	g_signal_connect(G_OBJECT(pmenuit), "activate", G_CALLBACK(store_drawing), w);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), gtk_separator_menu_item_new());
	gtk_menu_shell_append(GTK_MENU_SHELL(w->drawmenu), gtk_separator_menu_item_new());
	// close:
	menuit = gtk_image_menu_item_new_from_stock(GTK_STOCK_CLOSE, accel_group);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuit);
	gtk_widget_add_accelerator(menuit, "activate", accel_group, GDK_q, 0, 0);
	g_signal_connect_swapped(G_OBJECT(menuit), "activate", G_CALLBACK(gtk_widget_destroy), w->win);
	// preference:
	menuit = gtk_menu_item_new_with_mnemonic(LPREFMENU);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuit);
	menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuit), menu);
	// set complex plane:
	menuit = gtk_menu_item_new_with_mnemonic(LSETCPLX);
	pmenuit = gtk_menu_item_new_with_mnemonic(LSETCPLX);
	gtk_widget_add_accelerator(menuit, "activate", accel_group, GDK_r, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(pmenuit, "activate", accel_group, GDK_r, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuit);
	gtk_menu_shell_append(GTK_MENU_SHELL(w->drawmenu), pmenuit);
	g_signal_connect(G_OBJECT(menuit), "activate", G_CALLBACK(menu_setcplxplane), w);
	g_signal_connect(G_OBJECT(pmenuit), "activate", G_CALLBACK(menu_setcplxplane), w);
	// scalexy:
	w->mchkzoomprop = gtk_check_menu_item_new_with_mnemonic(LZOOMPROP);
	w->pmchkzoomprop = gtk_check_menu_item_new_with_mnemonic(LZOOMPROP);
	gtk_widget_add_accelerator(w->mchkzoomprop, "activate", accel_group, GDK_s, GDK_SUPER_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(w->pmchkzoomprop, "activate", accel_group, GDK_s, GDK_SUPER_MASK, GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), w->mchkzoomprop);
	gtk_menu_shell_append(GTK_MENU_SHELL(w->drawmenu), w->pmchkzoomprop);
	g_signal_connect(G_OBJECT(w->mchkzoomprop), "toggled", G_CALLBACK(toggle_zoomprop), w);
	g_signal_connect(G_OBJECT(w->pmchkzoomprop), "toggled", G_CALLBACK(toggle_zoomprop), w);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), gtk_separator_menu_item_new());
	gtk_menu_shell_append(GTK_MENU_SHELL(w->drawmenu), gtk_separator_menu_item_new());
	// set color (div./conv.):
	menuit = gtk_menu_item_new_with_mnemonic(LSETCOLOR);
	pmenuit = gtk_menu_item_new_with_mnemonic(LSETCOLOR);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuit);
	gtk_menu_shell_append(GTK_MENU_SHELL(w->drawmenu), pmenuit);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuit), convdiv_menu(w));
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(pmenuit), convdiv_menu(w));
	// set color algo:
	menuit = gtk_menu_item_new_with_mnemonic(LCOLORALGO);
	pmenuit = gtk_menu_item_new_with_mnemonic(LCOLORALGO);
	submenu = gtk_menu_new();
	psubmenu = gtk_menu_new();
	radio_group = pradio_group = NULL;
	for (i = 0; i < COLORFUNC; ++i) {
		gint *index = (gint *)g_malloc(sizeof(gint));
		w->mcolalgo[i] = gtk_radio_menu_item_new_with_mnemonic(radio_group, LCOLOR[i]);
		psmenuit = gtk_radio_menu_item_new_with_mnemonic(pradio_group, LCOLOR[i]);
		radio_group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(w->mcolalgo[i]));
		pradio_group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(psmenuit));
		*index = i;
		g_object_set_data_full(G_OBJECT(w->mcolalgo[i]), "index", index, g_free);
		g_object_set_data(G_OBJECT(psmenuit), "index", index);
		g_object_set_data(G_OBJECT(w->mcolalgo[i]), "sync_obj", psmenuit);
		g_object_set_data(G_OBJECT(psmenuit), "sync_obj", w->mcolalgo[i]);
		g_signal_connect(G_OBJECT(w->mcolalgo[i]), "toggled", G_CALLBACK(change_color_algo), w);
		g_signal_connect(G_OBJECT(psmenuit), "toggled", G_CALLBACK(change_color_algo), w);
		gtk_menu_shell_append(GTK_MENU_SHELL(submenu), w->mcolalgo[i]);
		gtk_menu_shell_append(GTK_MENU_SHELL(psubmenu), psmenuit);
	}
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuit), submenu);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(pmenuit), psubmenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuit);
	gtk_menu_shell_append(GTK_MENU_SHELL(w->drawmenu), pmenuit);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), gtk_separator_menu_item_new());
	// preference:
	menuit = gtk_menu_item_new_with_mnemonic(LPREF);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuit);
	g_signal_connect_swapped(G_OBJECT(menuit), "activate", G_CALLBACK(preference_show), w);
	// info menu:
	menuit = gtk_menu_item_new_with_mnemonic(LHELP);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuit);
	menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuit), menu);
/* 	menuit = gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP, accel_group);
 * 	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuit);
 * 	g_signal_connect(G_OBJECT(menuit), "activate", G_CALLBACK(^), w);
 */
	menuit = gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, accel_group);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuit);
	g_signal_connect(G_OBJECT(menuit), "activate", G_CALLBACK(about), w);
	w->pjmenu = gtk_menu_new();
	for (i = 0; i < 6; ++i) {
		gint *index = (gint *)g_malloc(sizeof(gint));
		*index = i;
		menuit = gtk_menu_item_new_with_label(LPJMENU[i]);
		gtk_menu_shell_append(GTK_MENU_SHELL(w->pjmenu), menuit);
		g_object_set_data_full(G_OBJECT(menuit), "index", index, g_free);
		g_signal_connect(G_OBJECT(menuit), "activate", G_CALLBACK(insert_j), w);
	}
	
	gtk_box_pack_start(GTK_BOX(rootbox), menubar, FALSE, FALSE, 0);
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(rootbox), vbox, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
	hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

	// create image:
	frame = gtk_frame_new(NULL);
	gtk_box_pack_start(GTK_BOX(hbox), frame, TRUE, TRUE, 0);
	frame2 = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(frame2), GTK_SHADOW_IN);
	vbox2 = gtk_vbox_new(FALSE, 0);
	w->lbldraw = gtk_label_new(LWCAP);
	gtk_label_set_width_chars(GTK_LABEL(w->lbldraw), 50);
	gtk_box_pack_start(GTK_BOX(vbox2), w->lbldraw, FALSE, FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox2), 3);
	gtk_container_add(GTK_CONTAINER(frame2), vbox2);
	
	vbox2 = gtk_vbox_new(FALSE, 15);
	align = gtk_alignment_new(0.5, 0.5, 0.6, 0.0);
	gtk_container_add(GTK_CONTAINER(align), frame2);

	gtk_box_pack_start(GTK_BOX(vbox2), align, FALSE, FALSE, 0);
	w->drawing = gtk_drawing_area_new();
	w->pixbufcalc = w->pixbufshow = NULL;
	gtk_widget_set_size_request(w->drawing, 400, -1);
	gtk_box_pack_start(GTK_BOX(vbox2), w->drawing, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox2), 10);
	gtk_container_add(GTK_CONTAINER(frame), vbox2);

	// create options:
	align = gtk_alignment_new(0.0, 0.0, 0.0, 0.0);
	vbox2 = gtk_vbox_new(FALSE, 8);

	// setting the complex-area:
	gtk_box_pack_start(GTK_BOX(vbox2), createcplxplane(w->txtcplx), FALSE, FALSE, 0);

	// setting data:
	frame = gtk_frame_new(LFKT);	
	table = gtk_table_new(2, 4, FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(table), 5);

	// degree:
	lbl = gtk_label_new_with_mnemonic(LDEGREE);
	gtk_misc_set_alignment(GTK_MISC(lbl), 0.0, 0.0);
	w->txtdegree = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(w->txtdegree), "2");
	gtk_entry_set_max_length(GTK_ENTRY(w->txtdegree), TEXTLEN);
	align = gtk_alignment_new(1.0, 0.0, 0.0, 0.0);
	gtk_widget_set_size_request(w->txtdegree, 20, -1);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl), w->txtdegree);
	gtk_table_set_col_spacing(GTK_TABLE(table), 0, 5);
	gtk_table_attach(GTK_TABLE(table), lbl, 0, 1, 0, 1, GTK_SHRINK | GTK_FILL, GTK_SHRINK, 0, 0);
	gtk_table_attach(GTK_TABLE(table), w->txtdegree, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	
	// itermax:
	lbl = gtk_label_new_with_mnemonic(LITERMAX);
	gtk_misc_set_alignment(GTK_MISC(lbl), 0.0, 1.0);
	w->txtitermax = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(w->txtitermax), TEXTLEN);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl), w->txtitermax);
	gtk_table_attach(GTK_TABLE(table), lbl, 0, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	gtk_table_attach(GTK_TABLE(table), w->txtitermax, 0, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	gtk_table_set_row_spacing(GTK_TABLE(table), 2, 6); 

	// notebook:
	// chkjulia
	notebook = gtk_notebook_new();
	gtk_container_set_border_width(GTK_CONTAINER(notebook), 0);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), FALSE);
	w->chkjulia = gtk_check_button_new_with_mnemonic(LCHKJULIA);
	g_signal_connect(G_OBJECT(w->chkjulia), "toggled", G_CALLBACK(chkj_toggled), w);
	vbox3 = gtk_vbox_new(FALSE, 4);
	gtk_box_pack_start(GTK_BOX(vbox3), w->chkjulia, FALSE, FALSE, 0);

	// julia-const j:
	eventbox = gtk_event_box_new();
	gtk_event_box_set_above_child(GTK_EVENT_BOX(eventbox), FALSE);
	frame2 = gtk_frame_new(LJCONST);
	table2 = gtk_table_new(2, 2, FALSE);
	gtk_table_set_col_spacing(GTK_TABLE(table2), 0, 5);
	gtk_container_set_border_width(GTK_CONTAINER(table2), 5);
	w->lbljre = gtk_label_new_with_mnemonic(LJCRE);
	gtk_misc_set_alignment(GTK_MISC(w->lbljre), 0.0, 0.0);
	w->txtjre = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(w->txtjre), "0");
	gtk_entry_set_max_length(GTK_ENTRY(w->txtjre), TEXTLEN);
	gtk_label_set_mnemonic_widget(GTK_LABEL(w->lbljre), w->txtjre);
	gtk_widget_set_size_request(w->txtjre, 50, -1);
	gtk_table_attach(GTK_TABLE(table2), w->lbljre, 0, 1, 0, 1, GTK_SHRINK | GTK_FILL, GTK_SHRINK, 0, 0);
	gtk_table_attach(GTK_TABLE(table2), w->txtjre, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	gtk_table_set_row_spacing(GTK_TABLE(table), 0 , 1);
	w->lbljim = gtk_label_new_with_mnemonic(LJCIM);
	gtk_misc_set_alignment(GTK_MISC(w->lbljim), 0.0, 0.0);
	w->txtjim = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(w->txtjim), "0");
	gtk_entry_set_max_length(GTK_ENTRY(w->txtjim), TEXTLEN);
	gtk_label_set_mnemonic_widget(GTK_LABEL(w->lbljim), w->txtjim);
	gtk_widget_set_size_request(w->txtjim, 50, -1);
	gtk_table_attach(GTK_TABLE(table2), w->lbljim, 0, 1, 1, 2, GTK_SHRINK | GTK_FILL, GTK_SHRINK, 0, 0);
	gtk_table_attach(GTK_TABLE(table2), w->txtjim, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	gtk_container_add(GTK_CONTAINER(frame2), table2);
	gtk_container_add(GTK_CONTAINER(eventbox), frame2);
	gtk_box_pack_start(GTK_BOX(vbox3), eventbox, FALSE, FALSE, 0);
	gtk_widget_set_events(eventbox, GDK_BUTTON_PRESS_MASK);
	g_signal_connect(G_OBJECT(w->txtjre), "button_press_event", G_CALLBACK(pjmenu), w);
	g_signal_connect(G_OBJECT(w->txtjim), "button_press_event", G_CALLBACK(pjmenu), w);
	g_signal_connect(G_OBJECT(eventbox), "button_press_event", G_CALLBACK(pjmenu), w);

	// getj-button:
	w->btnj = gtk_button_new_with_mnemonic(LGETJ);
	gtk_box_pack_start(GTK_BOX(vbox3), w->btnj, FALSE, FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox3), 5);
	gtk_container_add(GTK_CONTAINER(notebook), vbox3);
	gtk_table_attach(GTK_TABLE(table), notebook, 0, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	gtk_container_add(GTK_CONTAINER(frame), table);
	gtk_box_pack_start(GTK_BOX(vbox2), frame, FALSE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(align), vbox2);
	gtk_box_pack_start(GTK_BOX(hbox), align, FALSE, FALSE, 0);
	
	// create calculation-/close-buttons:
	align = gtk_alignment_new(1.0, 0.0, 0.0, 0.0);
	hbox = gtk_hbox_new(TRUE, 5);
	
	// draw:
	w->btncalc = gtk_button_new_with_mnemonic(LCALC);
	gtk_box_pack_start(GTK_BOX(hbox), w->btncalc, TRUE, TRUE, 0);

	// reset:
	w->btnreset = gtk_button_new_with_mnemonic(LRESET);
	//g_signal_connect(G_OBJECT(btn), "clicked", G_CALLBACK(reset), (gpointer)btn);
	gtk_box_pack_start(GTK_BOX(hbox), w->btnreset, TRUE, TRUE, 0);

	// close:
	w->btnclose = gtk_button_new_with_mnemonic(LCLOSE);
	gtk_box_pack_start(GTK_BOX(hbox), w->btnclose, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(align), hbox);
	gtk_box_pack_start(GTK_BOX(vbox), align, FALSE, FALSE, 0);
	gtk_container_add(GTK_CONTAINER(w->win), rootbox);

	gtk_widget_show_all(w->drawmenu);
	gtk_widget_show_all(w->pjmenu);
	
	statejctl(w);

	return w;
}

void setcplxplane(GtkWidget *txtcplx[4], const gdouble value[4], gdouble width, gdouble height)
{
	gdouble dre, dim;
	gchar buf[30];
	dre = (value[1] - value[0]) / width;
	dim = (value[3] - value[2]) / height;

	if (dre > dim) {
		gdouble m = value[2] + (value[3] - value[2]) / 2;
		g_sprintf(buf, NUMFORMAT, value[0]);
		gtk_entry_set_text(GTK_ENTRY(txtcplx[0]), buf);
		g_sprintf(buf, NUMFORMAT, value[1]);
		gtk_entry_set_text(GTK_ENTRY(txtcplx[1]), buf);
		g_sprintf(buf, NUMFORMAT, m - dre * height / 2);
		gtk_entry_set_text(GTK_ENTRY(txtcplx[2]), buf);
		g_sprintf(buf, NUMFORMAT, m + dre * height / 2);
		gtk_entry_set_text(GTK_ENTRY(txtcplx[3]), buf);
	} else {
		gdouble m = value[0] + (value[1] - value[0]) / 2;
		g_sprintf(buf, NUMFORMAT, m - dim * width / 2);
		gtk_entry_set_text(GTK_ENTRY(txtcplx[0]), buf);
		g_sprintf(buf, NUMFORMAT, m + dim * width / 2);
		gtk_entry_set_text(GTK_ENTRY(txtcplx[1]), buf);
		g_sprintf(buf, NUMFORMAT, value[2]);
		gtk_entry_set_text(GTK_ENTRY(txtcplx[2]), buf);
		g_sprintf(buf, NUMFORMAT, value[3]);
		gtk_entry_set_text(GTK_ENTRY(txtcplx[3]), buf);
	}
}

void restoredefaults(struct winctl *w)
{
	gint tmp = atoi(gtk_entry_get_text(GTK_ENTRY(w->txtdegree)));
	if ((tmp == 2) && !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w->chkjulia))) {
		setcplxplane(w->txtcplx, w->default_mcplxplane, w->drawing->allocation.width, w->drawing->allocation.height);
	} else {
		setcplxplane(w->txtcplx, w->default_cplxplane, w->drawing->allocation.width, w->drawing->allocation.height);
	}
}

void statejctl(struct winctl *w)
{
	if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w->chkjulia))) {
		gtk_widget_set_sensitive(w->lbljre, FALSE);
		gtk_widget_set_sensitive(w->lbljim, FALSE);
		gtk_widget_set_sensitive(w->txtjre, FALSE);
		gtk_widget_set_sensitive(w->txtjim, FALSE);
	} else {
		gtk_widget_set_sensitive(w->lbljre, TRUE);
		gtk_widget_set_sensitive(w->lbljim, TRUE);
		gtk_widget_set_sensitive(w->txtjre, TRUE);
		gtk_widget_set_sensitive(w->txtjim, TRUE);
	}
}

static GtkWidget *convdiv_menu(struct winctl *w)
{
	GtkWidget *menu, *menuit;
	menu = gtk_menu_new();
	menuit = gtk_menu_item_new_with_mnemonic(LCONVCOLOR);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuit);
	g_signal_connect(G_OBJECT(menuit), "activate", G_CALLBACK(change_convcol), w);
	menuit = gtk_menu_item_new_with_mnemonic(LDIVCOLOR);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuit);
	g_signal_connect(G_OBJECT(menuit), "activate", G_CALLBACK(change_divcol), w);
	return menu;
}

static void chkj_toggled(GtkWidget *widget, struct winctl *w)
{
	statejctl(w);
}

static void toggle_zoomprop(GtkWidget *widget, struct winctl *w)
{
	w->zoomprop = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(w->mchkzoomprop), w->zoomprop);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(w->pmchkzoomprop), w->zoomprop);
}

static void menu_setcplxplane(GtkWidget *widget, struct winctl *w)
{
	setcplxplane(w->txtcplx, w->it_param.cplxplane, w->drawing->allocation.width, w->drawing->allocation.height);
}

void change_color_algo(GtkWidget *widget, struct winctl *w)
{
	GtkWidget *sync_obj;
	sync_obj = (GtkWidget *)g_object_get_data(G_OBJECT(widget), "sync_obj");

	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(sync_obj))
			== gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
		return;
	}
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(sync_obj), TRUE);
	w->it_param.color_func_index = *(gint *)g_object_get_data(G_OBJECT(widget) , "index");
}

static gboolean pjmenu(GtkWidget *widget, GdkEventButton *event, struct winctl *w)
{
	if (event->button == 3) {
		gtk_menu_popup(GTK_MENU(w->pjmenu), NULL, NULL, NULL, NULL, event->button, event->time);
		return TRUE;
	}
	return FALSE;
}

static void insert_j(GtkWidget *widget, struct winctl *w)
{
	gchar buf[30];
	gint index = *(gint *)g_object_get_data(G_OBJECT(widget), "index");
	g_sprintf(buf, NUMFORMAT, JDEFINES[2 * index]);
	gtk_entry_set_text(GTK_ENTRY(w->txtjre), buf);
	g_sprintf(buf, NUMFORMAT, JDEFINES[2 * index + 1]);
	gtk_entry_set_text(GTK_ENTRY(w->txtjim), buf);
}

static void setcolor(GtkWidget *widget, GtkColorSelectionDialog *d)
{
	struct winctl *w = (struct winctl *)g_object_get_data(G_OBJECT(widget), "main_window");

	if ((*(gint *)g_object_get_data(G_OBJECT(widget), "type")) == 0) {
		// Color for convergent set:
		gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(d->colorsel), &w->convcol);
	}  else {
		// Color for divergent set:
		gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(d->colorsel), &w->divcol);
	}
	gtk_widget_destroy(GTK_WIDGET(d));
}

static void change_color(gint type, gchar* title, struct winctl *w)
{
	GtkWidget *colordialog;
	GtkWidget *btnok;
	GtkColorSelection *colorsel;
	gint *t = (gint *)g_malloc(sizeof(gint));
	*t = type;
	colordialog = gtk_color_selection_dialog_new(title);
	btnok = GTK_COLOR_SELECTION_DIALOG(colordialog)->ok_button;
	g_object_set_data_full(G_OBJECT(btnok), "type", t, g_free);
	g_object_set_data(G_OBJECT(btnok), "main_window", w);
	g_signal_connect(G_OBJECT(GTK_COLOR_SELECTION_DIALOG(colordialog)->ok_button), "clicked", G_CALLBACK(setcolor), colordialog);
	g_signal_connect_swapped(G_OBJECT(GTK_COLOR_SELECTION_DIALOG(colordialog)->cancel_button), "clicked", G_CALLBACK(gtk_widget_destroy), colordialog);
	g_signal_connect_swapped(G_OBJECT(GTK_COLOR_SELECTION_DIALOG(colordialog)), "destroy", G_CALLBACK(gtk_widget_destroy), colordialog);
	colorsel = GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(colordialog)->colorsel);
	if (type == 0)
		gtk_color_selection_set_current_color(colorsel, &w->convcol);
	else
		gtk_color_selection_set_current_color(colorsel, &w->divcol);
	gtk_window_set_modal(GTK_WINDOW(colordialog), TRUE);
	gtk_widget_show(colordialog);
}	

static void change_convcol(GtkWidget *widget, struct winctl *w)
{
	change_color(0, LCOLCAPCONV, w);
}

static void change_divcol(GtkWidget *widget, struct winctl *w)
{
	change_color(1, LCOLCAPDIV, w);
}

static void store_drawing(GtkWidget *widget, struct winctl *w)
{
	render_thread_pause(w->render_thread);
	gtk_button_set_label(GTK_BUTTON(w->btncalc), LCALC);
	store_drawing_show(w);
}

static void about(GtkWidget *widget, struct winctl *w)
{
	gtk_show_about_dialog(NULL,
			"copyright", "Copyright (c) by Lexif Systems Inc.",
	//		"license", "GPL",
			"version", LVERSION,
			"program-name", LWCAP,
			NULL);
}

