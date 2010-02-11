
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

# DEBUG can be set to YES to include debugging info, or NO otherwise
DEBUG           := YES

# ------------  run executable out of this Makefile  (yes/no)  -----------------
START           := YES

#------------  start application with params -----------------------------------
PARAMS          :=

# ------------  name of the executable  ----------------------------------------
EXECUTABLE      := mandelbrot

# ------------  list of all source files  --------------------------------------
SOURCES         := main.c defs.c interface.c layout.c config.c \
	render.c iterate.c draw.c color.c storedrawing.c validate.c

# ------------  compiler  ------------------------------------------------------
CC              := gcc

# ------------  compiler flags  ------------------------------------------------
DEBUG_CFLAGS    := -Wall -O0 -g
RELEASE_CFLAGS  := -Wall -O2

#-------------  Directories  ---------------------------------------------------
OBJ_DIR         := ./obj
DEBUG_DIR       := ./Debug
RELEASE_DIR     := ./Release

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

# ------------  additional pkg-config includes and libraries  ------------------
PKG_CONF_ARG    = gtk+-2.0 gthread-2.0 libconfig

# ------------  additional system include directories  -------------------------
GLOBAL_INC_DIR  =

# ------------  additional system library directories  -------------------------
GLOBAL_LIB_DIR  =

# ------------  additional system libraries  -----------------------------------
GLOBAL_LIBS     =

# ------------  system libraries  ----------------------------------------------
SYS_LIBS        = -lm

# ------------  private include directories  -----------------------------------
LOCAL_INC_DIR   =

# ------------  private library directories  -----------------------------------
LOCAL_LIB_DIR   =

# ------------  private libraries  ---------------------------------------------
LOCAL_LIBS      =

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
ALL_INC_DIR     = $(addprefix -I, $(LOCAL_INC_DIR) $(GLOBAL_INC_DIR)) $(INC_DIR_PKG_CONF)
ALL_LIB_DIR     = $(addprefix -L, $(LOCAL_LIB_DIR) $(GLOBAL_LIB_DIR))
GLOBAL_LIBSS    = $(addprefix $(GLOBAL_LIB_DIR)/, $(GLOBAL_LIBS))
LOCAL_LIBSS     = $(addprefix $(LOCAL_LIB_DIR)/, $(LOCAL_LIBS))
ALL_CFLAGS      = $(CFLAGS) $(ALL_INC_DIR)
ALL_LFLAGS      = $(LDFLAGS) $(ALL_LIB_DIR)
BASENAMES       = $(basename $(C_SOURCES))

# ------------  generate the names of the object files  ------------------------
OBJECTS         = $(addprefix $(OBJ_DIR)/, $(addsuffix .o,$(BASENAMES)))

# ------------  make (and start) the executable  -------------------------------
all: $(OUT_DIR)/$(EXECUTABLE)
ifeq (YES, ${START})
	$(CMD)
endif

# ------------  make the executable  -------------------------------------------
$(OUT_DIR)/$(EXECUTABLE):    $(OBJ_DIR) $(OUT_DIR) $(OBJECTS)
	$(CC) $(ALL_LFLAGS) -o $(OUT_DIR)/$(EXECUTABLE) $(OBJECTS) \
		$(LOCAL_LIBSS) $(GLOBAL_LIBSS) $(SYS_LIBS) $(LIBS_PKG_CONF)

#-------------  Create the directories -----------------------------------------
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OUT_DIR):
	@mkdir -p $(OUT_DIR)

# ------------  make the objects  ----------------------------------------------
$(OBJ_DIR)/%.o:	%.c
	$(CC)  -c $(ALL_CFLAGS) $< -o $@

# ------------  remove generated files  ----------------------------------------
clean:
	@rm -rf $(DEBUG_DIR) $(RELEASE_DIR) $(OBJ_DIR)

# ------------ tarball generation ----------------------------------------------
tarball:
	lokaldir=`pwd`; lokaldir=$${lokaldir##*/};   \
		rm --force $$lokaldir.tar.gz;             \
		tar --exclude=$(TARBALL_EXCLUDE)          \
		--create                                  \
		--gzip                                    \
		--verbose                                 \
		--file  $$lokaldir.tar.gz *

# ------------ zip -------------------------------------------------------------
zip:
	@lokaldir=`pwd`; lokaldir=$${lokaldir##*/}; \
		zip -r  $$lokaldir.zip * -x $(ZIP_EXCLUDE)

.PHONY: all clean tarball zip depend

#-------------  create dependencies  -------------------------------------------
depend:
	@makedepend -s'#object dependencies' -m -Y. -Y$(LOCAL_INC_DIR) -p$(OBJ_DIR)/ -- $(SOURCES) 2>/dev/null

#object dependencies

./obj/defs.o: defs.h
./obj/interface.o: interface.h defs.h iterate.h layout.h config.h render.h
./obj/interface.o: draw.h
./obj/layout.o: interface.h defs.h iterate.h layout.h storedrawing.h
./obj/config.o: config.h interface.h defs.h iterate.h
./obj/render.o: render.h interface.h defs.h iterate.h draw.h validate.h
./obj/iterate.o: iterate.h render.h interface.h defs.h iterate_template.c
./obj/draw.o: draw.h interface.h defs.h iterate.h color.h
./obj/color.o: color.h
./obj/storedrawing.o: storedrawing.h interface.h defs.h iterate.h layout.h
./obj/storedrawing.o: validate.h
./obj/validate.o: validate.h defs.h
