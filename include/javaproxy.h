// This file is part of SmallBASIC
//
// Copyright(C) 2024 Chris Warren-Smith.
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//

#pragma once

#include "config.h"
#include "include/var.h"
#include <jni.h>

JNIEnv *g_env;
JavaVM *g_jvm;
jobject g_activity;

#define ARRAY_SIZE 10

#if defined(ANDROID_MODULE)
  #define attachCurrentThread() g_jvm->AttachCurrentThread(&g_env, nullptr)
  #define detachCurrentThread() g_jvm->DetachCurrentThread()
#else
  #define attachCurrentThread()
  #define detachCurrentThread()
#endif

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

struct JavaProxy {
  JavaProxy():
    _clazz(nullptr),
    _instance(nullptr),
    _array(nullptr) {
  }

  virtual ~JavaProxy() {
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
      g_env->ExceptionDescribe();
      g_env->ExceptionClear();
      if (retval) {
        jclass clazz = g_env->FindClass("java/lang/Throwable");
        jmethodID methodId = g_env->GetMethodID(clazz, "getMessage", "()Ljava/lang/String;");
        jstring jstr = (jstring) g_env->CallObjectMethod(exc, methodId);
        const char *message = g_env->GetStringUTFChars(jstr, JNI_FALSE);
        error(retval, message);
        g_env->ReleaseStringUTFChars(jstr, message);
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
      jmethodID method = g_env->GetMethodID(_clazz, name, "(F)V");
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

  protected:
  jclass _clazz;
  jobject _instance;
  jbyteArray _array;
};

#if defined(ANDROID_MODULE)
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

#else

int createJVM(const char *arg1, const char *arg2) {
  JavaVMInitArgs vm_args;
  JavaVMOption options[6];
  options[0].optionString = (char *)"-Xrs";
  options[1].optionString = (char *)arg1;
  options[2].optionString = (char *)arg2;
  options[3].optionString = (char *)"-Xdebug";
  options[4].optionString = (char *)"-agentlib:jdwp=transport=dt_socket,server=y,address=5005,suspend=y";
  options[5].optionString = (char *)"-Xcheck:jni";
  vm_args.version = JNI_VERSION_1_8;
#if defined(DEBUG)
  vm_args.nOptions = 6;
#else
  vm_args.nOptions = 3;
#endif
  vm_args.ignoreUnrecognized = 1;
  vm_args.options = options;
  int result = (JNI_CreateJavaVM(&g_jvm, (void **)&g_env, &vm_args) == JNI_OK &&
                g_jvm->AttachCurrentThread((void **)&g_env, nullptr) == JNI_OK);
  if (!result) {
    fprintf(stderr, "Failed to create JVM\n");
  }
  return result;
}

#endif
