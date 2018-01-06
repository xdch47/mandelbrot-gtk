
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
#               make install      (install executable                       )
#               make uninstall    (uninstall executable                     )
#
#===============================================================================


# DEBUG can be set to YES to include debugging info, or NO otherwise
DEBUG           := NO

# ------------  build a shared or static library  ------------------------------
SHARED          := NO

# ------------  subinstall directory (must end with /)  ------------------------
RPATH          := mandelbrot/

# ------------  name of the target      ----------------------------------------
ifeq (YES, ${SHARED})
TARGET          := libmbcolor.so.0.0
LIBRARY_NAME    := libmbcolor.so
else
TARGET          := libmbcolor.a
LIBRARY_NAME    := libmbcolor.a
endif

# ------------  list of all source files  --------------------------------------
SOURCES         := color.c colormapfunc.c lookuptable.c avltree.c

# ------------  compiler  ------------------------------------------------------
CC              := gcc

# ------------  compiler flags  ------------------------------------------------
DEBUG_CFLAGS    := -Wall -O0 -g
RELEASE_CFLAGS  := -Wall -O2 -pipe -mtune=native -march=native \
	-DNDEBUG

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
QUIET           := NO
else
CFLAGS          := ${RELEASE_CFLAGS}
LDFLAGS         := ${RELEASE_LDFLAGS}
OUT_DIR         := ${RELEASE_DIR}
QUIET           := YES
endif

ifeq (YES, $(QUIET))
Q := @
else
Q :=
endif

# ------------  additional pkg-config includes and libraries  ------------------
# gtk-libs
PKG_CONF_ARG    = gtk+-3.0

# ------------  additional include directories  --------------------------------
INC_DIR  =

# ------------  additional library directories  --------------------------------
LIB_DIR  =

# ------------  additional libraries  ------------------------------------------
LIBS     = -lm

# ------------  archive generation ---------------------------------------------
TARBALL_EXCLUDE = *.{o,gz,zip}
ZIP_EXCLUDE     = *.{o,gz,zip}

# ------------  cmd line parameters for this executable  -----------------------

ifdef PKG_CONF_ARG
INC_DIR_PKG_CONF = $$(pkg-config --cflags $(PKG_CONF_ARG))
LIBS_PKG_CONF    = $$(pkg-config --libs $(PKG_CONF_ARG))
endif


ifeq (YES, ${DEBUG})
CMD = gdb $(OUT_DIR)/$(TARGET) $(PARAMS)
else
CMD = $(OUT_DIR)/$(TARGET) $(PARAMS)
endif

#===============================================================================
# The following statements usually need not to be changed
#===============================================================================
C_SOURCES       = $(SOURCES)
ALL_INC_DIR     = $(addprefix -I, $(INC_DIR)) $(INC_DIR_PKG_CONF)
ALL_LIB_DIR     = $(addprefix -L, $(LIB_DIR))
ALL_CFLAGS      = $(CFLAGS) $(ALL_INC_DIR)
ALL_LFLAGS      = $(LDFLAGS) $(ALL_LIB_DIR)
BASENAMES       = $(basename $(SOURCES))

ifeq (YES, ${SHARED})
	ALL_CFLAGS += -fPIC
	ALL_LFLAGS += -shared -fPIC -Wl,-soname,$(LIBRARY_NAME)
endif

# ------------  generate the names of the object files  ------------------------
OBJECTS         = $(addprefix $(OUT_DIR)/, $(addsuffix .o,$(BASENAMES)))

# ------------  make (and start) the executable  -------------------------------
all: $(OUT_DIR)/$(TARGET)
ifeq (YES, ${START})
	$(CMD)
endif

# ------------  make the executable  -------------------------------------------
$(OUT_DIR)/$(TARGET): $(OUT_DIR) $(OBJECTS)
ifeq (YES, ${QUIET})
	$Qecho "Building library $..."
endif
ifeq (YES, ${SHARED})
	$Q$(CC) $(ALL_LFLAGS) -o $(OUT_DIR)/$(TARGET) $(OBJECTS) \
		$(ALL_LIB_DIR) $(LIBS) $(LIBS_PKG_CONF)
	$Qln -sf $(OUT_DIR)/$(TARGET) ./$(LIBRARY_NAME)
else
	$Q$(AR) cr $(OUT_DIR)/$(TARGET) $(OBJECTS)
	$Qln -sf $(OUT_DIR)/$(TARGET) ./$(LIBRARY_NAME)
	$Qranlib ./$(LIBRARY_NAME)
endif

#-------------  Create the directories -----------------------------------------
$(OUT_DIR):
	$Qmkdir -p $(OUT_DIR)

# ------------  make the objects  ----------------------------------------------
$(OUT_DIR)/%.o:	%.c
ifeq (YES, ${QUIET})
	$Qecho "Compling $<..."
	$Q$(CC)  -c $(ALL_CFLAGS) $< -o $@ 2> /dev/null
else
	$Q$(CC)  -c $(ALL_CFLAGS) $< -o $@
endif

-include .depend
#-------------  create dependencies  -------------------------------------------
depend:
	$Qgcc -MM -E  $(SOURCES) | awk '{ printf("$(OUT_DIR)/%s\n", $$0); }' > ./.depend

# ------------  remove generated files  ----------------------------------------
clean:
	$Qrm -rf $(DEBUG_DIR) $(RELEASE_DIR)
	$Qrm -rf ./$(LIBRARY_NAME)

# ------------  tarball generation  --------------------------------------------
tarball:
	$Qlokaldir=`pwd`; lokaldir=$${lokaldir##*/};   \
		rm --force $$lokaldir.tar.gz;             \
		tar --exclude=$(TARBALL_EXCLUDE)          \
		--create                                  \
		--gzip                                    \
		--verbose                                 \
		--file  $$lokaldir.tar.gz *

# ------------  zip  -----------------------------------------------------------
zip:
	$Qlokaldir=`pwd`; lokaldir=$${lokaldir##*/}; \
		zip -r  $$lokaldir.zip * -x $(ZIP_EXCLUDE)

# ------------  installl  ------------------------------------------------------
install: $(TARGET)
ifeq (YES, ${SHARED})
	$Qinstall -D -m755 $(RELEASE_DIR)/$(TARGET) ${DESTDIR}$(PREFIX)/lib/${RPATH}$(TARGET)
	$Qln -sf ${DESTDIR}$(PREFIX)/lib/${RPATH}$(TARGET) \
		${DESTDIR}$(PREFIX)/lib/${RPATH}$(LIBRARY_NAME)
endif

# ------------  uninstall  -----------------------------------------------------
uninstall:
ifeq (YES, ${SHARED})
	rm -rf ${DESTDIR}$(PREFIX)/lib/${RPATH}$(TARGET)
	rm -rf ${DESTDIR}$(PREFIX)/lib/${RPATH}$(LIBRARY_NAME)
ifdef RPATH
	rmdir ${DESTDIR}$(PREFIX)/lib/${RPATH}
endif
endif

.PHONY: all depend clean tarball zip install uninstall
