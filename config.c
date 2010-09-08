
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>
#include <glib.h>
#include <string.h>
#include "interface.h"
#include "defs.h"
#include "libcolor/color.h"

#define XML_ROOTNODE "mandelbrot_julia_set"
#define XML_RENDERDATA "renderdata"
#define XML_ITERMAX "itermax"
#define XML_DEGREE "degree"
#define XML_TYPE "type"
#define XML_TYPE_JULIA "julia-set"
#define XML_TYPE_MANDELBROT "mandelbrot-set"
#define XML_RESTORE "reset"
#define XML_COMPLEXPLANE "complexplane"
#define XML_MIN_RE "minre"
#define XML_MAX_RE "maxre"
#define XML_MIN_IM "minim"
#define XML_MAX_IM "maxim"
#define XML_CONST_J "const_j"
#define XML_REPART_J "repart_j"
#define XML_IMPART_J "impart_j"
#define XML_SIZE "size"
#define XML_WIDTH "width"
#define XML_HEIGHT "height"
#define XML_PREFERENCE "preference"
#define XML_COLOR "color"
#define XML_COLOR_ALGO "algorithmen"
#define XML_COLOR_DIVERGENT "divergent-color"
#define XML_COLOR_CONVERGET "convergent-color"
#define XML_COLOR_FOCUS "focus-color"
#define XML_COLOR_RED "red"
#define XML_COLOR_GREEN "green"
#define XML_COLOR_BLUE "blue"
#define XML_ZOOM_PROP "zoom-prop"
#define XML_ZOOM_FACTOR "zoom-factor"
#define XML_GET_J_ITERMAX "get_j-itermax"
#define XML_THREADS_COUNT "threadscount"
#define XML_DEFMCPLXPLANE "default-mandelbrot-complexplane"
#define XML_DEFCPLXPLANE "default-complexplane"

#define forall_node(node) for (; node != NULL; node = node->next)
#define BUFSIZE_NUMTOSTR 30

enum xmlsavetype { SAVE_IMAGE, SAVE_APPSETTINGS };

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

	/* init params: */
	w->it_param.degree = 2.0;
	w->it_param.type = MANDELBROT_SET;
	w->it_param.j[0] = 0;
	w->it_param.j[1] = 0;
	w->it_param.itermax = DEFITERMAX;
	w->it_param.color_func_index = 0;
	w->it_param.update_func = update_pixbuf;
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
	w->get_jitermax = DEFITERMAX;
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

static void writexmlfile(const char *configfile, const struct winctl *w, enum xmlsavetype type)
{
	xmlTextWriter *writer;
	xmlDoc *doc;
	char c[BUFSIZE_NUMTOSTR];

	writer = xmlNewTextWriterDoc(&doc, 0);
	if (writer == NULL) {
		printf("Error creating the xml-file.\b");
		return;
	}

	xmlTextWriterStartDocument(writer, "1.0", "UTF-8", "yes");
	xmlTextWriterStartElement(writer, BAD_CAST XML_ROOTNODE);
	
	#ifdef XML_COMMENTS
	xmlTextWriterWriteComment(writer, BAD_CAST " data for rendering ");
	#endif

	xmlTextWriterStartElement(writer, BAD_CAST XML_RENDERDATA);
	xmlTextWriterWriteAttribute(writer, BAD_CAST XML_ITERMAX, BAD_CAST ltostr(w->it_param.itermax, c));
	xmlTextWriterWriteAttribute(writer, BAD_CAST XML_DEGREE, BAD_CAST ascii_dtostr(w->it_param.degree, c));
	if (w->it_param.type == MANDELBROT_SET)
		xmlTextWriterWriteAttribute(writer, BAD_CAST XML_TYPE, BAD_CAST XML_TYPE_MANDELBROT);
	else 
		xmlTextWriterWriteAttribute(writer, BAD_CAST XML_TYPE, BAD_CAST XML_TYPE_JULIA);

	/* <renderdata> */
	/* <complexplane> */
	xmlTextWriterStartElement(writer, BAD_CAST XML_COMPLEXPLANE);
	xmlTextWriterWriteElement(writer, BAD_CAST XML_MIN_RE, BAD_CAST ascii_dtostr(w->it_param.cplxplane[0], c));
	xmlTextWriterWriteElement(writer, BAD_CAST XML_MAX_RE, BAD_CAST ascii_dtostr(w->it_param.cplxplane[1], c));
	xmlTextWriterWriteElement(writer, BAD_CAST XML_MIN_IM, BAD_CAST ascii_dtostr(w->it_param.cplxplane[2], c));
	xmlTextWriterWriteElement(writer, BAD_CAST XML_MAX_IM, BAD_CAST ascii_dtostr(w->it_param.cplxplane[3], c));
	/* </complexplane> */
	xmlTextWriterEndElement(writer);

	if (type == SAVE_APPSETTINGS || w->it_param.type == JULIA_SET) {
		/* <const_j> */
		xmlTextWriterStartElement(writer, BAD_CAST XML_CONST_J);
		xmlTextWriterWriteElement(writer, BAD_CAST XML_REPART_J, BAD_CAST ascii_dtostr(w->it_param.j[0], c));
		xmlTextWriterWriteElement(writer, BAD_CAST XML_IMPART_J, BAD_CAST ascii_dtostr(w->it_param.j[1], c));
		/* </const_j> */
		xmlTextWriterEndElement(writer);
	}
	/* </renderdata> */
	xmlTextWriterEndElement(writer);

	if (type == SAVE_APPSETTINGS) {
		#ifdef XML_COMMENTS
		xmlTextWriterWriteComment(writer, BAD_CAST " windowsize ");
		#endif
		/* <size> */
		xmlTextWriterStartElement(writer, BAD_CAST XML_SIZE);
		xmlTextWriterWriteElement(writer, BAD_CAST XML_WIDTH, BAD_CAST ltostr(GTK_WIDGET(w->win)->allocation.width, c));
		xmlTextWriterWriteElement(writer, BAD_CAST XML_HEIGHT, BAD_CAST ltostr(GTK_WIDGET(w->win)->allocation.height, c));
		xmlTextWriterEndElement(writer);
		/* </size> */
	}

	/* <preference> */
	xmlTextWriterStartElement(writer, BAD_CAST XML_PREFERENCE);
	/* <color> */
	xmlTextWriterStartElement(writer, BAD_CAST XML_COLOR);
	xmlTextWriterWriteAttribute(writer, BAD_CAST XML_COLOR_ALGO, BAD_CAST ltostr(w->it_param.color_func_index, c));
	/* <convergent-color/> */
	xmlTextWriterWriteElement(writer, BAD_CAST XML_COLOR_CONVERGET, BAD_CAST NULL);
	xmlTextWriterWriteAttribute(writer, BAD_CAST XML_COLOR_RED, BAD_CAST ltostr(w->convcol.red, c));
	xmlTextWriterWriteAttribute(writer, BAD_CAST XML_COLOR_GREEN, BAD_CAST ltostr(w->convcol.green, c));
	xmlTextWriterWriteAttribute(writer, BAD_CAST XML_COLOR_BLUE, BAD_CAST ltostr(w->convcol.blue, c));
	xmlTextWriterEndElement(writer);
	if (type == SAVE_APPSETTINGS || w->it_param.color_func_index == getDivConv_idx()) {
		/* <divergent-color/> */
		xmlTextWriterWriteElement(writer, BAD_CAST XML_COLOR_DIVERGENT, BAD_CAST NULL);
		xmlTextWriterWriteAttribute(writer, BAD_CAST XML_COLOR_RED, BAD_CAST ltostr(w->divcol.red, c));
		xmlTextWriterWriteAttribute(writer, BAD_CAST XML_COLOR_GREEN, BAD_CAST ltostr(w->divcol.green, c));
		xmlTextWriterWriteAttribute(writer, BAD_CAST XML_COLOR_BLUE, BAD_CAST ltostr(w->divcol.blue, c));
		xmlTextWriterEndElement(writer);
	}
	if (type == SAVE_APPSETTINGS) {
		/* <focus-color/> */
		xmlTextWriterWriteElement(writer, BAD_CAST XML_COLOR_FOCUS, BAD_CAST NULL);
		xmlTextWriterWriteAttribute(writer, BAD_CAST XML_COLOR_RED, BAD_CAST ltostr(w->focus_color.red, c));
		xmlTextWriterWriteAttribute(writer, BAD_CAST XML_COLOR_GREEN, BAD_CAST ltostr(w->focus_color.green, c));
		xmlTextWriterWriteAttribute(writer, BAD_CAST XML_COLOR_BLUE, BAD_CAST ltostr(w->focus_color.blue, c));
		xmlTextWriterEndElement(writer);
	}
	/* </color> */
	xmlTextWriterEndElement(writer);

	if (type == SAVE_APPSETTINGS) {
		#ifdef XML_COMMENTS
		xmlTextWriterWriteComment(writer, BAD_CAST " zooming proportional ");
		#endif
		xmlTextWriterWriteElement(writer, BAD_CAST XML_ZOOM_PROP, BAD_CAST ltostr(w->zoomprop, c));
		xmlTextWriterWriteElement(writer, BAD_CAST XML_ZOOM_FACTOR, BAD_CAST ascii_dtostr(w->zoomfactor, c));
		xmlTextWriterWriteElement(writer, BAD_CAST XML_GET_J_ITERMAX, BAD_CAST ltostr(w->get_jitermax, c));

		#ifdef XML_COMMENTS
		xmlTextWriterWriteComment(writer, BAD_CAST " number of render threads ");
		#endif
		xmlTextWriterWriteElement(writer, BAD_CAST XML_THREADS_COUNT, BAD_CAST ltostr(w->it_param.threads_count, c));

		#ifdef XML_COMMENTS
		xmlTextWriterWriteComment(writer, BAD_CAST " default vaules for reset ");
		#endif
		/* <default-complexplane> */
		xmlTextWriterStartElement(writer, BAD_CAST XML_DEFCPLXPLANE);
		xmlTextWriterWriteElement(writer, BAD_CAST XML_MIN_RE, BAD_CAST ascii_dtostr(w->default_cplxplane[0], c));
		xmlTextWriterWriteElement(writer, BAD_CAST XML_MAX_RE, BAD_CAST ascii_dtostr(w->default_cplxplane[1], c));
		xmlTextWriterWriteElement(writer, BAD_CAST XML_MIN_IM, BAD_CAST ascii_dtostr(w->default_cplxplane[2], c));
		xmlTextWriterWriteElement(writer, BAD_CAST XML_MAX_IM, BAD_CAST ascii_dtostr(w->default_cplxplane[3], c));
		/* </default-complexplane> */
		xmlTextWriterEndElement(writer);
		/* <default-mandelbrot-complexplane> */
		xmlTextWriterStartElement(writer, BAD_CAST XML_DEFMCPLXPLANE);
		xmlTextWriterWriteElement(writer, BAD_CAST XML_MIN_RE, BAD_CAST ascii_dtostr(w->default_mcplxplane[0], c));
		xmlTextWriterWriteElement(writer, BAD_CAST XML_MAX_RE, BAD_CAST ascii_dtostr(w->default_mcplxplane[1], c));
		xmlTextWriterWriteElement(writer, BAD_CAST XML_MIN_IM, BAD_CAST ascii_dtostr(w->default_mcplxplane[2], c));
		xmlTextWriterWriteElement(writer, BAD_CAST XML_MAX_IM, BAD_CAST ascii_dtostr(w->default_mcplxplane[3], c));
		/* </default-mandelbrot-complexplane> */
		xmlTextWriterEndElement(writer);
	}

	/* </preference> */
	xmlTextWriterEndElement(writer);
	/* </mandelbrot_julia_set< */
	xmlTextWriterEndElement(writer);

	xmlFreeTextWriter(writer);

	xmlSaveFormatFile(configfile, doc, 1);

	xmlFreeDoc(doc);
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
			if (type == LOAD_CONFIG || type == LOAD_RENDER_CONFIG)
				cplxplane[0] = xmlNodeContenttod(node);
			else if (type == STORE_CONFIG)
				xmlNodeSetContent(node, BAD_CAST ascii_dtostr(cplxplane[0], c));
			retval = TRUE;
		} else if (strcmp(name, XML_MAX_RE) == 0) {
			if (type == LOAD_CONFIG || type == LOAD_RENDER_CONFIG)
				cplxplane[1] = xmlNodeContenttod(node);
			else if (type == STORE_CONFIG)
				xmlNodeSetContent(node, BAD_CAST ascii_dtostr(cplxplane[1], c));
			retval = TRUE;
		} else if (strcmp(name, XML_MIN_IM) == 0) {
			if (type == LOAD_CONFIG || type == LOAD_RENDER_CONFIG)
				cplxplane[2] = xmlNodeContenttod(node);
			else if (type == STORE_CONFIG)
				xmlNodeSetContent(node, BAD_CAST ascii_dtostr(cplxplane[2], c));
			retval = TRUE;
		} else if (strcmp(name, XML_MAX_IM) == 0) {
			if (type == LOAD_CONFIG || type == LOAD_RENDER_CONFIG)
				cplxplane[3] = xmlNodeContenttod(node);
			else if (type == STORE_CONFIG)
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
			if (type == LOAD_CONFIG || type == LOAD_RENDER_CONFIG)
				j[0] = xmlNodeContenttod(node);
			else if (type == STORE_CONFIG)
				xmlNodeSetContent(node, BAD_CAST ascii_dtostr(j[0], c));
		} else if (strcmp(name, XML_IMPART_J) == 0) {
			if (type == LOAD_CONFIG || type == LOAD_RENDER_CONFIG)
				j[1] = xmlNodeContenttod(node);
			else if (type == STORE_CONFIG)
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

	/* itermax: */
	attr = (char *)xmlGetProp(node, BAD_CAST XML_ITERMAX);
	if (attr != NULL) {
		if (type == LOAD_CONFIG || type == LOAD_RENDER_CONFIG)
			it_param->itermax = CLAMP(strtol(attr, NULL, 10), LOWITERMAX, HIITERMAX);
		else if (type == STORE_CONFIG)
			xmlSetProp(node, BAD_CAST XML_ITERMAX, BAD_CAST ltostr(it_param->itermax, c));
	}

	/* degree: */
	attr = (char *)xmlGetProp(node, BAD_CAST XML_DEGREE);
	if (attr != NULL) {
		if (type == LOAD_CONFIG || type == LOAD_RENDER_CONFIG) {
			it_param->degree = CLAMP(g_ascii_strtod(attr, NULL), LOWDEGREE, HIDEGREE);
		} else if (type == STORE_CONFIG) {
			xmlSetProp(node, BAD_CAST XML_DEGREE, BAD_CAST ascii_dtostr(it_param->degree, c));
		}
	}
	/* type: */
	attr = (char *)xmlGetProp(node, BAD_CAST XML_TYPE);
	if(type == LOAD_RENDER_CONFIG) 
		it_param->type = MANDELBROT_SET;
	if (attr != NULL) {
		if ((type == LOAD_CONFIG || type == LOAD_RENDER_CONFIG) && strcmp(attr, XML_TYPE_JULIA) == 0) {
			it_param->type = JULIA_SET;
		} else if (type == STORE_CONFIG) {
			xmlSetProp(node, BAD_CAST XML_TYPE, BAD_CAST ((it_param->type == JULIA_SET) ? XML_TYPE_JULIA : XML_TYPE_MANDELBROT));
		}
	}

	cplx_node = node;
	node = node->children;
	/* getting data: */
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
			if (type == LOAD_CONFIG )
				win_width = xmlNodeContenttol(node);
			else if (type == STORE_CONFIG) 
				xmlNodeSetContent(node, BAD_CAST ltostr(GTK_WIDGET(win)->allocation.width, c));
		} else if (strcmp(name, XML_HEIGHT) == 0) {	
			if (type == LOAD_CONFIG)
				win_height = xmlNodeContenttol(node);
			else if (type == STORE_CONFIG)
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
		if (type == LOAD_CONFIG || type == LOAD_RENDER_CONFIG) 
			color->red = strtol(attr, NULL, 10);
		else if (type == STORE_CONFIG)
			xmlSetProp(node, BAD_CAST XML_COLOR_RED, BAD_CAST ltostr(color->red, c));
	}
	attr = (char *)xmlGetProp(node, BAD_CAST XML_COLOR_GREEN);
	if (attr) {
		if (type == LOAD_CONFIG || type == LOAD_RENDER_CONFIG) 
			color->green = strtol(attr, NULL, 10);
		else if (type == STORE_CONFIG)
			xmlSetProp(node, BAD_CAST XML_COLOR_GREEN, BAD_CAST ltostr(color->green, c));
	}
	attr = (char *)xmlGetProp(node, BAD_CAST XML_COLOR_BLUE);
	if (attr) {
		if (type == LOAD_CONFIG || type == LOAD_RENDER_CONFIG) 
			color->blue = strtol(attr, NULL, 10);
		else if (type == STORE_CONFIG)
			xmlSetProp(node, BAD_CAST XML_COLOR_BLUE, BAD_CAST ltostr(color->blue, c));
	}
}

static void xmlcolor(xmlNode *node, struct winctl *w, enum configtype type)
{
	const char *attr;
	char c[BUFSIZE_NUMTOSTR];

	attr = (char *)xmlGetProp(node, BAD_CAST XML_COLOR_ALGO);
	if (attr != NULL) {
		if (type == LOAD_CONFIG || type == LOAD_RENDER_CONFIG) 
			w->it_param.color_func_index = CLAMP(strtol(attr, NULL, 10), 0, getColorFunc_count());
		else if (type == STORE_CONFIG)
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
			else if (type == STORE_CONFIG)
				xmlNodeSetContent(node, BAD_CAST ltostr(w->zoomprop, c));
		} else if (strcmp((char *)node->name, XML_ZOOM_FACTOR) == 0) {
			if (type == LOAD_CONFIG)
				w->zoomfactor = CLAMP(xmlNodeContenttod(node), 0.0, 1.0);
			else if (type == STORE_CONFIG)
				xmlNodeSetContent(node, BAD_CAST ascii_dtostr(w->zoomfactor, c));
		} else if (strcmp((char *)node->name, XML_GET_J_ITERMAX) == 0) {
			if (type == LOAD_CONFIG)
				w->get_jitermax = CLAMP(xmlNodeContenttol(node), LOWITERMAX, HIITERMAX);
			else if (type == STORE_CONFIG)
				xmlNodeSetContent(node, BAD_CAST ltostr(w->get_jitermax, c));
		} else if (strcmp((char *)node->name, XML_THREADS_COUNT) == 0) {
			if (type == LOAD_CONFIG) 
				w->it_param.threads_count = CLAMP(xmlNodeContenttol(node), 1, MAXTHREADS);
			else if (type == STORE_CONFIG)
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
			/* renderdata: */
			retval = xmlrenderdata(node, &w->it_param, type);
		} else if (strcmp((char *)node->name, XML_SIZE) == 0) {
			/* size: */
			xmlsize(node->children, GTK_WINDOW(w->win), type);
		} else if (strcmp((char *)node->name, XML_PREFERENCE) == 0) {
			/* preference: */
			xmlpreference(node->children, w, type);
		}
	}
	return retval;
}

gboolean configure_interface(struct winctl *w, const gchar *configfile, enum configtype type)
{
	xmlDoc *doc = NULL;
	xmlNode *root_node;
	gint i;
	char c[BUFSIZE_NUMTOSTR];
	gboolean retval = FALSE;

	LIBXML_TEST_VERSION

	if (type == LOAD_CONFIG)
		init_config(w);

	if (!g_file_test(configfile, G_FILE_TEST_EXISTS)) {
		writexmlfile(configfile, w, SAVE_APPSETTINGS);
		if (type == STORE_CONFIG) 
			return TRUE;
	} else {
		doc = xmlReadFile(configfile, NULL, 0);
		if (doc != NULL && (root_node = xmlDocGetRootElement(doc)) != NULL) {
			/* setup the config-file */
			retval = setxmlconfig(root_node->children, w, type);

			if (type == STORE_CONFIG) {
				xmlSaveFormatFile(configfile, doc, 1);
				xmlFreeDoc(doc);
				xmlCleanupParser();
				return TRUE;
			} else {
				xmlFreeDoc(doc);
				xmlCleanupParser();
			}
		}
	}
	
	w->it_param.colorfunc = getColorFunc(w->it_param.color_func_index);
	for (i = 0; i < 4; ++i) {
		dtostr(w->it_param.cplxplane[i], c);
		gtk_entry_set_text(GTK_ENTRY(w->txtcplx[i]), c);
	}

	gtk_entry_set_text(GTK_ENTRY(w->txtitermax), ltostr(w->it_param.itermax, c));
	gtk_entry_set_text(GTK_ENTRY(w->txtdegree), trunc0(dtostr(w->it_param.degree, c)));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w->chkjulia), (w->it_param.type == JULIA_SET) ? TRUE : FALSE);
	gtk_entry_set_text(GTK_ENTRY(w->txtjre), dtostr(w->it_param.j[0], c));
	gtk_entry_set_text(GTK_ENTRY(w->txtjim), dtostr(w->it_param.j[1], c));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(w->mcolalgo[w->it_param.color_func_index]), TRUE);

	if (type == LOAD_CONFIG) {
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(w->mchkzoomprop), w->zoomprop);
		gtk_widget_modify_fg(w->drawing, GTK_STATE_NORMAL, &w->focus_color);

		iterate_param_init(&w->it_param, w->it_param.threads_count);
	}
	return retval;
}

void save_rendersettings_xml(const gchar *filename, struct winctl *w)
{
	writexmlfile(filename, w, SAVE_IMAGE);
}
