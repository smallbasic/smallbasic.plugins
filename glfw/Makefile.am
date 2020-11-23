# SmallBASIC
# Copyright(C) 2001-2020 Chris Warren-Smith.
#
# This program is distributed under the terms of the GPL v2.0 or later
# Download the GNU Public License (GPL) from www.gnu.org
# 

AM_CXXFLAGS=-fno-rtti -std=c++14
AM_CPPFLAGS = \
  -I../raylib/raylib/src/external/glfw/include \
  -I../raylib/raylib/src/external/glfw/deps \
	-Wall -Wextra -Wshadow -Wdouble-promotion -Wno-unused-parameter -D_GLFW_BUILD_DLL=1
lib_LTLIBRARIES = libglfw.la
libglfw_la_SOURCES = \
  ../include/param.cpp \
  ../include/hashmap.cpp \
  ../raylib/raylib/src/rglfw.c \
  ../raylib/raylib/src/external/glfw/deps/glad_gl.c \
  main.cpp
libglfw_la_LDFLAGS = -module -rpath '$(libdir)' @RAYLIB_LDFLAGS@ @PLATFORM_LDFLAGS@

