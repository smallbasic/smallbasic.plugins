# SmallBASIC
# Copyright(C) 2001-2020 Chris Warren-Smith.
#
# This program is distributed under the terms of the GPL v2.0 or later
# Download the GNU Public License (GPL) from www.gnu.org
# 

# use https://glad.dav1d.de/ to generate glad.c
# select the latest gl version then click generate

AM_CPPFLAGS = -I../include -Iglad/include `pkg-config --cflags glfw3` \
	-Wall -Wextra -Wshadow -Wdouble-promotion -Wno-unused-parameter -fno-rtti -fno-exceptions -std=c++14
lib_LTLIBRARIES = libglfw.la
libglfw_la_SOURCES = ../include/param.cpp ../include/hashmap.cpp glad/src/glad.cpp main.cpp
libglfw_la_LDFLAGS = -module -rpath '$(libdir)' `pkg-config --libs glfw3` -ldl -lGL @PLATFORM_LDFLAGS@

