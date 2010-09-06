
#include "color.h"
#include <math.h>
#include "lookuptable.h"

#define N_(x) x

static int last_func = -1;
guint itermax;
static LookUpDescriptor *color_ltb = NULL;

#define EXPORT_LOOKUP_ALGO(name)                                                       \
        void name(const IterationInfo *iterinfo, guchar *pixel)                         \
        {                                                                              \
		LookUpItem item; \
		item.key = (void *)iterinfo; \
                cpRGB(pixel, (guchar *)(ltb_lookup(color_ltb, &item, (void *(*)(void *))name ## _algo))->value); \
        }

/* color value for the divergent color */
static guchar divcol[3];

static const guchar clbluedef[3 * 48] = {
	  0,   0,   0,
	  0,   0,  31,
	  0,   0,  63,
	  0,   0,  95,
	  0,   0, 127,
	  0,   0, 159,
	  0,   0, 191,
	  0,   0, 223,
	  0,   0, 255,
	  0,  31, 255,
	  0,  63, 255,
	  0,  95, 255,
	  0, 127, 255,
	  0, 159, 255,
	  0, 191, 255,
	  0, 223, 255,
	  0, 255, 255,
	 31, 255, 255,
	 63, 255, 255,
	 95, 255, 255,
	127, 255, 255,
	159, 255, 255,
	191, 255, 255,
	223, 255, 255,
	255, 255, 255,
	223, 255, 255,
	191, 255, 255,
	159, 255, 255,
	127, 255, 255,
	 95, 255, 255,
	 63, 255, 255,
	 31, 255, 255,
	  0, 255, 255,
	  0, 223, 255,
	  0, 191, 255,
	  0, 159, 255,
	  0, 127, 255,
	  0,  95, 255,
	  0,  63, 255,
	  0,  31, 255,
	  0,   0, 255,
	  0,   0, 223,
	  0,   0, 191,
	  0,   0, 159,
	  0,   0, 127,
	  0,   0,  95,
	  0,   0,  63,
	  0,   0,  31,
};

static int cmpIterMax(LookUpItem *it1, LookUpItem *it2)
{
	if (((IterationInfo *)it1->key)->iter < ((IterationInfo *)it2->key)->iter)
		return -1;
	else if (((IterationInfo *)it1->key)->iter == ((IterationInfo *)it2->key)->iter)
		return 0;
	else
		return 1;
}

void clblue(const IterationInfo *iterinfo, guchar *pixel)
{
	cpRGB(pixel, (clbluedef + (iterinfo->iter % 48) * 3));
}

void setDivergentColor(const guchar *color)
{
	cpRGB(divcol, color);
}

void divconv(const IterationInfo *iterinfo, guchar *pixel)
{
	cpRGB(pixel, divcol);
}

void initialize_lookuptable()
{
	color_ltb = ltb_new((cmpfunc)cmpIterMax);
}

void finalize_lookuptable()
{
	ltb_free(color_ltb);
	color_ltb = NULL;
}

static const guchar *mb_color_standard_sw_algo(const IterationInfo *iterinfo)
{
	#define COLOR_PERIOD_DEFAULT 40.0
	long double x;
	guchar *retval;
	guchar res;

	retval = (guchar *)malloc(sizeof(guchar));
	x = sqrt((long double)iterinfo->iter) / sqrt(COLOR_PERIOD_DEFAULT);
	x = 0.5 + 0.5 * cos(x);
	res = (guchar)(255 * x);
	retval[0] = res;
	retval[1] = res;
	retval[2] = res;
	return retval;
}
EXPORT_LOOKUP_ALGO(mb_color_standard_sw)

static const guchar *clRGBS5_algo(const IterationInfo *iterinfo)
{
	guchar cl;
	guchar cl_case;
	static const guint step1 = 25;
	static const guint step2 = 50;
	static const guint step3 = 100;
	static const guint step4 = 200;
	guchar *color = (guchar *)malloc(sizeof(guchar) * 3);
	guint length;
	guint iter = iterinfo->iter;

	length = 2 * step4;

	if (iter < length) {
		cl_case = iter;
	} else {
		cl_case = ((guint)(log(iter) / log(2))) % length;
	}
	if (cl_case < step1) {
		cl = (iter * (255 / step1)) % 255;
		color[0] = 0;
		color[1] = 0;
		color[2] = cl;
	} else if (cl_case < step2) {
		cl = (155 - (iter * (155 / (step2 - step1))) % 155) + 100;
		color[0] = 0;
		color[1] = cl;
		color[2] = cl;
	} else if (cl_case < step3) {
		cl = ((iter * (200 / (step3 - step2))) % 200) + 55;
		color[0] = 0;
		color[1] = cl;
		color[2] = 0;
	} else if (cl_case < step4) {
		cl = (200 - (iter * (200 / (step4 - step3))) % 200) + 55;
		color[0] = cl;
		color[1] = cl;
		color[2] = 0;
	} else {
		cl = (iter * 10) % 100 + 155;
		color[0] = 0xff;
		color[1] = cl;
		color[2] = 0;
	}
	return color;
} 
EXPORT_LOOKUP_ALGO(clRGBS5)

/* Function to access the color-algorithms based on fractINT map-files */
int getColorMapFunc_count();
const char *getColorMapFunc_name(int index);
ColorFunc getColorMapFunc(int index);

/* color settings: */
/* Be careful that position matchen the **_IDX defines. */
#define COLORFUNC_COUNT 4
const ColorFuncDescriptor CFD[COLORFUNC_COUNT] = {
	{ N_("Blue constants")            , NULL                  , NULL                , clblue }              ,
	#define DIVCONV_IDX 1
	{ N_("Convergent / Divergent Set"), NULL                  , NULL                , divconv }             ,
	{ N_("Dixius Algo Grey")          , initialize_lookuptable, finalize_lookuptable, mb_color_standard_sw },
	{ N_("RBG-S5")                    , initialize_lookuptable, finalize_lookuptable, clRGBS5 }             , 
};

int getColorFunc_count()
{
	return COLORFUNC_COUNT + getColorMapFunc_count();
}

const char *getColorFunc_name(int index)
{
	index = index % getColorFunc_count();
	if (index < COLORFUNC_COUNT) 
		return CFD[index].name;
	else
		return getColorMapFunc_name(index - COLORFUNC_COUNT);
}

void initialize_func(int index)
{
	index = index % getColorFunc_count();
	if (index >= COLORFUNC_COUNT) 
		index = index - COLORFUNC_COUNT;

	if (last_func == -1) {
		initialize_lookuptable_mutexes();

		if (CFD[index % COLORFUNC_COUNT].initialize_func) {
			(*CFD[index % COLORFUNC_COUNT].initialize_func)();
		}
	}
	else if (last_func != index) {
		if (CFD[last_func % COLORFUNC_COUNT].finalize_func) {
			(*CFD[last_func % COLORFUNC_COUNT].finalize_func)();
		}
		if (CFD[index % COLORFUNC_COUNT].initialize_func) {
			(*CFD[index % COLORFUNC_COUNT].initialize_func)();
		}
	}

	last_func = index;
}

ColorFunc getColorFunc(int index)
{
	index = index % getColorFunc_count();
	if (index < COLORFUNC_COUNT)
		return CFD[index % COLORFUNC_COUNT].colorfunc;
	else
		return getColorMapFunc(index - COLORFUNC_COUNT);
}

int getDivConv_idx()
{
	return DIVCONV_IDX;
}

void cpRGB(guchar *dest, const guchar *src)
{
	*dest++ = *src++;
	*dest++ = *src++;
	*dest = *src;
}
