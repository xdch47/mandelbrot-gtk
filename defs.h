 
#ifndef __DEF_H__
#define __DEF_H__

#include <gtk/gtk.h>

#define LVERSION "0.3"

// labels:
#define LWCAP _("Mandelbrot- / Juliaset")
#define LCAPM _(" Mandelbrotset [%0.2fs] ")
#define LCAPJ _("Juliaset [%0.2fs]")
#define LCAPMRENDER _("Rendering the Mandelbrotset..")
#define LCAPJRENDER _("Rendering the Juliaset..")
#define LCOORD _("Complex plane")
#define LFKT _("Renderdata")
#define LDEGREE _("Degree of Mandelbrotset:")
#define LITERMAX _("I_terationdepth")
#define LCHKJULIA _("_Juliaset:")
#define LJCONST _("Constante j")
#define LJCRE _("_Realpart:")
#define LJCIM _("_Imag.part:")
#define LGETJ _("Get j from the _Mandelbrotset")
#define LCALC _(" _Render ")
#define LSTOP _(" _Stop ")
#define LRESET _(" _Reset ")
#define LCLOSE _(" _Quit ")
#define LTHREADPAUSE _("The renderprocess was interrupted. Continue or restart process?")
#define LCONTDRAW _(" _Continue")
#define LREDRAW _("_Restart")
#define LCANCEL _("_Cancel")
const gchar *LCPLX[4];
#define TEXTLEN 20
#define BUFSIZE 200
#define NUMFORMAT "%.12f"
#define LFILE _("_File")
#define LPREFMENU _("_Option")
#define LZOOMPROP _("Retain aspect ratio")
#define LSETCPLX _("Adjust complex _plane to imagesize")
#define LSETCOLOR _("Set color")
#define LCOLORALGO _("_Color-Algorithm")
#define LPREF _("_Configuration")
#define LHELP _("?")
#define LCONVCOLOR _("_Convergent set")
#define LDIVCOLOR _("_Divergent set")
#define LCOLCAPCONV _("Color of the convergent set")
#define LCOLCAPDIV _("Color of the divergent set")
const gchar *LPJMENU[6];
const gdouble JDEFINES[12];
#define COLORFUNC 4
const gchar *LCOLOR[COLORFUNC];
const gdouble CPLXPLANE[4];
const gdouble MCPLXPLANE[4];

// save dialog:
#define LSAVECAP _("Save..")
#define LSAVEPATH _("Save as..")
#define LFILECHOOSER _("Save..")
#define LFILENAME _("_Filename:")
#define LRENDEROPT _("Render - Option")
#define LPICTOPT _("Imageoption")
#define LWIDTH _("Width (in Pixel):")
#define LHEIGTH _("Height (in Pixel):")
#define LRESTOREVAL _("Reload _values")
#define LSAVEBTN _("_Save")
#define LSAVECLOSE _("_Close")
#define LSSETCPLX _("_Adjust complexplane")
#define LSAVELABEL _("Please wait..\nRendering image...")
#define LSAVELABELDONE _("Saved Image to file.")
#define LSAVELABELCANCEL _("Rendering was aborted by user.")
#define LSAVEDONE _("_OK")
#define LSAVECANCEL _("Abort rendering?")
#define LSAVEERROR _("Error: Can't write file \"%s\".")
#define LDIRERR _("Directory doesn't exist.")
#define LPERMERR _("Can't save file. Permission denied.")
#define LFILEERR _("You must enter a valid filename.")
#define LFILEEXISTS _("The File \"%s\" already exists. Overwrite?")

// preference dialog:
#define LCPREF _("Option")
#define LPREFSTDCPLX _("Complex Plane (Std)")
#define LPREFLSTDCPLX _("Complex Plane\n(Reset-Werte)")
#define LPREFSTDMCPLX _("Complex Plane (Mb)")
#define LPREFLSTDMCPLX _("Complex Plane of the Mandelbortset: ")
#define LPREFRESET _("_Reset")
#define LPREFDEFAULT _("_Default Values")
#define LPREFMISC _("Miscellaneous")
#define LPREFZOOMFACTOR _("Zoomfactor")
#define LPREFLZOOM _("Zoomfactor in percent:")
#define LPREFTHREADS _("Threads")
#define LPREFTHREADSCOUNT _("Number of Render-Threads:")
#define LPREFITER _("Iterationdepth")
#define LPREFLITER _("Iterationdepth of \"Get j from the Mandelbrotset\":")
#define LPREFFOCUSCOLOR _("Focus color")
#define LPREFLFOCUSCOLOR _("Color of zoom-rectangle:")
#define LPREFOK _("_OK")
#define LPREFCANCEL _("_Cancel")

// response:
#define RESPONSE_CONTDRAW 1
#define RESPONSE_REDRAW 2
#define RESPONSE_CANCEL 3

// errors:
#define LOWITERMAX 10
#define HIITERMAX 100000
#define MAXTHREADS 512
#define LOWDEGREE -100.0
#define HIDEGREE 100.0
#define ERRVAL _("\"%s\" is not a valid Number")
#define ERRREMAXMIN _("The minimum realpart must be smaller than the maximum realpart.")
#define ERRIMMAXMIN _("The minimum imaginarypart must be smaller than the maximum imaginarypart.")
#define ERRITERMAX _("Please enter a valid number for the iterationdepth. (Between %d and %d)")
#define ERRDEGREE _("Please enter a valid number for the degree of the Mandelbrotset. (Between %f and %f)")

// config file
//#define CONFIGDIR "mandelbrot"
#define CONFIGFILE "mandelbrot.xml"

#define INTERPOLATION GDK_INTERP_BILINEAR

#endif /* __DEF_H__ */

