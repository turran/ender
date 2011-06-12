MAINTAINERCLEANFILES = Makefile.in

check_PROGRAMS = test01

test01_SOURCES = \
test01.c

test01_CFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
@ENDER_CFLAGS@

test01_DEPENDENCIES = $(top_builddir)/src/lib/libender.la

test01_LDADD = \
$(top_builddir)/src/lib/libender.la \
@ENDER_LIBS@

