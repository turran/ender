MAINTAINERCLEANFILES = Makefile.in

check_PROGRAMS = test01 test02

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

test02_SOURCES = \
test02.c

test02_CFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
@ENDER_CFLAGS@

test02_DEPENDENCIES = $(top_builddir)/src/lib/libender.la

test02_LDADD = \
$(top_builddir)/src/lib/libender.la \
@ENDER_LIBS@

