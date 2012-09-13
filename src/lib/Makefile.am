MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = tpl

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
-I$(top_srcdir)/src/lib/tpl \
-I$(top_builddir)/src/include \
-DPACKAGE_DATA_DIR=\"$(pkgdatadir)\" \
@ENDER_BUILD@ \
@ENDER_CFLAGS@

AM_YFLAGS = -d -p ender_
AM_LFLAGS = -o$(LEX_OUTPUT_ROOT).c

CLEANFILES = \
ender_lexer.c \
libender_la-ender_grammar.c \
libender_la-ender_grammar.h

lib_LTLIBRARIES = libender.la

libender_la_YFLAGS = -d

libender_la_SOURCES = \
eina_ordered_hash.c \
ender_container.c \
ender_constraint.c \
ender_descriptor.c \
ender_element.c \
ender_grammar.y \
ender_lexer.l \
ender_loader.c \
ender_main.c \
ender_misc.c \
ender_namespace.c \
ender_property.c \
ender_value.c

libender_la_LIBADD = \
tpl/libender_tpl.la \
@ENDER_LIBS@ @LEXLIB@ @EFL_DLOPEN_LIBS@

libender_la_LDFLAGS = -no-undefined -version-info @version_info@
