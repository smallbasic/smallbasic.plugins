# SmallBASIC
# Copyright(C) 2020 Chris Warren-Smith. [http://tinyurl.com/ja2ss]
#
# This program is distributed under the terms of the GPL v2.0 or later
# Download the GNU Public License (GPL) from www.gnu.org
# 

#
# cross-compile sdl: ./configure --host=x86_64-w64-mingw32 --prefix=/home/chrisws/mingw64 --disable-static
# use built sdl2-config: export PATH=~/mingw64/bin:${PATH}
# 

AM_CXXFLAGS=-fno-rtti -std=c++14
AM_CPPFLAGS = -I../include @NUKLEAR_CPPFLAGS@
lib_LTLIBRARIES = libnuklear.la
libnuklear_la_SOURCES =  ../include/param.cpp ../include/hashmap.cpp main.cpp
libnuklear_la_LDFLAGS = -module -rpath '$(libdir)' @NUKLEAR_LDFLAGS@ @PLATFORM_LDFLAGS@

