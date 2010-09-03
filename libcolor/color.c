
#include "color.h"
#include "colormap.h"

#define COLORFUNC_COUNT 4
#define N_(x) x

static int last_func = -1;

const gchar *colorfunc_names[COLORFUNC_COUNT] = {
	N_("Blue constants"),
	N_("Convergent / Divergent Set"),
	N_("Dixius Algo Grey"),
	N_("RBG-S5"),
};

/* color settings: */
/* Be careful that position matchen the **_IDX defines. */
const ColorFunc colorfunc[COLORFUNC_COUNT] = {
	clblue,
	divconv,
	mb_color_standard_sw, 
	clRGBS5,
};

#define DIVCONV_IDX 1
#define MB_COLOR_STANDARD_SW_IDX 2
#define RGBS5_IDX 3

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
	#define eqneq(x,y,z) (x == z && y != z)
	#define switchto(x) eqneq(index, last_func, x) 
	#define leave(x) eqneq(last_func, index, x)

	if (last_func == -1)
		initialize_colormap_mutexes();

	if (switchto(MB_COLOR_STANDARD_SW_IDX) || switchto(RGBS5_IDX)) {
		initialize_colormap();
	} else if (leave(MB_COLOR_STANDARD_SW_IDX) || leave(RGBS5_IDX)) {
		finalize_colormap();
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
