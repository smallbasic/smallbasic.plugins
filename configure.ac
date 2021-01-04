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
AC_CONFIG_SRCDIR([include/param.cpp])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([subdir-objects])
LT_INIT([win32-dll])

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

case "${host_os}" in
  *mingw* | *msys*)
    AC_DEFINE(_WIN32, 1, [building for win32])
    PLATFORM_LDFLAGS="-no-undefined -Wl,-static -avoid-version"
    RAYLIB_LDFLAGS="-pthread -lgdi32 -mwindows"
    CLIPBOARD_LDFLAGS=""
    IMGUI_LDFLAGS="-pthread -lgdi32 -mwindows"
    NUKLEAR_LDFLAGS="-lgdi32 -lmsimg32 -mwindows"
    NUKLEAR_CPPFLAGS=""
    WEBSOCKET_LDFLAGS="-lwsock32"
    STATIC_INCLUDE="yes"
    ;;

  *)
    PLATFORM_LDFLAGS="-Wl,--no-undefined -avoid-version"
    CLIPBOARD_LDFLAGS="`pkg-config xcb --libs` -lpthread"
    IMGUI_LDFLAGS="-lGL -lm -lpthread -ldl -lrt -lX11"
    NUKLEAR_LDFLAGS="`pkg-config --libs glfw3` -lGL -lm -lGLU -lGLEW"
    NUKLEAR_CPPFLAGS=""
    WEBSOCKET_LDFLAGS=""
    if test "${STATIC_INCLUDE}" = "yes"
    then
      RAYLIB_LDFLAGS="-lGL -lm -lpthread -ldl -lrt -lX11"
    else
      RAYLIB_LDFLAGS="-lraylib -lGL -lm -lpthread -ldl -lrt -lX11"
    fi
esac

AM_CONDITIONAL(WITH_STATIC_INCLUDE, test "${STATIC_INCLUDE}" = "yes")
AC_SUBST(DEBUG_LDFLAGS)
AC_SUBST(CLIPBOARD_LDFLAGS)
AC_SUBST(RAYLIB_LDFLAGS)
AC_SUBST(IMGUI_LDFLAGS)
AC_SUBST(NUKLEAR_CPPFLAGS)
AC_SUBST(NUKLEAR_LDFLAGS)
AC_SUBST(WEBSOCKET_LDFLAGS)
AC_SUBST(PLATFORM_LDFLAGS)

dnl change default aru setting to avoid warning
ARFLAGS=cr
AC_SUBST(ARFLAGS)

AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_PROG_CXX
LT_PREREQ([2.2])

BUILD_SUBDIRS="debug nuklear glfw clipboard websocket raylib"
AC_SUBST(BUILD_SUBDIRS)
checkDebugMode

libdir=${libdir}/smallbasic

AC_CONFIG_FILES([
Makefile
debug/Makefile
nuklear/Makefile
imgui/Makefile
glfw/Makefile
clipboard/Makefile
websocket/Makefile
raylib/Makefile])

AC_OUTPUT

# create config to libclipboard
cp config.h clipboard/libclipboard-config.h
