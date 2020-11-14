# SmallBASIC
# Copyright(C) 2001-2020 Chris Warren-Smith.
#
# This program is distributed under the terms of the GPL v2.0 or later
# Download the GNU Public License (GPL) from www.gnu.org
#

AM_CXXFLAGS=-fno-rtti -std=c++14
AM_CPPFLAGS = -I../include -Wall -I./libclipboard/include
lib_LTLIBRARIES = libclipboard.la
libclipboard_la_SOURCES = ../include/param.cpp ../include/hashmap.cpp clipboard.c main.cpp
libclipboard_la_LDFLAGS = -module -rpath '$(libdir)' @CLIPBOARD_LDFLAGS@ @PLATFORM_LDFLAGS@

