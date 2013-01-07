MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir) \
-I$(top_srcdir)/ender/include \
@ENDER_CFLAGS@

bin_PROGRAMS = ender_inspect ender_generator

ender_inspect_LDADD = \
$(top_builddir)/ender/lib/libender.la \
@ENDER_LIBS@

ender_inspect_SOURCES = \
ender_inspect.c

ender_generator_LDADD = \
$(top_builddir)/ender/lib/libender.la \
@ENDER_LIBS@

ender_generator_SOURCES = \
ender_generator.c

