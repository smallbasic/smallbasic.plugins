// This file is part of SmallBASIC
//
// Plugin for drawing on an image buffer
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2021 Chris Warren-Smith

#include "config.h"
#include <stdint.h>
#include <math.h>

#include "robin-hood-hashing/src/include/robin_hood.h"
#include "include/var.h"
#include "include/module.h"
#include "include/param.h"

typedef uint32_t pixel_t;

struct Canvas {
  Canvas();
  ~Canvas();

  bool create(int w, int h);
  void drawFill(int x, int y, int w, int h);
  void drawLine(int x, int y, int w, int h);
  pixel_t *getLine(int y) { return _pixels + (y * _w); }
  int _w;
  int _h;
  pixel_t _color;
  pixel_t *_pixels;
};

robin_hood::unordered_map<int, Canvas*> _canvas;
int _nextId = 1;

Canvas::Canvas() :
  _w(0),
  _h(0),
  _color(0),
  _pixels(nullptr) {
}

Canvas::~Canvas() {
  delete [] _pixels;
}

bool Canvas::create(int w, int h) {
  bool result;
  _w = w;
  _h = h;
  _pixels = new pixel_t[w * h];
  if (_pixels) {
    memset(_pixels, 0, w * h);
    result = true;
  } else {
    result = false;
  }
  return result;
}

void Canvas::drawFill(int left, int top, int width, int height) {
  for (int y = 0; y < height; y++) {
    int posY = y + top;
    if (posY == _h) {
      break;
    } else {
      pixel_t *line = getLine(posY);
      for (int x = 0; x < width; x++) {
        int posX = x + left;
        if (posX == _w) {
          break;
        } else {
          line[posX] = _color;
        }
      }
    }
  }
}

void Canvas::drawLine(int startX, int startY, int endX, int endY) {
  if (startY == endY) {
    // horizontal
    int x1 = startX;
    int x2 = endX;
    if (x1 > endX) {
      x1 = endX;
      x2 = startX;
    }
    if (x1 < 0) {
      x1 = 0;
    }
    if (x2 >= _w) {
      x2 = _w -1;
    }
    if (startY >= 0 && startY < _h) {
      pixel_t *line = getLine(startY);
      for (int x = x1; x <= x2; x++) {
        line[x] = _color;
      }
    }
  } else if (startX == endX) {
    // vertical
    int y1 = startY;
    int y2 = endY;
    if (y1 > y2) {
      y1 = endY;
      y2 = startY;
    }
    if (y1 < 0) {
      y1 = 0;
    }
    if (y2 >= _h) {
      y2 = _h - 1;
    }
    if (startX >= 0 && startX < _w) {
      for (int y = y1; y <= y2; y++) {
        pixel_t *line = getLine(y);
        line[startX] = _color;
      }
    }
  }
}

static int get_canvas_id(int argc, slib_par_t *params, int arg, var_t *retval) {
  int result = -1;
  if (is_param_map(argc, params, arg)) {
    // the passed in variable is a map
    int id = get_id(params, arg);
    if (id != -1 && _canvas.find(id) != _canvas.end()) {
      // the map contained an ID field with a live value
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "Canvas not found");
  }
  return result;
}

static int cmd_create(int argc, slib_par_t *params, var_t *retval) {
  auto canvas = new Canvas();
  int result;
  int width = get_param_int(argc, params, 0, 0);
  int height = get_param_int(argc, params, 1, 0);
  if (canvas && canvas->create(width, height)) {
    auto id = ++_nextId;
    _canvas[id] = canvas;
    map_init_id(retval, id);
    v_setint(map_add_var(retval, "w", 0), width);
    v_setint(map_add_var(retval, "h", 0), height);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_set_pixel(int argc, slib_par_t *params, var_t *retval) {
  int result = 0;
  int id = get_canvas_id(argc, params, 0, retval);
  if (id != -1) {
    Canvas *canvas = _canvas[id];
    auto x = get_param_int(argc, params, 1, 0);
    auto y = get_param_int(argc, params, 2, 0);
    auto line = canvas->getLine(y);
    line[x] = canvas->_color;
    result = 1;
  }
  return result;
}

static int cmd_color(int argc, slib_par_t *params, var_t *retval) {
  int result = 0;
  int id = get_canvas_id(argc, params, 0, retval);
  if (id != -1) {
    Canvas *canvas = _canvas[id];
    canvas->_color = get_param_int(argc, params, 1, 0);
    result = 1;
  }
  return result;
}

static int cmd_line(int argc, slib_par_t *params, var_t *retval) {
  int result = 0;
  int id = get_canvas_id(argc, params, 0, retval);
  if (id != -1) {
    Canvas *canvas = _canvas[id];
    auto x1 = get_param_int(argc, params, 1, 0);
    auto y1 = get_param_int(argc, params, 2, 0);
    auto x2 = get_param_int(argc, params, 3, 0);
    auto y2 = get_param_int(argc, params, 4, 0);
    canvas->drawLine(x1, y1, x2, y2);
    result = 1;
  }
  return result;
}

static int cmd_rect(int argc, slib_par_t *params, var_t *retval) {
  int result = 0;
  int id = get_canvas_id(argc, params, 0, retval);
  if (id != -1) {
    Canvas *canvas = _canvas[id];
    auto x1 = get_param_int(argc, params, 1, 0);
    auto y1 = get_param_int(argc, params, 2, 0);
    auto x2 = get_param_int(argc, params, 3, 0);
    auto y2 = get_param_int(argc, params, 4, 0);
    auto filled = get_param_int(argc, params, 5, 0);
    if (filled) {
      canvas->drawFill(x1, y1, x2, y2);
    } else {
      canvas->drawLine(x1, y1, x2, y1); // top
      canvas->drawLine(x1, y2, x2, y2); // bottom
      canvas->drawLine(x1, y1, x1, y2); // left
      canvas->drawLine(x2, y1, x2, y2); // right
    }
    result = 1;
  }
  return result;
}

static int cmd_get_pixel(int argc, slib_par_t *params, var_t *retval) {
  int result = 0;
  int id = get_canvas_id(argc, params, 0, retval);
  if (id != -1) {
    Canvas *canvas = _canvas[id];
    auto x = get_param_int(argc, params, 1, 0);
    auto y = get_param_int(argc, params, 2, 0);
    auto line = canvas->getLine(y);
    v_setint(retval, line[x]);
    result = 1;
  }
  return result;
}

FUNC_SIG lib_func[] = {
  {2, 2, "CREATE", cmd_create},
  {3, 3, "PIXEL", cmd_get_pixel},
};

FUNC_SIG lib_proc[] = {
  {2, 2, "COLOR", cmd_color},
  {5, 5, "LINE", cmd_line},
  {6, 6, "RECT", cmd_rect},
  {3, 3, "PIXEL", cmd_set_pixel},
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
  _canvas.clear();
}
