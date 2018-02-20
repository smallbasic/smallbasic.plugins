dnl 
dnl Configure script for SmallBASIC plugins
dnl
dnl Copyright(C) 2001-2018 Chris Warren-Smith.
dnl
dnl This program is distributed under the terms of the GPL v2.0
dnl Download the GNU Public License (GPL) from www.gnu.org
dnl

AC_INIT([smallbasic], [0.12.13])
AC_CONFIG_AUX_DIR(aux-dist)
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_SRCDIR([mysql/mysql.c])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE
LT_INIT([win32-dll])

function checkDebugMode() {
  AC_MSG_CHECKING([if debug mode is enabled])
  AC_ARG_WITH(debug,
  [  --with-debug            Build the debug version default=no],
      [with_debug=$withval],
      [with_debug=no])
  AC_MSG_RESULT([$with_debug])
  if test "$with_debug" = "yes" || test "$with_debug" = "full"
  then
    CFLAGS="${CFLAGS} -g -O0"
    CXXFLAGS="${CXXFLAGS} -g -O0"
    AC_DEFINE(_DEBUG, 1, [debugging build enabled])
  fi
  AC_SUBST(CFLAGS)
}

# checks for mysql
AC_CHECK_HEADER(mysql/mysql.h, [], [AC_MSG_ERROR("install mysql")])

# checks for nuklear
AC_CHECK_PROG(have_sdl, sdl2-config, [yes], [no])
if test "${have_sdl}" = "no" ; then
  AC_MSG_ERROR([libsdl2-dev not installed: configure failed.])
fi

dnl change default aru setting to avoid warning
ARFLAGS=cr
AC_SUBST(ARFLAGS)

AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_PROG_CXX
LT_PREREQ([2.2])

BUILD_SUBDIRS="mysql nuklear"
AC_SUBST(BUILD_SUBDIRS)
checkDebugMode

libdir=${libdir}/smallbasic

AC_CONFIG_FILES([
Makefile
nuklear/Makefile
mysql/Makefile])

AC_OUTPUT
