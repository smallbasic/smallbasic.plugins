// This file is part of SmallBASIC
//
// Plugin for glfw GUI library - https://www.glfw.org/
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2020 Chris Warren-Smith

#include "config.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <stdint.h>
#include <map>

#include "var.h"
#include "var_map.h"
#include "module.h"
#include "param.h"

#define WAIT_INTERVAL_MILLIS 5
#define WAIT_INTERVAL (WAIT_INTERVAL_MILLIS/1000)

std::map<int, GLFWwindow *> _windowMap;
int _nextId = 1;

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

GLFWwindow *get_window(int argc, slib_par_t *params) {
  GLFWwindow *window;
  int windowId = get_param_int(argc, params, 0, -1);
  if (windowId != -1) {
    window = _windowMap.at(windowId);
  } else {
    window = nullptr;
  }
  return window;
}

static void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

// glfw.poll_events()
int cmd_poll_events(int argc, slib_par_t *params, var_t *retval) {
  glfwPollEvents();
  return argc == 0;
}

// glfw.wait_events(n)
int cmd_wait_events(int argc, slib_par_t *params, var_t *retval) {
  int waitMillis = get_param_int(argc, params, 0, -1);
  if (waitMillis > 0) {
    glfwWaitEventsTimeout(waitMillis / 1000);
  } else {
    glfwWaitEvents();
  }
  return argc < 2;
}

// glfw.swap_buffers(window)
int cmd_swap_buffers(int argc, slib_par_t *params, var_t *retval) {
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
int cmd_terminate(int argc, slib_par_t *params, var_t *retval) {
  glfwTerminate();
  return argc == 0;
}

// window = glfw.create_window(640, 480, "Hello World")
int cmd_create_window(int argc, slib_par_t *params, var_t *retval) {
  int width = get_param_int(argc, params, 0, 640);
  int height = get_param_int(argc, params, 1, 480);
  const char *title = get_param_str(argc, params, 2, "SmallBASIC");

  int result = (argc == 3);
  if (result) {
    GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    result = window != nullptr;
    if (result) {
      glfwMakeContextCurrent(window);
      gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
      glfwSwapInterval(1);
      glfwSetErrorCallback(error_callback);
      glfwSetKeyCallback(window, key_callback);

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      // invert Y axis so increasing Y goes down
      //glScalef(1, -1, 1);

      // shift origin up to upper-left corner
      //  glTranslatef(0, -opt_pref_height, 0);

      result = ++_nextId;
      _windowMap[result] = window;
      sblib_settextcolor(1, 0);
      sblib_cls();

      GLenum error = glGetError();
      if (error != GL_NO_ERROR) {
        result = 0;
      }
    }
  }
  v_setint(retval, result);
  return result;
}

// if not glfw.init() then throw "error"
int cmd_init(int argc, slib_par_t *params, var_t *retval) {
  int result = glfwInit();
  if (result) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  }
  v_setint(retval, result);
  return argc == 0;
}

// n = glfw.window_should_close(window)
int cmd_window_should_close(int argc, slib_par_t *params, var_t *retval) {
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

API lib_proc[] = {
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

int sblib_proc_count() {
  return (sizeof(lib_proc) / sizeof(lib_proc[0]));
}

int sblib_func_count() {
  return (sizeof(lib_func) / sizeof(lib_func[0]));
}

int sblib_proc_getname(int index, char *proc_name) {
  int result;
  if (index < sblib_proc_count()) {
    strcpy(proc_name, lib_proc[index].name);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int sblib_func_getname(int index, char *proc_name) {
  int result;
  if (index < sblib_func_count()) {
    strcpy(proc_name, lib_func[index].name);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int sblib_proc_exec(int index, int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (index < sblib_proc_count()) {
    result = lib_proc[index].command(argc, params, retval);
  } else {
    result = 0;
  }
  return result;
}

int sblib_func_exec(int index, int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (index < sblib_proc_count()) {
    result = lib_func[index].command(argc, params, retval);
  } else {
    result = 0;
  }
  return result;
}

int sblib_events(int wait_flag) {
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
  return 0;
}

void sblib_cls() {
  glClear(GL_COLOR_BUFFER_BIT);
}

void sblib_settextcolor(long fg, long bg) {
  _fg_color = get_color(fg);
  _bg_color = get_color(bg);
  glClearColor(_bg_color._r, _bg_color._g, _bg_color._b, 1.0f);
}

void sblib_setcolor(long fg) {
  _fg_color = get_color(fg);
}

// draw a line
void sblib_line(int x1, int y1, int x2, int y2) {
}

// draw an ellipse
void sblib_ellipse(int xc, int yc, int xr, int yr, int fill) {
}

// draw an arc
void sblib_arc(int xc, int yc, double r, double as, double ae, double aspect) {
}

// draw a pixel
void sblib_setpixel(int x, int y) {
  glBegin(GL_POINTS);
  glColor3f(_fg_color._r, _fg_color._g, _fg_color._b);
  glVertex2i(x, y);
  glEnd();
}

// draw rectangle
void sblib_rect(int x1, int y1, int x2, int y2, int fill) {
}
