
#include <libconfig.h>
#include "config.h"


int load_config(struct winctl *w)
{
	config_t config;
	config_setting_t *sapp;
	config_setting_t *s;


	config_init(&config);
	if (!config_read_file(&config, w->configfile)) {
		config_destroy(&config);
		return 0;
	}

	if ((sapp = config_lookup(&config, CONFGROUPAPP))) {
		if ((s = config_setting_get_member(sapp, CONFCPLXPLANE))) {
			guint i;
			for (i = 0; i < 4; ++i) {
				gtk_entry_set_text(GTK_ENTRY(w->txtcplx[i]), 
						config_setting_get_string_elem(s, i));
			}
		} else {
			config_destroy(&config);
			return 0;
		}
		if ((s = config_setting_get_member(sapp, CONFITERMAX))) 
				gtk_entry_set_text(GTK_ENTRY(w->txtitermax),
					config_setting_get_string(s));
		
		if ((s = config_setting_get_member(sapp, CONFDEG)))
				gtk_entry_set_text(GTK_ENTRY(w->txtdegree),
					config_setting_get_string(s));
		
		if ((s = config_setting_get_member(sapp, CONFJ)))
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w->chkjulia), 
					config_setting_get_bool(s));

		if ((s = config_setting_get_member(sapp, CONFJRE)))
				gtk_entry_set_text(GTK_ENTRY(w->txtjre),
					config_setting_get_string(s));
		if ((s = config_setting_get_member(sapp, CONFJIM)))
				gtk_entry_set_text(GTK_ENTRY(w->txtjim),
					config_setting_get_string(s));
	}
	if ((sapp = config_lookup(&config, CONFGROUPSIZE))) {
		gint width = 0, height = 0;
		if ((s = config_setting_get_member(sapp, CONFWIDTH))) 
			width = config_setting_get_int(s);
		if ((s = config_setting_get_member(sapp, CONFHEIGHT)))
			height = config_setting_get_int(s);

		if (width && height) {
			gtk_window_resize(GTK_WINDOW(w->win), width, height);
		}
	}
	if ((sapp = config_lookup(&config, CONFGROUPPREF))) {
		if ((s = config_setting_get_member(sapp, CONFXYSCALE))) 
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(w->mchkxyscale), config_setting_get_bool(s));
		if ((s = config_setting_get_member(sapp, CONFCOLORALGO))) {
			w->color_func_index = config_setting_get_int(s); 
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(w->mcolalgo[w->color_func_index]), TRUE);
		}
		if ((s = config_setting_get_member(sapp, CONFCOLORCONV))) {
			w->convcol.red = config_setting_get_int_elem(s, 0);
			w->convcol.green = config_setting_get_int_elem(s, 1);
			w->convcol.blue = config_setting_get_int_elem(s, 2);
		}
		if ((s = config_setting_get_member(sapp, CONFCOLORDIV))) {
			w->divcol.red = config_setting_get_int_elem(s, 0);
			w->divcol.green = config_setting_get_int_elem(s, 1);
			w->divcol.blue = config_setting_get_int_elem(s, 2);
		}
	}

	config_destroy(&config);
	return 1;
}

void save_config(struct winctl *w)
{
	config_t config;
	config_setting_t *sapp;
	config_setting_t *s;
	guint i;

	config_init(&config);
	
	sapp = config_setting_add(config_root_setting(&config), CONFGROUPAPP, CONFIG_TYPE_GROUP);
	s = config_setting_add(sapp, CONFCPLXPLANE , CONFIG_TYPE_ARRAY);
	for (i = 0; i < 4; ++i)
		config_setting_set_string_elem(s, -1, gtk_entry_get_text(GTK_ENTRY(w->txtcplx[i])));
	s = config_setting_add(sapp, CONFITERMAX, CONFIG_TYPE_STRING);
	config_setting_set_string(s, gtk_entry_get_text(GTK_ENTRY(w->txtitermax)));
	s = config_setting_add(sapp, CONFDEG, CONFIG_TYPE_STRING);
	config_setting_set_string(s, gtk_entry_get_text(GTK_ENTRY(w->txtdegree)));
	s = config_setting_add(sapp, CONFJ, CONFIG_TYPE_BOOL);
	config_setting_set_bool(s, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w->chkjulia)));
	s = config_setting_add(sapp, CONFJRE, CONFIG_TYPE_STRING);
	config_setting_set_string(s, gtk_entry_get_text(GTK_ENTRY(w->txtjre)));
	s = config_setting_add(sapp, CONFJIM, CONFIG_TYPE_STRING);
	config_setting_set_string(s, gtk_entry_get_text(GTK_ENTRY(w->txtjim)));

	sapp = config_setting_add(config_root_setting(&config), CONFGROUPSIZE, CONFIG_TYPE_GROUP);
	s = config_setting_add(sapp, CONFWIDTH, CONFIG_TYPE_INT);
	config_setting_set_int(s, w->win->allocation.width);
	s = config_setting_add(sapp, CONFHEIGHT, CONFIG_TYPE_INT);
	config_setting_set_int(s, w->win->allocation.height);

	sapp = config_setting_add(config_root_setting(&config), CONFGROUPPREF, CONFIG_TYPE_GROUP);
	s = config_setting_add(sapp, CONFXYSCALE, CONFIG_TYPE_BOOL);
	config_setting_set_bool(s, gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(w->mchkxyscale)));
	s = config_setting_add(sapp, CONFCOLORALGO, CONFIG_TYPE_INT);
	config_setting_set_int(s, w->color_func_index);
	s = config_setting_add(sapp, CONFCOLORCONV, CONFIG_TYPE_ARRAY);
	config_setting_set_int_elem(s, -1, w->convcol.red);
	config_setting_set_int_elem(s, -1, w->convcol.green);
	config_setting_set_int_elem(s, -1, w->convcol.blue);
	s = config_setting_add(sapp, CONFCOLORDIV, CONFIG_TYPE_ARRAY);
	config_setting_set_int_elem(s, -1, w->divcol.red);
	config_setting_set_int_elem(s, -1, w->divcol.green);
	config_setting_set_int_elem(s, -1, w->divcol.blue);

	config_write_file(&config, w->configfile); 
	config_destroy(&config);
}
