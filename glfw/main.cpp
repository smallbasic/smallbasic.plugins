// This file is part of SmallBASIC
//
// Plugin for glfw GUI library - https://www.glfw.org/
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2020 Chris Warren-Smith

#include "config.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <unordered_map>

#include "include/var.h"
#include "include/module.h"
#include "include/param.h"

#define WAIT_INTERVAL_MILLIS 5
#define WAIT_INTERVAL (WAIT_INTERVAL_MILLIS/1000)
#define M_PI 3.14159265358979323846

std::unordered_map<int, GLFWwindow *> _windowMap;
int _nextId = 1;
float _width, _height;
double ellipse_segments = 20.0;

struct GLColor {
  GLColor() : _r(0), _g(0), _b(0) {}
  GLColor(const GLColor &c) : _r(c._r), _g(c._g), _b(c._b) {}
  GLColor(GLclampf r, GLclampf g, GLclampf b) : _r(r), _g(g), _b(b) {}

  GLColor& operator=(const GLColor &c) {
    _r = c._r; _b = c._b; _g = c._g;
    return *this;
  }

  GLclampf _r;
  GLclampf _g;
  GLclampf _b;
};

GLColor _bg_color;
GLColor _fg_color;

const GLColor _colors[] = {
  {0.12f,0.1f, 0.12f}, // 0 black
  {0.0f, 0.0f, 0.5f }, // 1 blue
  {0.0f, 0.5f, 0.0f }, // 2 green
  {0.0f, 0.5f, 0.5f }, // 3 cyan
  {0.5f, 0.0f, 0.0f }, // 4 red
  {0.5f, 0.0f, 0.5f }, // 5 magenta
  {0.5f, 0.5f, 0.0f }, // 6 yellow
  {0.75f,0.75f,0.75f}, // 7 white
  {0.5f, 0.5f, 0.5f }, // 8 gray
  {0.0f, 0.0f, 1.0f }, // 9 light blue
  {0.0f, 1.0f, 0.0f }, // 10 light green
  {0.0f, 1.0f, 1.0f }, // 11 light cyan
  {1.0f, 0.0f, 0.0f }, // 12 light red
  {1.0f, 0.0f, 1.0f }, // 13 light magenta
  {1.0f, 1.0f, 0.0f }, // 14 light yellow
  {1.0f, 1.0f, 1.0f }  // 15 bright white
};

struct GLColor get_color(long c) {
  GLColor result;
  if (c >= 0 && c < 16) {
    result = _colors[c];
  } else {
    if (c < 0) {
      c = -c;
    }
    result._r = ((c & 0xff0000) >> 16) / 255.0f;
    result._g = ((c & 0xff00) >> 8) / 255.0f;
    result._b = (c & 0xff) / 255.0f;
  }
  return result;
}

static GLFWwindow *get_window(int argc, slib_par_t *params) {
  GLFWwindow *window;
  int windowId = get_param_int(argc, params, 0, -1);
  if (windowId != -1) {
    window = _windowMap.at(windowId);
  } else {
    window = nullptr;
  }
  return window;
}

static float scaleX(int x) {
  return ((2 * x) / _width) - 1;
}

static float scaleY(int y) {
  return ((2 * (_height - y)) / _height) - 1;
}

static void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

static void window_size_callback(GLFWwindow* window, int width, int height) {
  _width = width;
  _height = height;
}

// if not glfw.init() then throw "error"
static int cmd_init(int argc, slib_par_t *params, var_t *retval) {
  int result = glfwInit();
  if (result) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, 4);
  }
  v_setint(retval, result);
  return argc == 0;
}

// window = glfw.create_window(640, 480, "Hello World")
static int cmd_create_window(int argc, slib_par_t *params, var_t *retval) {
  int width = get_param_int(argc, params, 0, 640);
  int height = get_param_int(argc, params, 1, 480);
  const char *title = get_param_str(argc, params, 2, "SmallBASIC");

  int result = (argc == 3);
  if (result) {
    GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    result = window != nullptr;
    if (result) {
      glfwMakeContextCurrent(window);
      gladLoadGL((GLADloadfunc) glfwGetProcAddress);
      glfwSetErrorCallback(error_callback);
      glfwSetKeyCallback(window, key_callback);
      glfwSetWindowSizeCallback(window, window_size_callback);

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      glEnable(GL_BLEND);
      glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
      glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
      glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      result = ++_nextId;
      _windowMap[result] = window;
      _width = width;
      _height = height;

      v_setint(retval, result);
      sblib_settextcolor(1, 0);
      sblib_cls();

      GLenum error = glGetError();
      if (error != GL_NO_ERROR) {
        char message[100];
        snprintf(message, sizeof(message), "GL Error %d", error);
        v_setstr(retval, message);
        result = 0;
      }
    } else {
      char message[100];
      snprintf(message, sizeof(message), "Failed to create window [%d X %d]", width, height);
      v_setstr(retval, message);
    }
  } else {
    v_setstr(retval, "Incorrect number of parameters");
  }
  return result;
}

// n = glfw.window_should_close(window)
static int cmd_window_should_close(int argc, slib_par_t *params, var_t *retval) {
  int result;
  GLFWwindow *window = get_window(argc, params);
  if (window != nullptr) {
    if (glfwWindowShouldClose(window)) {
      glfwDestroyWindow(window);
      glfwTerminate();
      v_setint(retval, 1);
      int windowId = get_param_int(argc, params, 0, -1);
      if (windowId != -1) {
        _windowMap.erase(windowId);
      }
    } else {
      v_setint(retval, 0);
    }
    result = (argc == 1);
  } else {
    result = 0;
  }
  return result;
}

// glfw.line_width(10)
static int cmd_line_width(int argc, slib_par_t *params, var_t *retval) {
  glLineWidth(get_param_int(argc, params, 0, 1));
  return argc == 1;
}

// glfw.poll_events()
static int cmd_poll_events(int argc, slib_par_t *params, var_t *retval) {
  glfwPollEvents();
  return argc == 0;
}

// glfw.wait_events(n)
static int cmd_wait_events(int argc, slib_par_t *params, var_t *retval) {
  int waitMillis = get_param_int(argc, params, 0, -1);
  if (waitMillis > 0) {
    glfwWaitEventsTimeout(waitMillis / 1000);
  } else {
    glfwWaitEvents();
  }
  return argc < 2;
}

// glfw.swap_buffers(window)
static int cmd_swap_buffers(int argc, slib_par_t *params, var_t *retval) {
  int result;
  GLFWwindow *window = get_window(argc, params);
  if (window != nullptr) {
    glfwSwapBuffers(window);
    result = (argc == 1);
  } else {
    result = 0;
  }
  return result;
}

// glfw.terminate()
static int cmd_terminate(int argc, slib_par_t *params, var_t *retval) {
  glfwTerminate();
  return argc == 0;
}

API lib_proc[] = {
  {"LINE_WIDTH", cmd_line_width},
  {"POLL_EVENTS", cmd_poll_events},
  {"WAIT_EVENTS", cmd_wait_events},
  {"SWAP_BUFFERS", cmd_swap_buffers},
  {"TERMINATE", cmd_terminate}
};

API lib_func[] = {
  {"CREATE_WINDOW", cmd_create_window},
  {"INIT", cmd_init},
  {"WINDOW_SHOULD_CLOSE", cmd_window_should_close},
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
    strcpy(proc_name, lib_proc[index].name);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

SBLIB_API int sblib_func_getname(int index, char *proc_name) {
  int result;
  if (index < sblib_func_count()) {
    strcpy(proc_name, lib_func[index].name);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

SBLIB_API int sblib_proc_exec(int index, int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (index < sblib_proc_count()) {
    result = lib_proc[index].command(argc, params, retval);
  } else {
    result = 0;
  }
  return result;
}

SBLIB_API int sblib_func_exec(int index, int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (index < sblib_func_count()) {
    result = lib_func[index].command(argc, params, retval);
  } else {
    result = 0;
  }
  return result;
}

SBLIB_API int sblib_events(int wait_flag, int *w, int *h) {
  switch (wait_flag) {
  case 1:
    glfwWaitEvents();
    break;
  case 2:
    glfwWaitEventsTimeout(WAIT_INTERVAL);
    break;
  default:
    glfwPollEvents();
    break;
  }

  *w = _width;
  *h = _height;
  return 0;
}

SBLIB_API void sblib_close(void) {
  glfwTerminate();
}

SBLIB_API void sblib_cls() {
  glClear(GL_COLOR_BUFFER_BIT);
}

SBLIB_API void sblib_settextcolor(long fg, long bg) {
  _fg_color = get_color(fg);
  _bg_color = get_color(bg);

  if (_windowMap.size() > 0) {
    glClearColor(_bg_color._r, _bg_color._g, _bg_color._b, 1.0f);
  }
}

SBLIB_API void sblib_setcolor(long fg) {
  _fg_color = get_color(fg);
}

// draw a line
SBLIB_API void sblib_line(int x1, int y1, int x2, int y2) {
  glColor3f(_fg_color._r, _fg_color._g, _fg_color._b);
  glBegin(GL_LINES);
  glVertex2f(scaleX(x1), scaleY(y1));
  glVertex2f(scaleX(x2), scaleY(y2));
  glEnd();
}

// draw a pixel
SBLIB_API void sblib_setpixel(int x, int y) {
  glColor3f(_fg_color._r, _fg_color._g, _fg_color._b);
  glBegin(GL_POINTS);
  glVertex2f(scaleX(x), scaleY(y));
  glEnd();
}

// draw rectangle
SBLIB_API void sblib_rect(int x1, int y1, int x2, int y2, int fill) {
  glColor3f(_fg_color._r, _fg_color._g, _fg_color._b);
  glBegin(fill ? GL_POLYGON : GL_LINE_LOOP);
  glVertex2f(scaleX(x1), scaleY(y1));
  glVertex2f(scaleX(x2), scaleY(y1));
  glVertex2f(scaleX(x2), scaleY(y2));
  glVertex2f(scaleX(x1), scaleY(y2));
  glEnd();
}

// draw ellipse
// see: https://stackoverflow.com/questions/5886628/effecient-way-to-draw-ellipse-with-opengl-or-d3d
SBLIB_API void sblib_ellipse(int xc, int yc, int xr, int yr, int fill) {
  // precalculate the sine and cosine
  double theta = (2.0 * M_PI) / ellipse_segments;
  float s = sin(theta);
  float c = cos(theta);
  float x = 1;
  float y = 0;

  glColor3f(_fg_color._r, _fg_color._g, _fg_color._b);
  glBegin(fill ? GL_POLYGON : GL_LINE_LOOP);

  for (int i = 0; i < ellipse_segments; i++)  {
    // apply radius and offset
    glVertex2f(scaleX(x * xr + xc), scaleY(y * yr + yc));

    // apply the rotation matrix
    float t = x;
    x = c * x - s * y;
    y = s * t + c * y;
  }
  glEnd();
}

