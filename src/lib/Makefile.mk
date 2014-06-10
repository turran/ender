
lib_LTLIBRARIES += src/lib/libender.la

installed_headersdir = $(pkgincludedir)-$(VMAJ)
dist_installed_headers_DATA = \
src/lib/Ender.h \
src/lib/ender_item.h \
src/lib/ender_item_struct.h \
src/lib/ender_item_basic.h \
src/lib/ender_item_object.h \
src/lib/ender_item_arg.h \
src/lib/ender_item_attr.h \
src/lib/ender_item_function.h \
src/lib/ender_item_enum.h \
src/lib/ender_lib.h \
src/lib/ender_main.h \
src/lib/ender_parser.h \
src/lib/ender_value.h

src_lib_libender_la_YFLAGS = -d -p ender_
src_lib_libender_la_LFLAGS = -o$(LEX_OUTPUT_ROOT).c

src_lib_libender_la_SOURCES = \
src/lib/ender_item.c \
src/lib/ender_item_private.h \
src/lib/ender_item_arg.c \
src/lib/ender_item_arg_private.h \
src/lib/ender_item_attr.c \
src/lib/ender_item_attr_private.h \
src/lib/ender_item_basic.c \
src/lib/ender_item_basic_private.h \
src/lib/ender_item_object.c \
src/lib/ender_item_object_private.h \
src/lib/ender_item_struct.c \
src/lib/ender_item_struct_private.h \
src/lib/ender_item_function.c \
src/lib/ender_item_function_private.h \
src/lib/ender_item_enum.c \
src/lib/ender_item_enum_private.h \
src/lib/ender_lib.c \
src/lib/ender_main.c \
src/lib/ender_main_private.h \
src/lib/ender_parser.c \
src/lib/ender_private.h \
src/lib/ender_value.c \
src/lib/ender_value_private.h

src_lib_libender_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
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
