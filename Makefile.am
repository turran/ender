AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4

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
ender.pc \
escen.pc

lib_LTLIBRARIES =
check_PROGRAMS =
EXTRA_DIST =
CLEAN_LOCAL = ender.pc

if HAVE_ESCEN
CLEAN_LOCAL += escen.pc

include escen/lib/Makefile.mk
include ender/bin/Makefile.mk
#include escen/tests/Makefile.mk

endif

### Library

include src/lib/Makefile.mk

### Binary

### Tests

include src/tests/Makefile.mk

### Data

include src/data/Makefile.mk

### Documentation

include doc/Makefile.mk

EXTRA_DIST += \
AUTHORS \
COPYING \
README \
doc/Doxyfile.in \
ender.pc.in \
escen.pc.in \
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
-e 's,@requirements_deps_escen_pc\@,$(requirements_deps_escen_pc),g' \
-e 's,@requirements_escen_libs\@,$(requirements_escen_libs),g' \
< $< > $@ || rm $@

pc_verbose = $(pc_verbose_@AM_V@)
pc_verbose_ = $(pc_verbose_@AM_DEFAULT_V@)
pc_verbose_0 = @echo "  PC      " $@;

ender.pc: $(top_srcdir)/ender.pc.in Makefile
	@rm -f $@
	$(pc_verbose)$(sed_process)

if HAVE_ESCEN
escen.pc: $(top_srcdir)/escen.pc.in Makefile
	@rm -f $@
	$(pc_verbose)$(sed_process)
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ender.pc

if HAVE_ESCEN
pkgconfig_DATA += escen.pc
endif

clean-local: doc-clean
	-rm -rf $(CLEAN_LOCAL)
