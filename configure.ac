dnl
dnl Configure script for SmallBASIC plugins
dnl
dnl Copyright(C) 2001-2024 Chris Warren-Smith.
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

function generate_wayland_protocols() {
  RAYLIB_SRC_PATH="${srcdir}/raylib/raylib/src"
  WL_PROTOCOLS_DIR="${RAYLIB_SRC_PATH}/external/glfw/deps/wayland"
  AC_MSG_NOTICE([Generating Wayland protocol headers])
  wl_generate() {
    protocol="$1"
    basename="$2"
    "$WAYLAND_SCANNER" client-header "$protocol" "$RAYLIB_SRC_PATH/$basename.h" || exit 1
    "$WAYLAND_SCANNER" private-code "$protocol" "$RAYLIB_SRC_PATH/$basename-code.h" || exit 1
  }
  wl_generate "$WL_PROTOCOLS_DIR/wayland.xml" wayland-client-protocol
  wl_generate "$WL_PROTOCOLS_DIR/xdg-shell.xml" xdg-shell-client-protocol
  wl_generate "$WL_PROTOCOLS_DIR/xdg-decoration-unstable-v1.xml" xdg-decoration-unstable-v1-client-protocol
  wl_generate "$WL_PROTOCOLS_DIR/viewporter.xml" viewporter-client-protocol
  wl_generate "$WL_PROTOCOLS_DIR/relative-pointer-unstable-v1.xml" relative-pointer-unstable-v1-client-protocol
  wl_generate "$WL_PROTOCOLS_DIR/pointer-constraints-unstable-v1.xml" pointer-constraints-unstable-v1-client-protocol
  wl_generate "$WL_PROTOCOLS_DIR/fractional-scale-v1.xml" fractional-scale-v1-client-protocol
  wl_generate "$WL_PROTOCOLS_DIR/xdg-activation-v1.xml" xdg-activation-v1-client-protocol
  wl_generate "$WL_PROTOCOLS_DIR/idle-inhibit-unstable-v1.xml" idle-inhibit-unstable-v1-client-protocol
}

AC_ARG_WITH(mlpack,
  [AS_HELP_STRING([--with-mlpack], [Build the mlpack module])],
  [MLPACK="yes"],
  [MLPACK="no"])

AC_ARG_WITH(gtk-server,
  [AS_HELP_STRING([--with-gtk-server], [Build the gtk-server module])],
  [GTK_SERVER="yes"],
  [GTK_SERVER="no"])

AC_ARG_WITH(ioio,
  [AS_HELP_STRING([--with-ioio], [Build the ioio module])],
  [IOIO="yes"],
  [IOIO="no"])

case "${host_os}" in
  *mingw* | *msys*)
    AC_DEFINE(_WIN32, 1, [building for win32])
    PLATFORM_LDFLAGS="-no-undefined -Wc,-static-libgcc -Wl,-static -avoid-version"
    RAYLIB_LDFLAGS="-pthread -lgdi32 -mwindows"
    CLIPBOARD_LDFLAGS=""
    NUKLEAR_LDFLAGS="-pthread -lgdi32 -mwindows"
    WEBSOCKET_LDFLAGS="-lwsock32"
    GTK_SERVER_LDFLAGS=""
    GTK_SERVER_CPPFLAGS="-DGTK_SERVER_WIN32"
    IOIO_CPPFLAGS=""
    IOIO_LDFLAGS=""
    NUKLEAR_CPPFLAGS=""
    ;;

  *)
    PLATFORM_LDFLAGS="-Wl,--no-undefined -avoid-version"
    CLIPBOARD_LDFLAGS="`pkg-config xcb --libs` -lpthread"
    WEBSOCKET_LDFLAGS=""
    GTK_SERVER_LDFLAGS="`pkg-config --libs gtk+-3.0` -lXm -lXt"
    GTK_SERVER_CPPFLAGS="`pkg-config --cflags gtk+-3.0` -DGTK_SERVER_FFI -DGTK_SERVER_LIBRARY -DGTK_SERVER_UNIX -DGTK_SERVER_GTK3x"
    RAYLIB_LDFLAGS="`pkg-config wayland-client wayland-cursor wayland-egl xkbcommon --libs`"
    JVM_CPPFLAGS="-I/usr/lib/jvm/java-1.8.0-openjdk-amd64/include -I/usr/lib/jvm/java-1.8.0-openjdk-amd64/include/linux"
    JVM_LDFLAGS="-L/usr/lib/jvm/java-1.8.0-openjdk-amd64/jre/lib/amd64/server -ljvm"
    NUKLEAR_CPPFLAGS="-D_GLFW_WAYLAND=1"
    NUKLEAR_LDFLAGS="`pkg-config wayland-client wayland-cursor wayland-egl xkbcommon --libs`"

    AC_ARG_VAR([WAYLAND_SCANNER], [Path to wayland-scanner])
    AC_PATH_PROG([WAYLAND_SCANNER], [wayland-scanner])
    AS_IF([test -n "$WAYLAND_SCANNER"], [
      generate_wayland_protocols
    ], [
      AC_MSG_WARN([wayland-scanner not found; Wayland support disabled])
    ])
esac

AC_SUBST(DEBUG_LDFLAGS)
AC_SUBST(CLIPBOARD_LDFLAGS)
AC_SUBST(RAYLIB_LDFLAGS)
AC_SUBST(NUKLEAR_LDFLAGS)
AC_SUBST(WEBSOCKET_LDFLAGS)
AC_SUBST(PLATFORM_LDFLAGS)
AC_SUBST(GTK_SERVER_LDFLAGS)
AC_SUBST(GTK_SERVER_CPPFLAGS)
AC_SUBST(JVM_LDFLAGS)
AC_SUBST(JVM_CPPFLAGS)
AC_SUBST(NUKLEAR_CPPFLAGS)

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
if test "${GTK_SERVER}" = "yes"
then
  BUILD_SUBDIRS="${BUILD_SUBDIRS} gtk-server"
fi
if test "${IOIO}" = "yes"
then
  BUILD_SUBDIRS="${BUILD_SUBDIRS} ioio"
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
mlpack/Makefile
ioio/Makefile
gtk-server/Makefile])

AC_OUTPUT

# create config to libclipboard
cp config.h clipboard/libclipboard-config.h
