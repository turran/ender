MAINTAINERCLEANFILES = \
Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
-I$(top_builddir)/src/include \
@ENDER_BUILD@ \
@ENDER_CFLAGS@

noinst_LTLIBRARIES = libender_tpl.la

libender_tpl_la_SOURCES = \
tpl.c
