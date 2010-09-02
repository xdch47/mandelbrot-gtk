
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
/* Be careful that position matchen the **_IDX defines. */
const ColorFunc colorfunc[COLORFUNC_COUNT] = {
	clblue,
	divconv,
	mb_color_standard_sw, 
};

#define DIVCONV_IDX 1
#define MB_COLOR_STANDARD_SW 2


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
	if (index == MB_COLOR_STANDARD_SW && last_func != MB_COLOR_STANDARD_SW) {
		init_mb_color_standard_sw();
	} else if (last_func == MB_COLOR_STANDARD_SW && index != MB_COLOR_STANDARD_SW) {
		final_mb_color_standard_sw();
	}
	
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
