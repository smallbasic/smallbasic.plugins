# SmallBASIC
# Copyright(C) 2024 Chris Warren-Smith.
#
# This program is distributed under the terms of the GPL v2.0 or later
# Download the GNU Public License (GPL) from www.gnu.org
# 

JNI_PATH := $(call my-dir)
LOCAL_PATH := $(JNI_PATH)

include $(CLEAR_VARS)
LOCAL_MODULE     := ioio
LOCAL_CFLAGS     := -DHAVE_CONFIG_H=1 -DANDROID_MODULE -Wno-unknown-pragmas -I../ -I../../ -I../include
LOCAL_SRC_FILES  := ../../include/param.cpp \
                    ../../include/hashmap.cpp \
                    ../../include/apiexec.cpp \
                    ../main.cpp
LOCAL_LDLIBS     := -llog -landroid
LOCAL_LDFLAGS += "-Wl,-z,max-page-size=16384"
LOCAL_LDFLAGS += "-Wl,-z,common-page-size=16384"
include $(BUILD_SHARED_LIBRARY)

