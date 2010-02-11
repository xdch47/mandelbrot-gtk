
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <glib.h>
#include <string.h>
#include "interface.h"
#include "config.h"
#include "defs.h"

#define forall_node(node) for (; node != NULL; node = node->next)
#define BUFSIZE_NUMTOSTR 30

static char *trunc0(char *c)
{
	char *it;
	it = c + strlen(c) - 1;
	for (;;) {
		if (*it == '0') {
			--it;
		} else
			break;
	}
	if (*it != ',') {
		++it;
	}
	*it = '\0';
	return c;
}

static void init_config(struct winctl *w)
{
	gint i;

	// init params:
	w->it_param.degree = 2.0;
	w->it_param.type = MANDELBROT_SET;
	w->it_param.j[0] = 0;
	w->it_param.j[1] = 0;
	w->it_param.itermax = DEFITERMAX;
	w->it_param.color_func_index = 0;
	w->it_param.threads_count = CALCTHREADS;

	for (i = 0; i < 4; ++i) {
		w->default_cplxplane[i] = CPLXPLANE[i];
		w->default_mcplxplane[i] = MCPLXPLANE[i];
		w->it_param.cplxplane[i] = MCPLXPLANE[i];
	}

	w->zoomfactor = ZOOMFACTOR;
	w->convcol.red = 0;
	w->convcol.green = 0;
	w->convcol.blue = 0;
	w->divcol.red = 0xffff;
	w->divcol.green = 0xffff;
	w->divcol.blue = 0xffff;

	w->focus_color.red = 0xffff;
	w->focus_color.green = 0xffff;
	w->focus_color.blue = 0xffff;
	w->get_j = FALSE;
	w->get_jitermax = DEFITERMAX,
	gtk_widget_modify_fg(w->drawing, GTK_STATE_NORMAL, &w->focus_color);
}

static char *ascii_dtostr(double d, char *c)
{
	return g_ascii_dtostr(c, BUFSIZE_NUMTOSTR, d);
}

static char *dtostr(double d, char *c)
{
	g_snprintf(c, BUFSIZE_NUMTOSTR, NUMFORMAT, d);
	return c;
}

static char *ltostr(long int l, char *c)
{
	g_snprintf(c, BUFSIZE_NUMTOSTR, "%ld", l);
	return c;
}

static double xmlNodeContenttod(xmlNode *node)
{
	const char *c = (char *)xmlNodeGetContent(node);
	if (c == NULL)
		return 0.0;

	return g_ascii_strtod((char *)xmlNodeGetContent(node), NULL);
}

static int xmlNodeContenttol(xmlNode *node)
{
	const char *c = (char *)xmlNodeGetContent(node);
	if (c == NULL)
		return 0;

	return strtol((char *)xmlNodeGetContent(node), NULL, 10);
}

static gboolean xmlcplxplane(xmlNode *node, double *cplxplane, enum configtype type)
{
	const char *name;
	char c[BUFSIZE_NUMTOSTR];
	gboolean retval = FALSE;

	forall_node(node) {
		if (node->type != XML_ELEMENT_NODE)
			continue;

		name = (char *)node->name;
		if (strcmp(name, XML_MIN_RE) == 0) {
			if (type == LOAD_CONFIG)
				cplxplane[0] = xmlNodeContenttod(node);
			else
				xmlNodeSetContent(node, BAD_CAST ascii_dtostr(cplxplane[0], c));
			retval = TRUE;
		} else if (strcmp(name, XML_MAX_RE) == 0) {
			if (type == LOAD_CONFIG)
				cplxplane[1] = xmlNodeContenttod(node);
			else
				xmlNodeSetContent(node, BAD_CAST ascii_dtostr(cplxplane[1], c));
			retval = TRUE;
		} else if (strcmp(name, XML_MIN_IM) == 0) {
			if (type == LOAD_CONFIG)
				cplxplane[2] = xmlNodeContenttod(node);
			else {
				xmlNodeSetContent(node, BAD_CAST ascii_dtostr(cplxplane[2], c));
			}
			retval = TRUE;
		} else if (strcmp(name, XML_MAX_IM) == 0) {
			if (type == LOAD_CONFIG)
				cplxplane[3] = xmlNodeContenttod(node);
			else
				xmlNodeSetContent(node, BAD_CAST ascii_dtostr(cplxplane[3], c));
			retval = TRUE;
		}
	}
	return retval;
}

static void xmlconstj(xmlNode *node, double j[2], enum configtype type)
{
	const char *name;
	char c[BUFSIZE_NUMTOSTR];
	forall_node(node) {
		if (node->type != XML_ELEMENT_NODE)
			continue;

		name = (char *)node->name;
		if (strcmp(name, XML_REPART_J) == 0) {
			if (type == LOAD_CONFIG)
				j[0] = xmlNodeContenttod(node);
			else
				xmlNodeSetContent(node, BAD_CAST ascii_dtostr(j[0], c));
		} else if (strcmp(name, XML_IMPART_J) == 0) {
			if (type == LOAD_CONFIG)
				j[1] = xmlNodeContenttod(node);
			else
				xmlNodeSetContent(node, BAD_CAST ascii_dtostr(j[1], c));
		}
	}
}

static gboolean xmlrenderdata(xmlNode *node, struct iterate_param *it_param, enum configtype type)
{
	xmlNode *cplx_node;
	const char *attr, *name;
	char c[BUFSIZE_NUMTOSTR];
	gboolean retval = FALSE;

	// itermax:
	attr = (char *)xmlGetProp(node, BAD_CAST XML_ITERMAX);
	if (attr != NULL) {
		if (type == LOAD_CONFIG)
			it_param->itermax = CLAMP(strtol(attr, NULL, 10), LOWITERMAX, HIITERMAX);
		else 
			xmlSetProp(node, BAD_CAST XML_ITERMAX, BAD_CAST ltostr(it_param->itermax, c));
	}

	// degree:
	attr = (char *)xmlGetProp(node, BAD_CAST XML_DEGREE);
	if (attr != NULL) {
		if (type == LOAD_CONFIG) {
			it_param->degree = CLAMP(g_ascii_strtod(attr, NULL), LOWDEGREE, HIDEGREE);
		} else {
			xmlSetProp(node, BAD_CAST XML_DEGREE, BAD_CAST ascii_dtostr(it_param->degree, c));
		}
	}
	// type:
	attr = (char *)xmlGetProp(node, BAD_CAST XML_TYPE);
	if (attr != NULL) {
		if (type == LOAD_CONFIG && strcmp(attr, XML_TYPE_JULIA) == 0) {
			it_param->type = JULIA_SET;
		} else if (type == STORE_CONFIG) {
			xmlSetProp(node, BAD_CAST XML_TYPE, BAD_CAST ((it_param->type == JULIA_SET) ? XML_TYPE_JULIA : XML_TYPE_MANDELBROT));
		}
	}

	cplx_node = node;
	node = node->children;
	// getting data:
	forall_node(node) {
		if (node->type != XML_ELEMENT_NODE)
			continue;

		name = (char *)node->name;
		if (strcmp(name, XML_COMPLEXPLANE) == 0) {
			retval = xmlcplxplane(node->children, it_param->cplxplane, type);
		} else if (strcmp(name, XML_CONST_J) == 0) {
			xmlconstj(node->children, it_param->j, type);
		}
	}

	attr = (char *)xmlGetProp(cplx_node, BAD_CAST XML_RESTORE);
	if (attr != NULL) {
		if (type == LOAD_CONFIG && strcmp(attr, "true") == 0) {
			retval = FALSE;
		}
	}
	return retval;
}

static void xmlsize(xmlNode *node, GtkWindow *win, enum configtype type)
{
	int win_width, win_height;
	const char *name;
	char c[BUFSIZE_NUMTOSTR];
	win_width = win_height = -1;
	forall_node(node) {
		if (node->type != XML_ELEMENT_NODE)
			continue;

		name = (char *)node->name;
		if (strcmp(name, XML_WIDTH) == 0) {
			if (type == LOAD_CONFIG)
				win_width = xmlNodeContenttol(node);
			else
				xmlNodeSetContent(node, BAD_CAST ltostr(GTK_WIDGET(win)->allocation.width, c));
		} else if (strcmp(name, XML_HEIGHT) == 0) {	
			if (type == LOAD_CONFIG)
				win_height = xmlNodeContenttol(node);
			else 
				xmlNodeSetContent(node, BAD_CAST ltostr(GTK_WIDGET(win)->allocation.height, c));
		}
	}
	if (type == LOAD_CONFIG)
		gtk_window_resize(GTK_WINDOW(win), win_width, win_height);
}

static void xmlsetcolor(xmlNode *node, GdkColor *color, enum configtype type)
{
	const char *attr;
	char c[BUFSIZE_NUMTOSTR];

	attr = (char *)xmlGetProp(node, BAD_CAST XML_COLOR_RED);
	if (attr) {
		if (type == LOAD_CONFIG) 
			color->red = strtol(attr, NULL, 10);
		else
			xmlSetProp(node, BAD_CAST XML_COLOR_RED, BAD_CAST ltostr(color->red, c));
	}
	attr = (char *)xmlGetProp(node, BAD_CAST XML_COLOR_GREEN);
	if (attr) {
		if (type == LOAD_CONFIG) 
			color->green = strtol(attr, NULL, 10);
		else
			xmlSetProp(node, BAD_CAST XML_COLOR_GREEN, BAD_CAST ltostr(color->green, c));
	}
	attr = (char *)xmlGetProp(node, BAD_CAST XML_COLOR_BLUE);
	if (attr) {
		if (type == LOAD_CONFIG) 
			color->blue = strtol(attr, NULL, 10);
		else
			xmlSetProp(node, BAD_CAST XML_COLOR_BLUE, BAD_CAST ltostr(color->blue, c));
	}
}

static void xmlcolor(xmlNode *node, struct winctl *w, enum configtype type)
{
	const char *attr;
	char c[BUFSIZE_NUMTOSTR];

	attr = (char *)xmlGetProp(node, BAD_CAST XML_COLOR_ALGO);
	if (attr != NULL) {
		if (type == LOAD_CONFIG)
			w->it_param.color_func_index = CLAMP(strtol(attr, NULL, 10), 0, COLORFUNC);
		else
			xmlSetProp(node, BAD_CAST XML_COLOR_ALGO, BAD_CAST ltostr(w->it_param.color_func_index, c));
	}

	node = node->children;
	forall_node(node) {
		if (node->type != XML_ELEMENT_NODE)
			continue;
		if (strcmp((char *)node->name, XML_COLOR_DIVERGENT) == 0) {
				xmlsetcolor(node, &w->divcol, type);
		} else if (strcmp((char *)node->name, XML_COLOR_CONVERGET) == 0) {
			xmlsetcolor(node, &w->convcol, type);
		} else if (strcmp((char *)node->name, XML_COLOR_FOCUS) == 0) {
			xmlsetcolor(node, &w->focus_color, type);
		}
	}
}

static void xmlpreference(xmlNode *node, struct winctl *w, enum configtype type)
{
	char c[BUFSIZE_NUMTOSTR];

	for (; node != NULL; node = node->next) {
		if (node->type != XML_ELEMENT_NODE)
			continue;

		if (strcmp((char *)node->name, XML_COLOR) == 0) {
			xmlcolor(node, w, type);
		} else if (strcmp((char *)node->name, XML_ZOOM_PROP) == 0) {
			if (type == LOAD_CONFIG)
				w->zoomprop = (xmlNodeContenttol(node) == 0) ? FALSE : TRUE;
			else
				xmlNodeSetContent(node, BAD_CAST ltostr(w->zoomprop, c));
		} else if (strcmp((char *)node->name, XML_ZOOM_FACTOR) == 0) {
			if (type == LOAD_CONFIG)
				w->zoomfactor = CLAMP(xmlNodeContenttod(node), 0.0, 1.0);
			else
				xmlNodeSetContent(node, BAD_CAST ascii_dtostr(w->zoomfactor, c));
		} else if (strcmp((char *)node->name, XML_GET_J_ITERMAX) == 0) {
			if (type == LOAD_CONFIG)
				w->get_jitermax = CLAMP(xmlNodeContenttol(node), LOWITERMAX, HIITERMAX);
			else
				xmlNodeSetContent(node, BAD_CAST ltostr(w->get_jitermax, c));
		} else if (strcmp((char *)node->name, XML_THREADS_COUNT) == 0) {
			if (type == LOAD_CONFIG) 
				w->it_param.threads_count = CLAMP(xmlNodeContenttol(node), 1, MAXTHREADS);
			else 
				xmlNodeSetContent(node, BAD_CAST ltostr(w->it_param.threads_count, c));
		} else if (strcmp((char *)node->name, XML_DEFCPLXPLANE) == 0) {
			xmlcplxplane(node->children, w->default_cplxplane, type);
		} else if (strcmp((char *)node->name, XML_DEFMCPLXPLANE) == 0) {
			xmlcplxplane(node->children, w->default_mcplxplane, type);
		}
	}
}

static gboolean setxmlconfig(xmlNode *node, struct winctl *w, enum configtype type)
{
	gboolean retval = FALSE;
	for (; node; node = node->next) {
		if (strcmp((char *)node->name, XML_RENDERDATA) == 0) {
			// renderdata:
			retval = xmlrenderdata(node, &w->it_param, type);
		} else if (strcmp((char *)node->name, XML_SIZE) == 0) {
			// size:
			xmlsize(node->children, GTK_WINDOW(w->win), type);
		} else if (strcmp((char *)node->name, XML_PREFERENCE) == 0) {
			// preferenc:
			xmlpreference(node->children, w, type);
		}
	}
	return retval;
}

gboolean configure_interface(struct winctl *w, enum configtype type)
{
	xmlDoc *doc = NULL;
	xmlNode *root_node;
	gint i;
	char c[BUFSIZE_NUMTOSTR];
	gboolean retval = FALSE;

	if (type == LOAD_CONFIG)
		init_config(w);

	LIBXML_TEST_VERSION

	doc = xmlReadFile(w->configfile, NULL, 0);
	if (doc == NULL) {
		iterate_param_init(&w->it_param, w->it_param.threads_count);
	}

	if ((root_node = xmlDocGetRootElement(doc)) != NULL)
		retval = setxmlconfig(root_node->children, w, type);

	if (type == STORE_CONFIG) {
		xmlSaveFormatFile(w->configfile, doc, 1);
		xmlFreeDoc(doc);
		xmlCleanupParser();
		return TRUE;
	}
	
	for (i = 0; i < 4; ++i) {
		dtostr(w->it_param.cplxplane[i], c);
		gtk_entry_set_text(GTK_ENTRY(w->txtcplx[i]), c);
	}

	gtk_entry_set_text(GTK_ENTRY(w->txtitermax), ltostr(w->it_param.itermax, c));
	gtk_entry_set_text(GTK_ENTRY(w->txtdegree), trunc0(dtostr(w->it_param.degree, c)));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w->chkjulia), (w->it_param.type == JULIA_SET) ? TRUE : FALSE);
	gtk_entry_set_text(GTK_ENTRY(w->txtjre), dtostr(w->it_param.j[0], c));
	gtk_entry_set_text(GTK_ENTRY(w->txtjim), dtostr(w->it_param.j[1], c));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(w->mchkzoomprop), w->zoomprop);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(w->mcolalgo[w->it_param.color_func_index]), TRUE);
	gtk_widget_modify_fg(w->drawing, GTK_STATE_NORMAL, &w->focus_color);

	iterate_param_init(&w->it_param, w->it_param.threads_count);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return retval;
}
