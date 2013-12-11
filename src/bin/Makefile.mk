
bin_PROGRAMS = ender/bin/ender_inspect ender/bin/ender_generator

ender_bin_ender_inspect_LDADD = \
$(top_builddir)/ender/lib/libender.la \
@ENDER_LIBS@

ender_bin_ender_inspect_CPPFLAGS = \
-I$(top_srcdir)/ender/lib \
@ENDER_CFLAGS@

ender_bin_ender_inspect_SOURCES = \
ender/bin/ender_inspect.c

ender_bin_ender_generator_CPPFLAGS = \
-I$(top_srcdir)/ender/lib \
@ENDER_CFLAGS@

ender_bin_ender_generator_LDADD = \
$(top_builddir)/ender/lib/libender.la \
@ENDER_LIBS@

ender_bin_ender_generator_SOURCES = \
ender/bin/ender_generator.c

