# SmallBASIC
# Copyright(C) 2001-2022 Chris Warren-Smith.
#
# This program is distributed under the terms of the GPL v2.0 or later
# Download the GNU Public License (GPL) from www.gnu.org
#

generated = func-def.h proc-def.h proc.h func.h
sbasic=sbasic

raylib/tools/rlparser/output/raylib_api.json: raylib/src/raylib.h raylib/tools/rlparser/rlparser.c
	(cd raylib/tools/rlparser && make && ./rlparser --format JSON --input ../../src/raylib.h --output raylib_api.json)

UNSUPPORTED.md: $(generated)
	$(sbasic)	mkraylib.bas unsupported > $@

README.md: $(generated) mkreadme.bas UNSUPPORTED.md
	$(sbasic)	mkreadme.bas `grep RAYLIB_VERSION raylib/src/raylib.h | sed 's/#define RAYLIB_VERSION//g' | sed 's/\"//g'` > README.md

$(generated): raylib/parser/raylib_api.json mkraylib.bas
	$(sbasic) mkraylib.bas $@ > $@
	@touch main.cpp

gen: $(generated) README.md

AM_CXXFLAGS=-fno-rtti -std=c++14 -fpermissive
AM_CPPFLAGS = -Iraylib/src @RAYLIB_CPPFLAGS@ \
  -DPLATFORM_DESKTOP_SDL=1 -DPLATFORM_DESKTOP_SDL3=1 -DSUPPORT_BUSY_WAIT_LOOP=1 -DSUPPORT_SCREEN_CAPTURE=1 \
  -DSUPPORT_GIF_RECORDING=1 -DSUPPORT_COMPRESSION_API=1 \
  -Wall -Wextra -Wshadow -Wdouble-promotion -Wno-unused-parameter -fPIC

lib_LTLIBRARIES = libraylib.la

libraylib_la_SOURCES = \
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

libraylib_la_LDFLAGS = -module -rpath '$(libdir)' @RAYLIB_LDFLAGS@ @PLATFORM_LDFLAGS@

