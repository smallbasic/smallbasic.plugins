// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2026 Chris Warren-Smith

#include "config.h"

#include "robin-hood-hashing/src/include/robin_hood.h"
#include "include/log.h"
#include "include/var.h"
#include "include/module.h"
#include "include/param.h"

#include "llama-sb.h"

#define CLASS_ID 1

int g_nextId = 1;
robin_hood::unordered_map<int, Llama> g_map;

static int get_id(var_s *map, var_s *retval) {
  int result = -1;
  if (is_map(map)) {
    int id = map->v.m.id;
    if (id != -1 && g_map.find(id) != g_map.end()) {
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "Llama not found");
  }
  return result;
}

const char *llm_llama_chat(int id,
                           const char * user_message,
                           int max_tokens,
                           float temperature) {
  static thread_local string result;

  // auto it = g_llamas.find(h);
  // if (it == g_llamas.end()) {
  //   result = "[invalid llama handle]";
  //   return result.c_str();
  // }

  // Llama * llm = it->second.get();

  // // build accumulated prompt
  // string prompt = build_chat_prompt(llm, user_message);

  // // run generation WITHOUT clearing cache
  // result = llm->generate(prompt,
  //                        max_tokens,
  //                        temperature,
  //                        false);   // echo = false

  // // append assistant reply to history
  // llm->chat_prompt += result;
  // llm->chat_prompt += "\n";

  return result.c_str();
}

//
// make the model forget everything
//
void llm_llama_reset() {
  // std::lock_guard<std::mutex> lock(g_mutex);

  // auto it = g_llamas.find(h);
  // if (it == g_llamas.end()) return;

  // llama_kv_cache_clear(it->second->ctx);
  // it->second->chat_prompt.clear();
}

//
// string generate(prompt, max_tokens, temperature)
//
const char *llm_llama_generate(const char * prompt,
                               int max_tokens,
                               float temperature) {
  // static thread_local string result;

  // std::lock_guard<std::mutex> lock(g_mutex);

  // auto it = g_llamas.find(h);
  // if (it == g_llamas.end()) {
  //   result = "[invalid llama handle]";
  //   return result.c_str();
  // }

  // try {
  //   result = it->second->generate(prompt,
  //                                 max_tokens,
  //                                 temperature);
  // } catch (const std::exception & e) {
  //   result = e.what();
  // }

  // return result.c_str();
  return nullptr;
}


static int llm_llama_create(const char *model_path, int n_ctx) {
  // std::lock_guard<std::mutex> lock(g_mutex);

  // llama_handle id = g_next_id++;

  // try {
  //   g_llamas[id] = std::make_unique<Llama>(model_path, n_ctx);
  // } catch (...) {
  //   return 0;
  // }

  return 0;
}

void llm_llama_destroy() {
  // std::lock_guard<std::mutex> lock(g_mutex);
  // g_llamas.erase(h);
}

// const char *llm_llama_generate(llama_handle h,
//                                 const char *prompt,
//                                 int max_tokens,
//                                 float temperature) {
//   static thread_local string result;

//   auto it = g_llamas.find(h);
//   if (it == g_llamas.end()) {
//     result = "[invalid llama handle]";
//     return result.c_str();
//   }

//   try {
//     result = it->second->generate(prompt, max_tokens, temperature);
//   } catch (const std::exception &e) {
//     result = e.what();
//   }

//   return result.c_str();
// }

string expand_path(const char *path) {
  string result;
  if (path && path[0] == '~') {
    const char *home = getenv("HOME");
    if (home != nullptr) {
      result.append(home);
      result.append(path + 1);
    } else {
      result = path;
    }
  } else {
    result = path;
  }
  return result;
}

static int cmd_create_llama(int argc, slib_par_t *params, var_t *retval) {
  int result;
  auto model = expand_path(get_param_str(argc, params, 0, ""));
  int n_ctx = get_param_int(argc, params, 0, 2048);
  int disable_log = get_param_int(argc, params, 0, 1);
  int id = ++g_nextId;
  Llama &llama = g_map[id];
  if (llama.create(model, n_ctx, disable_log)) {
    map_init_id(retval, id, CLASS_ID);

    // v_create_callback(map, "getVoltage", cmd_analoginput_getvoltage);
    // v_create_callback(map, "getVoltageSync", cmd_analoginput_getvoltagesync);
    // v_create_callback(map, "getReference", cmd_analoginput_getreference);
    // v_create_callback(map, "read", cmd_analoginput_read);
    // v_create_callback(map, "readSync", cmd_analoginput_readsync);
    // v_create_callback(map, "getOverflowCount", cmd_analoginput_getoverflowcount);
    // v_create_callback(map, "available", cmd_analoginput_available);
    // v_create_callback(map, "readBuffered", cmd_analoginput_readbuffered);
    // v_create_callback(map, "getVoltageBuffered", cmd_analoginput_getvoltagebuffered);
    // v_create_callback(map, "getSampleRate", cmd_analoginput_getsamplerate);

    result = 1;
  } else {
    error(retval, llama.last_error());
    g_map.erase(id);
    result = 0;
  }
  return result;
}

FUNC_SIG lib_func[] = {
  {1, 3, "LLAMA", cmd_create_llama},
};

SBLIB_API int sblib_func_count() {
  return 1;
}

FUNC_SIG lib_proc[] = {};

SBLIB_API int sblib_proc_count() {
  return 0;
}

//
// Program startup
//
int sblib_init(const char *sourceFile) {
  return 1;
}

//
// Release variables falling out of scope
//
SBLIB_API void sblib_free(int cls_id, int id) {
  if (id != -1) {
    switch (cls_id) {
    case CLASS_ID:
      if (g_map.find(id) != g_map.end()) {
        g_map.erase(id);
      }
      break;
    }
  }
}

//
// Program termination
//
void sblib_close(void) {
  if (!g_map.empty()) {
    fprintf(stderr, "LLM leak detected\n");
    g_map.clear();
  }
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
