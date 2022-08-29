# SmallBASIC
# Copyright(C) 2020 Chris Warren-Smith. [http://tinyurl.com/ja2ss]
#
# This program is distributed under the terms of the GPL v2.0 or later
# Download the GNU Public License (GPL) from www.gnu.org
# 

AM_CXXFLAGS=-fno-rtti -std=c++14
AM_CPPFLAGS = -D_GLFW_BUILD_DLL=1 \
  -I../raylib/raylib/src/external/glfw/include \
  -I../raylib/raylib/src/external/glfw/deps
lib_LTLIBRARIES = libnuklear.la
libnuklear_la_SOURCES = \
  ../include/param.cpp \
  ../include/hashmap.cpp \
  ../raylib/raylib/src/rglfw.c \
  main.cpp
libnuklear_la_LDFLAGS = -module -rpath '$(libdir)' @NUKLEAR_LDFLAGS@ @PLATFORM_LDFLAGS@
