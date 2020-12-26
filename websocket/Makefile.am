# SmallBASIC
# Copyright(C) 2001-2020 Chris Warren-Smith.
#
# This program is distributed under the terms of the GPL v2.0 or later
# Download the GNU Public License (GPL) from www.gnu.org
#

AM_CXXFLAGS=-fno-rtti -std=c++14
AM_CPPFLAGS = -I../include -Wall -DMG_ENABLE_LOG=0 -DMG_ENABLE_DIRECTORY_LISTING=1\
  -Wall -Wextra -Wshadow -Wdouble-promotion -Wno-unused-parameter
lib_LTLIBRARIES = libwebsocket.la
libwebsocket_la_SOURCES = ../include/param.cpp ../include/hashmap.cpp ./mongoose/mongoose.c main.cpp
libwebsocket_la_LDFLAGS = -module -rpath '$(libdir)' @WEBSOCKET_LDFLAGS@ @PLATFORM_LDFLAGS@

