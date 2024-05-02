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
  int result = createJVM("-Djava.class.path=./ioio-1.0-jar-with-dependencies.jar", "-Dioio.SerialPorts=IOIO0");
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
