# SmallBASIC
# Copyright(C) 2001-2020 Chris Warren-Smith.
#
# This program is distributed under the terms of the GPL v2.0 or later
# Download the GNU Public License (GPL) from www.gnu.org
#

AM_CPPFLAGS = -DMG_ENABLE_CALLBACK_USERDATA=1 -I../include -Wall
lib_LTLIBRARIES = libwebsocket.la
libwebsocket_la_SOURCES = ../include/param.cpp ../include/hashmap.cpp ./mongoose/mongoose.c main.cpp
libwebsocket_la_LDFLAGS = -module -rpath '$(libdir)' @PLATFORM_LDFLAGS@

