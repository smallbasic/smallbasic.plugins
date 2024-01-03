// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2020 Chris Warren-Smith

#include "config.h"
#include "include/var.h"
#include "include/module.h"
#include "include/param.h"

#include <cstring>
#include <cstdio>
#include <jni.h>
#include <pthread.h>

JNIEnv *env;
JavaVM *jvm;
jclass analogInputClass = nullptr;
jobject analogInput = nullptr;

// void callVoidMethod(jmethodID method, int n) {
//   env->CallVoidMethod(instance, method, n);
// }

// jmethodID getMethodID(const char *name, const char *signature) {
//   return env->GetMethodID(clazz, name, signature);
// }

// jmethodID getStaticMethodId(const char *name, const char *signature) {
//   return env->GetStaticMethodID(clazz, name, signature);
// }

jobject createInstance(jclass clazz) {
  jobject result = nullptr;
  if (clazz == nullptr) {
    env->ExceptionDescribe();
  } else {
    jmethodID constructor = env->GetMethodID(clazz, "<init>", "()V");
    if (constructor == nullptr) {
      env->ExceptionDescribe();
    } else {
      result = env->NewObject(clazz, constructor);
    }
  }
  return result;
}

int sblib_init(const char *sourceFile) {
  JavaVMInitArgs vm_args;
  JavaVMOption options[2];
  options[0].optionString = (char *)"-Djava.class.path=./target/ioio-1.0-jar-with-dependencies.jar";
  options[1].optionString = (char *)"-Dioio.SerialPorts=ACM0";
  vm_args.version = JNI_VERSION_1_8;
  vm_args.nOptions = 2;
  vm_args.options = options;
  vm_args.ignoreUnrecognized = 0;
  int result = (JNI_CreateJavaVM(&jvm, (void **)&env, &vm_args) == JNI_OK);
  if (!result) {
    fprintf(stderr, "Unable to create JVM\n");
  }
  return result;
}

void sblib_close(void) {
  jvm->DestroyJavaVM();
  analogInputClass = nullptr;
  analogInput = nullptr;
  env = nullptr;
  jvm = nullptr;
}

static int cmd_openanaloginput(int argc, slib_par_t *params, var_t *retval) {
  int pin = get_param_int(argc, params, 0, 0);
  int result = 0;
  if (analogInput == nullptr && jvm->AttachCurrentThread((void**)&env, nullptr) == JNI_OK) {
    analogInputClass = env->FindClass("net/sourceforge/smallbasic/ioio/AnalogInput");
    analogInput = createInstance(analogInputClass);
    if (analogInput != nullptr) {
      jmethodID method = env->GetMethodID(analogInputClass, "openInput", "(I)V");
      if (method != nullptr) {
        env->CallVoidMethod(analogInput, method, pin);
        result = 1;
      } else {
        env->ExceptionDescribe();
      }
    }
    jvm->DetachCurrentThread();
  }
  if (!result) {
    error(retval, "openAnalogInput() failed");
  }
  return result;
}

FUNC_SIG lib_func[] = {
  {1, 1, "OPENANALOGINPUT", cmd_openanaloginput},
};

FUNC_SIG lib_proc[] = {
};

SBLIB_API int sblib_proc_count() {
  return (sizeof(lib_proc) / sizeof(lib_proc[0]));
}

SBLIB_API int sblib_func_count() {
  return (sizeof(lib_func) / sizeof(lib_func[0]));
}


