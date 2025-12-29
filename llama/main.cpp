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

#define CLASS_ID_LLAMA 1
#define CLASS_ID_LLAMA_ITER 2

int g_nextId = 1;
robin_hood::unordered_map<int, Llama> g_llama;
robin_hood::unordered_map<int, LlamaIter> g_llama_iter;

static int get_llama_class_id(var_s *map, var_s *retval) {
  int result = -1;
  if (is_map(map)) {
    int id = map->v.m.id;
    if (id != -1 && g_llama.find(id) != g_llama.end()) {
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "Llama not found");
  }
  return result;
}

static int get_llama_iter_class_id(var_s *map, var_s *retval) {
  int result = -1;
  if (is_map(map)) {
    int id = map->v.m.id;
    if (id != -1 && g_llama_iter.find(id) != g_llama_iter.end()) {
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "Llama iter not found");
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
// llama.add_stop('xyz')
//
static int cmd_llama_add_stop(var_s *self, int argc, slib_par_t *arg, var_s *retval) {
  int result = 0;
  if (argc != 1) {
    error(retval, "llama.add_stop", 1, 1);
  } else {
    int id = get_llama_class_id(self, retval);
    if (id != -1) {
      Llama &llama = g_llama.at(id);
      llama.add_stop(get_param_str(argc, arg, 0, "stop"));
      result = 1;
    }
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
    int id = get_llama_class_id(self, retval);
    if (id != -1) {
      Llama &llama = g_llama.at(id);
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
    int id = get_llama_class_id(self, retval);
    if (id != -1) {
      Llama &llama = g_llama.at(id);
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
    int id = get_llama_class_id(self, retval);
    if (id != -1) {
      Llama &llama = g_llama.at(id);
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
    int id = get_llama_class_id(self, retval);
    if (id != -1) {
      Llama &llama = g_llama.at(id);
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
    int id = get_llama_class_id(self, retval);
    if (id != -1) {
      Llama &llama = g_llama.at(id);
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
    int id = get_llama_class_id(self, retval);
    if (id != -1) {
      Llama &llama = g_llama.at(id);
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
    int id = get_llama_class_id(self, retval);
    if (id != -1) {
      Llama &llama = g_llama.at(id);
      llama.set_top_p(get_param_num(argc, arg, 0, 0));
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
    int id = get_llama_class_id(self, retval);
    if (id != -1) {
      Llama &llama = g_llama.at(id);
      llama.reset();
      result = 1;
    }
  }
  return result;
}

//
// iter.has_next()
//
static int cmd_llama_has_next(var_s *self, int argc, slib_par_t *arg, var_s *retval) {
  int result = 0;
  if (argc != 0) {
    error(retval, "iter.has_next", 0, 0);
  } else {
    int id = get_llama_iter_class_id(self, retval);
    if (id != -1) {
      LlamaIter &llamaIter = g_llama_iter.at(id);
      v_setint(retval, llamaIter._has_next);
      result = 1;
    }
  }
  return result;
}

//
// iter.next()
//
static int cmd_llama_next(var_s *self, int argc, slib_par_t *arg, var_s *retval) {
  int result = 0;
  if (argc != 0) {
    error(retval, "iter.next", 0, 0);
  } else {
    int id = get_llama_iter_class_id(self, retval);
    if (id != -1) {
      LlamaIter &iter = g_llama_iter.at(id);
      auto out = iter._llama->next(iter);
      v_setstr(retval, out.c_str());
      result = 1;
    }
  }
  return result;
}

//
// iter.tokens_sec
//
static int cmd_llama_tokens_sec(var_s *self, int argc, slib_par_t *arg, var_s *retval) {
  int result = 0;
  if (argc != 0) {
    error(retval, "iter.tokens_sec", 0, 0);
  } else {
    int id = get_llama_iter_class_id(self, retval);
    if (id != -1) {
      LlamaIter &iter = g_llama_iter.at(id);
      auto t_end = std::chrono::high_resolution_clock::now();
      double secs = std::chrono::duration<double>(t_end - iter._t_start).count();
      double tokens_sec = secs > 0 ? iter._tokens_generated / secs : 0;
      v_setreal(retval, tokens_sec);
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
    int id = get_llama_class_id(self, retval);
    if (id != -1) {
      int iter_id = ++g_nextId;
      LlamaIter &iter = g_llama_iter[iter_id];
      Llama &llama = g_llama.at(id);
      auto prompt = get_param_str(argc, arg, 0, "");
      if (llama.generate(iter, prompt)) {
        map_init_id(retval, iter_id, CLASS_ID_LLAMA_ITER);
        v_create_callback(retval, "has_next", cmd_llama_has_next);
        v_create_callback(retval, "next", cmd_llama_next);
        v_create_callback(retval, "tokens_sec", cmd_llama_tokens_sec);
        result = 1;
      } else {
        error(retval, llama.last_error());
      }
    }
  }
  return result;
}

static int cmd_create_llama(int argc, slib_par_t *params, var_t *retval) {
  int result;
  auto model = expand_path(get_param_str(argc, params, 0, ""));
  auto n_ctx = get_param_int(argc, params, 1, 2048);
  auto n_batch = get_param_int(argc, params, 2, 1024);
  auto n_gpu_layers = get_param_int(argc, params, 3, -1);
  int id = ++g_nextId;
  Llama &llama = g_llama[id];
  if (llama.construct(model, n_ctx, n_batch, n_gpu_layers)) {
    map_init_id(retval, id, CLASS_ID_LLAMA);
    v_create_callback(retval, "add_stop", cmd_llama_add_stop);
    v_create_callback(retval, "generate", cmd_llama_generate);
    v_create_callback(retval, "reset", cmd_llama_reset);
    v_create_callback(retval, "set_penalty_repeat", cmd_llama_set_penalty_repeat);
    v_create_callback(retval, "set_penalty_last_n", cmd_llama_set_penalty_last_n);
    v_create_callback(retval, "set_max_tokens", cmd_llama_set_max_tokens);
    v_create_callback(retval, "set_min_p", cmd_llama_set_min_p);
    v_create_callback(retval, "set_temperature", cmd_llama_set_temperature);
    v_create_callback(retval, "set_top_k", cmd_llama_set_top_k);
    v_create_callback(retval, "set_top_p", cmd_llama_set_top_p);
    result = 1;
  } else {
    error(retval, llama.last_error());
    g_llama.erase(id);
    result = 0;
  }
  return result;
}

FUNC_SIG lib_func[] = {
  {1, 4, "LLAMA", cmd_create_llama},
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
    case CLASS_ID_LLAMA:
      if (g_llama.find(id) != g_llama.end()) {
        g_llama.erase(id);
      }
      break;
    case CLASS_ID_LLAMA_ITER:
      if (g_llama_iter.find(id) != g_llama_iter.end()) {
        g_llama_iter.erase(id);
      }
      break;
    }
  }
}

//
// Program termination
//
void sblib_close(void) {
  if (!g_llama.empty()) {
    fprintf(stderr, "LLM leak detected\n");
    g_llama.clear();
  }
  if (!g_llama_iter.empty()) {
    fprintf(stderr, "LLM iter leak detected\n");
    g_llama_iter.clear();
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
