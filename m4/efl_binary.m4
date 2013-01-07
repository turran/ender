dnl Copyright (C) 2013 Vincent Torri <vincent dot torri at gmail dot com>
dnl That code is public domain and can be freely used or copied.

dnl Macro that specify the binary to use

dnl Usage: EFL_WITH_BIN(package, binary)
dnl Call AC_SUBST(_binary) (_binary is the lowercase of binary, - being transformed into _ by default, or the value set by the user)

AC_DEFUN([EFL_WITH_BIN],
[

m4_pushdef([UP], m4_translit([[$2]], [-a-z], [_A-Z]))dnl
m4_pushdef([DOWN], m4_translit([[$2]], [-A-Z], [_a-z]))dnl

dnl configure option

AC_ARG_WITH([$2],
   [AS_HELP_STRING([--with-$2=PATH], [specify a specific path to ]DOWN[ @<:@default=]DOWN[@:>@])],
   [
    _efl_with_binary=${withval}
    _efl_want_binary="yes"
   ],
   [
    _efl_with_binary=$(pkg-config --variable=prefix $1)/bin/]DOWN[
    _efl_want_binary="no"
   ])

DOWN=${_efl_with_binary}
AC_MSG_NOTICE(DOWN[ set to ${_efl_with_binary}])

with_binary_[]m4_defn([DOWN])=${_efl_with_binary}

AC_SUBST(DOWN)

AM_CONDITIONAL([HAVE_]UP, [test "x${_efl_want_binary}" = "xyes"])

m4_popdef([UP])
m4_popdef([DOWN])

])
