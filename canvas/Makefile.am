# SmallBASIC
# Copyright(C) 2001-2021 Chris Warren-Smith.
#
# This program is distributed under the terms of the GPL v2.0 or later
# Download the GNU Public License (GPL) from www.gnu.org
#

AM_CXXFLAGS=-fno-rtti -std=c++14
AM_CPPFLAGS = -I../include -Wall
lib_LTLIBRARIES = libcanvas.la
libcanvas_la_SOURCES = ../include/param.cpp ../include/hashmap.cpp main.cpp
libcanvas_la_LDFLAGS = -module -rpath '$(libdir)' @PLATFORM_LDFLAGS@

