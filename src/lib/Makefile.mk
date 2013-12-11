
lib_LTLIBRARIES += ender/lib/libender.la

installed_ender_headersdir = $(pkgincludedir)-$(VMAJ)
dist_installed_ender_headers_DATA = \
ender/lib/Ender.h

ender_lib_libender_la_YFLAGS = -d -p ender_
ender_lib_libender_la_LFLAGS = -o$(LEX_OUTPUT_ROOT).c

ender_lib_libender_la_SOURCES = \
ender/lib/eina_ordered_hash.c \
ender/lib/ender_container.c \
ender/lib/ender_constraint.c \
ender/lib/ender_descriptor.c \
ender/lib/ender_element.c \
ender/lib/ender_function.c \
ender/lib/ender_grammar.y \
ender/lib/ender_lexer.l \
ender/lib/ender_loader.c \
ender/lib/ender_marshaller.c \
ender/lib/ender_main.c \
ender/lib/ender_misc.c \
ender/lib/ender_namespace.c \
ender/lib/ender_object.c \
ender/lib/ender_parser.c \
ender/lib/ender_property.c \
ender/lib/ender_serializer.c \
ender/lib/ender_serializer_eet.c \
ender/lib/ender_struct.c \
ender/lib/ender_union.c \
ender/lib/ender_value.c \
ender/lib/ender_private.h \
ender/lib/tpl/tpl.c \
ender/lib/tpl/tpl.h

if HAVE_WIN32
ender_lib_libender_la_SOURCES += \
ender/lib/tpl/mmap.c \
ender/lib/tpl/mman.h
endif

ender_lib_libender_la_CPPFLAGS = \
-I$(top_srcdir)/ender/lib \
-I$(top_srcdir)/ender/lib/tpl \
-DPACKAGE_DATA_DIR=\"$(pkgdatadir)\" \
-DENDER_BUILD \
@ENDER_CFLAGS@

ender_lib_libender_la_LIBADD = \
@ENDER_LIBS@ \
@LEXLIB@ \
@EFL_DLOPEN_LIBS@

ender_lib_libender_la_LDFLAGS = -no-undefined -version-info @version_info@

CLEAN_LOCAL += \
ender/lib/ender_lexer.c \
ender/lib/libender_la-ender_grammar.c \
ender/lib/libender_la-ender_grammar.h
