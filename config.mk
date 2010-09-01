
#
# install configuration-settings
#

PREFIX  := /usr
MO_DIR  := ${PREFIX}/share/locale
DOC_DIR := ${PREFIX}/share/doc

# Build shared libcolor?
SHARED  := NO

# Must end with / -> ${PREFIX}/lib/$(RPATH)$(LIBRARYNAME)
RPATH   := mandelbrot/
