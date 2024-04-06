// This file is part of SmallBASIC
//
// Copyright(C) 2024 Chris Warren-Smith.
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//

#pragma once

#include "config.h"

#if defined(ANDROID_MODULE)
 #include <android/log.h>
 #if defined(_DEBUG)
   #define deviceLog(...) __android_log_print(ANDROID_LOG_ERROR, "smallbasic", __VA_ARGS__)
 #else
   #define deviceLog(...) __android_log_print(ANDROID_LOG_INFO, "smallbasic", __VA_ARGS__)
 #endif
#else
 #define deviceLog(...) printf(__VA_ARGS__)
#endif

#if defined(_DEBUG)
 #define trace(...) deviceLog(__VA_ARGS__)
#else
 #define trace(...)
#endif

#define logEntered() trace("%s entered (%s %d)", __FUNCTION__, __FILE__, __LINE__);
#define logLeaving() trace("%s leaving (%s %d)", __FUNCTION__, __FILE__, __LINE__);
