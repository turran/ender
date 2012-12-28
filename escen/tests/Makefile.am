MAINTAINERCLEANFILES = Makefile.in

BUILT_SOURCES = \
escen_datatype_ender.c

check_PROGRAMS = \
escen_test_datatype

escen_test_datatype_SOURCES = escen_test_datatype.c
escen_test_datatype_CFLAGS = -I. -I$(top_srcdir)/src/include @ESCEN_CFLAGS@
escen_test_datatype_LDADD = $(top_builddir)/src/lib/libescen.la @ESCEN_LIBS@

escen_datatype_ender.c: $(top_srcdir)/src/tests/escen_datatype.ender
	ender_generator $(top_srcdir)/src/tests/escen_datatype.ender datatype $@

clean-local:
	rm -f escen_datatype_ender.c
