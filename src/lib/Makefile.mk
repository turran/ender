
lib_LTLIBRARIES += src/lib/libender.la

installed_ender_headersdir = $(pkgincludedir)-$(VMAJ)
dist_installed_ender_headers_DATA = \
src/lib/Ender.h

#src_lib_libender_la_YFLAGS = -d -p ender_
#src_lib_libender_la_LFLAGS = -o$(LEX_OUTPUT_ROOT).c

#src_lib_libender_la_SOURCES = \
src/lib/eina_ordered_hash.c \
src/lib/ender_container.c \
src/lib/ender_constraint.c \
src/lib/ender_descriptor.c \
src/lib/ender_element.c \
src/lib/ender_function.c \
src/lib/ender_grammar.y \
src/lib/ender_lexer.l \
src/lib/ender_loader.c \
src/lib/ender_marshaller.c \
src/lib/ender_main.c \
src/lib/ender_misc.c \
src/lib/ender_namespace.c \
src/lib/ender_object.c \
src/lib/ender_parser.c \
src/lib/ender_property.c \
src/lib/ender_serializer.c \
src/lib/ender_serializer_eet.c \
src/lib/ender_struct.c \
src/lib/ender_union.c \
src/lib/ender_value.c \
src/lib/ender_private.h \
src/lib/tpl/tpl.c \
src/lib/tpl/tpl.h

src_lib_libender_la_SOURCES = \
src/lib/ender_document.c \
src/lib/ender_element_ender.c \
src/lib/ender_element_instance.c \
src/lib/ender_element_object.c \
src/lib/ender_element_states.c \
src/lib/ender_element_state.c \
src/lib/ender_main.c

if HAVE_WIN32
src_lib_libender_la_SOURCES += \
src/lib/tpl/mmap.c \
src/lib/tpl/mman.h
endif

src_lib_libender_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/lib/tpl \
-DPACKAGE_DATA_DIR=\"$(pkgdatadir)\" \
-DENDER_BUILD \
@ENDER_CFLAGS@

src_lib_libender_la_LIBADD = \
@ENDER_LIBS@ \
@EFL_DLOPEN_LIBS@

src_lib_libender_la_LDFLAGS = -no-undefined -version-info @version_info@

CLEAN_LOCAL += \
src/lib/ender_lexer.c \
src/lib/libender_la-ender_grammar.c \
src/lib/libender_la-ender_grammar.h
