// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2024 Chris Warren-Smith

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

jclass digitalOutputClass = nullptr;
jobject digitalOutput = nullptr;

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

int invokeIV(jclass clazz, jobject instance, const char *name, int value) {
  int result = 0;
  if (instance != nullptr) {
    jmethodID method = env->GetMethodID(clazz, name, "(I)V");
    if (method != nullptr) {
      env->CallVoidMethod(instance, method, value);
      result = 1;
    } else {
      env->ExceptionDescribe();
    }
  }
  return result;
}

int invokeOpen(jclass clazz, jobject instance, int pin) {
  return invokeIV(clazz, instance, "open", pin);
}

static void cmd_digital_output_write(var_s *self, var_s *retval) {
  int value = 0;
  if (digitalOutput != nullptr && jvm->AttachCurrentThread((void**)&env, nullptr) == JNI_OK) {
    invokeIV(digitalOutputClass, digitalOutput, "write", value);
    jvm->DetachCurrentThread();
  }
}

static int cmd_openanaloginput(int argc, slib_par_t *params, var_t *retval) {
  int pin = get_param_int(argc, params, 0, 0);
  int result = 0;
  if (analogInput == nullptr && jvm->AttachCurrentThread((void**)&env, nullptr) == JNI_OK) {
    analogInputClass = env->FindClass("net/sourceforge/smallbasic/ioio/AnalogInput");
    analogInput = createInstance(analogInputClass);
    result = invokeOpen(analogInputClass, analogInput, pin);
    jvm->DetachCurrentThread();
  }
  if (!result) {
    error(retval, "openAnalogInput() failed");
  }
  return result;
}

static int cmd_opendigitaloutput(int argc, slib_par_t *params, var_t *retval) {
  int pin = get_param_int(argc, params, 0, 0);
  int result = 0;
  if (digitalOutput == nullptr && jvm->AttachCurrentThread((void**)&env, nullptr) == JNI_OK) {
    digitalOutputClass = env->FindClass("net/sourceforge/smallbasic/ioio/DigitalOutput");
    digitalOutput = createInstance(digitalOutputClass);
    if (digitalOutput != nullptr) {
      result = invokeOpen(digitalOutputClass, digitalOutput, pin);
    }
    jvm->DetachCurrentThread();
  }
  if (!result) {
    error(retval, "openDigitalOutput() failed");
  } else {
    map_init(retval);
    v_create_func(retval, "write", cmd_digital_output_write);
  }
  return result;
}

FUNC_SIG lib_func[] = {
  {1, 1, "OPENANALOGINPUT", cmd_openanaloginput},
  {1, 1, "OPENDIGITALOUTPUT", cmd_opendigitaloutput},
};

FUNC_SIG lib_proc[] = {
};

SBLIB_API int sblib_proc_count() {
  return (sizeof(lib_proc) / sizeof(lib_proc[0]));
}

SBLIB_API int sblib_func_count() {
  return (sizeof(lib_func) / sizeof(lib_func[0]));
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

