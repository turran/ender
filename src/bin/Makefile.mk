
bin_PROGRAMS = \
src/bin/ender-inspect \
src/bin/ender_loader
#src/bin/ender_generator
#src/bin/ender_generator

src_bin_ender_inspect_LDADD = \
$(top_builddir)/src/lib/libender.la \
@ENDER_LIBS@

src_bin_ender_inspect_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
@ENDER_CFLAGS@

src_bin_ender_inspect_SOURCES = \
src/bin/ender_inspect.c

src_bin_ender_generator_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
@ENDER_CFLAGS@

src_bin_ender_generator_LDADD = \
$(top_builddir)/src/lib/libender.la \
@ENDER_LIBS@

src_bin_ender_generator_SOURCES = \
src/bin/ender_generator.c

src_bin_ender_loader_LDADD = \
$(top_builddir)/src/lib/libender.la \
@ENDER_LIBS@

src_bin_ender_loader_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
@ENDER_CFLAGS@

src_bin_ender_loader_SOURCES = \
src/bin/ender_loader.c

