
lib_LTLIBRARIES += src/lib/libender.la

installed_headersdir = $(pkgincludedir)-$(VMAJ)
dist_installed_headers_DATA = \
src/lib/Ender.h \
src/lib/ender_build.h \
src/lib/ender_item.h \
src/lib/ender_item_struct.h \
src/lib/ender_item_basic.h \
src/lib/ender_item_def.h \
src/lib/ender_item_object.h \
src/lib/ender_item_arg.h \
src/lib/ender_item_attr.h \
src/lib/ender_item_constant.h \
src/lib/ender_item_function.h \
src/lib/ender_item_enum.h \
src/lib/ender_lib.h \
src/lib/ender_main.h \
src/lib/ender_parser.h \
src/lib/ender_utils.h \
src/lib/ender_value.h

src_lib_libender_la_SOURCES = \
src/lib/ender_item.c \
src/lib/ender_item_private.h \
src/lib/ender_item_arg.c \
src/lib/ender_item_arg_private.h \
src/lib/ender_item_attr.c \
src/lib/ender_item_attr_private.h \
src/lib/ender_item_basic.c \
src/lib/ender_item_basic_private.h \
src/lib/ender_item_def.c \
src/lib/ender_item_def_private.h \
src/lib/ender_item_object.c \
src/lib/ender_item_object_private.h \
src/lib/ender_item_struct.c \
src/lib/ender_item_struct_private.h \
src/lib/ender_item_constant.c \
src/lib/ender_item_constant_private.h \
src/lib/ender_item_function.c \
src/lib/ender_item_function_private.h \
src/lib/ender_item_enum.c \
src/lib/ender_item_enum_private.h \
src/lib/ender_lib.c \
src/lib/ender_lib_private.h \
src/lib/ender_main.c \
src/lib/ender_main_private.h \
src/lib/ender_parser.c \
src/lib/ender_private.h \
src/lib/ender_utils.c \
src/lib/ender_value.c \
src/lib/ender_value_private.h

src_lib_libender_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-DDESCRIPTIONS_DIR=\"$(pkgdatadir)/descriptions\" \
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
