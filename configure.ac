#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.50)

AC_INIT(nyello, 0.4.0, seb@cine7.net)

VERSION=0.4.0
AM_INIT_AUTOMAKE($DEPS, $VERSION)

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

AC_SUBST(VERSION)

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_MAKE_SET

# For cmd_parser
AC_PROG_LIBTOOL

# Checks for libraries.

dnl Check for xmms2 (check in /usr/local too)
pkg_modules="xmms2-client xmms2-client-cpp xmms2-client-cpp-glib"
PKG_CHECK_MODULES(XMMS2, [$pkg_modules])
AC_SUBST(XMMS2_CFLAGS)
AC_SUBST(XMMS2_LIBS)

dnl Check for readline
AC_CHECK_LIB(readline, readline)
AC_CHECK_HEADER([readline/readline.h],, AC_MSG_ERROR([libreadline is missing]))
READLINE_LIBS="-lreadline"
AC_SUBST(READLINE_LIBS)

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE

# Checks for library functions.
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_STRTOD
AC_CHECK_FUNCS([strtol])
AC_OUTPUT([Makefile src/Makefile src/cmd_parser/Makefile])
