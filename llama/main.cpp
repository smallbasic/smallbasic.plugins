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

static int get_class_id(var_s *map, var_s *retval) {
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

static string expand_path(const char *path) {
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


//
// llama.set_penalty_repeat(0.8)
//
static int cmd_llama_set_penalty_repeat(var_s *self, int argc, slib_par_t *arg, var_s *retval) {
  int result = 0;
  if (argc != 1) {
    error(retval, "llama.set_penalty_repeat", 1, 1);
  } else {
    int id = get_class_id(self, retval);
    if (id != -1) {
      Llama &llama = g_map.at(id);
      llama.set_penalty_repeat(get_param_num(argc, arg, 0, 0));
      result = 1;
    }
  }
  return result;
}

//
// llama.set_penalty_last_n(0.8)
//
static int cmd_llama_set_penalty_last_n(var_s *self, int argc, slib_par_t *arg, var_s *retval) {
  int result = 0;
  if (argc != 1) {
    error(retval, "llama.set_penalty_last_n", 1, 1);
  } else {
    int id = get_class_id(self, retval);
    if (id != -1) {
      Llama &llama = g_map.at(id);
      llama.set_penalty_last_n(get_param_num(argc, arg, 0, 0));
      result = 1;
    }
  }
  return result;
}


//
// llama.set_max_tokens(50)
//
static int cmd_llama_set_max_tokens(var_s *self, int argc, slib_par_t *arg, var_s *retval) {
  int result = 0;
  if (argc != 1) {
    error(retval, "llama.set_max_tokens", 1, 1);
  } else {
    int id = get_class_id(self, retval);
    if (id != -1) {
      Llama &llama = g_map.at(id);
      llama.set_max_tokens(get_param_int(argc, arg, 0, 0));
      result = 1;
    }
  }
  return result;
}

//
// llama.set_min_p(0.5)
//
static int cmd_llama_set_min_p(var_s *self, int argc, slib_par_t *arg, var_s *retval) {
  int result = 0;
  if (argc != 1) {
    error(retval, "llama.set_min_p", 1, 1);
  } else {
    int id = get_class_id(self, retval);
    if (id != -1) {
      Llama &llama = g_map.at(id);
      llama.set_min_p(get_param_num(argc, arg, 0, 0));
      result = 1;
    }
  }
  return result;
}

//
// llama.set_temperature(0.8)
//
static int cmd_llama_set_temperature(var_s *self, int argc, slib_par_t *arg, var_s *retval) {
  int result = 0;
  if (argc != 1) {
    error(retval, "llama.set_temperature", 1, 1);
  } else {
    int id = get_class_id(self, retval);
    if (id != -1) {
      Llama &llama = g_map.at(id);
      llama.set_temperature(get_param_num(argc, arg, 0, 0));
      result = 1;
    }
  }
  return result;
}

//
// llama.set_top_k(10.0)
//
static int cmd_llama_set_top_k(var_s *self, int argc, slib_par_t *arg, var_s *retval) {
  int result = 0;
  if (argc != 1) {
    error(retval, "llama.set_top_k", 1, 1);
  } else {
    int id = get_class_id(self, retval);
    if (id != -1) {
      Llama &llama = g_map.at(id);
      llama.set_top_k(get_param_int(argc, arg, 0, 0));
      result = 1;
    }
  }
  return result;
}

//
// llama.set_top_p(0)
//
static int cmd_llama_set_top_p(var_s *self, int argc, slib_par_t *arg, var_s *retval) {
  int result = 0;
  if (argc != 1) {
    error(retval, "llama.set_top_p", 1, 1);
  } else {
    int id = get_class_id(self, retval);
    if (id != -1) {
      Llama &llama = g_map.at(id);
      llama.set_top_p(get_param_num(argc, arg, 0, 0));
      result = 1;
    }
  }
  return result;
}

//
// print llama.chat("Hello")
//
static int cmd_llama_chat(var_s *self, int argc, slib_par_t *arg, var_s *retval) {
  int result = 0;
  if (argc != 1) {
    error(retval, "llama.chat", 1, 1);
  } else {
    int id = get_class_id(self, retval);
    if (id != -1) {
      Llama &llama = g_map.at(id);
      auto prompt = get_param_str(argc, arg, 0, "");

      // build accumulated prompt
      string updated_prompt = llama.build_chat_prompt(prompt);

      // run generation WITHOUT clearing cache
      string response = llama.generate(updated_prompt);

      // append assistant reply to history
      llama.append_response(response);

      v_setstr(retval, response.c_str());
      result = 1;
    }
  }
  return result;
}

//
// llama.reset() - make the model forget everything
//
static int cmd_llama_reset(var_s *self, int argc, slib_par_t *arg, var_s *retval) {
  int result = 0;
  if (argc != 0) {
    error(retval, "llama.reset", 0, 0);
  } else {
    int id = get_class_id(self, retval);
    if (id != -1) {
      Llama &llama = g_map.at(id);
      llama.reset();
      result = 1;
    }
  }
  return result;
}

//
// print llama.generate("please generate as simple program in BASIC to draw a cat")
//
static int cmd_llama_generate(var_s *self, int argc, slib_par_t *arg, var_s *retval) {
  int result = 0;
  if (argc != 1) {
    error(retval, "llama.generate", 1, 1);
  } else {
    int id = get_class_id(self, retval);
    if (id != -1) {
      Llama &llama = g_map.at(id);
      auto prompt = get_param_str(argc, arg, 0, "");
      string response = llama.generate(prompt);
      v_setstr(retval, response.c_str());
      result = 1;
    }
  }
  return result;
}

static int cmd_create_llama(int argc, slib_par_t *params, var_t *retval) {
  int result;
  auto model = expand_path(get_param_str(argc, params, 0, ""));
  auto n_ctx = get_param_int(argc, params, 1, 2048);
  auto n_batch = get_param_int(argc, params, 2, 1024);
  auto temperature = get_param_num(argc, params, 3, 0.25);
  int id = ++g_nextId;
  Llama &llama = g_map[id];
  if (llama.construct(model, n_ctx, n_batch)) {
    llama.set_temperature(temperature);
    map_init_id(retval, id, CLASS_ID);
    v_create_callback(retval, "set_penalty_repeat", cmd_llama_set_penalty_repeat);
    v_create_callback(retval, "set_penalty_last_n", cmd_llama_set_penalty_last_n);
    v_create_callback(retval, "set_max_tokens", cmd_llama_set_max_tokens);
    v_create_callback(retval, "set_min_p", cmd_llama_set_min_p);
    v_create_callback(retval, "set_temperature", cmd_llama_set_temperature);
    v_create_callback(retval, "set_top_k", cmd_llama_set_top_k);
    v_create_callback(retval, "set_top_p", cmd_llama_set_top_p);
    v_create_callback(retval, "chat", cmd_llama_chat);
    v_create_callback(retval, "generate", cmd_llama_generate);
    v_create_callback(retval, "reset", cmd_llama_reset);
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
