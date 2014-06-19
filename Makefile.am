AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4 -I common/m4

MAINTAINERCLEANFILES = \
Makefile.in \
aclocal.m4 \
build-stamp \
config.guess \
config.h \
config.h.in \
config.sub \
configure \
configure-stamp \
depcomp \
install-sh \
ltconfig \
ltmain.sh \
missing mkinstalldirs \
stamp-h.in \
ender.pc

lib_LTLIBRARIES =
check_PROGRAMS =
EXTRA_DIST =
CLEAN_LOCAL = ender.pc

### Documentation

include common/Makefile_doxygen.mk

### Library

include src/lib/Makefile.mk

### Binary

include src/bin/Makefile.mk

### Tests

#include src/tests/Makefile.mk

### Data

include data/Makefile.mk

EXTRA_DIST += \
AUTHORS \
COPYING \
README \
ender.pc.in \
m4/efl_dlfcn.m4 \
m4/ens_doxygen.m4

sed_process = \
$(SED) \
-e 's,@VERSION\@,$(VERSION),g' \
-e 's,@VMAJ\@,$(VMAJ),g' \
-e 's,@prefix\@,$(prefix),g' \
-e 's,@exec_prefix\@,$(exec_prefix),g' \
-e 's,@libdir\@,$(libdir),g' \
-e 's,@includedir\@,$(includedir),g' \
-e 's,@pkgincludedir\@,$(pkgincludedir),g' \
-e 's,@pkglibdir\@,$(pkglibdir),g' \
-e 's,@datarootdir\@,$(datarootdir),g' \
-e 's,@pkgdatadir\@,$(pkgdatadir),g' \
-e 's,@requirements_ender_pc\@,$(requirements_ender_pc),g' \
< $< > $@ || rm $@

pc_verbose = $(pc_verbose_@AM_V@)
pc_verbose_ = $(pc_verbose_@AM_DEFAULT_V@)
pc_verbose_0 = @echo "  PC      " $@;

ender.pc: $(top_srcdir)/ender.pc.in Makefile
	@rm -f $@
	$(pc_verbose)$(sed_process)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ender.pc

clean-local: doc-clean
	-rm -rf $(CLEAN_LOCAL)
