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

  bool invokeIV(const char *name, int value) {
    bool result = false;
    if (_instance != nullptr) {
      jmethodID method = env->GetMethodID(_clazz, name, "(I)V");
      if (method != nullptr) {
        env->CallVoidMethod(_instance, method, value);
      }
      auto exc = env->ExceptionOccurred();
      if (exc) {
        env->ExceptionDescribe();
        env->ExceptionClear();
      } else {
        result = (method != nullptr);
      }
    }
    return result;
  }

  bool open(int pin) {
    return invokeIV("open", pin);
  }

  bool write(int value) {
    return invokeIV("write", value);
  }

  private:
  jclass _clazz;
  jobject _instance;
};

#define CLS_IOCLASS 1
robin_hood::unordered_map<int, IOClass> _classMap;
int _nextId = 1;

static int get_io_class_id(var_s *map) {
  int result = -1;
  if (is_map(map)) {
    int id = map->v.m.id;
    if (id != -1 && _classMap.find(id) != _classMap.end()) {
      result = id;
    }
  }
  return result;
}

static int cmd_digital_output_write(var_s *self, int param_count, slib_par_t *params, var_s *retval) {
  int result = 0;
  if (param_count == 1) {
    int id = get_io_class_id(self);
    if (id != -1) {
      int value = get_param_int(param_count, params, 0, 0);
      _classMap.at(id).write(value);
      result = 1;
    } else {
      error(retval, "IOClass not found");
    }
  } else {
    error(retval, "Missing value argument");
  }
  return result;
}

static int cmd_openanaloginput(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int pin = get_param_int(argc, params, 0, 0);
  int id = ++_nextId;
  IOClass &input = _classMap[id];
  if (input.create("net/sourceforge/smallbasic/ioio/AnalogInput") &&
      input.open(pin)) {
    map_init_id(retval, id, CLS_IOCLASS);
    //v_create_func(retval, "write", cmd_digital_output_write);
    result = 1;
  } else {
    _classMap.erase(id);
    error(retval, "openAnalogInput() failed");
    result = 0;
  }
  return result;
}

static int cmd_opendigitaloutput(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int pin = get_param_int(argc, params, 0, 0);
  int id = ++_nextId;
  IOClass &output = _classMap[id];
  if (output.create("net/sourceforge/smallbasic/ioio/DigitalOutput") &&
      output.open(pin)) {
    map_init_id(retval, id, CLS_IOCLASS);
    v_create_func(retval, "write", cmd_digital_output_write);
    result = 1;
  } else {
    _classMap.erase(id);
    error(retval, "openDigitalOutput() failed");
    result = 0;
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
    case CLS_IOCLASS:
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
  // hangs
  //jvm->DestroyJavaVM();
  env = nullptr;
  jvm = nullptr;
}

