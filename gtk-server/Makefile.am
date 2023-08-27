# SmallBASIC
# Copyright(C) 2001-2023 Chris Warren-Smith.
#
# This program is distributed under the terms of the GPL v2.0 or later
# Download the GNU Public License (GPL) from www.gnu.org
#

AM_CXXFLAGS=-fno-rtti -std=c++14
AM_CPPFLAGS = -I../include -I./uthash/src -Wall @GTK_SERVER_CPPFLAGS@
lib_LTLIBRARIES = libgtkserver.la
libgtkserver_la_SOURCES = ../include/param.cpp ../include/hashmap.cpp ../include/apiexec.cpp gtk-server.c main.cpp
libgtkserver_la_LDFLAGS = -module -rpath '$(libdir)' @GTK_SERVER_LDFLAGS@ @PLATFORM_LDFLAGS@ -lffi

