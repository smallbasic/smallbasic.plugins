// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2024 Chris Warren-Smith

#include "config.h"
#include <cstring>
#include <cstdio>
#include "robin-hood-hashing/src/include/robin_hood.h"
#include "include/log.h"
#include "include/var.h"
#include "include/module.h"
#include "include/param.h"
#include "include/javaproxy.h"

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
#define SPI_WRITE_MAX 62
#define TWI_WRITE_MAX 255

struct IOTask : JavaProxy {
  IOTask() : JavaProxy() {
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

  // int readWrite(bytes, byte[] write) {
  int invokeSpiReadWrite(int argc, slib_par_t *arg, var_s *retval) {
    int result = 0;
    int writeLen = populateByteArray(argc, arg, 1, SPI_WRITE_MAX);
    if (writeLen > SPI_WRITE_MAX) {
      error(retval, "write array", 1, SPI_WRITE_MAX);
    } else if (_instance != nullptr) {
      attachCurrentThread();
      jmethodID method = g_env->GetMethodID(_clazz, "readWrite", "(I[BI)J");
      var_int_t value = 0;
      if (method != nullptr) {
        auto readBytes = get_param_int(argc, arg, 0, 2);
        value = g_env->CallIntMethod(_instance, method, readBytes, _array, writeLen);
      }
      if (!checkException(retval)) {
        v_setint(retval, value);
        result = 1;
      }
      detachCurrentThread();
    }
    return result;
  }

  // int write(byte[] write, int length) {
  int invokeSpiWrite(int argc, slib_par_t *arg, var_s *retval) {
    int result = 0;
    int maxSize = SPI_WRITE_MAX;
    if (is_param_array(argc, arg, 0)) {
      // allow an entire LCD to be updated within one IOIOLooper.loop() call
      maxSize = v_asize(arg[0].var_p);
    }
    int writeLen = populateByteArray(argc, arg, 0, maxSize);
    if (writeLen > maxSize) {
      error(retval, "write array", 1, maxSize);
    } else if (_instance != nullptr) {
      attachCurrentThread();
      jmethodID method = g_env->GetMethodID(_clazz, "write", "([BI)V");
      if (method != nullptr) {
        g_env->CallVoidMethod(_instance, method, _array, writeLen);
      }
      if (!checkException(retval)) {
        result = 1;
      }
      detachCurrentThread();
    }
    return result;
  }
  
  // int readWrite(int address, byte[] write) {
  int invokeTwiReadWrite(int argc, slib_par_t *arg, var_s *retval) {
    int result = 0;
    int writeLen = populateByteArray(argc, arg, 2, TWI_WRITE_MAX);
    if (writeLen > TWI_WRITE_MAX) {
      error(retval, "write array", 1, TWI_WRITE_MAX);
    } else if (_instance != nullptr) {
      attachCurrentThread();
      jmethodID method = g_env->GetMethodID(_clazz, "readWrite", "(II[BI)J");
      var_int_t value = 0;
      if (method != nullptr) {
        auto address = get_param_int(argc, arg, 0, 0);
        auto readBytes = get_param_int(argc, arg, 1, 2);
        value = g_env->CallIntMethod(_instance, method, address, readBytes, _array, writeLen);
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
  int invokeTwiWrite(int argc, slib_par_t *arg, var_s *retval) {
    int result = 0;
    int writeLen = populateByteArray(argc, arg, 1, TWI_WRITE_MAX);
    if (writeLen > TWI_WRITE_MAX) {
      error(retval, "write array", 1, TWI_WRITE_MAX);
    } else if (_instance != nullptr) {
      attachCurrentThread();
      jmethodID method = g_env->GetMethodID(_clazz, "write", "(I[BI)V");
      if (method != nullptr) {
        auto address = get_param_int(argc, arg, 0, 0);
        g_env->CallVoidMethod(_instance, method, address, _array, writeLen);
      }
      if (!checkException(retval)) {
        result = 1;
      }
      detachCurrentThread();
    }
    return result;
  }
};

robin_hood::unordered_map<int, IOTask> g_ioTaskMap;
IOTask *g_ioioTask;
int g_nextId = 1;

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
    error(retval, "TwiMaster.readWrite(address, read-bytes, [data]", 2, TWI_WRITE_MAX);
  } else if (readBytes < 1 || readBytes > 8) {
    error(retval, "read-bytes value out of range. Expected a number between 1 and 8");
  } else {
    int id = get_io_class_id(self, retval);
    if (id != -1) {
      result = g_ioTaskMap.at(id).invokeTwiReadWrite(argc, arg, retval);
    }
  }
  return result;
}

static int cmd_twimaster_write(var_s *self, int argc, slib_par_t *arg, var_s *retval) {
  int result = 0;
  if (argc < 2) {
    error(retval, "TwiMaster.write", 2, TWI_WRITE_MAX);
  } else {
    int id = get_io_class_id(self, retval);
    if (id != -1) {
      result = g_ioTaskMap.at(id).invokeTwiWrite(argc, arg, retval);
    }
  }
  return result;
}

static int cmd_spimaster_readwrite(var_s *self, int argc, slib_par_t *arg, var_s *retval) {
  int result = 0;
  auto readBytes = get_param_int(argc, arg, 0, 0);
  if (argc < 2) {
    error(retval, "SpiMaster.readWrite(read-bytes, [data]", 2, SPI_WRITE_MAX);
  } else if (readBytes < 1 || readBytes > 8) {
    error(retval, "read-bytes value out of range. Expected a number between 1 and 8");
  } else {
    int id = get_io_class_id(self, retval);
    if (id != -1) {
      result = g_ioTaskMap.at(id).invokeSpiReadWrite(argc, arg, retval);
    }
  }
  return result;
}

static int cmd_spimaster_write(var_s *self, int argc, slib_par_t *arg, var_s *retval) {
  int result = 0;
  if (argc < 1) {
    error(retval, "SpiMaster.write", 1, SPI_WRITE_MAX);
  } else {
    int id = get_io_class_id(self, retval);
    if (id != -1) {
      result = g_ioTaskMap.at(id).invokeSpiWrite(argc, arg, retval);
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
  v_create_callback(map, "readWrite", cmd_spimaster_readwrite);
}

#include "api.h"

FUNC_SIG lib_func[] = {
  {1, 1, "OPENANALOGINPUT", cmd_openanaloginput},
  {1, 1, "OPENCAPSENSE", cmd_opencapsense},
  {1, 1, "OPENDIGITALINPUT", cmd_opendigitalinput},
  {1, 1, "OPENDIGITALOUTPUT", cmd_opendigitaloutput},
  {1, 1, "OPENPULSEINPUT", cmd_openpulseinput},
  {2, 2, "OPENPWMOUTPUT", cmd_openpwmoutput},
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
  {0, 0, "WAITFORCONNECT", cmd_ioio_waitforconnect},
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
  int result = createJVM("-Djava.class.path=./ioio-1.0-jar-with-dependencies.jar", "-Dioio.SerialPorts=ttyACM0", false);
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
