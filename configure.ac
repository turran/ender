
### Version

m4_define([v_maj], [0])
m4_define([v_min], [0])
m4_define([v_mic], [5])
m4_define([v_ver], [v_maj.v_min.v_mic])

m4_define([lt_cur], m4_eval(v_maj + v_min))
m4_define([lt_rev], v_mic)
m4_define([lt_age], v_min)

AC_INIT([ender], [v_ver], [enesim-devel@googlegroups.com])
AC_PREREQ([2.60])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([src/include/config.h])

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_PREREQ([2.2])
LT_INIT([win32-dll])

version_info="lt_cur:lt_rev:lt_age"
AC_SUBST([version_info])


### Needed information

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

ENDER_BUILD=""
case "$host_os" in
   mingw*)
      ENDER_BUILD="-DENDER_BUILD"
   ;;
esac
AC_SUBST([ENDER_BUILD])

requirements_ender="enesim >= 0.0.15 eina >= 1.0.0"
AC_SUBST([requirements_ender])


### Checks for programs

AC_PROG_CC

# pkg-config
PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   pkgconfig_requires_private="Requires.private"
else
   pkgconfig_requires_private="Requires"
fi
AC_SUBST([pkgconfig_requires_private])

# lex

AM_PROG_LEX
if test "x$ac_cv_prog_LEX" = "x"; then
   AC_MSG_ERROR(Cannot find flex)
fi

AC_PROG_YACC
if test "x$ac_cv_prog_YACC" = "x"; then
   AC_MSG_ERROR(Cannot find bison)
fi

# doxygen program for documentation building
EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])


### Checks for libraries

PKG_CHECK_MODULES([ENDER], [${requirements_ender}])

## Make the debug preprocessor configurable

AC_OUTPUT([
Makefile
ender.pc
data/Makefile
src/Makefile
src/lib/Makefile
src/include/Makefile
src/bin/Makefile
src/tests/Makefile
])


#####################################################################
## Info

echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo
echo
echo "Configuration Options Summary:"
echo
echo "Compilation............: make (or gmake)"
echo "  CPPFLAGS.............: $CPPFLAGS"
echo "  CFLAGS...............: $CFLAGS"
echo "  LDFLAGS..............: $LDFLAGS"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo
