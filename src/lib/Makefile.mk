
lib_LTLIBRARIES += src/lib/libender.la

installed_ender_headersdir = $(pkgincludedir)-$(VMAJ)
dist_installed_ender_headers_DATA = \
src/lib/ender_item.h \
src/lib/ender_main.h \
src/lib/ender_parser.h

src_lib_libender_la_YFLAGS = -d -p ender_
src_lib_libender_la_LFLAGS = -o$(LEX_OUTPUT_ROOT).c

src_lib_libender_la_SOURCES = \
src/lib/ender_item.c \
src/lib/ender_item_private.h \
src/lib/ender_main.c \
src/lib/ender_main_private.h \
src/lib/ender_parser.c \
src/lib/ender_private.h

src_lib_libender_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/lib/tpl \
-DPACKAGE_DATA_DIR=\"$(pkgdatadir)\" \
-DENDER_BUILD \
@ENDER_CFLAGS@

src_lib_libender_la_LIBADD = \
@ENDER_LIBS@ \
@LEXLIB@ \
@EFL_DLOPEN_LIBS@

src_lib_libender_la_LDFLAGS = -no-undefined -version-info @version_info@

CLEAN_LOCAL += \
src/lib/ender_lexer.c \
src/lib/libender_la-ender_grammar.c \
src/lib/libender_la-ender_grammar.h
