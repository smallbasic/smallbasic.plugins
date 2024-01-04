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

struct IOClass {
  IOClass(): _clazz(nullptr), _instance(nullptr) {}

  bool create(const char *path) {
    bool result;
    if (_instance != nullptr) {
      // error: already constructed
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

  bool invoke(const char *name, int value) {
    bool result = false;
    if (_instance != nullptr) {
      jmethodID method = env->GetMethodID(_clazz, name, "(I)V");
      if (method != nullptr) {
        env->CallVoidMethod(_instance, method, value);
        result = true;
      } else {
        env->ExceptionDescribe();
      }
    }
    return result;
  }

  bool open(int pin) {
    return invoke("open", pin);
  }
  
  bool write(int value) {
    return invoke("write", value);
  }
  
  private:
  jclass _clazz;
  jobject _instance;
};

IOClass analogInput;
IOClass digitalOutput;

static void cmd_digital_output_write(var_s *self, var_s *retval) {
  static int value = !value;
  digitalOutput.write(value);
}

static int cmd_openanaloginput(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int pin = get_param_int(argc, params, 0, 0);
  if (analogInput.create("net/sourceforge/smallbasic/ioio/AnalogInput") &&
      analogInput.open(pin)) {
    result = 1;
  } else {
    error(retval, "openAnalogInput() failed");
    result = 0;
  }
  return result;
}

static int cmd_opendigitaloutput(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int pin = get_param_int(argc, params, 0, 0);
  if (digitalOutput.create("net/sourceforge/smallbasic/ioio/DigitalOutput") &&
      digitalOutput.open(pin)) {
    map_init(retval);
    v_create_func(retval, "write", cmd_digital_output_write);
    result = 1;
  } else {
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
  options[1].optionString = (char *)"-Dioio.SerialPorts=ACM0";
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

void sblib_close(void) {
  jvm->DetachCurrentThread();
  jvm->DestroyJavaVM();
  env = nullptr;
  jvm = nullptr;
}

