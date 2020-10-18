# SmallBASIC
# Copyright(C) 2001-2020 Chris Warren-Smith.
#
# This program is distributed under the terms of the GPL v2.0 or later
# Download the GNU Public License (GPL) from www.gnu.org
# 

AM_CPPFLAGS = -I../include -Wall -Wextra -Wshadow -Wdouble-promotion -Wno-unused-parameter -fno-rtti -fno-exceptions -std=c++14 -fPIC
lib_LTLIBRARIES = libraylib.la
libraylib_la_SOURCES = main.cpp ../include/param.cpp
libraylib_la_LDFLAGS = -module -rpath '$(libdir)' -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
