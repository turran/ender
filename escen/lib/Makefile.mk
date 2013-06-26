
escen_lib_libescen_la_YFLAGS = -d -p escen_
escen_lib_libescen_la_LFLAGS = -o$(LEX_OUTPUT_ROOT).c

if HAVE_ESCEN
lib_LTLIBRARIES += escen/lib/libescen.la

installed_escen_headersdir = $(includedir)/escen-$(VMAJ)
dist_installed_escen_headers_DATA = \
escen/lib/Escen.h

escen_lib_libescen_la_SOURCES = \
escen/lib/escen_copier.c \
escen/lib/escen_animation.c \
escen/lib/escen_ender.c \
escen/lib/escen_escen.c \
escen/lib/escen_grammar.y \
escen/lib/escen_instance.c \
escen/lib/escen_lexer.l \
escen/lib/escen_main.c \
escen/lib/escen_misc.c \
escen/lib/escen_parser.c \
escen/lib/escen_proxy.c \
escen/lib/escen_setter.c \
escen/lib/escen_script.c \
escen/lib/escen_scriptor.c \
escen/lib/escen_state.c \
escen/lib/escen_private.h

if BUILD_SCRIPT_NEKO
escen/lib/script/neko/scriptor.c
endif

escen_lib_libescen_la_CPPFLAGS = \
-I$(top_srcdir)/ender/lib \
-DESCEN_BUILD \
@ESCEN_CFLAGS@

escen_lib_libescen_la_LIBADD = \
$(top_builddir)/ender/lib/libender.la \
@ESCEN_LIBS@ \
@LEXLIB@

if BUILD_SCRIPT_NEKO
escen_lib_libescen_la_LIBADD += \
@NEKO_LIBS@
endif

escen_lib_libescen_la_LDFLAGS = -no-undefined -version-info @version_info@
endif

CLEAN_LOCAL += \
escen/lib/libescen_la-escen_grammar.c \
escen/lib/libescen_la-escen_grammar.h
