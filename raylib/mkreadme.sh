#!/bin/bash

~/src/SmallBASIC/src/platform/console/sbasic mkreadme.bas `grep RAYLIB_VERSION raylib/src/raylib.h | awk '{print $3}' | sed 's/\"//g'` > README.md

