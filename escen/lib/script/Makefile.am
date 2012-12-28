MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

noinst_LTLIBRARIES = libescen_script.la

libescen_script_la_SOURCES =
libescen_script_la_DEPENDENCIES =
libescen_script_la_LIBADD =

if BUILD_SCRIPT_NEKO

SUBDIRS += neko

libescen_script_la_LIBADD += $(top_builddir)/escen/lib/script/neko/libescen_script_neko.la

endif
