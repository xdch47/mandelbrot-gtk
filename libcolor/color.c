
#include "color.h"

#define COLORFUNC_COUNT 3
#define N_(x) x

static int last_func = -1;

const gchar *colorfunc_names[COLORFUNC_COUNT] = {
	N_("Blue constants"),
	N_("Convergent / Divergent Set"),
	N_("Dixius Algo Grey"),
};

/* color settings: */
const ColorFunc colorfunc[COLORFUNC_COUNT] = {
	clblue,
	divconv, /* Be careful that this position matches the DIVCONV_IDX define. */
	mb_color_standard_sw,
};

#define DIVCONV_IDX 1

int getcolorfunc_count()
{
	return COLORFUNC_COUNT;
}

const char *getcolorfunc_name(int index)
{
	return colorfunc_names[index % COLORFUNC_COUNT];
}

void initialize_func(int index)
{

	last_func = index;
}

void finalize_func(int index)
{

}

ColorFunc getcolorfunc(int index)
{
	return colorfunc[index % COLORFUNC_COUNT];
}

int getdivconv_idx()
{
	return DIVCONV_IDX;
}
