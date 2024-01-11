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
#include "robin-hood-hashing/src/include/robin_hood.h"

JNIEnv *env;
JavaVM *jvm;
int nextId = 1;

#define CLASS_ANALOGINPUT "net/sourceforge/smallbasic/ioio/AnalogInput"
#define CLASS_DIGITALINPUT "net/sourceforge/smallbasic/ioio/DigitalInput"
#define CLASS_DIGITALOUTPUT "net/sourceforge/smallbasic/ioio/DigitalOutput"
#define CLASS_PULSEINPUT "net/sourceforge/smallbasic/ioio/PulseInput"
#define CLASS_PWMOUTPUT "net/sourceforge/smallbasic/ioio/PwmOutput"
#define CLASS_CAPSENSE "net/sourceforge/smallbasic/ioio/Capsense"
#define CLASS_IOCLASS 1

struct IOClass {
  IOClass(): _clazz(nullptr), _instance(nullptr) {}

  virtual ~IOClass() {
    _clazz = nullptr;
    _instance = nullptr;
  }

  bool create(const char *path) {
    bool result;
    if (_instance != nullptr) {
      // error when already constructed
      result = false;
    } else {
      _clazz = env->FindClass(path);
      if (_clazz == nullptr) {
        env->ExceptionDescribe();
      } else {
        jmethodID constructor = env->GetMethodID(_clazz, "<init>", "()V");
        if (constructor == nullptr) {
          env->ExceptionDescribe();
        } else {
          _instance = env->NewObject(_clazz, constructor);
        }
      }
      result = _instance != nullptr;
    }
    return result;
  }

  bool checkException() {
    auto exc = env->ExceptionOccurred();
    if (exc) {
      env->ExceptionDescribe();
      env->ExceptionClear();
    }
    return exc;
  }

  // float foo(void)
  int invokeFloatVoid(const char *name, var_s *retval) {
    int result = 0;
    return result;
  }
  
  // int foo(void)
  int invokeIntVoid(const char *name, var_s *retval) {
    int result = 0;
    if (_instance != nullptr) {
      jmethodID method = env->GetMethodID(_clazz, name, "()I");
      int value = 0;
      if (method != nullptr) {
        value = env->CallIntMethod(_instance, method);
      }
      if (!checkException()) {
        v_setint(retval, value);
        result = 1;
      }
    }
    return result;
  }

  // void foo(float)
  int invokeVoidFloat(const char *name, var_num_t value, var_s *retval) {
    int result = 0;
    return result;
  }
  
  // void foo(int)
  int invokeVoidInt(const char *name, int value, var_s *retval) {
    int result = 0;
    if (_instance != nullptr) {
      jmethodID method = env->GetMethodID(_clazz, name, "(I)V");
      if (method != nullptr) {
        env->CallVoidMethod(_instance, method, value);
      }
      if (!checkException()) {
        result = 1;
      }
    }
    return result;
  }

  // void foo(void)
  int invokeVoidVoid(const char *name, var_s *retval) {
    int result = 0;
    if (_instance != nullptr) {
      jmethodID method = env->GetMethodID(_clazz, name, "()V");
      if (method != nullptr) {
        env->CallVoidMethod(_instance, method);
      }
      if (!checkException()) {
        result = 1;
      }
    }
    return result;
  }

  int open(int pin, var_s *retval) {
    return invokeVoidInt("open", pin, retval);
  }

  private:
  jclass _clazz;
  jobject _instance;
};

robin_hood::unordered_map<int, IOClass> _classMap;

static int get_io_class_id(var_s *map, var_s *retval) {
  int result = -1;
  if (is_map(map)) {
    int id = map->v.m.id;
    if (id != -1 && _classMap.find(id) != _classMap.end()) {
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "IOClass not found");
  }
  return result;
}

#include "api.h"

FUNC_SIG lib_func[] = {
  {1, 2, "OPENANALOGINPUT", cmd_openanaloginput},
  {1, 2, "OPENCAPSENSE", cmd_opencapsense},
  {1, 2, "OPENDIGITALINPUT", cmd_opendigitalinput},
  {1, 2, "OPENDIGITALOUTPUT", cmd_opendigitaloutput},
  {1, 2, "OPENPULSEINPUT", cmd_openpulseinput},
  {1, 2, "OPENPWMOUTPUT", cmd_openpwmoutput},
};

FUNC_SIG lib_proc[] = {};

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
  options[1].optionString = (char *)"-Dioio.SerialPorts=IOIO0";
  //options[2].optionString = "-Xdebug";
  //options[3].optionString = "-agentlib:jdwp=transport=dt_socket,server=y,address=5005,suspend=y";
  //options[2].optionString = (char *)"-Xcheck:jni";
  vm_args.version = JNI_VERSION_1_8;
  vm_args.nOptions = 2;
  vm_args.options = options;
  vm_args.ignoreUnrecognized = 0;
  int result = (JNI_CreateJavaVM(&jvm, (void **)&env, &vm_args) == JNI_OK &&
                jvm->AttachCurrentThread((void **)&env, nullptr) == JNI_OK);
  if (!result) {
    fprintf(stderr, "Failed to create JVM\n");
  }
  return result;
}

SBLIB_API void sblib_free(int cls_id, int id) {
  if (id != -1) {
    switch (cls_id) {
    case CLASS_IOCLASS:
      _classMap.erase(id);
      break;
    }
  }
}

void sblib_close(void) {
  if (!_classMap.empty()) {
    fprintf(stderr, "IOClass leak detected\n");
    _classMap.clear();
  }
  jvm->DetachCurrentThread();
  // calling this hangs
  //jvm->DestroyJavaVM();
  env = nullptr;
  jvm = nullptr;
}

