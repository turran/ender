MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
@ENDER_CFLAGS@

bin_PROGRAMS = ender_inspect

ender_inspect_DEPENDENCIES = $(top_builddir)/src/lib/libender.la
ender_inspect_LDADD = \
$(top_builddir)/src/lib/libender.la \
@ENDER_LIBS@

ender_inspect_SOURCES = \
ender_inspect.c

