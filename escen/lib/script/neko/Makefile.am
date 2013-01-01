MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/ender/include \
-I$(top_srcdir)/escen/include \
@ESCEN_BUILD@ \
@ESCEN_CFLAGS@

noinst_LTLIBRARIES = libescen_script_neko.la

libescen_script_neko_la_SOURCES = scriptor.c
libescen_script_neko_la_LIBADD = @NEKO_LIBS@
