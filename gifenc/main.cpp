// This file is part of SmallBASIC
//
// Plugin for encoding a GIF image file
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2021 Chris Warren-Smith

#include "config.h"
#include <stdint.h>
#include <math.h>

#include "gifenc/gifenc.h"
#include "robin-hood-hashing/src/include/robin_hood.h"
#include "include/var.h"
#include "include/module.h"
#include "include/param.h"

robin_hood::unordered_map<int, ge_GIF *> _gifMap;
int _nextId = 1;

static int get_gif_id(int argc, slib_par_t *params, int arg, var_t *retval) {
  int result = -1;
  if (is_param_map(argc, params, arg)) {
    // the passed in variable is a map
    int id = get_id(params, arg);
    if (id != -1 && _gifMap.find(id) != _gifMap.end()) {
      // the map contained an ID field with a live value
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "GIF not found");
  }
  return result;
}

static uint8_t *get_param_byte_p(int argc, slib_par_t *params, int n) {
  uint8_t *result = nullptr;
  if (is_param_array(argc, params, n)) {
    int size = v_asize(params[n].var_p);
    if (size) {
      result = new uint8_t[size + 1];
      for (int i = 0; i < size; i++) {
        result[i] = get_array_elem_num(params[n].var_p, i);
      }
    }
  }
  return result;
}

static void v_setgif(var_t *var, ge_GIF *gif) {
  auto id = ++_nextId;
  _gifMap[id] = gif;
  map_init_id(var, id);
  v_setint(map_add_var(var, "w", 0), gif->w);
  v_setint(map_add_var(var, "h", 0), gif->h);
  v_setint(map_add_var(var, "depth", 0), gif->depth);
  v_setint(map_add_var(var, "bgindex", 0), gif->bgindex);
  v_setint(map_add_var(var, "offset", 0), gif->offset);
  v_setint(map_add_var(var, "nframes", 0), gif->nframes);
}

static int cmd_new_gif(int argc, slib_par_t *params, var_t *retval) {
  auto fname = get_param_str(argc, params, 0, 0);
  auto width = get_param_int(argc, params, 1, 0);
  auto height = get_param_int(argc, params, 2, 0);
  auto palette = get_param_byte_p(argc, params, 3);
  auto bgindex = get_param_int(argc, params, 4, 0);
  auto loop = get_param_int(argc, params, 5, 0);

  int depth = 8;
  int result = 1;
  if (palette) {
    int size = v_asize(params[3].var_p);
    if (size % 3 != 0) {
      error(retval, "Invalid pallete");
      result = 0;
    } else {
      depth = log2(size / 3);
    }
  }

  if (result) {
    auto fnResult = ge_new_gif(fname, width, height, palette, depth, bgindex, loop);
    if (fnResult != nullptr) {
      v_setgif(retval, fnResult);
    } else {
      result = 0;
    }
  }
  delete [] palette;
  return result;
}

static int cmd_add_frame(int argc, slib_par_t *params, var_t *retval) {
  int result = 0;
  int id = get_gif_id(argc, params, 0, retval);
  if (id != -1) {
    ge_GIF *gif = _gifMap[id];
    auto pixels = get_param_byte_p(argc, params, 2);
    if (pixels) {
      int maxFrame = gif->w * gif->h;
      int size = v_asize(params[2].var_p) - 1;
      if (size > maxFrame) {
        error(retval, "Pixel array too large");
      } else {
        memcpy(gif->frame, pixels, size);
        auto delay = get_param_int(argc, params, 1, 0);
        ge_add_frame(gif, delay);
        v_setint(retval, 1);
        result = 1;
      }
      delete [] pixels;
    } else {
      error(retval, "Pixel array empty");
    }
  }
  return result;
}

static int cmd_close_gif(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_gif_id(argc, params, 0, retval);
  if (id != -1) {
    ge_close_gif(_gifMap[id]);
    v_setint(retval, 1);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

FUNC_SIG lib_func[] = {
  {6, 6, "CREATE", cmd_new_gif},
};

FUNC_SIG lib_proc[] = {
  {3, 3, "ADD_FRAME", cmd_add_frame},
  {1, 1, "CLOSE", cmd_close_gif},
};

SBLIB_API int sblib_proc_count() {
  return (sizeof(lib_proc) / sizeof(lib_proc[0]));
}

SBLIB_API int sblib_func_count() {
  return (sizeof(lib_func) / sizeof(lib_func[0]));
}

SBLIB_API int sblib_proc_getname(int index, char *proc_name) {
  int result;
  if (index < sblib_proc_count()) {
    strcpy(proc_name, lib_proc[index]._name);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

SBLIB_API int sblib_func_getname(int index, char *proc_name) {
  int result;
  if (index < sblib_func_count()) {
    strcpy(proc_name, lib_func[index]._name);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

SBLIB_API int sblib_proc_exec(int index, int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (index >= 0 && index < sblib_proc_count()) {
    if (argc < lib_proc[index]._min || argc > lib_proc[index]._max) {
      if (lib_proc[index]._min == lib_proc[index]._max) {
        error(retval, lib_proc[index]._name, lib_proc[index]._min);
      } else {
        error(retval, lib_proc[index]._name, lib_proc[index]._min, lib_proc[index]._max);
      }
      result = 0;
    } else {
      result = lib_proc[index]._command(argc, params, retval);
    }
  } else {
    fprintf(stderr, "PROC index error [%d]\n", index);
    result = 0;
  }
  return result;
}

SBLIB_API int sblib_func_exec(int index, int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (index >= 0 && index < sblib_func_count()) {
    if (argc < lib_func[index]._min || argc > lib_func[index]._max) {
      if (lib_func[index]._min == lib_func[index]._max) {
        error(retval, lib_func[index]._name, lib_func[index]._min);
      } else {
        error(retval, lib_func[index]._name, lib_func[index]._min, lib_func[index]._max);
      }
      result = 0;
    } else {
      result = lib_func[index]._command(argc, params, retval);
    }
  } else {
    fprintf(stderr, "FUNC index error [%d]\n", index);
    result = 0;
  }
  return result;
}

SBLIB_API void sblib_close(void) {
  _gifMap.clear();
}
