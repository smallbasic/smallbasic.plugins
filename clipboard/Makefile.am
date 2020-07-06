# SmallBASIC
# Copyright(C) 2001-2020 Chris Warren-Smith.
#
# This program is distributed under the terms of the GPL v2.0 or later
# Download the GNU Public License (GPL) from www.gnu.org
# 

AM_CPPFLAGS = -I../include -Wall
lib_LTLIBRARIES = libclipboard.la
libclipboard_la_SOURCES = main.cpp clipboard.c ../include/param.cpp
libclipboard_la_SOURCES_la_LDFLAGS = -module -rpath '$(libdir)'
