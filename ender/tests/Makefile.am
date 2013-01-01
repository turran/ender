MAINTAINERCLEANFILES = Makefile.in

check_PROGRAMS = test01 test02

test01_SOURCES = \
test01.c

test01_CPPFLAGS = \
-I. \
-I$(top_srcdir)/ender/include \
-I$(top_builddir)/ender/include \
@ENDER_CFLAGS@

test01_LDADD = \
$(top_builddir)/ender/lib/libender.la \
@ENDER_LIBS@

test02_SOURCES = \
test02.c

test02_CPPFLAGS = \
-I. \
-I$(top_srcdir)/ender/include \
-I$(top_builddir)/ender/include \
@ENDER_CFLAGS@

test02_LDADD = \
$(top_builddir)/ender/lib/libender.la \
@ENDER_LIBS@

