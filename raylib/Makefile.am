# SmallBASIC
# Copyright(C) 2001-2022 Chris Warren-Smith.
#
# This program is distributed under the terms of the GPL v2.0 or later
# Download the GNU Public License (GPL) from www.gnu.org
#

sbasic = /home/chrisws/src/SmallBASIC/src/platform/console/sbasic

generated = func-def.h proc-def.h proc.h func.h

CLEANFILES = $(generated)

$(generated): raylib/parser/raylib_api.json mkraylib.bas
	$(sbasic) mkraylib.bas $@ > $@
	@touch main.cpp

gen: $(generated)
	@make all -j 32

AM_CXXFLAGS=-fno-rtti -std=c++14
AM_CPPFLAGS = -Iraylib/src -Iraylib/src/external/glfw/include -Iraylib/src/external/glfw/deps/mingw \
  -DPLATFORM_DESKTOP=1 -DSUPPORT_BUSY_WAIT_LOOP=1 -DSUPPORT_SCREEN_CAPTURE=1 \
  -DSUPPORT_GIF_RECORDING=1 -DSUPPORT_COMPRESSION_API=1 -D_GLFW_BUILD_DLL=1 \
  -Wall -Wextra -Wshadow -Wdouble-promotion -Wno-unused-parameter -fPIC

lib_LTLIBRARIES = libraylib.la

if WITH_STATIC_INCLUDE
  libraylib_la_SOURCES = \
   raylib/src/rglfw.c \
   raylib/src/rmodels.c \
   raylib/src/raudio.c \
   raylib/src/rcore.c \
   raylib/src/rshapes.c \
   raylib/src/rtextures.c \
   raylib/src/rtext.c \
   raylib/src/utils.c \
   ../include/param.cpp \
   ../include/hashmap.cpp \
   physac.cpp \
   raygui.cpp \
   main.cpp
else
  libraylib_la_SOURCES = \
   ../include/param.cpp \
   ../include/hashmap.cpp \
   physac.cpp \
   raygui.cpp \
   main.cpp
endif

libraylib_la_LDFLAGS = -module -rpath '$(libdir)' @RAYLIB_LDFLAGS@ @PLATFORM_LDFLAGS@

