 
#ifndef __DEF_H__
#define __DEF_H__

#include <gtk/gtk.h>

#define LVERSION "0.2-rc1"

// labels:
#define LWCAP "Mandelbrot- / Juliamenge"
#define LCAPM " Mandelbrotmenge [%0.2fs] "
#define LCAPJ "Juliamenge [%0.2fs]"
#define LCAPMRENDER "Mandelbrotmenge wird gerendert.."
#define LCAPJRENDER "Juliamenge wird gerendert.."
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
#define LRESET " _Reset "
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
#define LPREFMENU "_Einstellungen"
#define LZOOMPROP "Seitenverhältnis beibehalten"
#define LSETCPLX "Komplex _Ebene an Bildgröße anpassen"
#define LSETCOLOR "F_arben für"
#define LCOLORALGO "Aktueller _Farb-Algorithmus"
#define LPREF "_Konfiguration"
#define LHELP "?"
#define LCONVCOLOR "_Konvergente Menge"
#define LDIVCOLOR "_Divergente Menge"
#define LCOLCAPCONV "Farben für die konvergente Menge"
#define LCOLCAPDIV "Farben für die divergente Menge"
const gchar *LPJMENU[6];
const gdouble JDEFINES[12];
#define COLORFUNC 4
const gchar *LCOLOR[COLORFUNC];
const gdouble CPLXPLANE[4];
const gdouble MCPLXPLANE[4];

// save dialog:
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
#define LSAVECLOSE "S_chließen"
#define LSSETCPLX "_Zahleneb. anpassen"
#define LSAVELABEL "Bitte warten..\nGrafik wird gerendert..."
#define LSAVELABELDONE "Grafik wurde gespeichert."
#define LSAVELABELCANCEL "Rendervorgang abgebrochen."
#define LSAVEDONE "_OK"
#define LSAVECANCEL "Sind sie sicher, dass sie das Speicher abbrechen wollen?"
#define LSAVEERROR "Error: Die Datei \"%s\" konnte nicht zum Schreiben geöffnet werden"
#define LDIRERR "Das angegeben Verzeichnis existiert nicht!"
#define LPERMERR "Sie haben nicht die notwendigen Rechte um die Datei im angegebenen Verzeichnis ablegen zu können!"
#define LFILEERR "Sie müssen eine Dateiname angeben."
#define LFILEEXISTS "Die Datei \"%s\" existiert bereits. Sind sie sicher, dass sie die Datei überschreiben wollen?"

// preference dialog:
#define LCPREF "Einstellungen"
#define LPREFSTDCPLX "Komplexe Eb. (Std)"
#define LPREFLSTDCPLX "Standardeinstellung für die Komplexe Zahlenebene:\n(Reset-Werte)"
#define LPREFSTDMCPLX "Komplexe Eb. (Mb)"
#define LPREFLSTDMCPLX "Standardeinstellung für die Komplexe Zahlenebene\nder Mandelbortmenge (Nur 2te Ordnung): "
#define LPREFRESET "_Zurücksetzen"
#define LPREFDEFAULT "_Defaultwerte"
#define LPREFMISC "Verschiedenes"
#define LPREFZOOMFACTOR "Zoomfaktor"
#define LPREFLZOOM "Zoomfaktor in Prozent:"
#define LPREFTHREADS "Threads"
#define LPREFTHREADSCOUNT "Anzahl Render-Threads:"
#define LPREFITER "Iterationstief"
#define LPREFLITER "Iterationstiefe bei \"j von der Mandelbrotmenge\":"
#define LPREFFOCUSCOLOR "Focus-Farbe"
#define LPREFLFOCUSCOLOR "Farbe des Zoom-Vierecks:"
#define LPREFOK "_OK"
#define LPREFCANCEL "_Abbrechen"

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
#define ERRVAL "\"%s\" ist kein gültiger Wert eingeben!"
#define ERRREMAXMIN "Der Min-Realteil muss kleiner als der Max-Realteil sein."
#define ERRIMMAXMIN "Der Min-Imaginärteil muss kleiner als der Max-Imaginärteil sein."
#define ERRITERMAX "Gegen sie einen gültigen Wert für maximale Anzahle der Iterationen ein. (Zw. %d und %d)"
#define ERRDEGREE "Gegen sie einen gültigen Wert für die Ordnung der Mandelbrotmenge. (Zw. %f und %f)"


// config file
#define CONFIGDIR "mandelbrot"
#define CONFIGFILE "config.xml"

#define INTERPOLATION GDK_INTERP_BILINEAR

#endif /* __DEF_H__ */

