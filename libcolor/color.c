
#include "color.h"

#define COLORFUNC_COUNT 4
#define N_(x) x

const gchar *colorfunc_names[COLORFUNC_COUNT] = {
	N_("Blue constants"),
	N_("Convergent / Divergent Set"),
	N_("Dixius-Algo_1"),
	N_("Dixius-Algo_2")
};

/* color settings: */
const ColorFunc colorfunc[COLORFUNC_COUNT] = {
	clblue,
	divconv, /* Be careful that this position matches the DIVCONV_IDX define. */
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

ColorFunc getcolorfunc(int index)
{
	return colorfunc[index % COLORFUNC_COUNT];
}

int getdivconv_idx()
{
	return DIVCONV_IDX;
}
