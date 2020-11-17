# SmallBASIC
# Copyright(C) 2020 Chris Warren-Smith. [http://tinyurl.com/ja2ss]
#
# This program is distributed under the terms of the GPL v2.0 or later
# Download the GNU Public License (GPL) from www.gnu.org
# 

AM_CXXFLAGS=-fno-rtti -std=c++14
AM_CPPFLAGS = \
  -I./imgui \
  -I../raylib/raylib/src/external/glfw/include \
  -I../raylib/raylib/src/external/glfw/deps \
	-DIMGUI_IMPL_OPENGL_LOADER_GLAD2=1 \
	-Wall -Wextra -Wshadow -Wdouble-promotion -Wno-unused-parameter
lib_LTLIBRARIES = libimgui.la
libimgui_la_SOURCES =  \
  ../include/param.cpp \
  ../include/hashmap.cpp \
  ../raylib/raylib/src/rglfw.c \
  ../raylib/raylib/src/external/glfw/deps/glad_gl.c \
  imgui/backends/imgui_impl_glfw.cpp \
  imgui/backends/imgui_impl_opengl3.cpp \
  imgui/imgui.cpp \
  imgui/imgui_widgets.cpp \
  imgui/imgui_draw.cpp \
  main.cpp

libimgui_la_LDFLAGS = -module -rpath '$(libdir)' @IMGUI_LDFLAGS@ @PLATFORM_LDFLAGS@

