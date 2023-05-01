dnl
dnl Configure script for SmallBASIC plugins
dnl
dnl Copyright(C) 2001-2021 Chris Warren-Smith.
dnl
dnl This program is distributed under the terms of the GPL v2.0
dnl Download the GNU Public License (GPL) from www.gnu.org
dnl

AC_INIT([smallbasic],[27.0])
AC_CONFIG_AUX_DIR(aux-dist)
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_SRCDIR([include/param.cpp])
AC_CONFIG_HEADERS([config.h])
AC_LANG([C])
AC_LANG([C++])
AM_INIT_AUTOMAKE([subdir-objects])
LT_INIT([win32-dll])

dnl sbasic is required for raylib code generation
AC_CHECK_PROG(SBASIC_CHECK,sbasic, yes)
AS_IF([test x"$SBASIC_CHECK" != x"yes"], [AC_MSG_ERROR([Please install sbasic before configuring.])])

function checkDebugMode() {
  AC_MSG_CHECKING([if debug mode is enabled])
  AC_ARG_WITH(debug,
  [  --with-debug Build the debug version default=no],
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

AC_ARG_WITH(static-include,
  [AS_HELP_STRING([--with-static-include], [Include module sources])],
  [STATIC_INCLUDE="yes"],
  [STATIC_INCLUDE="no"])

AC_ARG_WITH(mlpack,
  [AS_HELP_STRING([--with-mlpack], [Build the mlpack module])],
  [MLPACK="yes"],
  [MLPACK="no"])

case "${host_os}" in
  *mingw* | *msys*)
    AC_DEFINE(_WIN32, 1, [building for win32])
    PLATFORM_LDFLAGS="-no-undefined -Wc,-static-libgcc -Wl,-static -avoid-version"
    RAYLIB_LDFLAGS="-pthread -lgdi32 -mwindows"
    CLIPBOARD_LDFLAGS=""
    NUKLEAR_LDFLAGS="-pthread -lgdi32 -mwindows"
    WEBSOCKET_LDFLAGS="-lwsock32"
    STATIC_INCLUDE="yes"
    ;;

  *)
    PLATFORM_LDFLAGS="-Wl,--no-undefined -avoid-version"
    CLIPBOARD_LDFLAGS="`pkg-config xcb --libs` -lpthread"
    NUKLEAR_LDFLAGS="-lGL -lm -lpthread -ldl -lrt -lX11"
    WEBSOCKET_LDFLAGS=""
    if test "${STATIC_INCLUDE}" = "yes"
    then
      RAYLIB_LDFLAGS="-lGL -lm -lpthread -ldl -lrt -lX11"
    else
      RAYLIB_LDFLAGS="-lraylib -lGL -lm -lpthread -ldl -lrt -lX11"
    fi
esac

AM_CONDITIONAL(WITH_STATIC_INCLUDE, test "${STATIC_INCLUDE}" = "yes")
AM_CONDITIONAL(WITH_MLPACK, test "${MLPACK}" = "yes")
AC_SUBST(DEBUG_LDFLAGS)
AC_SUBST(CLIPBOARD_LDFLAGS)
AC_SUBST(RAYLIB_LDFLAGS)
AC_SUBST(NUKLEAR_LDFLAGS)
AC_SUBST(WEBSOCKET_LDFLAGS)
AC_SUBST(PLATFORM_LDFLAGS)

dnl change default aru setting to avoid warning
ARFLAGS=cr
AC_SUBST(ARFLAGS)

LT_INIT([dlopen])
LT_PREREQ([2.2])
AC_PROG_CXX

BUILD_SUBDIRS="debug nuklear gifenc glfw clipboard websocket raylib"
if test "${MLPACK}" = "yes"
then
  BUILD_SUBDIRS="${BUILD_SUBDIRS} mlpack"
fi

AC_SUBST(BUILD_SUBDIRS)
checkDebugMode

libdir=${libdir}/smallbasic

AC_CONFIG_FILES([
Makefile
debug/Makefile
nuklear/Makefile
gifenc/Makefile
glfw/Makefile
clipboard/Makefile
websocket/Makefile
raylib/Makefile
mlpack/Makefile])

AC_OUTPUT

# create config to libclipboard
cp config.h clipboard/libclipboard-config.h
