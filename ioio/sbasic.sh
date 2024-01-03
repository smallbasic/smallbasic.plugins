#!/bin/bash

export LD_LIBRARY_PATH=/usr/lib/jvm/java-1.8.0-openjdk-amd64/jre/lib/amd64/server:$LD_LIBRARY_PATH

#valgrind --tool=callgrind
# type "backtrace" to see stack trace 
# gdb -ex run --args \
# valgrind --leak-check=full --track-origins=yes \
#gdb -ex='set confirm on' -ex=run -ex=quit --args \

~/src/SmallBASIC/src/platform/console/sbasic --module-path="/home/chrisws/src/smallbasic.plugins/ioio/.libs:/home/chrisws/src/smallbasic.plugins/debug/.libs" $1
