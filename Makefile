
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

# DEBUG can be set to YES to include debugging info, or NO otherwise
DEBUG           := YES

# ------------  run executable out of this Makefile  (yes/no)  -----------------
START           := NO

#------------  start application with params -----------------------------------
PARAMS          :=

# ------------  name of the executable  ----------------------------------------
EXECUTABLE      := mandelbrot

# ------------  list of all source files  --------------------------------------
SOURCES         := main.c defs.c interface.c layout.c config.c \
	render.c iterate.c storedrawing.c validate.c pref.c

# ------------  compiler  ------------------------------------------------------
CC               := gcc
#CC               := icc

# ------------  archiver  ------------------------------------------------------
#AR              := xiar

# ------------  compiler flags  ------------------------------------------------
CFLAGS_G_DISABLE_SINGLE_INCLUDES = -DG_DISABLE_SINGLE_INCLUDES -DGDK_PIXBUF_DISABLE_SINGLE_INCLUDES -DGTK_DISABLE_SINGLE_INCLUDES
CFLAGS_G_DISABLE_DEPRECATED = -DG_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED
DEBUG_CFLAGS    := -Wall -O0 -g ${CFLAGS_G_DISABLE_SINGLE_INCLUDES} ${CFLAGS_G_DISABLE_DEPRECATED}
RELEASE_CFLAGS  := -O2 -pipe -mtune=native -march=native \
				   -DNDEBUG -DPACKAGE="\"$(EXECUTABLE)\"" -DMO_DIR="\"$(MO_DIR)\"" \
#RELEASE_CFLAGS  := -w0 -ipo -O3 -mtune=core2 -xSSE4.1 -axSSE4.1 -unroll-agressiv -static-intel \
#				    -DNDEBUG -DPACKAGE="\"$(EXECUTABLE)\"" -DMO_DIR="\"$(MO_DIR)\""

#-------------  Directories  ---------------------------------------------------
DEBUG_DIR       := ./debug
RELEASE_DIR     := ./release

# ------------  linker flags  --------------------------------------------------
DEBUG_LDFLAGS   := -g
RELEASE_LDFLAGS :=

ifeq (YES, ${DEBUG})
CFLAGS          := ${DEBUG_CFLAGS}
LDFLAGS         := ${DEBUG_LDFLAGS}
OUT_DIR         := ${DEBUG_DIR}
else
CFLAGS          := ${RELEASE_CFLAGS}
LDFLAGS         := ${RELEASE_LDFLAGS}
OUT_DIR         := ${RELEASE_DIR}
endif

ifneq (YES, ${SHARED})
STATIC=-Wl,-Bstatic
endif

# ------------  additional pkg-config includes and libraries  ------------------
PKG_CONF_ARG    = gtk+-3.0 libxml-2.0

# ------------  additional include directories  --------------------------------
INC_DIR  =

# ------------  additional library directories  --------------------------------
LIB_DIR  = ./libcolor

# ------------  additional libraries  ------------------------------------------
LIBS     = $(STATIC) -lmbcolor -Wl,-Bdynamic -lm

# ------------  archive generation ---------------------------------------------
TARBALL_EXCLUDE = *.{o,gz,zip}
ZIP_EXCLUDE     = *.{o,gz,zip}

# ------------  cmd line parameters for this executable  -----------------------

ifdef PKG_CONF_ARG
INC_DIR_PKG_CONF = $$(pkg-config --cflags $(PKG_CONF_ARG))
LIBS_PKG_CONF    = $$(pkg-config --libs $(PKG_CONF_ARG))
endif


ifeq (YES, ${DEBUG})
CMD = gdb $(OUT_DIR)/$(EXECUTABLE) $(PARAMS)
else
CMD = $(OUT_DIR)/$(EXECUTABLE) $(PARAMS)
endif

#===============================================================================
# The following statements usually need not to be changed
#===============================================================================
C_SOURCES       = $(SOURCES)
ALL_INC_DIR     = $(addprefix -I, $(INC_DIR)) $(INC_DIR_PKG_CONF)
ALL_LIB_DIR     = $(addprefix -L, $(LIB_DIR))
ALL_CFLAGS      = $(CFLAGS) $(ALL_INC_DIR)
ALL_LFLAGS      = $(LDFLAGS) $(ALL_LIB_DIR)
BASENAMES       = $(basename $(C_SOURCES))

ifeq (YES, ${SHARED})
ifdef RPATH
ALL_LFLAGS += -Wl,-rpath,${PREFIX}/lib/${RPATH}
endif
endif

# ------------  generate the names of the object files  ------------------------
OBJECTS         = $(addprefix $(OUT_DIR)/, $(addsuffix .o,$(BASENAMES)))

# ------------  make (and start) the executable  -------------------------------
all: $(OUT_DIR)/$(EXECUTABLE)
ifeq (YES, ${START})
	$(CMD)
endif

# ------------  make the executable  -------------------------------------------
$(OUT_DIR)/$(EXECUTABLE): $(OUT_DIR) $(OBJECTS)
	+make -C ./libcolor CC=${CC} AR=${AR} CFLAGS="${CFLAGS}" DEBUG=${DEBUG} SHARED=${SHARED} \
		DESTDIR=${DESTDIR} PREFIX=${PREFIX} RPATH=${RPATH}
	$(CC) -o $(OUT_DIR)/$(EXECUTABLE) $(OBJECTS) \
		$(ALL_LFLAGS) $(ALL_LIB_DIR) $(LIBS) $(LIBS_PKG_CONF)

#-------------  Create the directories -----------------------------------------
$(OUT_DIR):
	@mkdir -p $(OUT_DIR)

# ------------  make the objects  ----------------------------------------------
$(OUT_DIR)/%.o:	%.c
	$(CC)  -c $(ALL_CFLAGS) $< -o $@

# ------------  remove generated files  ----------------------------------------
clean:
	@rm -rf $(DEBUG_DIR) $(RELEASE_DIR)
	@+make -C ./libcolor clean

# ------------  tarball generation  --------------------------------------------
tarball:
	lokaldir=`pwd`; lokaldir=$${lokaldir##*/};   \
		rm --force $$lokaldir.tar.gz;             \
		tar --exclude=$(TARBALL_EXCLUDE)          \
		--create                                  \
		--gzip                                    \
		--verbose                                 \
		--file  $$lokaldir.tar.gz *

# ------------  zip  -----------------------------------------------------------
zip:
	@lokaldir=`pwd`; lokaldir=$${lokaldir##*/}; \
		zip -r  $$lokaldir.zip * -x $(ZIP_EXCLUDE)


# ------------  installl  ------------------------------------------------------
install:
	@[ -f $(RELEASE_DIR)/$(EXECUTABLE) ] || ( echo "Executable does not exist. You must build the project before install."; exit 1 )
	@for POFILE in ./po/*.po; do \
		LOCALE_DIR=${DESTDIR}$(MO_DIR)/$$(basename $$POFILE .po)/LC_MESSAGES; \
		install -d $$LOCALE_DIR; \
		msgfmt $$POFILE -o $$LOCALE_DIR/$(EXECUTABLE).mo; \
	done
	install -D -m644 ./mandelbrot.xml ${DESTDIR}$(DOC_DIR)/$(EXECUTABLE)/mandelbrot.xml
	install -D -m755 $(RELEASE_DIR)/$(EXECUTABLE) ${DESTDIR}$(PREFIX)/bin/$(EXECUTABLE)
	+make -C ./libcolor DEBUG=${DEBUG} SHARED=${SHARED} \
		DESTDIR=${DESTDIR} PREFIX=${PREFIX} RPATH=${RPATH} install

# ------------  uninstall  -----------------------------------------------------
uninstall:
	for POFILE in ./po/*.po; do  \
		LOCALE_DIR=${DESTDIR}$(MO_DIR)/$$(basename $$POFILE .po)/LC_MESSAGES; \
		rm -rf  $$LOCALE_DIR/$(EXECUTABLE).mo; \
	done
	rm -rf ${DESTDIR}$(PREFIX)/bin/$(EXECUTABLE)
	rm -rf ${DESTDIR}$(DOC_DIR)/$(EXECUTABLE)
	+make -C ./libcolor DEBUG=${DEBUG} SHARED=${SHARED} \
		DESTDIR=${DESTDIR} PREFIX=${PREFIX} RPATH=${RPATH} uninstall

# ------------  xgettext  ------------------------------------------------------
xgettext:
	xgettext -k_ -kN_ -LC --no-location --omit-header -o ./po/messages.pot *.c *.h \
		./libcolor/*.h ./libcolor/*.c

#-------------  create dependencies  -------------------------------------------
depend:
	gcc -MM -E  $(SOURCES) | awk '{ printf("$(OUT_DIR)/%s\n", $$0); }' > ./.depend

include .depend

.PHONY: all clean tarball zip depend install uninstall xgettext depend
