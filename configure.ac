
### Version

m4_define([v_maj], [0])
m4_define([v_min], [0])
m4_define([v_mic], [8])
m4_define([v_ver], [v_maj.v_min.v_mic])

m4_define([lt_cur], m4_eval(v_maj + v_min))
m4_define([lt_rev], v_mic)
m4_define([lt_age], v_min)

AC_INIT([ender], [v_ver], [enesim-devel@googlegroups.com])
AC_PREREQ([2.60])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_PREREQ([2.2])
LT_INIT([win32-dll])

version_info="lt_cur:lt_rev:lt_age"
AC_SUBST([version_info])

AC_DEFINE_UNQUOTED([VERSION_MAJOR], [v_maj], [Major version])
AC_DEFINE_UNQUOTED([VERSION_MINOR], [v_min], [Minor version])
AC_DEFINE_UNQUOTED([VERSION_MICRO], [v_mic], [Micro version])

VMAJ=v_maj
AC_SUBST([VMAJ])

## Add parameters for aclocal (keep in sync with Makefile.am)
AC_SUBST(ACLOCAL_AMFLAGS, "-I m4 -I common/m4")

### Additional options to configure

EFL_WITH_BIN([ender], [ender-generator])

### default values

AC_CANONICAL_HOST

have_win32="no"
case "$host_os" in
   mingw*)
      have_win32="yes"
   ;;
esac

AM_CONDITIONAL([HAVE_WIN32], [test "x${have_win32}" = "xyes"])

### Checks for programs

AC_PROG_CC
AM_PROG_CC_C_O

# pkg-config
PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if ! $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   AC_MSG_ERROR([pkg-config >= 0.22 is required])
fi

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
ENS_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])

LT_SYS_MODULE_EXT

### Checks for libraries

requirements_ender_pc="${requirements_ender_pc} eina >= 1.7.0 enesim >= 0.0.21"
if test "x${have_win32}" = "xyes" ; then
   requirements_ender_pc="${requirements_ender_pc} evil >= 1.7.0"
fi

PKG_CHECK_MODULES([ENDER], [${requirements_ender_pc}])
AC_SUBST([requirements_ender_pc])

# Scripting
## Neko
have_neko="no"
NEKO_LIBS=""
AC_CHECK_LIB([neko],
   [neko_val_callEx],
   [have_neko="yes"],
   [have_neko="no"])

if test "x${have_neko}" = "xyes" ; then
   AC_CHECK_HEADERS([neko.h neko_vm.h],
      [have_neko="yes"],
      [have_neko="no"])
fi

if test "x${have_neko}" = "xyes" ; then
  AC_CHECK_PROG([have_neko_bin], [nekoc], [yes], [no])
fi

if test "x${have_neko}" = "xyes" && test "x${have_neko_bin}" = "xyes" ; then
   requirements_escen_libs=-lneko
   AC_DEFINE([BUILD_SCRIPT_NEKO], [1], [Build support for NekoVM scripts])
fi

AM_CONDITIONAL([BUILD_SCRIPT_NEKO], [test "x${have_neko}" = "xyes" && test "x${have_neko_bin}" = "xyes"])
AC_SUBST([requirements_escen_libs])


### Checks for library functions

EFL_CHECK_DLOPEN

## Make the debug preprocessor configurable

AC_CONFIG_FILES([
Makefile
doc/Doxyfile
])

AC_OUTPUT

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
echo "Ender........................................: yes"
echo "  Serialization:"
echo "  TPL........................................: no"
echo
echo "Compilation............: make (or gmake)"
echo "  CPPFLAGS.............: $CPPFLAGS"
echo "  CFLAGS...............: $CFLAGS"
echo "  LDFLAGS..............: $LDFLAGS"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo
