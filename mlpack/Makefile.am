# SmallBASIC
# Copyright(C) 2001-2023 Chris Warren-Smith.
#
# This program is distributed under the terms of the GPL v2.0 or later
# Download the GNU Public License (GPL) from www.gnu.org
#

AM_CXXFLAGS=-std=c++14
AM_CPPFLAGS =  -Imlpack/src -Wall -Wextra -Wno-shadow -Wdouble-promotion -Wno-unused-parameter -Wno-unknown-pragmas -fPIC
lib_LTLIBRARIES = libmlpack.la
libmlpack_la_SOURCES = main.cpp ../include/apiexec.cpp ../include/param.cpp ../include/hashmap.cpp mlpack/mlpack/src/mlpack.hpp
libmlpack_la_LDFLAGS = -module -rpath '$(libdir)' @PLATFORM_LDFLAGS@ -fopenmp -larmadillo
