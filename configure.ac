# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(ender, 0.0.5, enesim-devel@googlegroups.com)
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR(configure.ac)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER(src/include/config.h)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AC_C_BIGENDIAN

AC_LIBTOOL_WIN32_DLL
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

AM_PROG_LEX
if test "x$ac_cv_prog_LEX" = "x"; then
  AC_MSG_ERROR(Cannot find flex)
fi
AC_PROG_YACC
if test "x$ac_cv_prog_YACC" = "x"; then
  AC_MSG_ERROR(Cannot find bison)
fi

dnl Set PACKAGE_DATA_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/${PACKAGE}", [Shared Data Directory])
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/${PACKAGE}", [Shared Data Directory])
fi

VMAJ=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $1);}'`
VMIN=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $2);}'`
VMIC=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $3);}'`
SNAP=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $4);}'`
version_info=`expr $VMAJ + $VMIN`":$VMIC:$VMIN"
AC_SUBST(version_info)

create_shared_lib=""
case "$host_os" in
    mingw|mingw32)
	create_shared_lib="-no-undefined"
    ;;
esac
AC_SUBST(create_shared_lib)

# Common
PKG_CHECK_MODULES(EINA, [eina >= 1.0.0])
PKG_CHECK_MODULES(ENESIM, [enesim >= 0.0.4])

ENDER_CFLAGS="$EINA_CFLAGS $ENESIM_CFLAGS"
ENDER_LIBS="$EINA_LIBS $ENESIM_LIBS"
AC_SUBST(ENDER_CFLAGS)
AC_SUBST(ENDER_LIBS)

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
],[
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
echo "Installation Path.........: $prefix"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
echo
