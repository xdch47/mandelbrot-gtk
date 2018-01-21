
#include <stdlib.h>
#include <glib/gprintf.h>
#include <gdk-pixbuf/gdk-pixdata.h>
#include "interface.h"
#include "libcolor/color.h"
#include "icon.h"


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
static void open_xmlfile(GtkWidget *widget, struct winctl *w);
static void save_xmlfile(GtkWidget *widget, struct winctl *w);
static void about(GtkWidget *widget, struct winctl *w);
static gboolean tooltip_color(GtkWidget *widget, gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip, struct winctl *w);
static gboolean key_stroke(GtkWidget *widget, GdkEventKey *event, struct winctl *w);

GtkWidget *createcplxplane(GtkWidget *txtcplx[4])
{
	GtkWidget *frame, *vbox, *hbox, *lbl;
	guint i;
	frame = gtk_frame_new(LCOORD);

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	for (i = 0; i <= 3; ++i) {
		lbl = gtk_label_new(_(LCPLX[i]));
		gtk_widget_set_halign(lbl, GTK_ALIGN_START);
		gtk_box_pack_start(GTK_BOX(vbox), lbl, TRUE, TRUE, 0);
	}
	gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 0);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	for (i = 0; i <= 3; ++i) {
		txtcplx[i] = gtk_entry_new();
		gtk_entry_set_max_length(GTK_ENTRY(txtcplx[i]), TEXTLEN);
		gtk_widget_set_size_request(txtcplx[i], 160, -1);
		gtk_widget_set_halign(txtcplx[i], GTK_ALIGN_END);
		gtk_box_pack_start(GTK_BOX(vbox), txtcplx[i], TRUE, TRUE, 0);
	}
	gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 0);

	gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);
	gtk_container_add(GTK_CONTAINER(frame), hbox);
	return frame;
}

struct winctl *buildinterface(void)
{
	GtkWidget *rootbox, *vbox, *vbox2, *vbox3, *hbox, *grid, *grid2;
	GtkWidget *frame, *frame2, *lbl;
	GtkWidget *notebook;
	GtkWidget *menubar, *menu, *menuit, *pmenuit, *submenu, *psubmenu, *psmenuit;
	GtkWidget *eventbox;
	GtkAccelGroup *accel_group;
	GSList *radio_group, *pradio_group;
	gint i;

	/* create mainwindow: */
	struct winctl *w = (struct winctl *)g_malloc(sizeof(struct winctl));
	w->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(w->win), LWCAP);
	rootbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	g_signal_connect(G_OBJECT(w->win), "key_press_event", G_CALLBACK(key_stroke), w);

	/* menu: */
	accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(w->win), accel_group);
	menubar = gtk_menu_bar_new();
	menuit = gtk_menu_item_new_with_mnemonic(LFILE);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuit);
	menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuit) , menu);
	w->drawmenu = gtk_menu_new();
	/* draw: */
	menuit = gtk_menu_item_new_with_mnemonic(LCALC);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuit);
	gtk_widget_add_accelerator(menuit, "activate", accel_group, GDK_KEY_F9, 0, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(menuit), "activate", G_CALLBACK(calc), w);
	/* reset: */
	menuit = gtk_menu_item_new_with_mnemonic(LRESET);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuit);
	gtk_widget_add_accelerator(menuit, "activate", accel_group, GDK_KEY_F2, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(menuit), "activate", G_CALLBACK(reset), w);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), gtk_separator_menu_item_new());
	/* xml: */
#ifdef GTK_DISABLE_DEPRECATED
	menuit = gtk_menu_item_new_with_mnemonic(_("_Open"));
#else
	menuit = gtk_image_menu_item_new_with_mnemonic(_("_Open"));
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuit),
			gtk_image_new_from_icon_name("document-open", GTK_ICON_SIZE_MENU));
#endif

	g_signal_connect(G_OBJECT(menuit), "activate", G_CALLBACK(open_xmlfile), w);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuit);
#ifdef GTK_DISABLE_DEPRECATED
	menuit = gtk_menu_item_new_with_mnemonic(LSAVEXML);
#else
	menuit = gtk_image_menu_item_new_with_mnemonic(LSAVEXML);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuit),
			gtk_image_new_from_icon_name("document-save-as", GTK_ICON_SIZE_MENU));
#endif
	g_signal_connect(G_OBJECT(menuit), "activate", G_CALLBACK(save_xmlfile), w);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuit);
	/* save: */
#ifdef GTK_DISABLE_DEPRECATED
	menuit = gtk_menu_item_new_with_mnemonic(_("_Save"));
#else
	menuit = gtk_image_menu_item_new_with_mnemonic(_("_Save"));
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuit),
			gtk_image_new_from_icon_name("document-save", GTK_ICON_SIZE_MENU));
#endif
#ifdef GTK_DISABLE_DEPRECATED
	pmenuit = gtk_menu_item_new_with_mnemonic(_("_Save"));
#else
	pmenuit = gtk_image_menu_item_new_with_mnemonic(_("_Save"));
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(pmenuit),
			gtk_image_new_from_icon_name("document-save", GTK_ICON_SIZE_MENU));
#endif
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuit);
	gtk_menu_shell_append(GTK_MENU_SHELL(w->drawmenu), pmenuit);
	g_signal_connect(G_OBJECT(menuit) , "activate", G_CALLBACK(store_drawing), w);
	g_signal_connect(G_OBJECT(pmenuit), "activate", G_CALLBACK(store_drawing), w);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu)	   , gtk_separator_menu_item_new());
	gtk_menu_shell_append(GTK_MENU_SHELL(w->drawmenu), gtk_separator_menu_item_new());
	/* close: */
#ifdef GTK_DISABLE_DEPRECATED
	menuit = gtk_menu_item_new_with_mnemonic(_("_Close"));
#else
	menuit = gtk_image_menu_item_new_with_mnemonic(_("_Close"));
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuit),
			gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU));
#endif
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuit);
	gtk_widget_add_accelerator(menuit, "activate", accel_group, GDK_KEY_w, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect_swapped(G_OBJECT(menuit), "activate", G_CALLBACK(gtk_widget_destroy), w->win);
	/* preference: */
	menuit = gtk_menu_item_new_with_mnemonic(LPREFMENU);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuit);
	menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuit), menu);
	/* set complex plane: */
	menuit = gtk_menu_item_new_with_mnemonic(LSETCPLX);
	pmenuit = gtk_menu_item_new_with_mnemonic(LSETCPLX);
	gtk_widget_add_accelerator(menuit , "activate", accel_group, GDK_KEY_r, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(pmenuit, "activate", accel_group, GDK_KEY_r, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu)	   , menuit);
	gtk_menu_shell_append(GTK_MENU_SHELL(w->drawmenu), pmenuit);
	g_signal_connect(G_OBJECT(menuit) , "activate", G_CALLBACK(menu_setcplxplane), w);
	g_signal_connect(G_OBJECT(pmenuit), "activate", G_CALLBACK(menu_setcplxplane), w);
	/* scalexy: */
	w->mchkzoomprop = gtk_check_menu_item_new_with_mnemonic(LZOOMPROP);
	w->pmchkzoomprop = gtk_check_menu_item_new_with_mnemonic(LZOOMPROP);
	gtk_widget_add_accelerator(w->mchkzoomprop, "activate", accel_group, GDK_KEY_s, GDK_SUPER_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(w->pmchkzoomprop, "activate", accel_group, GDK_KEY_s, GDK_SUPER_MASK, GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), w->mchkzoomprop);
	gtk_menu_shell_append(GTK_MENU_SHELL(w->drawmenu), w->pmchkzoomprop);
	g_signal_connect(G_OBJECT(w->mchkzoomprop), "toggled", G_CALLBACK(toggle_zoomprop), w);
	g_signal_connect(G_OBJECT(w->pmchkzoomprop), "toggled", G_CALLBACK(toggle_zoomprop), w);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu)	   , gtk_separator_menu_item_new());
	gtk_menu_shell_append(GTK_MENU_SHELL(w->drawmenu), gtk_separator_menu_item_new());
	/* set color (div./conv.): */
	menuit  = gtk_menu_item_new_with_mnemonic(LSETCOLOR);
	pmenuit = gtk_menu_item_new_with_mnemonic(LSETCOLOR);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu)	   , menuit);
	gtk_menu_shell_append(GTK_MENU_SHELL(w->drawmenu), pmenuit);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuit)  , convdiv_menu(w));
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(pmenuit) , convdiv_menu(w));
	/* set color algo: */
	menuit  = gtk_menu_item_new_with_mnemonic(LCOLORALGO);
	g_object_set(G_OBJECT(menuit), "has-tooltip", TRUE, NULL);
	g_signal_connect(G_OBJECT(menuit), "query-tooltip", G_CALLBACK(tooltip_color), w);
	pmenuit = gtk_menu_item_new_with_mnemonic(LCOLORALGO);
	g_object_set(G_OBJECT(pmenuit), "has-tooltip", TRUE, NULL);
	g_signal_connect(G_OBJECT(pmenuit), "query-tooltip", G_CALLBACK(tooltip_color), w);
	submenu  = gtk_menu_new();
	psubmenu = gtk_menu_new();
	radio_group = pradio_group = NULL;
	w->mcolalgo = (GtkWidget **)g_malloc(sizeof(GtkWidget *) * getColorFunc_count());
	for (i = 0; i < getColorFunc_count(); ++i) {
		gint *index = (gint *)g_malloc(sizeof(gint));
		w->mcolalgo[i] = gtk_radio_menu_item_new_with_mnemonic(radio_group , _(getColorFunc_name(i)));
		psmenuit	   = gtk_radio_menu_item_new_with_mnemonic(pradio_group, _(getColorFunc_name(i)));
		radio_group  = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(w->mcolalgo[i]));
		pradio_group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(psmenuit));
		*index = i;
		g_object_set_data_full(G_OBJECT(w->mcolalgo[i]), "index", index  , g_free);
		g_object_set_data	 (G_OBJECT(psmenuit)	  , "index", index);
		g_object_set_data(G_OBJECT(w->mcolalgo[i]), "sync_obj", psmenuit);
		g_object_set_data(G_OBJECT(psmenuit)	  , "sync_obj", w->mcolalgo[i]);
		g_object_set(G_OBJECT(w->mcolalgo[i]), "has-tooltip", TRUE, NULL);
		g_object_set(G_OBJECT(psmenuit), "has-tooltip", TRUE, NULL);
		g_signal_connect(G_OBJECT(w->mcolalgo[i]), "toggled", G_CALLBACK(change_color_algo), w);
		g_signal_connect(G_OBJECT(w->mcolalgo[i]), "query-tooltip", G_CALLBACK(tooltip_color), w);
		g_signal_connect(G_OBJECT(psmenuit)	  , "toggled", G_CALLBACK(change_color_algo), w);
		g_signal_connect(G_OBJECT(psmenuit)	  , "query-tooltip", G_CALLBACK(tooltip_color), w);
		gtk_menu_shell_append(GTK_MENU_SHELL(submenu) , w->mcolalgo[i]);
		gtk_menu_shell_append(GTK_MENU_SHELL(psubmenu), psmenuit);
	}
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuit) , submenu);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(pmenuit), psubmenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu)	   , menuit);
	gtk_menu_shell_append(GTK_MENU_SHELL(w->drawmenu), pmenuit);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu)	   , gtk_separator_menu_item_new());
	/* preference: */
#ifdef GTK_DISABLE_DEPRECATED
	menuit = gtk_menu_item_new_with_mnemonic(LPREF);
#else
	menuit = gtk_image_menu_item_new_with_mnemonic(LPREF);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuit),
			gtk_image_new_from_icon_name("preferences-system", GTK_ICON_SIZE_MENU));
#endif
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuit);
	g_signal_connect_swapped(G_OBJECT(menuit), "activate", G_CALLBACK(preference_show), w);
	/* info menu: */
	menuit = gtk_menu_item_new_with_mnemonic(LHELP);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuit);
	menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuit), menu);
#ifdef GTK_DISABLE_DEPRECATED
	menuit = gtk_menu_item_new_with_mnemonic(_("_About"));
#else
	menuit = gtk_image_menu_item_new_with_mnemonic(_("_About"));
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuit),
			gtk_image_new_from_icon_name("help-about", GTK_ICON_SIZE_MENU));
#endif
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuit);
	g_signal_connect(G_OBJECT(menuit), "activate", G_CALLBACK(about), w);
	w->pjmenu = gtk_menu_new();
	for (i = 0; i < 6; ++i) {
		gint *index = (gint *)g_malloc(sizeof(gint));
		*index = i;
		menuit = gtk_menu_item_new_with_label(_(LPJMENU[i]));
		gtk_menu_shell_append(GTK_MENU_SHELL(w->pjmenu), menuit);
		g_object_set_data_full(G_OBJECT(menuit), "index", index, g_free);
		g_signal_connect(G_OBJECT(menuit), "activate", G_CALLBACK(insert_j), w);
	}

	gtk_box_pack_start(GTK_BOX(rootbox), menubar, FALSE, FALSE, 0);
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_pack_start(GTK_BOX(rootbox), vbox, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

	/* create image: */
	frame = gtk_frame_new(NULL);
	gtk_box_pack_start(GTK_BOX(hbox), frame, TRUE, TRUE, 0);
	frame2 = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(frame2), GTK_SHADOW_IN);
	vbox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	w->lbldraw = gtk_label_new(LWCAP);
	gtk_label_set_width_chars(GTK_LABEL(w->lbldraw), 50);
	gtk_box_pack_start(GTK_BOX(vbox2), w->lbldraw, FALSE, FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox2), 3);
	gtk_container_add(GTK_CONTAINER(frame2), vbox2);

	vbox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);

	gtk_box_pack_start(GTK_BOX(vbox2), frame2, FALSE, FALSE, 0);
	w->pixbufcalc = NULL;
	w->pixbufdraw = NULL;
	w->pixbufshow = NULL;
    w->focus_draw = FALSE;
	w->drawing = gtk_drawing_area_new();
	w->itermap = NULL;
	gtk_widget_set_size_request(w->drawing, 400, -1);
	gtk_box_pack_start(GTK_BOX(vbox2), w->drawing, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox2), 10);
	gtk_container_add(GTK_CONTAINER(frame), vbox2);

	/* create options: */
	vbox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_box_pack_start(GTK_BOX(hbox), vbox2, FALSE, FALSE, 0);

	/* setting the complex-area: */
	gtk_box_pack_start(GTK_BOX(vbox2), createcplxplane(w->txtcplx), FALSE, FALSE, 0);

	/* setting data: */
	frame = gtk_frame_new(LFKT);
	grid = gtk_grid_new();
	gtk_grid_set_column_spacing (GTK_GRID(grid), 5);
	gtk_container_set_border_width(GTK_CONTAINER(grid), 8);

	/* degree: */
	lbl = gtk_label_new_with_mnemonic(LDEGREE);
	gtk_widget_set_halign(lbl, GTK_ALIGN_START);
	gtk_widget_set_margin_end(lbl, 10);
	w->txtdegree = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(w->txtdegree), "2");
	gtk_entry_set_max_length(GTK_ENTRY(w->txtdegree), TEXTLEN);
	//gtk_widget_set_size_request(w->txtdegree, 20, -1);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl), w->txtdegree);
	gtk_grid_attach(GTK_GRID(grid), lbl, 0, 0, 1, 1);
	gtk_widget_set_halign(lbl, GTK_ALIGN_START);
	gtk_grid_attach(GTK_GRID(grid), w->txtdegree, 1, 0, 1, 1);

	/* itermax: */
	lbl = gtk_label_new_with_mnemonic(LITERMAX);
	gtk_widget_set_halign(lbl, GTK_ALIGN_START);
	w->txtitermax = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(w->txtitermax), TEXTLEN);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl), w->txtitermax);
	gtk_grid_attach(GTK_GRID(grid), lbl, 0, 1, 2, 1);
	gtk_grid_attach(GTK_GRID(grid), w->txtitermax, 0, 2, 2, 1);
	//gtk_table_set_row_spacing(GTK_TABLE(table), 2, 6);

	/* notebook: */
	/* chkjulia */
	notebook = gtk_notebook_new();
	gtk_container_set_border_width(GTK_CONTAINER(notebook), 0);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), FALSE);
	w->chkjulia = gtk_check_button_new_with_mnemonic(LCHKJULIA);
	g_signal_connect(G_OBJECT(w->chkjulia), "toggled", G_CALLBACK(chkj_toggled), w);
	vbox3 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
	gtk_box_pack_start(GTK_BOX(vbox3), w->chkjulia, FALSE, FALSE, 0);

	/* julia-const j: */
	eventbox = gtk_event_box_new();
	gtk_event_box_set_above_child(GTK_EVENT_BOX(eventbox), FALSE);
	frame2 = gtk_frame_new(LJCONST);
	grid2 = gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(grid2), 5);
	gtk_grid_set_column_spacing(GTK_GRID(grid2), 5);
	//gtk_table_set_col_spacing(GTK_TABLE(table2), 0, 5);
	gtk_container_set_border_width(GTK_CONTAINER(grid2), 5);
	w->lbljre = gtk_label_new_with_mnemonic(LJCRE);
	gtk_widget_set_halign(lbl, GTK_ALIGN_START);
	w->txtjre = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(w->txtjre), "0");
	gtk_entry_set_max_length(GTK_ENTRY(w->txtjre), TEXTLEN);
	gtk_label_set_mnemonic_widget(GTK_LABEL(w->lbljre), w->txtjre);
	gtk_widget_set_size_request(w->txtjre, 50, -1);
	gtk_grid_attach(GTK_GRID(grid2), w->lbljre, 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid2), w->txtjre, 1, 0, 1, 1);
	w->lbljim = gtk_label_new_with_mnemonic(LJCIM);
	gtk_widget_set_halign(lbl, GTK_ALIGN_START);
	w->txtjim = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(w->txtjim), "0");
	gtk_entry_set_max_length(GTK_ENTRY(w->txtjim), TEXTLEN);
	gtk_label_set_mnemonic_widget(GTK_LABEL(w->lbljim), w->txtjim);
	gtk_widget_set_size_request(w->txtjim, 50, -1);
	gtk_grid_attach(GTK_GRID(grid2), w->lbljim, 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid2), w->txtjim, 1, 1, 1, 1);
	gtk_container_add(GTK_CONTAINER(frame2), grid2);
	gtk_container_add(GTK_CONTAINER(eventbox), frame2);
	gtk_box_pack_start(GTK_BOX(vbox3), eventbox, FALSE, FALSE, 0);
	gtk_widget_set_events(eventbox, GDK_BUTTON_PRESS_MASK);
	g_signal_connect(G_OBJECT(w->txtjre), "button_press_event", G_CALLBACK(pjmenu), w);
	g_signal_connect(G_OBJECT(w->txtjim), "button_press_event", G_CALLBACK(pjmenu), w);
	g_signal_connect(G_OBJECT(eventbox) , "button_press_event", G_CALLBACK(pjmenu), w);

	/* getj-button: */
	w->btnj = gtk_button_new_with_mnemonic(LGETJ);
	gtk_box_pack_start(GTK_BOX(vbox3), w->btnj, FALSE, FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox3), 5);
	gtk_container_add(GTK_CONTAINER(notebook), vbox3);
	gtk_grid_attach(GTK_GRID(grid), notebook, 0, 3, 2, 1);
	//gtk_widget_set_hexpand(notebook, TRUE);
	gtk_container_add(GTK_CONTAINER(frame), grid);
	gtk_box_pack_start(GTK_BOX(vbox2), frame, FALSE, FALSE, 0);

	gtk_widget_set_halign (vbox2, GTK_ALIGN_END);

	/* create calculation-/close-buttons: */
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	/* draw: */
	w->btncalc = gtk_button_new_with_mnemonic(LCALC);
	gtk_box_pack_start(GTK_BOX(hbox), w->btncalc, TRUE, TRUE, 0);

	/* reset: */
	w->btnreset = gtk_button_new_with_mnemonic(LRESET);
	gtk_box_pack_start(GTK_BOX(hbox), w->btnreset, TRUE, TRUE, 0);

	/* close: */
	w->btnclose = gtk_button_new_with_mnemonic(LCLOSE);
	gtk_box_pack_start(GTK_BOX(hbox), w->btnclose, TRUE, TRUE, 0);
	gtk_widget_set_halign (hbox, GTK_ALIGN_END);
	gtk_box_pack_end(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(w->win), rootbox);

	gtk_widget_show_all(w->drawmenu);
	gtk_widget_show_all(w->pjmenu);

	statejctl(w);

	return w;
}

void setcplxplane(GtkWidget *txtcplx[4], const gdouble value[4], gint width, gint height)
{
	gdouble dre, dim;
	gchar buf[30];
	dre = (value[1] - value[0]) / (gdouble)width;
	dim = (value[3] - value[2]) / (gdouble)height;

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
	GtkAllocation drawing_alloc;

	gtk_widget_get_allocation(w->drawing, &drawing_alloc);
	if ((tmp == 2) && !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w->chkjulia))) {
		setcplxplane(w->txtcplx, w->default_mcplxplane, drawing_alloc.width, drawing_alloc.height);
	} else {
		setcplxplane(w->txtcplx, w->default_cplxplane, drawing_alloc.width, drawing_alloc.height);
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
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(w->mchkzoomprop) , w->zoomprop);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(w->pmchkzoomprop), w->zoomprop);
}

static void menu_setcplxplane(GtkWidget *widget, struct winctl *w)
{
	GtkAllocation drawing_alloc;

	gtk_widget_get_allocation(w->drawing, &drawing_alloc);
	setcplxplane(w->txtcplx, w->it_param.cplxplane, drawing_alloc.width, drawing_alloc.height);
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

	if (w->succ_render) {
		redraw_pixbuf(w);
	} else {
		w->redraw = TRUE;
	}
}

static gboolean pjmenu(GtkWidget *widget, GdkEventButton *event, struct winctl *w)
{
	if (event->button == 3) {
		gtk_menu_popup_at_pointer(GTK_MENU(w->pjmenu), (GdkEvent *)event);
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

static void change_color(gint type, const gchar* title, struct winctl *w)
{
	GtkWidget *colordialog;

	colordialog = gtk_color_chooser_dialog_new(title, GTK_WINDOW(w->win));

	if (type == 0)
		gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(colordialog), &w->convcol);
	else
		gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(colordialog), &w->divcol);
	if (gtk_dialog_run(GTK_DIALOG(colordialog)) == GTK_RESPONSE_OK) {
		if (type == 0) {
			/* Color for convergent set: */
			gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(colordialog), &w->convcol);
		}  else {
			/* Color for divergent set: */
			gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(colordialog), &w->divcol);
		}
		gtk_widget_destroy(GTK_WIDGET(colordialog));
		if (w->succ_render) {
			redraw_pixbuf(w);
		} else {
			w->redraw = TRUE;
		}
	}
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
	if (is_render_thread_alive(w->render_thread)) {
		render_thread_pause(w->render_thread);
		g_timer_stop(w->timer);
	}
	gtk_button_set_label(GTK_BUTTON(w->btncalc), LCALC);
	store_drawing_show(w);
}

static void open_xmlfile(GtkWidget *widget, struct winctl *w)
{
	GtkWidget *filechooser = gtk_file_chooser_dialog_new(LOPENCAP, GTK_WINDOW(w->win),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			_("_OK"), GTK_RESPONSE_OK,
			_("_Cancel"), GTK_RESPONSE_CANCEL, NULL);
	GtkFileFilter *filefilter = gtk_file_filter_new();
	gtk_file_filter_set_name(GTK_FILE_FILTER(filefilter), LFILTERXMLNAME);
	gtk_file_filter_add_mime_type(GTK_FILE_FILTER(filefilter), "text/xml");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(filechooser), filefilter);
	if (gtk_dialog_run(GTK_DIALOG(filechooser)) == GTK_RESPONSE_OK) {
		gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooser));
		w->succ_render = FALSE;
		configure_interface(w, filename, LOAD_RENDER_CONFIG);
		g_free(filename);
	}
	gtk_widget_destroy(filechooser);

	render_thread_kill(w->render_thread);
	calc(w->btncalc, w);
}

static void save_xmlfile(GtkWidget *widget, struct winctl *w)
{
	gchar *filename;
	gchar *dirname;
	gboolean chkdir;
	GtkWidget *filechooser = gtk_file_chooser_dialog_new(LSAVECAP, GTK_WINDOW(w->win),
			GTK_FILE_CHOOSER_ACTION_SAVE,
			_("_OK"), GTK_RESPONSE_OK,
			_("_Cancel"), GTK_RESPONSE_CANCEL, NULL);
	GtkFileFilter *filefilter = gtk_file_filter_new();
	gtk_file_filter_set_name(GTK_FILE_FILTER(filefilter), LFILTERXMLNAME);
	gtk_file_filter_add_mime_type(GTK_FILE_FILTER(filefilter), "text/xml");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(filechooser), filefilter);
	if (gtk_dialog_run(GTK_DIALOG(filechooser)) == GTK_RESPONSE_OK) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooser));
		dirname = g_path_get_dirname(filename);
		/* check the directory for existing and read/write permission: */
		chkdir = g_file_test(dirname, G_FILE_TEST_IS_DIR);

		if (!chkdir) {
			errdialog(GTK_WINDOW(w->win), LDIRERR);
			g_free(filename);
			g_free(dirname);
			gtk_widget_destroy(filechooser);
			return;
		}
#ifdef G_OS_LINUX
		if(g_access(dirname, 0600) != 0) {
			errdialog(GTK_WINDOW(w->win), LPERMERR);
			g_free(filename);
			g_free(dirname);
			gtk_widget_destroy(filechooser);
			return;
		}
#endif
		g_free(dirname);
		if (g_file_test(filename, G_FILE_TEST_IS_REGULAR)) {
			GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(w->win), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, LFILEEXISTS, filename);
			if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_YES) {
				g_free(filename);
				gtk_widget_destroy(dialog);
				gtk_widget_destroy(filechooser);
				return;
			}
			gtk_widget_destroy(dialog);
		}
		save_rendersettings_xml(filename, w);
		g_free(filename);
	}
	gtk_widget_destroy(filechooser);
}

static void about(GtkWidget *widget, struct winctl *w)
{
	gtk_show_about_dialog(GTK_WINDOW(w->win),
			"logo", gdk_pixbuf_from_pixdata(&app_icon, FALSE, NULL),
			"copyright", "Copyright (c) by Lexif Systems Inc.",
			"license", GPLv3,
			//"license-type", GTK_LICENSE_GPL_3_0,
			"version", LVERSION,
			"program-name", LWCAP,
			"website-label", "mandelbrot-gtk",
			"website", "http://sourceforge.net/projects/mandelbrot-gtk/",
			NULL);
}

static gboolean key_stroke(GtkWidget *widget, GdkEventKey *event, struct winctl *w)
{
	if (event->keyval == GDK_KEY_i) {
		zoom(w, ZOOM_IN);
		return TRUE;
	} else if (event->keyval == GDK_KEY_o) {
		zoom(w, ZOOM_OUT);
		return TRUE;
	} else if (event->keyval == GDK_KEY_n) {
		w->it_param.color_func_index = (w->it_param.color_func_index + 1) % getColorFunc_count();
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(w->mcolalgo[w->it_param.color_func_index]), TRUE);
		return TRUE;
	} else if (event->keyval == GDK_KEY_p) {
		w->it_param.color_func_index = (getColorFunc_count() + w->it_param.color_func_index - 1) % getColorFunc_count();
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(w->mcolalgo[w->it_param.color_func_index]), TRUE);
		return TRUE;
	}
	return FALSE;
}

static gboolean tooltip_color(GtkWidget *widget, gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip, struct winctl *w)
{
	gtk_tooltip_set_text(tooltip, _("Key-Stroke: p (previous) / n (next)"));
	return TRUE;
}
