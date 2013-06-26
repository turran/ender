
if HAVE_ESCEN

ender_verbose = $(ender_verbose_@AM_V@)
ender_verbose_ = $(ender_verbose_@AM_DEFAULT_V@)
ender_verbose_0 = @echo "  ENDER "$@;

BUILT_SOURCES = \
escen/tests/escen_datatype_ender.c

check_PROGRAMS += escen/tests/escen_test_datatype

escen_tests_escen_test_datatype_SOURCES = escen/tests/escen_test_datatype.c
escen_tests_escen_test_datatype_CFLAGS = -I. -I$(top_srcdir)/src/include @ESCEN_CFLAGS@
escen_tests_escen_test_datatype_LDADD = $(top_builddir)/src/lib/libescen.la @ESCEN_LIBS@

escen_tests_escen_datatype_ender.c: $(top_srcdir)/escen/tests/escen_datatype.ender
if HAVE_ENDER_GENERATOR
	$(ender_verbose)@ender_generator@ $(top_srcdir)/escen/tests/escen_datatype.ender datatype $@
else
	$(ender_verbose)${top_builddir}/ender/bin/ender_generator $(top_srcdir)/escen/tests/escen_datatype.ender datatype $@
endif

endif

#CLEAN_LOCAL += escen/tests/escen_datatype_ender.c
