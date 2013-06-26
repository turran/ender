
check_PROGRAMS += ender/tests/test01 ender/tests/test02

ender_tests_test01_SOURCES = \
ender/tests/test01.c \
ender/tests/test_dummy.c \
ender/tests/test_dummy.h

ender_tests_test01_CPPFLAGS = \
-I$(top_srcdir)/ender/lib \
@ENDER_CFLAGS@

ender_tests_test01_LDADD = \
$(top_builddir)/ender/lib/libender.la \
@ENDER_LIBS@

ender_tests_test02_SOURCES = \
ender/tests/test02.c \
ender/tests/test_dummy.c \
ender/tests/test_dummy.h

ender_tests_test02_CPPFLAGS = \
-I$(top_srcdir)/ender/lib \
@ENDER_CFLAGS@

ender_tests_test02_LDADD = \
$(top_builddir)/ender/lib/libender.la \
@ENDER_LIBS@
