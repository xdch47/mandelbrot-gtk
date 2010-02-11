 
#ifndef __DEF_H__
#define __DEF_H__

#include <gtk/gtk.h>

// labels:
#define LWCAP "Mandelbrot- / Juliamenge"
#define LCOORD "Komplexe Zahlenebene"
#define LFKT "Funktionsdaten"
#define LDEGREE "Mandelbrotmenge der Ordnung:"
#define LITERMAX "I_terationstiefe"
#define LCHKJULIA "_Julia Menge:"
#define LJCONST "Konstante j"
#define LJCRE "_Realteil:"
#define LJCIM "_Imag.teil:"
#define LGETJ "j von der _Mandelbrotmenge"
#define LITER "Anzahl der Iterationen"
#define LCALC " _Zeichnen "
#define LSTOP " _Stop "
#define LRESET " R_eset "
#define LCLOSE " _Beenden "
#define LTHREADPAUSE "Der Renderprozess würde gestoppt. Soll er fortgesetzt oder neugestarted werden?"
#define LCONTDRAW " _Fortsetzen"
#define LREDRAW "_Neustart"
#define LCANCEL "_Abbrechen"
const gchar *LCPLX[4];
#define TEXTLEN 20
#define BUFSIZE 200
#define NUMFORMAT "%.12f"
#define LFILE "_Datei"
#define LOPTION "_Option"
#define LXYSCALE "Seitenverhältnis beibehalten"
#define LSETCPLX "_Komplex Ebene an Bildgröße anpassen"
#define LSETCOLOR "F_arben für"
#define LCOLORALGO "Aktueller _Farb-Algorithmus"
#define LPREF "_Einstellungen..."
#define LHELP "?"
#define LCONVCOLOR "_Konvergente Menge"
#define LDIVCOLOR "_Divergente Menge"
#define LCOLCAPCONV "Farben für die konvergente Menge"
#define LCOLCAPDIV "Farben für die divergente Menge"
const gchar *LPJMENU[6];
const gdouble JDEFINES[12];
#define COLORFUNC 2
const gchar *LCOLOR[COLORFUNC];
const gdouble CPLXPLANE[4];
const gdouble MCPLXPLANE[4];
// save:
#define LSAVECAP "Speichern.."
#define LSAVEPATH "Speichern unter.."
#define LFILECHOOSER "Speichern.."
#define LFILENAME "_Dateiname:"
#define LRENDEROPT "Render - Optionen"
#define LPICTOPT "Bildoptionen"
#define LWIDTH "Breite (in Pixel):"
#define LHEIGTH "Höhe (in Pixel):"
#define LRESTOREVAL "Aktuelle _Werte"
#define LSAVEBTN "_Speichern"
#define LSSETCPLX "_Zahleneb. anpassen"

// response:
#define RESPONSE_CONTDRAW 1
#define RESPONSE_REDRAW 2
#define RESPONSE_CANCEL 3

// errors:
#define LOWITERMAX 10
#define HIITERMAX 99999
#define LOWDEGREE -100.0
#define HIDEGREE 100.0
#define ERRVAL "\"%s\" ist kein gültiger Wert eingeben!"
#define ERRREMAXMIN "Der Min-Realteil muss kleiner als der Max-Realteil sein."
#define ERRIMMAXMIN "Der Min-Imaginärteil muss kleiner als der Max-Imaginärteil sein."
#define ERRITERMAX "Gegen sie einen gültigen Wert für maximale Anzahle der Iterationen ein. (Zw. %d und %d)"
#define ERRDEGREE "Gegen sie einen gültigen Wert für die Ordnung der Mandelbrotmenge. (Zw. %f und %f)"

#define DEFITERMAX "100"

// config file
#define CONFIGFILE ".config"
#define CONFGROUPAPP "data"
#define CONFCPLXPLANE "complex-plane"
#define CONFITERMAX "max-iterations"
#define CONFDEG "degree"
#define CONFJ "calculate_juliaset"
#define CONFJRE "repart_j"
#define CONFJIM "impart_j"
#define CONFGROUPSIZE "size"
#define CONFWIDTH "width"
#define CONFHEIGHT "height"
#define CONFGROUPPREF "preference"
#define CONFXYSCALE "xyscale"
#define CONFCOLORALGO "color-algorithmen"
#define CONFCOLORCONV "convergent-color"
#define CONFCOLORDIV "divergent-color"

// threads
#define CALCTHREADS 4
#define REDRAWTIME 100
#define THREAD_PAUSE 1
#define THREAD_TERMINATE 2
const guint XSTART[CALCTHREADS];
const guint YSTART[CALCTHREADS];
const guint XOFFSET[CALCTHREADS];
const guint YOFFSET[CALCTHREADS];

#define INTERPOLATION GDK_INTERP_BILINEAR

// Doubleclick scale-factor
#define SCALEFACTOR 0.65

#endif /* __DEF_H__ */

