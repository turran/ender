MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/include \
-I. \
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
ender_container.c \
ender_constraint.c \
ender_descriptor.c \
ender_element.c \
ender_generator.c \
ender_grammar.y \
ender_lexer.l \
ender_loader.c \
ender_main.c \
ender_misc.c \
ender_namespace.c \
ender_property.c \
ender_value.c

libender_la_LIBADD = \
@ENDER_LIBS@ \
@LEXLIB@

libender_la_DEPENDENCIES =

libender_la_LDFLAGS = @create_shared_lib@ -version-info @version_info@
