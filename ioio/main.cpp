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

struct IOTask;

JNIEnv *env;
JavaVM *jvm;
IOTask *ioioTask;
int nextId = 1;

#define CLASS_ANALOGINPUT "net/sourceforge/smallbasic/ioio/AnalogInputImpl"
#define CLASS_DIGITALINPUT "net/sourceforge/smallbasic/ioio/DigitalInputImpl"
#define CLASS_DIGITALOUTPUT "net/sourceforge/smallbasic/ioio/DigitalOutputImpl"
#define CLASS_PULSEINPUT "net/sourceforge/smallbasic/ioio/PulseInputImpl"
#define CLASS_PWMOUTPUT "net/sourceforge/smallbasic/ioio/PwmOutputImpl"
#define CLASS_CAPSENSE "net/sourceforge/smallbasic/ioio/CapsenseImpl"
#define CLASS_TWIMASTER "net/sourceforge/smallbasic/ioio/TwiMasterImpl"
#define CLASS_SPIMASTER "net/sourceforge/smallbasic/ioio/SpiMasterImpl"
#define CLASS_IOIO "net/sourceforge/smallbasic/ioio/IOIOImpl"
#define CLASS_IOTASK_ID 1
#define ARRAY_SIZE 10

struct IOTask {
  IOTask():
    _clazz(nullptr),
    _instance(nullptr),
    _array(nullptr) {
  }

  virtual ~IOTask() {
    if (_array) {
      env->DeleteLocalRef(_array);
    }
    _clazz = nullptr;
    _instance = nullptr;
    _array = nullptr;
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

  bool checkException(var_s *retval) {
    auto exc = env->ExceptionOccurred();
    if (exc) {
      if (retval) {
        jclass clazz = env->FindClass("java/lang/Object");
        jmethodID methodId = env->GetMethodID(clazz, "toString", "()Ljava/lang/String;");
        jstring jstr = (jstring) env->CallObjectMethod(exc, methodId);
        const char *message = env->GetStringUTFChars(jstr, JNI_FALSE);
        error(retval, message);
        env->ReleaseStringUTFChars(jstr, message);
      } else {
        env->ExceptionDescribe();
        env->ExceptionClear();
      }
    }
    return exc;
  }

  // boolean foo(void)
  int invokeBoolVoid(const char *name, var_s *retval) {
    int result = 0;
    if (_instance != nullptr) {
      jmethodID method = env->GetMethodID(_clazz, name, "()Z");
      int value = 0;
      if (method != nullptr) {
        value = env->CallBooleanMethod(_instance, method);
      }
      if (!checkException(retval)) {
        v_setint(retval, value);
        result = 1;
      }
    }
    return result;
  }

  // float foo(void)
  int invokeFloatVoid(const char *name, var_s *retval) {
    int result = 0;
    if (_instance != nullptr) {
      jmethodID method = env->GetMethodID(_clazz, name, "()F");
      var_num_t value = 0;
      if (method != nullptr) {
        value = env->CallFloatMethod(_instance, method);
      }
      if (!checkException(retval)) {
        v_setreal(retval, value);
        result = 1;
      }
    }
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
      if (!checkException(retval)) {
        v_setint(retval, value);
        result = 1;
      }
    }
    return result;
  }

  // void foo(boolean)
  int invokeVoidBool(const char *name, int value, var_s *retval) {
    int result = 0;
    if (_instance != nullptr) {
      jmethodID method = env->GetMethodID(_clazz, name, "(Z)V");
      if (method != nullptr) {
        env->CallVoidMethod(_instance, method, value);
      }
      if (!checkException(retval)) {
        result = 1;
      }
    }
    return result;
  }

  // void foo(float)
  int invokeVoidFloat(const char *name, var_num_t value, var_s *retval) {
    int result = 0;
    if (_instance != nullptr) {
      jmethodID method = env->GetMethodID(_clazz, name, "()F");
      if (method != nullptr) {
        env->CallVoidMethod(_instance, method, value);
      }
      if (!checkException(retval)) {
        result = 1;
      }
    }
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
      if (!checkException(retval)) {
        result = 1;
      }
    }
    return result;
  }

  // void foo(int, int)
  int invokeVoidInt2(const char *name, int value1, int value2, var_s *retval) {
    int result = 0;
    if (_instance != nullptr) {
      jmethodID method = env->GetMethodID(_clazz, name, "(II)V");
      if (method != nullptr) {
        env->CallVoidMethod(_instance, method, value1, value2);
      }
      if (!checkException(retval)) {
        result = 1;
      }
    }
    return result;
  }

  // void foo(int, int, int, int)
  int invokeVoidInt4(const char *name, int value1, int value2, int value3, int value4, var_s *retval) {
    int result = 0;
    if (_instance != nullptr) {
      jmethodID method = env->GetMethodID(_clazz, name, "(IIII)V");
      if (method != nullptr) {
        env->CallVoidMethod(_instance, method, value1, value2, value3, value4);
      }
      if (!checkException(retval)) {
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
      if (!checkException(retval)) {
        result = 1;
      }
    }
    return result;
  }

  // int readWrite(int address, byte[] write) {
  int invokeReadWrite(int argc, slib_par_t *arg, var_s *retval) {
    int result = 0;
    if (_instance != nullptr) {
      jmethodID method = env->GetMethodID(_clazz, "readWrite", "(II[BI)J");
      var_int_t value = 0;
      if (method != nullptr) {
        auto address = get_param_int(argc, arg, 0, 0);
        auto readBytes = get_param_int(argc, arg, 1, 2);
        populateByteArray(argc, arg, 2);
        value = env->CallIntMethod(_instance, method, address, readBytes, _array, argc - 1);
      }
      if (!checkException(retval)) {
        v_setint(retval, value);
        result = 1;
      }
    }
    return result;
  }

  // int write(int address, byte[] write) {
  int invokeWrite(int argc, slib_par_t *arg, var_s *retval) {
    int result = 0;
    if (_instance != nullptr) {
      jmethodID method = env->GetMethodID(_clazz, "write", "(I[BI)V");
      if (method != nullptr) {
        auto address = get_param_int(argc, arg, 0, 0);
        populateByteArray(argc, arg, 1);
        env->CallVoidMethod(_instance, method, address, _array, argc - 1);
      }
      if (!checkException(retval)) {
        result = 1;
      }
    }
    return result;
  }

  // populate the java byte array with the contents of the basic array
  void populateByteArray(int argc, slib_par_t *params, int offset) {
    if (!_array) {
      _array = env->NewByteArray(ARRAY_SIZE);
    }
    jbyte *elements = env->GetByteArrayElements(_array, nullptr);
    if ((argc - offset) == 1 && is_param_array(argc, params, offset)) {
      // argument is an array (assume of ints)
      var_s *array = params[offset].var_p;
      int size = v_asize(array);
      for (int i = 0; i < size && i < ARRAY_SIZE; i++) {
        var_s *elem = v_elem(array, i);
        elements[i] = v_is_type(elem, V_INT) ? elem->v.i : elem->v.n;
      }
    } else {
      for (int i = offset, j = 0; i < argc && i < ARRAY_SIZE; i++, j++) {
        elements[j] = get_param_int(argc, params, i, 0);
      }
    }
    // make the changes available to the java side
    env->ReleaseByteArrayElements(_array, elements, 0);
  }

  int open(int pin, var_s *retval) {
    return invokeVoidInt("open", pin, retval);
  }

  int open2(int pin1, int pin2, var_s *retval) {
    return invokeVoidInt2("open", pin1, pin2, retval);
  }

  int open4(int pin1, int pin2, int pin3, int pin4, var_s *retval) {
    return invokeVoidInt4("open", pin1, pin2, pin3, pin4, retval);
  }

  private:
  jclass _clazz;
  jobject _instance;
  jbyteArray _array;
};

robin_hood::unordered_map<int, IOTask> _ioTaskMap;

static int get_io_class_id(var_s *map, var_s *retval) {
  int result = -1;
  if (is_map(map)) {
    int id = map->v.m.id;
    if (id != -1 && _ioTaskMap.find(id) != _ioTaskMap.end()) {
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "IOTask not found");
  }
  return result;
}

static int cmd_twimaster_readwrite(var_s *self, int argc, slib_par_t *arg, var_s *retval) {
  int result = 0;
  if (argc < 2) {
    error(retval, "TwiMaster.readWrite(address, bytes, [data]", 2, ARRAY_SIZE);
  } else {
    int id = get_io_class_id(self, retval);
    if (id != -1) {
      result = _ioTaskMap.at(id).invokeReadWrite(argc, arg, retval);
    }
  }
  return result;
}

static int cmd_twimaster_write(var_s *self, int argc, slib_par_t *arg, var_s *retval) {
  int result = 0;
  if (argc < 2) {
    error(retval, "TwiMaster.write", 2, ARRAY_SIZE);
  } else {
    int id = get_io_class_id(self, retval);
    if (id != -1) {
      result = _ioTaskMap.at(id).invokeWrite(argc, arg, retval);
    }
  }
  return result;
}

static int cmd_spimaster_write(var_s *self, int argc, slib_par_t *arg, var_s *retval) {
  int result = 0;
  if (argc != 2) {
    error(retval, "SpiMaster.write", 2);
  } else {
    int id = get_io_class_id(self, retval);
    if (id != -1) {
      auto address = get_param_int(argc, arg, 0, 0);
      auto data = get_param_int(argc, arg, 1, 0);
      result = _ioTaskMap.at(id).invokeVoidInt2("write", address, data, retval);
    }
  }
  return result;
}

static void create_twimaster(var_t *map) {
  v_create_callback(map, "write", cmd_twimaster_write);
  v_create_callback(map, "readWrite", cmd_twimaster_readwrite);
}

static void create_spimaster(var_t *map) {
  v_create_callback(map, "write", cmd_spimaster_write);
}

#include "api.h"

FUNC_SIG lib_func[] = {
  {1, 1, "OPENANALOGINPUT", cmd_openanaloginput},
  {1, 1, "OPENCAPSENSE", cmd_opencapsense},
  {1, 1, "OPENDIGITALINPUT", cmd_opendigitalinput},
  {1, 1, "OPENDIGITALOUTPUT", cmd_opendigitaloutput},
  {1, 1, "OPENPULSEINPUT", cmd_openpulseinput},
  {1, 1, "OPENPWMOUTPUT", cmd_openpwmoutput},
  {2, 2, "OPENTWIMASTER", cmd_opentwimaster},
  {4, 4, "OPENSPIMASTER", cmd_openspimaster},
};

FUNC_SIG lib_proc[] = {
  {0, 0, "BEGINBATCH", cmd_ioio_beginbatch},
  {0, 0, "DISCONNECT", cmd_ioio_disconnect},
  {0, 0, "ENDBATCH", cmd_ioio_endbatch},
  {0, 0, "HARDRESET", cmd_ioio_hardreset},
  {0, 0, "SOFTRESET", cmd_ioio_softreset},
  {0, 0, "SYNC", cmd_ioio_sync},
  {1, 1, "WAITFORCONNECT", cmd_ioio_waitforconnect},
  {0, 0, "WAITFORDISCONNECT", cmd_ioio_waitfordisconnect},
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

  ioioTask = new IOTask();
  if (!ioioTask || !ioioTask->create(CLASS_IOIO)) {
    fprintf(stderr, "Failed to IOIOTask\n");
    result = 0;
  }
  return result;
}

SBLIB_API void sblib_free(int cls_id, int id) {
  if (id != -1) {
    switch (cls_id) {
    case CLASS_IOTASK_ID:
      if (id != -1 && _ioTaskMap.find(id) != _ioTaskMap.end()) {
        _ioTaskMap.at(id).invokeVoidVoid("close", nullptr);
        _ioTaskMap.erase(id);
      }
      break;
    }
  }
}

void sblib_close(void) {
  if (ioioTask) {
    delete ioioTask;
  }
  if (!_ioTaskMap.empty()) {
    fprintf(stderr, "IOTask leak detected\n");
    _ioTaskMap.clear();
  }
  if (jvm) {
    jvm->DetachCurrentThread();
  }
  // calling this hangs
  //jvm->DestroyJavaVM();
  env = nullptr;
  jvm = nullptr;
}

