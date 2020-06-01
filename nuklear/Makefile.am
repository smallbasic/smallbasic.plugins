# SmallBASIC
# Copyright(C) 2020 Chris Warren-Smith. [http://tinyurl.com/ja2ss]
#
# This program is distributed under the terms of the GPL v2.0 or later
# Download the GNU Public License (GPL) from www.gnu.org
# 

AM_CPPFLAGS = -I../include `sdl2-config --cflags` -fno-exceptions -pedantic -O2
lib_LTLIBRARIES = libnuklear.la
libnuklear_la_SOURCES = main.cpp ../include/param.cpp
libnuklear_la_LDFLAGS = -module -rpath '$(libdir)' `sdl2-config --libs` -lGL

