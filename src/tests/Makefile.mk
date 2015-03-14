if ENS_ENABLE_TESTS

TESTS += src/tests/ender_test_utils

check_PROGRAMS += src/tests/ender_test_utils
#test01 src/tests/test02

src_tests_ender_test_utils_SOURCES = src/tests/ender_test_utils.c
src_tests_ender_test_utils_CPPFLAGS = -I$(top_srcdir)/src/lib @ENDER_CFLAGS@ @CHECK_CFLAGS@
src_tests_ender_test_utils_LDADD = $(top_builddir)/src/lib/libender.la @ENDER_LIBS@ @CHECK_LIBS@

src_tests_test01_SOURCES = \
src/tests/test01.c \
src/tests/test_dummy.c \
src/tests/test_dummy.h

src_tests_test01_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
@ENDER_CFLAGS@

src_tests_test01_LDADD = \
$(top_builddir)/src/lib/libender.la \
@ENDER_LIBS@

src_tests_test02_SOURCES = \
src/tests/test02.c \
src/tests/test_dummy.c \
src/tests/test_dummy.h

src_tests_test02_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
@ENDER_CFLAGS@

src_tests_test02_LDADD = \
$(top_builddir)/src/lib/libender.la \
@ENDER_LIBS@

endif
