// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2024 Chris Warren-Smith

#include "config.h"
#include "log.h"
#include "include/var.h"
#include "include/module.h"
#include "include/param.h"

#include <cstring>
#include <cstdio>
#include <jni.h>
#include <pthread.h>
#include "robin-hood-hashing/src/include/robin_hood.h"

struct IOTask;

JNIEnv *g_env;
JavaVM *g_jvm;
IOTask *g_ioioTask;
int     g_nextId = 1;
jobject g_activity;

#if defined(ANDROID_MODULE)
  #define attachCurrentThread() g_jvm->AttachCurrentThread(&g_env, nullptr)
  #define detachCurrentThread() g_jvm->DetachCurrentThread()
#else
  #define attachCurrentThread()
  #define detachCurrentThread()
#endif

#define CLASS_ANALOGINPUT "ioio/smallbasic/AnalogInputImpl"
#define CLASS_DIGITALINPUT "ioio/smallbasic/DigitalInputImpl"
#define CLASS_DIGITALOUTPUT "ioio/smallbasic/DigitalOutputImpl"
#define CLASS_PULSEINPUT "ioio/smallbasic/PulseInputImpl"
#define CLASS_PWMOUTPUT "ioio/smallbasic/PwmOutputImpl"
#define CLASS_CAPSENSE "ioio/smallbasic/CapsenseImpl"
#define CLASS_TWIMASTER "ioio/smallbasic/TwiMasterImpl"
#define CLASS_SPIMASTER "ioio/smallbasic/SpiMasterImpl"
#define CLASS_IOIO "ioio/smallbasic/IOIOImpl"
#define CLASS_IOTASK_ID 1
#define ARRAY_SIZE 10

#if defined(ANDROID_MODULE)
//
// calls MainActivity.findClass() to return the loaded jclass for path
//
jclass findClass(const char *path) {
  jclass clazz = g_env->GetObjectClass(g_activity);
  jmethodID methodId = g_env->GetMethodID(clazz, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
  jstring className = g_env->NewStringUTF(path);
  jclass result = (jclass)g_env->CallObjectMethod(g_activity, methodId, className);
  g_env->DeleteLocalRef(className);
  g_env->DeleteLocalRef(clazz);
  return reinterpret_cast<jclass>(g_env->NewGlobalRef(result));
}

jobject createInstance(jclass clazz) {
  jmethodID constructor = g_env->GetMethodID(clazz, "<init>", "()V");
  jobject result;
  if (constructor != nullptr) {
    result = g_env->NewObject(clazz, constructor);
    result = reinterpret_cast<jclass>(g_env->NewGlobalRef(result));
  } else {
    result = nullptr;
  }
  return result;
}

#else
jclass findClass(const char *path) {
  return g_env->FindClass(path);
}

jobject createInstance(jclass clazz) {
  jmethodID constructor = g_env->GetMethodID(clazz, "<init>", "()V");
  jobject result;
  if (constructor != nullptr) {
    result = g_env->NewObject(clazz, constructor);
  } else {
    result = nullptr;
  }
  return result;
}
#endif

struct IOTask {
  IOTask():
    _clazz(nullptr),
    _instance(nullptr),
    _array(nullptr) {
  }

  virtual ~IOTask() {
    attachCurrentThread();
    if (_array) {
      g_env->DeleteLocalRef(_array);
    }
#if defined(ANDROID_MODULE)
    g_env->DeleteGlobalRef(_clazz);
    g_env->DeleteGlobalRef(_instance);
#endif
    detachCurrentThread();
    _clazz = nullptr;
    _instance = nullptr;
    _array = nullptr;
  }

  bool create(const char *path, var_s *retval) {
    bool result;
    if (_instance != nullptr) {
      error(retval, "Internal error - already constructed");
      result = false;
    } else {
      attachCurrentThread();
      _clazz = findClass(path);
      if (_clazz != nullptr) {
        _instance = createInstance(_clazz);
      }
      result = _instance != nullptr;
      if (!result) {
        checkException(retval);
      }
      detachCurrentThread();
    }
    return result;
  }

  bool checkException(var_s *retval) {
    auto exc = g_env->ExceptionOccurred();
    if (exc) {
      if (retval) {
        g_env->ExceptionClear();
        jclass clazz = g_env->FindClass("java/lang/Throwable");
        jmethodID methodId = g_env->GetMethodID(clazz, "getMessage", "()Ljava/lang/String;");
        jstring jstr = (jstring) g_env->CallObjectMethod(exc, methodId);
        const char *message = g_env->GetStringUTFChars(jstr, JNI_FALSE);
        error(retval, message);
        g_env->ReleaseStringUTFChars(jstr, message);
      } else {
        g_env->ExceptionDescribe();
        g_env->ExceptionClear();
      }
    }
    return exc;
  }

  // boolean foo(void)
  int invokeBoolVoid(const char *name, var_s *retval) {
    int result = 0;
    if (_instance != nullptr) {
      jmethodID method = g_env->GetMethodID(_clazz, name, "()Z");
      int value = 0;
      if (method != nullptr) {
        value = g_env->CallBooleanMethod(_instance, method);
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
      attachCurrentThread();
      jmethodID method = g_env->GetMethodID(_clazz, name, "()F");
      var_num_t value = 0;
      if (method != nullptr) {
        value = g_env->CallFloatMethod(_instance, method);
      }
      if (!checkException(retval)) {
        v_setreal(retval, value);
        result = 1;
      }
      detachCurrentThread();
    }
    return result;
  }

  // int foo(void)
  int invokeIntVoid(const char *name, var_s *retval) {
    int result = 0;
    if (_instance != nullptr) {
      attachCurrentThread();
      jmethodID method = g_env->GetMethodID(_clazz, name, "()I");
      int value = 0;
      if (method != nullptr) {
        value = g_env->CallIntMethod(_instance, method);
      }
      if (!checkException(retval)) {
        v_setint(retval, value);
        result = 1;
      }
      detachCurrentThread();
    }
    return result;
  }

  // void foo(boolean)
  int invokeVoidBool(const char *name, int value, var_s *retval) {
    int result = 0;
    if (_instance != nullptr) {
      attachCurrentThread();
      jmethodID method = g_env->GetMethodID(_clazz, name, "(Z)V");
      if (method != nullptr) {
        g_env->CallVoidMethod(_instance, method, value);
      }
      if (!checkException(retval)) {
        result = 1;
      }
      detachCurrentThread();
    }
    return result;
  }

  // void foo(float)
  int invokeVoidFloat(const char *name, var_num_t value, var_s *retval) {
    int result = 0;
    if (_instance != nullptr) {
      attachCurrentThread();
      jmethodID method = g_env->GetMethodID(_clazz, name, "()F");
      if (method != nullptr) {
        g_env->CallVoidMethod(_instance, method, value);
      }
      if (!checkException(retval)) {
        result = 1;
      }
      detachCurrentThread();
    }
    return result;
  }

  // void foo(int)
  int invokeVoidInt(const char *name, int value, var_s *retval) {
    int result = 0;
    if (_instance != nullptr) {
      attachCurrentThread();
      jmethodID method = g_env->GetMethodID(_clazz, name, "(I)V");
      if (method != nullptr) {
        g_env->CallVoidMethod(_instance, method, value);
      }
      if (!checkException(retval)) {
        result = 1;
      }
      detachCurrentThread();
    }
    return result;
  }

  // void foo(int, int)
  int invokeVoidInt2(const char *name, int value1, int value2, var_s *retval) {
    int result = 0;
    if (_instance != nullptr) {
      attachCurrentThread();
      jmethodID method = g_env->GetMethodID(_clazz, name, "(II)V");
      if (method != nullptr) {
        g_env->CallVoidMethod(_instance, method, value1, value2);
      }
      if (!checkException(retval)) {
        result = 1;
      }
      detachCurrentThread();
    }
    return result;
  }

  // void foo(int, int, int, int)
  int invokeVoidInt4(const char *name, int value1, int value2, int value3, int value4, var_s *retval) {
    int result = 0;
    if (_instance != nullptr) {
      attachCurrentThread();
      jmethodID method = g_env->GetMethodID(_clazz, name, "(IIII)V");
      if (method != nullptr) {
        g_env->CallVoidMethod(_instance, method, value1, value2, value3, value4);
      }
      if (!checkException(retval)) {
        result = 1;
      }
      detachCurrentThread();
    }
    return result;
  }

  // void foo(void)
  int invokeVoidVoid(const char *name, var_s *retval) {
    int result = 0;
    if (_instance != nullptr) {
      attachCurrentThread();
      jmethodID method = g_env->GetMethodID(_clazz, name, "()V");
      if (method != nullptr) {
        g_env->CallVoidMethod(_instance, method);
      }
      if (!checkException(retval)) {
        result = 1;
      }
      detachCurrentThread();
    }
    return result;
  }

  // int readWrite(int address, byte[] write) {
  int invokeReadWrite(int argc, slib_par_t *arg, var_s *retval) {
    int result = 0;
    if (_instance != nullptr) {
      attachCurrentThread();
      jmethodID method = g_env->GetMethodID(_clazz, "readWrite", "(II[BI)J");
      var_int_t value = 0;
      if (method != nullptr) {
        auto address = get_param_int(argc, arg, 0, 0);
        auto readBytes = get_param_int(argc, arg, 1, 2);
        populateByteArray(argc, arg, 2);
        value = g_env->CallIntMethod(_instance, method, address, readBytes, _array, argc - 1);
      }
      if (!checkException(retval)) {
        v_setint(retval, value);
        result = 1;
      }
      detachCurrentThread();
    }
    return result;
  }

  // int write(int address, byte[] write) {
  int invokeWrite(int argc, slib_par_t *arg, var_s *retval) {
    int result = 0;
    if (_instance != nullptr) {
      attachCurrentThread();
      jmethodID method = g_env->GetMethodID(_clazz, "write", "(I[BI)V");
      if (method != nullptr) {
        auto address = get_param_int(argc, arg, 0, 0);
        populateByteArray(argc, arg, 1);
        g_env->CallVoidMethod(_instance, method, address, _array, argc - 1);
      }
      if (!checkException(retval)) {
        result = 1;
      }
      detachCurrentThread();
    }
    return result;
  }

  // populate the java byte array with the contents of the basic array
  void populateByteArray(int argc, slib_par_t *params, int offset) {
    if (!_array) {
      _array = g_env->NewByteArray(ARRAY_SIZE);
    }
    jbyte *elements = g_env->GetByteArrayElements(_array, nullptr);
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
    g_env->ReleaseByteArrayElements(_array, elements, 0);
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

robin_hood::unordered_map<int, IOTask> g_ioTaskMap;

static int get_io_class_id(var_s *map, var_s *retval) {
  int result = -1;
  if (is_map(map)) {
    int id = map->v.m.id;
    if (id != -1 && g_ioTaskMap.find(id) != g_ioTaskMap.end()) {
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
  auto readBytes = get_param_int(argc, arg, 1, 0);
  if (argc < 2) {
    error(retval, "TwiMaster.readWrite(address, read-bytes, [data]", 2, ARRAY_SIZE);
  } else if (readBytes < 1 || readBytes > 8) {
    error(retval, "read-bytes value out of range. Expected a number between 1 and 8");
  } else {
    int id = get_io_class_id(self, retval);
    if (id != -1) {
      result = g_ioTaskMap.at(id).invokeReadWrite(argc, arg, retval);
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
      result = g_ioTaskMap.at(id).invokeWrite(argc, arg, retval);
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
      result = g_ioTaskMap.at(id).invokeVoidInt2("write", address, data, retval);
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

//
// Program startup
//
int sblib_init(const char *sourceFile) {
#if defined(DESKTOP_MODULE)
  JavaVMInitArgs vm_args;
  JavaVMOption options[3];
  options[0].optionString = (char *)"-Djava.class.path=./ioio-1.0-jar-with-dependencies.jar";
  options[1].optionString = (char *)"-Dioio.SerialPorts=IOIO0";
  options[2].optionString = (char *)"-Xrs";
  //options[2].optionString = "-Xdebug";
  //options[3].optionString = "-agentlib:jdwp=transport=dt_socket,server=y,address=5005,suspend=y";
  //options[2].optionString = (char *)"-Xcheck:jni";
  vm_args.version = JNI_VERSION_1_8;
  vm_args.nOptions = 3;
  vm_args.options = options;
  vm_args.ignoreUnrecognized = 0;
  int result = (JNI_CreateJavaVM(&g_jvm, (void **)&g_env, &vm_args) == JNI_OK &&
                g_jvm->AttachCurrentThread((void **)&g_env, nullptr) == JNI_OK);
  if (!result) {
    fprintf(stderr, "Failed to create JVM\n");
  }
#else
  int result = 1;
#endif
  if (g_jvm == nullptr) {
    result = 0;
  } else {
    g_ioioTask = new IOTask();
    var_t retval;
    if (!g_ioioTask || !g_ioioTask->create(CLASS_IOIO, &retval)) {
      fprintf(stderr, "Failed to IOIOTask: %s\n", v_getstr(&retval));
      result = 0;
    }
  }
  return result;
}

#if defined(ANDROID_MODULE)
//
// Stores the Android JavaVM reference
//
extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void* reserved) {
  logEntered();
  g_jvm = vm;

  jint result;
  if (g_jvm->GetEnv((void **)&g_env, JNI_VERSION_1_6) != JNI_OK) {
    result = JNI_ERR;
  } else {
    result = JNI_VERSION_1_6;
  }
  logLeaving();
  return result;
}

//
// Retrieves the _app->activity->clazz value sent from App/JNI to Java to IOIOLoader
//
extern "C" JNIEXPORT void JNICALL Java_ioio_smallbasic_android_IOIOLoader_init
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
      if (g_ioTaskMap.find(id) != g_ioTaskMap.end()) {
        g_ioTaskMap.at(id).invokeVoidVoid("close", nullptr);
        g_ioTaskMap.erase(id);
      }
      break;
    }
  }
}

//
// Program termination
//
void sblib_close(void) {
  if (g_ioioTask) {
    g_ioioTask->invokeVoidVoid("close", nullptr);
    delete g_ioioTask;
  }
  if (!g_ioTaskMap.empty()) {
    fprintf(stderr, "IOTask leak detected\n");
    g_ioTaskMap.clear();
  }
#if defined(DESKTOP_MODULE)
  if (g_jvm) {
    g_jvm->DetachCurrentThread();
  }
  // calling this hangs
  //jvm->DestroyJavaVM();
  g_env = nullptr;
  g_jvm = nullptr;
#endif
}
