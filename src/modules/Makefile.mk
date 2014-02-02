## Enesim

if BUILD_STATIC_MODULE_ENESIM

src_lib_libender_la_SOURCES += src/modules/ender_enesim.c

endif

if BUILD_MODULE_ENESIM

ender_enesim_LTLIBRARIES = src/modules/ender_enesim.la
ender_enesimdir = $(pkglibdir)

src_modules_ender_enesim_la_SOURCES = src/modules/ender_enesim.c

src_modules_ender_enesim_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-DENDER_BUILD \
@ENDER_CFLAGS@

src_modules_ender_enesim_la_LIBADD = \
src/lib/libender.la \
@ENDER_LIBS@

src_modules_ender_enesim_la_LDFLAGS = -no-undefined -module -avoid-version
src_modules_ender_enesim_la_LIBTOOLFLAGS = --tag=disable-static

endif
