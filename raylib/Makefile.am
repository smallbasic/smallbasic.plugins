# SmallBASIC
# Copyright(C) 2001-2020 Chris Warren-Smith.
#
# This program is distributed under the terms of the GPL v2.0 or later
# Download the GNU Public License (GPL) from www.gnu.org
#

AM_CXXFLAGS=-fno-rtti -std=c++14
AM_CPPFLAGS = -Iraylib/src -Iraylib/src/external/glfw/include -Iraylib/src/external/glfw/deps/mingw \
  -DPLATFORM_DESKTOP=1 -DSUPPORT_BUSY_WAIT_LOOP=1 -DSUPPORT_SCREEN_CAPTURE=1 \
  -DSUPPORT_GIF_RECORDING=1 -DSUPPORT_COMPRESSION_API=1 -D_GLFW_BUILD_DLL=1 \
  -Wall -Wextra -Wshadow -Wdouble-promotion -Wno-unused-parameter -fPIC

lib_LTLIBRARIES = libraylib.la

if WITH_STATIC_INCLUDE
  libraylib_la_SOURCES = \
   raylib/src/rglfw.c \
   raylib/src/models.c \
   raylib/src/raudio.c \
   raylib/src/core.c \
   raylib/src/shapes.c \
   raylib/src/textures.c \
   raylib/src/text.c \
   raylib/src/utils.c \
   ../include/param.cpp \
   ../include/hashmap.cpp \
   physac.cpp \
   raygui.cpp \
   main.cpp
else
  libraylib_la_SOURCES = \
   ../include/param.cpp \
   ../include/hashmap.cpp \
   physac.cpp \
   raygui.cpp \
   main.cpp
endif

libraylib_la_LDFLAGS = -module -rpath '$(libdir)' @RAYLIB_LDFLAGS@ @PLATFORM_LDFLAGS@
