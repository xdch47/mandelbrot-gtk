 
#ifndef __DEF_H__
#define __DEF_H__

#include <gtk/gtk.h>

#define LVERSION "0.3"

#ifndef MO_DIR
	#define MO_DIR g_path_get_dirname(*argv)
#endif
#ifndef PACKAGE
	#define PACKAGE "mandelbrot"
#endif

/* config-file location */
/* this string is g_free()d in interface.c */
#ifdef G_OS_UNIX
	#define CONFIGDIR g_strdup(g_get_user_config_dir())
	/* #define CONFIGDIR s_build_filename(g_get_user_config_dir(), "subdir", NULL) */
#else
  #define CONFIGDIR g_path_get_dirname(file_name)
#endif
#define CONFIGFILE "mandelbrot.xml"

/* labels: */
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
#define LTHREADPAUSE _("The renderprocess has been interrupted. Continue or restart process?")
#define LCONTDRAW _(" _Continue")
#define LREDRAW _("_Restart")
#define LCANCEL _("_Cancel")
extern const gchar *LCPLX[4];
#define TEXTLEN 20
#define BUFSIZE 200
#define NUMFORMAT "%.12f"
#define LFILE _("_File")
#define LSAVEXML _("Save as _XML")
#define LFILTERXMLNAME _("xml-Files")
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
extern const gchar *LPJMENU[6];
extern const gdouble JDEFINES[12];
extern const gdouble CPLXPLANE[4];
extern const gdouble MCPLXPLANE[4];

/* save dialog: */
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

/* preference dialog: */
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

/* response: */
#define RESPONSE_CONTDRAW 1
#define RESPONSE_REDRAW 2
#define RESPONSE_CANCEL 3

/* errors: */
#define LOWITERMAX 10
#define HIITERMAX 1000000
#define MAXTHREADS 512
#define LOWDEGREE -100.0
#define HIDEGREE 100.0
#define ERRVAL _("\"%s\" is not a valid Number")
#define ERRREMAXMIN _("The minimum realpart must be smaller than the maximum realpart.")
#define ERRIMMAXMIN _("The minimum imaginarypart must be smaller than the maximum imaginarypart.")
#define ERRITERMAX _("Please enter a valid number for the iterationdepth. (Between %d and %d)")
#define ERRDEGREE _("Please enter a valid number for the degree of the Mandelbrotset. (Between %f and %f)")

/* config-file with comments: */
#define XML_COMMENTS

/* default values: */
#define ZOOMFACTOR 0.65
#define DEFITERMAX 100
#define CALCTHREADS 4

extern const guchar clbluedef[];
#define INTERPOLATION GDK_INTERP_BILINEAR

#define GPLv3                                                                     \
"Mandelbrot v" LVERSION "\n"                                                      \
"Copyright (C) 2010 Lexif Systems, Inc.\n"                                        \
"License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n" \
"This program is free software; you can redistribute it and/or modify\n"          \
"it under the terms of the GNU General Public License as published by\n"          \
"the Free Software Foundation; either version 3 of the License, or\n"             \
"any later version.\n"                                                            \
"There is NO WARRANTY, to the extent permitted by law.\n"

#endif /* __DEF_H__ */

