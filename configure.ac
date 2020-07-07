dnl 
dnl Configure script for SmallBASIC plugins
dnl
dnl Copyright(C) 2001-2020 Chris Warren-Smith.
dnl
dnl This program is distributed under the terms of the GPL v2.0
dnl Download the GNU Public License (GPL) from www.gnu.org
dnl

AC_INIT([smallbasic], [20.0])
AC_CONFIG_AUX_DIR(aux-dist)
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_SRCDIR([mysql/mysql.c])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([subdir-objects])
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

case "${host_os}" in
  *mingw* | pw32* | cygwin*)
    AC_DEFINE(WIN32, 1, [building for win32])
    ;;

  *)
    CLIPBOARD_LDFLAGS=`pkg-config xcb --libs`
    AC_SUBST(CLIPBOARD_LDFLAGS)
esac

# checks for mysql
AC_CHECK_HEADER(mysql/mysql.h, [], [AC_MSG_ERROR("install mysql")])

# checks for nuklear
AC_CHECK_PROG(have_sdl, sdl2-config, [yes], [no])
if test "${have_sdl}" = "no" ; then
  AC_MSG_ERROR([libsdl2-dev not installed: configure failed.])
fi

# checks for glfw
AC_CHECK_HEADERS([GL/gl.h], [], [AC_MSG_ERROR([GL/gl.h not found])])

# checks for clipboard
AC_CHECK_HEADERS([libclipboard.h], [], [AC_MSG_ERROR([libclipboard.h not found])])

dnl change default aru setting to avoid warning
ARFLAGS=cr
AC_SUBST(ARFLAGS)

AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_PROG_CXX
LT_PREREQ([2.2])

BUILD_SUBDIRS="mysql nuklear glfw clipboard"
AC_SUBST(BUILD_SUBDIRS)
checkDebugMode

libdir=${libdir}/smallbasic

AC_CONFIG_FILES([
Makefile
nuklear/Makefile
glfw/Makefile
clipboard/Makefile
mysql/Makefile])

AC_OUTPUT
