# SmallBASIC
# Copyright(C) 2020 Chris Warren-Smith. [http://tinyurl.com/ja2ss]
#
# This program is distributed under the terms of the GPL v2.0 or later
# Download the GNU Public License (GPL) from www.gnu.org
# 

AM_CXXFLAGS=-fno-rtti -std=c++14
AM_CPPFLAGS = @IMGUI_CPPFLAGS@
lib_LTLIBRARIES = libimgui.la
libimgui_la_SOURCES =  \
 ../include/param.cpp \
 ../include/hashmap.cpp \
  imgui/imgui.cpp \
  imgui/imgui_widgets.cpp \
  imgui/imgui_draw.cpp \
  main.cpp

libimgui_la_LDFLAGS = -module -rpath '$(libdir)' @IMGUI_LDFLAGS@ @PLATFORM_LDFLAGS@

