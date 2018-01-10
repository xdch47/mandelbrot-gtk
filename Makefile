
#===============================================================================
#
#     Filename: Makefile
#  Description:
#
#        Usage: make              (generate executable                      )
#               make depend       (generate dependencies of the objects     )
#               make clean        (remove objects, executable, prerequisits )
#               make tarball      (generate compressed archive              )
#               make zip          (generate compressed archive              )
#
#===============================================================================

include config.mk

#QUIET can be set to yes in order to suppress commands

# DEBUG can be set to YES to include debugging info, or NO otherwise
DEBUG           := NO

#------------  start application with params -----------------------------------
PARAMS          :=

# ------------  name of the executable  ----------------------------------------
APPNAME         := mandelbrot-gtk

# ------------  list of all source files  --------------------------------------
SOURCES         := main.c defs.c interface.c layout.c config.c \
                   render.c iterate.c storedrawing.c validate.c pref.c

# ------------  compiler  ------------------------------------------------------
CC               := gcc

# ------------  archiver  ------------------------------------------------------
#AR              := xiar

CFLAGS          ?= -O2

# ------------  compiler flags  ------------------------------------------------
CFLAGS_G_DISABLE_SINGLE_INCLUDES = -DG_DISABLE_SINGLE_INCLUDES -DGDK_PIXBUF_DISABLE_SINGLE_INCLUDES -DGTK_DISABLE_SINGLE_INCLUDES
#CFLAGS_G_DISABLE_DEPRECATED = -DG_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED
CFLAGS_G_DISABLE_DEPRECATED = -DG_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED
DEBUG_CFLAGS    := -Wall -O0 -g -DMO_DIR="\"./mo\"" ${CFLAGS_G_DISABLE_SINGLE_INCLUDES} ${CFLAGS_G_DISABLE_DEPRECATED}
RELEASE_CFLAGS  := ${CFLAGS} -pipe \
				   -DNDEBUG -DPACKAGE="\"${APPNAME}\"" -DMO_DIR="\"${MO_DIR}\""

#-------------  Directories  ---------------------------------------------------
DEBUG_DIR       := ./debug
RELEASE_DIR     := ./release
DESTDIR         := 

# ------------  linker flags  --------------------------------------------------
LDFLAGS         ?= 

ifeq (YES, ${DEBUG})
CFLAGS          := ${DEBUG_CFLAGS}
OUT_DIR         := ${DEBUG_DIR}
QUIET           := NO
else
CFLAGS          := ${RELEASE_CFLAGS}
OUT_DIR         := ${RELEASE_DIR}
QUIET           := YES
endif

ifneq (YES, ${SHARED})
STATIC=-Wl,-Bstatic
endif

ifeq (YES, ${QUIET})
	Q := @
else
	Q := 
endif

# ------------  additional pkg-config includes and libraries  ------------------
PKG_CONF_ARG    = gtk+-3.0 libxml-2.0

# ------------  additional include directories  --------------------------------
INC_DIR  =

# ------------  additional library directories  --------------------------------
LIB_DIR  = ./libcolor

# ------------  additional libraries  ------------------------------------------
LIBS     = ${STATIC} -lmbcolor -Wl,-Bdynamic -lm

# ------------  archive generation ---------------------------------------------
TARBALL_EXCLUDE = *.{o,gz,zip}
ZIP_EXCLUDE     = *.{o,gz,zip}

# ------------  cmd line parameters for this executable  -----------------------

ifdef PKG_CONF_ARG
INC_DIR_PKG_CONF = $(shell pkg-config --cflags $(PKG_CONF_ARG))
LIBS_PKG_CONF    = $(shell pkg-config --libs $(PKG_CONF_ARG))
endif


#===============================================================================
# The following statements usually need not to be changed
#===============================================================================
C_SOURCES       = ${SOURCES}
ALL_INC_DIR     = $(addprefix -I, ${INC_DIR}) ${INC_DIR_PKG_CONF}
ALL_LIB_DIR     = $(addprefix -L, ${LIB_DIR})
ALL_CFLAGS      = ${CFLAGS} ${ALL_INC_DIR}
ALL_LFLAGS      = ${LDFLAGS} ${ALL_LIB_DIR}
BASENAMES       = $(basename ${C_SOURCES})

ifeq (YES, ${SHARED})
ifdef RPATH
ALL_LFLAGS += -Wl,-rpath,${PREFIX}/lib/${RPATH}
endif
endif

# ------------  generate the names of the object files  ------------------------
OBJECTS         = $(addprefix ${OUT_DIR}/, $(addsuffix .o,${BASENAMES}))

# ------------  execute the executable  ----------------------------------------
all: ${OUT_DIR}/${APPNAME}

run: ${OUT_DIR}/${APPNAME}
ifeq (YES, ${DEBUG})
	$Qgdb ${OUT_DIR}/${APPNAME} ${PARAMS}
else
	$Q${OUT_DIR}/${APPNAME} ${PARAMS}
endif

# ------------  make the executable  -------------------------------------------
${OUT_DIR}/${APPNAME}: ${OUT_DIR} ${OBJECTS}
	$Q${MAKE} -C ./libcolor CC=${CC} AR=${AR} CFLAGS="${CFLAGS}" DEBUG=${DEBUG} SHARED=${SHARED} \
		DESTDIR=${DESTDIR} PREFIX=${PREFIX} RPATH=${RPATH}
ifeq (YES, ${QUIET})
	$Qecho "Linking ${APPNAME}..."
endif
	$Q${CC} -o ${OUT_DIR}/${APPNAME} ${OBJECTS} \
		${ALL_LFLAGS} ${ALL_LIB_DIR} ${LIBS} ${LIBS_PKG_CONF}

#-------------  Create the directories -----------------------------------------
${OUT_DIR}:
	$Qmkdir -p ${OUT_DIR}

# ------------  make the objects  ----------------------------------------------
${OUT_DIR}/%.o:	%.c
ifeq (YES, ${QUIET})
	$Qecho "Compling $<..."
	$Q${CC}  -c ${ALL_CFLAGS} $< -o $@ 2> /dev/null
else
	$Q${CC}  -c ${ALL_CFLAGS} $< -o $@
endif

# ------------  remove generated files  ----------------------------------------
clean:
	$Qrm -rf ${DEBUG_DIR} ${RELEASE_DIR}
	$Qrm -f ./i18n/*.mo
	$Q+make -C ./libcolor clean

# ------------  tarball generation  --------------------------------------------
tarball:
	$Qlokaldir=`pwd`; lokaldir=$${lokaldir##*/}; \
		rm --force $$lokaldir.tar.gz;               \
		tar --exclude=${TARBALL_EXCLUDE}            \
		--create                                    \
		--gzip                                      \
		--verbose                                   \
		--file  $$lokaldir.tar.gz *

# ------------  zip  -----------------------------------------------------------
zip:
	$Qlokaldir=`pwd`; lokaldir=$${lokaldir##*/}; \
		zip -r  $$lokaldir.zip * -x ${ZIP_EXCLUDE}


LOCALE_DIR := ${DESTDIR}${MO_DIR}/$(basename ${POFILE} .po)/LC_MESSAGES
LANGUAGES  := $(notdir $(basename $(wildcard i18n/*.po) .po))
MO_FILES   := $(addsuffix .mo, $(addprefix ./i18n/, ${LANGUAGES}))

# ------------  installl  ------------------------------------------------------
i18n/%.mo: i18n/%.po
	$Qmkdir -p ./mo
	$Qmsgfmt $< -o $@

install: ${OUT_DIR}/${APPNAME} ${MO_FILES}
ifeq (YES, ${QUIET})
	$Qecho "Installing application..."
endif
	$Qinstall -m755 -D  ${RELEASE_DIR}/${APPNAME}        ${DESTDIR}${PREFIX}/bin/${APPNAME}
	$Qfor lang in ${LANGUAGES} ; do \
		install -m644 -D ./i18n/$${lang}.mo              ${DESTDIR}${PREFIX}/share/locale/$$lang/LC_MESSAGES/${APPNAME}.mo ; \
	done
	$Qinstall -m644 -D  ./${APPNAME}.xml                 ${DESTDIR}${DOC_DIR}/${APPNAME}/${APPNAME}.xml
	$Qinstall -m644 -D ./applications/${APPNAME}.desktop ${DESTDIR}${PREFIX}/share/applications/${APPNAME}.desktop
	$Qinstall -m644 -D ./pixmaps/${APPNAME}.png          ${DESTDIR}${PREFIX}/share/pixmaps/${APPNAME}.png
	$Q${MAKE} -C ./libcolor DEBUG=${DEBUG} SHARED=${SHARED} \
		DESTDIR=${DESTDIR} PREFIX=${PREFIX} RPATH=${RPATH} install

# ------------  uninstall  -----------------------------------------------------
uninstall:
	for POFILE in ./i18n/*.po ; do  \
		LOCALE_DIR=${DESTDIR}${MO_DIR}/$${basename $$POFILE .po}/LC_MESSAGES ; \
		rm -rf  $$LOCALE_DIR/${APPNAME}.mo ; \
	done
	rm -rf ${DESTDIR}${PREFIX}/bin/${APPNAME}
	rm -rf ${DESTDIR}${DOC_DIR}/${APPNAME}
	${MAKE} -C ./libcolor DEBUG=${DEBUG} SHARED=${SHARED} \
		DESTDIR=${DESTDIR} PREFIX=${PREFIX} RPATH=${RPATH} uninstall

# ------------  xgettext  ------------------------------------------------------
xgettext:
	xgettext -k_ -kN_ -LC --no-location --omit-header -o ./i18n/messages.pot *.c *.h \
		./libcolor/*.h ./libcolor/*.c

#-------------  create dependencies  -------------------------------------------
depend:
	$Qgcc -MM -E  ${SOURCES} | awk '{ printf("${OUT_DIR}/%s\n", $$0); }' > ./.depend

-include .depend

.PHONY: all clean tarball zip depend install uninstall xgettext depend
