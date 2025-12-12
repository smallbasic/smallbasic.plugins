// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2026 Chris Warren-Smith

#include "config.h"

#include <cstring>
#include <cstdio>
#include "robin-hood-hashing/src/include/robin_hood.h"
#include "include/log.h"
#include "include/var.h"
#include "include/module.h"
#include "include/param.h"

#define CLASS_IOTASK_ID 1

int g_nextId = 1;

FUNC_SIG lib_func[] = {
};

FUNC_SIG lib_proc[] = {
};

SBLIB_API int sblib_proc_count() {
  return 0;
}

SBLIB_API int sblib_func_count() {
  return 0;
}

//
// Program startup
//
int sblib_init(const char *sourceFile) {
  return 1;
}

#if defined(ANDROID_MODULE)
//
// Retrieves the _app->activity->clazz value sent from App/JNI to Java to IOIOLoader
//
extern "C" JNIEXPORT void JNICALL Java_ioio_smallbasic_android_ModuleLoader_init
  (JNIEnv *env, jclass clazz, jobject activity) {
  logEntered();
  jclass longClass = env->FindClass("java/lang/Long");
  jmethodID longValueMethod = env->GetMethodID(longClass, "longValue", "()J");
  g_activity = (jobject)env->CallLongMethod(activity, longValueMethod);
  g_env = env;
}

#endif

//
// Release ioio variables falling out of scope
//
SBLIB_API void sblib_free(int cls_id, int id) {
  if (id != -1) {
    switch (cls_id) {
    case CLASS_IOTASK_ID:
      break;
    }
  }
}

//
// Program termination
//
void sblib_close(void) {
}
