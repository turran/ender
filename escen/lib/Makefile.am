MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = script

AM_CPPFLAGS = \
-I$(top_builddir) \
-I$(top_srcdir)/ender/include \
-I$(top_srcdir)/escen/include \
-I. \
@ESCEN_BUILD@ \
@ESCEN_CFLAGS@

AM_YFLAGS = -d -p escen_
AM_LFLAGS = -o$(LEX_OUTPUT_ROOT).c

CLEANFILES = \
libescen_la-escen_grammar.c \
libescen_la-escen_grammar.h

lib_LTLIBRARIES = libescen.la

libescen_la_YFLAGS = -d

libescen_la_SOURCES = \
escen_copier.c \
escen_animation.c \
escen_ender.c \
escen_escen.c \
escen_grammar.y \
escen_instance.c \
escen_lexer.l \
escen_main.c \
escen_misc.c \
escen_parser.c \
escen_proxy.c \
escen_setter.c \
escen_script.c \
escen_scriptor.c \
escen_state.c

libescen_la_LIBADD = $(top_builddir)/escen/lib/script/libescen_script.la \
@ESCEN_LIBS@ \
@LEXLIB@
libescen_la_LDFLAGS = -no-undefined -version-info @version_info@
