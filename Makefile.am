ACLOCAL_AMFLAGS = -I m4

SUBDIRS = ender

if HAVE_ESCEN
SUBDIRS += escen
endif

MAINTAINERCLEANFILES = \
Makefile.in \
aclocal.m4 \
build-stamp \
config.guess \
config.h \
config.h.in \
config.sub \
configure \
configure-stamp \
depcomp \
install-sh \
ltconfig \
ltmain.sh \
missing mkinstalldirs \
stamp-h.in \
ender.pc \
escen.pc

EXTRA_DIST = \
AUTHORS \
COPYING \
gendoc \
Doxyfile \
README \
ender.pc.in \
escen.pc.in \
m4/efl_dlfcn.m4 \
m4/efl_doxygen.m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ender.pc

if HAVE_ESCEN
pkgconfig_DATA += escen.pc
endif
