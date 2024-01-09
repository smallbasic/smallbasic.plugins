# SmallBASIC
# Copyright(C) 2024 Chris Warren-Smith.
#
# This program is distributed under the terms of the GPL v2.0 or later
# Download the GNU Public License (GPL) from www.gnu.org
#
# export LD_LIBRARY_PATH=/usr/lib/jvm/java-1.8.0-openjdk-amd64/jre/lib/amd64/server:$LD_LIBRARY_PATH
# javap -s -p -cp target/ioio-1.0.jar 'net.sourceforge.smallbasic.ioio.AnalogInput'
#

sbasic=sbasic

generated = api.h

main.lo: $(generated)

all-am: $(generated)

CLEANFILES = $(generated)

AM_CXXFLAGS=-fno-rtti -std=c++14
AM_CPPFLAGS = -I../include -I/usr/lib/jvm/java-1.8.0-openjdk-amd64/include -I/usr/lib/jvm/java-1.8.0-openjdk-amd64/include/linux -Wall
lib_LTLIBRARIES = libioio.la

libioio_la_SOURCES = ../include/param.cpp ../include/hashmap.cpp ../include/apiexec.cpp main.cpp $(generated)
libioio_la_LDFLAGS = -module -rpath '$(libdir)' @PLATFORM_LDFLAGS@ -L/usr/lib/jvm/java-1.8.0-openjdk-amd64/jre/lib/amd64/server -ljvm

$(generated): api.json mkapi.bas
	$(sbasic) mkapi.bas > $@
	@touch main.cpp
