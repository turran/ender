MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/ender/include \
-I$(top_srcdir)/ender/lib/tpl \
-DPACKAGE_DATA_DIR=\"$(pkgdatadir)\" \
-DENDER_BUILD \
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
ender_function.c \
ender_grammar.y \
ender_lexer.l \
ender_loader.c \
ender_marshaller.c \
ender_main.c \
ender_misc.c \
ender_namespace.c \
ender_object.c \
ender_parser.c \
ender_property.c \
ender_serializer.c \
ender_serializer_eet.c \
ender_struct.c \
ender_union.c \
ender_value.c \
tpl/tpl.c \
tpl/tpl.h

if HAVE_WIN32
libender_la_SOURCES += tpl/mmap.c tpl/mman.h
endif

libender_la_LIBADD = \
@ENDER_LIBS@ @LEXLIB@ @EFL_DLOPEN_LIBS@

libender_la_LDFLAGS = -no-undefined -version-info @version_info@
