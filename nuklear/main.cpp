// This file is part of SmallBASIC
//
// Plugin for nukler GUI library - https://github.com/vurtun/nuklear.git
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2020 Chris Warren-Smith

#include "config.h"

#define NK_GLFW_GL2_IMPLEMENTATION
#define NK_IMPLEMENTATION
#define NK_INCLUDE_COMMAND_USERDATA
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_KEYSTATE_BASED_INPUT
#define GLAD_GL_IMPLEMENTATION

#include <cstddef>
#include <cstdio>
#include <cstddef>
#include <cstring>

#include "Nuklear/nuklear.h"
#include "include/var.h"
#include "include/module.h"
#include "include/param.h"
#include "nkbd.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "Nuklear/demo/glfw_opengl2/nuklear_glfw_gl2.h"

static GLFWwindow *_window;

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 480
#define MAX_FLOATS 40
#define MAX_COMBOBOX_ITEMS 10
#define MAX_EDIT_BUFFER_LEN 2048
#define WAIT_INTERVAL_MILLIS 5
#define WAIT_INTERVAL (WAIT_INTERVAL_MILLIS/1000)
#define M_PI 3.14159265358979323846

static double ellipse_segments = 20.0;
static struct nk_context *_ctx;
static float _floats[MAX_FLOATS];
static const char *_comboboxItems[MAX_COMBOBOX_ITEMS];
static char _edit_buffer[MAX_EDIT_BUFFER_LEN];
static struct nk_color _fg_color;
static struct nk_color _bg_color;
static struct nk_color _focus_color;
static float _line_thickness;
static bool _isExit;
static int _width;
static int _height;

enum drawmode {DRAW_FILL, DRAW_LINE, DRAW_NONE};

const nk_color _colors[] = {
  {0  ,0  ,0  ,255}, // 0 black
  {0  ,0  ,128,255}, // 1 blue
  {0  ,128,0  ,255}, // 2 green
  {0  ,128,128,255}, // 3 cyan
  {128,0  ,0  ,255}, // 4 red
  {128,0  ,128,255}, // 5 magenta
  {128,128,0  ,255}, // 6 yellow
  {192,192,192,255}, // 7 white
  {128,128,128,255}, // 8 gray
  {0  ,0  ,255,255}, // 9 light blue
  {0  ,255,0  ,255}, // 10 light green
  {0  ,255,255,255}, // 11 light cyan
  {255,0  ,0  ,255}, // 12 light red
  {255,0  ,255,255}, // 13 light magenta
  {255,255,0  ,255}, // 14 light yellow
  {255,255,255,255}  // 15 bright white
};

static void error_callback(int e, const char *d) {
  _isExit = true;
  printf("Error %d: %s\n", e, d);
}

static void window_size_callback(GLFWwindow* window, int width, int height) {
  _width = width;
  _height = height;
}

nk_context *nkp_create_window(const char *title, int width, int height) {
  if (!glfwInit()) {
    fprintf(stdout, "[GFLW] failed to init!\n");
    return nullptr;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  _window = glfwCreateWindow((width < 10) ? WINDOW_WIDTH : width,
                             (height < 10) ? WINDOW_HEIGHT : height,
                             title, nullptr, nullptr);

  glfwMakeContextCurrent(_window);
  gladLoadGL((GLADloadfunc) glfwGetProcAddress);
  glfwSetErrorCallback(error_callback);
  glfwSetWindowSizeCallback(_window, window_size_callback);
  glfwGetWindowSize(_window, &_width, &_height);
  glViewport(0, 0, _width, _height);

  nk_context *result = nk_glfw3_init(_window, NK_GLFW3_INSTALL_CALLBACKS);
  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

  struct nk_font_atlas *atlas;
  nk_glfw3_font_stash_begin(&atlas);
  nk_glfw3_font_stash_end();
  _isExit = false;

  nkbd_create(result);
  return result;
}

bool nkp_process_events() {
  if (glfwWindowShouldClose(_window)) {
    _isExit = true;
  } else {
    glfwWaitEvents();
    nk_glfw3_new_frame();
  }

  return true;
}

void nkp_set_window_title(const char *title) {
  glfwSetWindowTitle(_window, title);
}

void nkp_windowend() {
  glViewport(0, 0, _width, _height);
  glClear(GL_COLOR_BUFFER_BIT);
  struct nk_colorf c = nk_color_cf(_bg_color);
  glClearColor(c.r, c.g, c.b, c.a);
  nk_glfw3_render(NK_ANTI_ALIASING_ON);
  glfwSwapBuffers(_window);
}

void nkp_close() {
  nkbd_destroy(_ctx);
  nk_glfw3_shutdown();
  glfwTerminate();
}

static int is_hex(char c) {
  return ((c >= '0' && c <= '9') ||
          (c >= 'a' && c <= 'f') ||
          (c >= 'A' && c <= 'F'));
}

static int is_color(const char *str, size_t len) {
  int result = 0;
  if ((len == 7 || len == 9) && str[0] == '#') {
    result = 1;
    for (int i = 1; i < len && result; i++) {
      result = is_hex(str[i]);
    }
  }
  return result;
}

static nk_flags get_alignment(const char *s) {
  nk_flags result;
  if (!strcasecmp(s, "left")) {
    result = NK_TEXT_LEFT;
  } else if (!strcasecmp(s, "centered")) {
    result = NK_TEXT_CENTERED;
  } else if (!strcasecmp(s, "right")) {
    result = NK_TEXT_RIGHT;
  } else if (!strcasecmp(s, "top left")) {
    result = NK_TEXT_ALIGN_TOP | NK_TEXT_ALIGN_LEFT;
  } else if (!strcasecmp(s, "top centered")) {
    result = NK_TEXT_ALIGN_TOP | NK_TEXT_ALIGN_CENTERED;
  } else if (!strcasecmp(s, "top right")) {
    result = NK_TEXT_ALIGN_TOP | NK_TEXT_ALIGN_RIGHT;
  } else if (!strcasecmp(s, "bottom left")) {
    result = NK_TEXT_ALIGN_BOTTOM | NK_TEXT_ALIGN_LEFT;
  } else if (!strcasecmp(s, "bottom centered")) {
    result = NK_TEXT_ALIGN_BOTTOM | NK_TEXT_ALIGN_CENTERED;
  } else if (!strcasecmp(s, "bottom right")) {
    result = NK_TEXT_ALIGN_BOTTOM | NK_TEXT_ALIGN_RIGHT;
  } else {
    result = 0;
  }
  return result;
}

static struct nk_color get_color(long c) {
  nk_color result;
  if (c >= 0 && c < 16) {
    result = _colors[c];
  } else {
    if (c < 0) {
      c = -c;
    }
    uint8_t r = (c & 0xff0000) >> 16;
    uint8_t g = (c & 0xff00) >> 8;
    uint8_t b = (c & 0xff);
    result = nk_rgba(r, g, b, 255);
  }
  return result;
}

static void get_rgba_str(struct nk_color c, char *str) {
  if (c.a < 255) {
    sprintf(str, "#%02x%02x%02x%02x", c.r, c.g, c.b, c.a);
  } else {
    sprintf(str, "#%02x%02x%02x", c.r, c.g, c.b);
  }
}

static int get_value(const char *str, int range) {
  char *end;
  int result = nk_strtoi(str, &end);
  if (*end == '%') {
    result = result * range / 100;
  }
  for (const char *next = end; *next != '\0'; next++) {
    if (*next == '+') {
      result += nk_strtoi(next + 1, &end);
      break;
    } else if (*next == '-') {
      result -= nk_strtoi(next + 1, &end);
      break;
    }
  }
  return result;
}

static nk_color get_param_color(int argc, slib_par_t *params, int n) {
  nk_color result;
  const char *color = get_param_str(argc, params, n, nullptr);
  if (color != nullptr && is_color(color, v_strlen(params[n].var_p))) {
    result = nk_rgb_hex(color);
  } else {
    result = nk_black;
  }
  return result;
}

static struct nk_rect get_param_rect(int argc, slib_par_t *params, int n) {
  int w, h;
  int x = get_param_int(argc, params, n, 0);
  int y = get_param_int(argc, params, n + 1, 0);

  bool strw = is_param_str(argc, params, n + 2);
  bool strh = is_param_str(argc, params, n + 3);
  if (strw || strh) {
    if (strw) {
      w = get_value(get_param_str(argc, params, n + 2, ""), _width);
    } else {
      w = get_param_int(argc, params, n + 2, WINDOW_WIDTH);
    }
    if (strh) {
      h = get_value(get_param_str(argc, params, n + 3, ""), _height);
    } else {
      h = get_param_int(argc, params, n + 3, WINDOW_HEIGHT);
    }
  } else {
    w = get_param_int(argc, params, n + 2, WINDOW_WIDTH);
    h = get_param_int(argc, params, n + 3, WINDOW_HEIGHT);
  }
  return nk_rect(x, y, w, h);
}

static enum drawmode get_param_draw_mode(int argc, slib_par_t *params, int n) {
  const char *mode = get_param_str(argc, params, n, "none");
  drawmode result;
  if (!strcasecmp(mode, "fill")) {
    result = DRAW_FILL;
  } else if (!strcasecmp(mode, "line")) {
    result = DRAW_LINE;
  } else {
    result = DRAW_NONE;
  }
  return result;
}

static nk_flags get_param_window_flags(int argc, slib_par_t *params, int n) {
  nk_flags flags = 0;
  for (int i = n; i < argc; i++) {
    const char *flag = get_param_str(argc, params, i, nullptr);
    if (flag != nullptr && flag[0] != '\0') {
      if (!strcasecmp(flag, "border")) {
        flags |= NK_WINDOW_BORDER;
      } else if (!strcasecmp(flag, "movable")) {
        flags |= NK_WINDOW_MOVABLE;
      } else if (!strcasecmp(flag, "scalable")) {
        flags |= NK_WINDOW_SCALABLE;
      } else if (!strcasecmp(flag, "closable")) {
        flags |= NK_WINDOW_CLOSABLE;
      } else if (!strcasecmp(flag, "minimizable")) {
        flags |= NK_WINDOW_MINIMIZABLE;
      } else if (!strcasecmp(flag, "no_scrollbar")) {
        flags |= NK_WINDOW_NO_SCROLLBAR;
      } else if (!strcasecmp(flag, "title")) {
        flags |= NK_WINDOW_TITLE;
      } else if (!strcasecmp(flag, "scroll_auto_hide")) {
        flags |= NK_WINDOW_SCROLL_AUTO_HIDE;
      } else if (!strcasecmp(flag, "background")) {
        flags |= NK_WINDOW_BACKGROUND;
      } else if (!strcasecmp(flag, "scale_left")) {
        flags |= NK_WINDOW_SCALE_LEFT;
      } else if (!strcasecmp(flag, "no_input")) {
        flags |= NK_WINDOW_NO_INPUT;
      }
    }
  }
  return flags;
}

static int cmd_arc(int argc, slib_par_t *params, var_t *retval) {
  enum drawmode mode = get_param_draw_mode(argc, params, 0);
  float cx = get_param_num(argc, params, 1, 0);
  float cy = get_param_num(argc, params, 2, 0);
  float r = get_param_num(argc, params, 3, 0);
  float a0 = get_param_num(argc, params, 4, 0);
  float a1 = get_param_num(argc, params, 5, 0);
  if (mode == DRAW_FILL) {
    nk_fill_arc(&_ctx->current->buffer, cx, cy, r, a0, a1, _fg_color);
  } else if (mode == DRAW_LINE) {
    nk_stroke_arc(&_ctx->current->buffer, cx, cy, r, a0, a1, _line_thickness, _fg_color);
  }
  return 1;
}

static int cmd_button(int argc, slib_par_t *params, var_t *retval) {
  const char *title = get_param_str(argc, params, 0, nullptr);
  int result;
  if (title != nullptr) {
    nkbd_widget_begin(_ctx);
    if (is_color(title, v_strlen(params[0].var_p))) {
      v_setint(retval, nk_button_color(_ctx, nk_rgb_hex(title)));
    } else {
      v_setint(retval, nk_button_label(_ctx, title));
    }
    if (nkbd_widget_end(_ctx, retval->v.i)) {
      retval->v.i = 1;
    }
    result = 1;
  } else {
    v_setstr(retval, "Invalid button input");
    result = 0;
  }
  return result;
}

static int cmd_checkbox(int argc, slib_par_t *params, var_t *retval) {
  const char *label = get_param_str(argc, params, 0, nullptr);
  if (label != nullptr && is_param_map(argc, params, 1)) {
    nkbd_widget_begin(_ctx);
    var_t *map = params[1].var_p;
    int value = map_get_bool(map, "value");
    int changed = nk_checkbox_label(_ctx, label, &value);
    if (changed) {
      // value mutated in nk_checkbox_label()
      map_set_int(map, "value", value);
    }
    if (nkbd_widget_end(_ctx, changed) && !changed) {
      changed = 1;
      map_set_int(map, "value", !value);
    }
    v_setint(retval, value);
  } else {
    v_setstr(retval, "Invalid checkbox input");
  }
  return 1;
}

static int cmd_circle(int argc, slib_par_t *params, var_t *retval) {
  enum drawmode mode = get_param_draw_mode(argc, params, 0);
  float x = get_param_num(argc, params, 1, 0);
  float y = get_param_num(argc, params, 2, 0);
  float r = get_param_num(argc, params, 3, 0);
  if (mode == DRAW_FILL) {
    nk_fill_circle(&_ctx->current->buffer, nk_rect(x - r, y - r, r * 2, r * 2), _fg_color);
  } else if (mode == DRAW_LINE) {
    nk_stroke_circle(&_ctx->current->buffer, nk_rect(x - r, y - r, r * 2, r * 2), _line_thickness, _fg_color);
  }
  return 1;
}

static int cmd_colorpicker(int argc, slib_par_t *params, var_t *retval) {
  int result = 1;
  char new_color_string[10];
  if (is_param_str(argc, params, 0)) {
    struct nk_colorf color = nk_color_cf(get_param_color(argc, params, 0));
    color = nk_color_picker(_ctx, color, NK_RGB);
    get_rgba_str(nk_rgba_cf(color), new_color_string);
  } else if (is_param_map(argc, params, 0)) {
    const char *value = get_param_str_field(argc, params, 0, "value");
    if (value != nullptr) {
      struct nk_colorf color = nk_color_cf(nk_rgb_hex(value));
      int changed = nk_color_pick(_ctx, &color, NK_RGB);
      if (changed) {
        get_rgba_str(nk_rgba_cf(color), new_color_string);
        v_setstr(map_get(params[0].var_p, "value"), new_color_string);
      }
    } else {
      v_setstr(retval, "Invalid colorpicker input");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid colorpicker input");
    result = 0;
  }
  return result;
}

static int cmd_combobox(int argc, slib_par_t *params, var_t *retval) {
  int success = 0;
  if (is_param_map(argc, params, 0)) {
    var_t *map = params[0].var_p;
    var_p_t v_value = map_get(map, "value");
    var_p_t v_items = map_get(map, "items");
    if (v_is_type(v_value, V_INT) &&
        v_is_type(v_items, V_ARRAY)) {
      int len = v_asize(v_items);
      int count = 0;
      for (int i = 0; i < len && i < MAX_COMBOBOX_ITEMS; i++) {
        var_p_t elem = v_elem(v_items, i);
        if (v_is_type(elem, V_STR)) {
          _comboboxItems[count++] = elem->v.p.ptr;
        }
      }
      struct nk_rect bounds = nk_widget_bounds(_ctx);
      struct nk_vec2 size = nk_vec2(bounds.w, bounds.h * 8);
      int selected = v_value->v.i;

      nkbd_combo_begin(_ctx);
      nk_combobox(_ctx, _comboboxItems, count, &selected, bounds.h, size);

      nk_flags state;
      bool active = nk_button_behavior(&state, bounds, &_ctx->input, NK_BUTTON_DEFAULT);
      nkbd_widget_end(_ctx, active);

      v_value->v.i = selected;
      success = 1;
    }
  }
  if (!success) {
    v_setstr(retval, "Invalid combobox input");
  }
  return success;
}

static int cmd_contextualbegin(int argc, slib_par_t *params, var_t *retval) {
  struct nk_vec2 size;
  size.x = get_param_num(argc, params, 0, 0);
  size.y = get_param_num(argc, params, 1, 0);
  struct nk_rect trigger = get_param_rect(argc, params, 2);
  nk_flags flags = NK_WINDOW_NO_SCROLLBAR;
  v_setint(retval, nk_contextual_begin(_ctx, flags, size, trigger));
  return 1;
}

static int cmd_contextualend(int argc, slib_par_t *params, var_t *retval) {
  nk_contextual_end(_ctx);
  return 1;
}

static int cmd_contextualitem(int argc, slib_par_t *params, var_t *retval) {
  const char *text = get_param_str(argc, params, 0, nullptr);
  if (text != nullptr) {
    nk_contextual_item_symbol_label(_ctx, NK_SYMBOL_NONE, text, NK_TEXT_LEFT);
  }
  return 1;
}

static int cmd_curve(int argc, slib_par_t *params, var_t *retval) {
  float ax = get_param_num(argc, params, 0, 0);
  float ay = get_param_num(argc, params, 1, 0);
  float ctrl0x = get_param_num(argc, params, 2, 0);
  float ctrl0y = get_param_num(argc, params, 3, 0);
  float ctrl1x = get_param_num(argc, params, 4, 0);
  float ctrl1y = get_param_num(argc, params, 5, 0);
  float bx = get_param_num(argc, params, 6, 0);
  float by = get_param_num(argc, params, 7, 0);
  nk_stroke_curve(&_ctx->current->buffer, ax, ay, ctrl0x, ctrl0y, ctrl1x, ctrl1y, bx, by, _line_thickness, _fg_color);
  return 1;
}

static int cmd_edit(int argc, slib_par_t *params, var_t *retval) {
  int success = 0;
  const char *styleStr = get_param_str(argc, params, 0, nullptr);
  if (styleStr != nullptr && is_param_map(argc, params, 1)) {
    nk_flags style = 0;
    if (!strcasecmp(styleStr, "simple")) {
      style = NK_EDIT_SIMPLE;
    } else if (!strcasecmp(styleStr, "field")) {
      style = NK_EDIT_FIELD;
    } else if (!strcasecmp(styleStr, "editor")) {
      style = NK_EDIT_EDITOR;
    } else if (!strcasecmp(styleStr, "box")) {
      style = NK_EDIT_BOX;
    }
    style &= ~NK_EDIT_ALLOW_TAB;
    const char *value = get_param_str_field(argc, params, 1, "value");
    if (value != nullptr && style != 0) {
      nkbd_widget_begin(_ctx);
      size_t len = NK_CLAMP(0, strlen(value), MAX_EDIT_BUFFER_LEN - 1);
      memcpy(_edit_buffer, value, len);
      _edit_buffer[len] = '\0';
      nk_flags event = nk_edit_string_zero_terminated(_ctx, style, _edit_buffer, MAX_EDIT_BUFFER_LEN - 1, nk_filter_default);
      v_setstr(map_get(params[1].var_p, "value"), _edit_buffer);
      nkbd_edit_end(_ctx, 0);
      success = 1;
    }
  } else {
    v_setstr(retval, "Invalid edit input");
  }
  return success;
}

static int cmd_text(int argc, slib_par_t *params, var_t *retval) {
  const char *text = get_param_str(argc, params, 0, nullptr);
  float x = get_param_num(argc, params, 1, 0);
  float y = get_param_num(argc, params, 2, 0);
  float w = get_param_num(argc, params, 3, 0);
  float h = get_param_num(argc, params, 4, 0);
  nk_draw_text(&_ctx->current->buffer, nk_rect(x, y, w, h), text, strlen(text),
               _ctx->style.font, nk_rgba(0, 0, 0, 0), _fg_color);
  return 1;
}

static int cmd_ellipse(int argc, slib_par_t *params, var_t *retval) {
  enum drawmode mode = get_param_draw_mode(argc, params, 0);
  float x = get_param_num(argc, params, 1, 0);
  float y = get_param_num(argc, params, 2, 0);
  float rx = get_param_num(argc, params, 3, 0);
  float ry = get_param_num(argc, params, 4, 0);
  if (mode == DRAW_FILL) {
    nk_fill_circle(&_ctx->current->buffer, nk_rect(x - rx, y - ry, rx * 2, ry * 2), _fg_color);
  } else if (mode == DRAW_LINE) {
    nk_stroke_circle(&_ctx->current->buffer, nk_rect(x - rx, y - ry, rx * 2, ry * 2), _line_thickness, _fg_color);
  }
  return 1;
}

static int cmd_groupbegin(int argc, slib_par_t *params, var_t *retval) {
  const char *title = get_param_str(argc, params, 0, nullptr);
  nk_flags flags = get_param_window_flags(argc, params, 1);
  return nk_group_begin(_ctx, title, flags);
}

static int cmd_groupend(int argc, slib_par_t *params, var_t *retval) {
  nk_group_end(_ctx);
  return 1;
}

static int cmd_label(int argc, slib_par_t *params, var_t *retval) {
  const char *label = get_param_str(argc, params, 0, nullptr);
  const char *position = get_param_str(argc, params, 1, "left");
  int result;
  if (label != nullptr) {
    bool wrap = false;
    bool use_color = 0;
    struct nk_color color;
    for (int n = 2; n < argc; n++) {
      if (params[n].var_p->type == V_STR) {
        const char *str = params[n].var_p->v.p.ptr;
        if (!strcasecmp(str, "wrap")) {
          wrap = true;
        } else {
          color = nk_rgb_hex(str);
          use_color = true;
        }
      }
    }
    if (wrap && use_color) {
      nk_label_colored_wrap(_ctx, label, color);
    } else if (use_color) {
      nk_label_colored(_ctx, label, get_alignment(position), color);
    } else if (wrap) {
      nk_label_wrap(_ctx, label);
    } else {
      nk_label(_ctx, label, get_alignment(position));
    }
    result = 1;
  } else {
    v_setstr(retval, "Invalid label input");
    result = 0;
  }
  return result;
}

static int cmd_layoutrow(int argc, slib_par_t *params, var_t *retval) {
  const char *format = get_param_str(argc, params, 0, "_invalid");
  var_num_t height = get_param_num(argc, params, 1, 0);
  int cols = get_param_int(argc, params, 2, 0);
  int width = get_param_int(argc, params, 3, 0);
  int result = 1;
  if (!strcasecmp("dynamic", format)) {
    nk_row_layout(_ctx, NK_DYNAMIC, height, cols, width);
  } else if (!strcasecmp("static", format)) {
    nk_row_layout(_ctx, NK_STATIC, height, width, cols);
  } else {
    v_setstr(retval, "Invalid layout format");
    result = 0;
  }
  return result;
}

static int cmd_layoutrowbegin(int argc, slib_par_t *params, var_t *retval) {
  const char *format = get_param_str(argc, params, 0, "_invalid");
  var_num_t height = get_param_num(argc, params, 1, 0);
  int cols = get_param_int(argc, params, 2, 0);
  int result = 1;
  if (!strcasecmp("static", format)) {
    nk_layout_row_begin(_ctx, NK_STATIC, height, cols);
  } else if (!strcasecmp("dynamic", format)) {
    nk_layout_row_begin(_ctx, NK_DYNAMIC, height, cols);
  } else {
    v_setstr(retval, "Invalid layout format");
    result = 0;
  }
  return result;
}

static int cmd_layoutrowpush(int argc, slib_par_t *params, var_t *retval) {
  var_num_t ratio_or_width = get_param_num(argc, params, 0, 0);
  nk_layout_row_push(_ctx, ratio_or_width);
  return 1;
}

static int cmd_layoutrowend(int argc, slib_par_t *params, var_t *retval) {
  nk_layout_row_end(_ctx);
  return 1;
}

static int cmd_line(int argc, slib_par_t *params, var_t *retval) {
  for (int i = 0; i < argc && i < MAX_FLOATS; ++i) {
    _floats[i] = get_param_num(argc, params, i, 0);
  }
  nk_stroke_polyline(&_ctx->current->buffer, _floats, argc / 2, _line_thickness, _fg_color);
  return (argc >= 4 && argc % 2 == 0);
}

static int cmd_menubegin(int argc, slib_par_t *params, var_t *retval) {
  int result;
  const char *text = get_param_str(argc, params, 0, nullptr);
  if (text != nullptr) {
    struct nk_vec2 size;
    size.x = get_param_num(argc, params, 2, 0);
    size.y = get_param_num(argc, params, 3, 0);
    v_setint(retval, nk_menu_begin_label(_ctx, text, NK_TEXT_LEFT, size));
    result = 1;
  } else {
    v_setstr(retval, "Invalid menu begin input");
    result = 0;
  }
  return result;
}

static int cmd_menuend(int argc, slib_par_t *params, var_t *retval) {
  nk_menu_end(_ctx);
  return 1;
}

static int cmd_menuitem(int argc, slib_par_t *params, var_t *retval) {
  int result;
  const char *text = get_param_str(argc, params, 0, nullptr);
  if (text != nullptr) {
    v_setint(retval, nk_menu_item_label(_ctx, text, NK_TEXT_LEFT));
    result = 1;
  } else {
    v_setstr(retval, "Invalid menu item input");
    result = 0;
  }
  return result;
}

static int cmd_menubarbegin(int argc, slib_par_t *params, var_t *retval) {
  nk_menubar_begin(_ctx);
  return 1;
}

static int cmd_menubarend(int argc, slib_par_t *params, var_t *retval) {
  nk_menubar_end(_ctx);
  return 1;
}

static int cmd_polygon(int argc, slib_par_t *params, var_t *retval) {
  enum drawmode mode = get_param_draw_mode(argc, params, 0);
  for (int i = 0; i < argc - 1 && i < MAX_FLOATS; ++i) {
    _floats[i] = get_param_num(argc, params, i + 1, 0);
  }
  if (mode == DRAW_FILL) {
    nk_fill_polygon(&_ctx->current->buffer, _floats, (argc - 1) / 2, _fg_color);
  } else if (mode == DRAW_LINE) {
    nk_stroke_polygon(&_ctx->current->buffer, _floats, (argc - 1) / 2, _line_thickness, _fg_color);
  }
  return (argc >= 2);
}

static int cmd_progress(int argc, slib_par_t *params, var_t *retval) {
  nk_size max = get_param_num(argc, params, 1, 0);
  int modifiable = get_param_int(argc, params, 2, 0);
  if (is_param_map(argc, params, 0)) {
    nk_size value = (nk_size)get_param_num_field(argc, params, 0, "value");
    int changed = nk_progress(_ctx, &value, max, modifiable);
    if (changed) {
      v_setreal(map_get(params[0].var_p, "value"), value);
    }
  } else {
    v_setstr(retval, "Invalid progress input");
  }
  return 1;
}

static int cmd_property(int argc, slib_par_t *params, var_t *retval) {
  const char *name = get_param_str(argc, params, 0, nullptr);
  double min = get_param_num(argc, params, 1, 0);
  double max = get_param_num(argc, params, 3, 0);
  double step = get_param_num(argc, params, 4, 0);
  double inc_per_pixel = get_param_num(argc, params, 5, 0);
  if (is_param_num(argc, params, 2)) {
    double value = get_param_num(argc, params, 2, 0);
    value = nk_propertyd(_ctx, name, min, value, max, step, inc_per_pixel);
  } else if (is_param_map(argc, params, 2)) {
    double value = get_param_num_field(argc, params, 2, "value");
    double old = value;
    nk_property_double(_ctx, name, min, &value, max, step, inc_per_pixel);
    if (value != old) {
      v_setreal(map_get(params[2].var_p, "value"), value);
    }
  } else {
    v_setstr(retval, "Invalid property input");
  }
  return 1;
}

static int cmd_radio(int argc, slib_par_t *params, var_t *retval) {
  int result;
  const char *name = get_param_str(argc, params, 0, nullptr);
  if (name != nullptr && is_param_map(argc, params, 1)) {
    nkbd_widget_begin(_ctx);
    const char *value = get_param_str_field(argc, params, 1, "value");
    int active = !strcasecmp(value, name);
    int changed = nk_radio_label(_ctx, name, &active);
    if (changed && active) {
      v_setstr(map_get(params[1].var_p, "value"), name);
    }
    if (nkbd_widget_end(_ctx, changed) && !changed) {
      v_setstr(map_get(params[1].var_p, "value"), name);
    }
    result = 1;
  } else {
    v_setstr(retval, "Invalid radio input");
    result = 0;
  }
  return result;
}

static int cmd_rectmulticolor(int argc, slib_par_t *params, var_t *retval) {
  float x = get_param_num(argc, params, 0, 0);
  float y = get_param_num(argc, params, 1, 0);
  float w = get_param_num(argc, params, 2, 0);
  float h = get_param_num(argc, params, 3, 0);
  struct nk_color topLeft = get_param_color(argc, params, 4);
  struct nk_color topRight = get_param_color(argc, params, 5);
  struct nk_color bottomLeft = get_param_color(argc, params, 6);
  struct nk_color bottomRight = get_param_color(argc, params, 7);
  nk_fill_rect_multi_color(&_ctx->current->buffer, nk_rect(x, y, w, h), topLeft, topRight, bottomLeft, bottomRight);
  return 1;
}

static int cmd_selectable(int argc, slib_par_t *params, var_t *retval) {
  const char *text = get_param_str(argc, params, 0, nullptr);
  if (text != nullptr && is_param_map(argc, params, 1)) {
    nkbd_widget_begin(_ctx);
    int value = get_param_num_field(argc, params, 1, "value");
    nk_flags align = NK_TEXT_LEFT;
    int changed = nk_selectable_label(_ctx, text, align, &value);
    if (changed) {
      v_setint(map_get(params[1].var_p, "value"), value);
    }
    if (nkbd_widget_end(_ctx, changed) && !changed) {
      v_setint(map_get(params[1].var_p, "value"), !value);
    }
  } else {
    v_setstr(retval, "Invalid selectable input");
  }
  return 1;
}

static int cmd_slider(int argc, slib_par_t *params, var_t *retval) {
  float min = get_param_num(argc, params, 0, 0);
  float max = get_param_num(argc, params, 2, 0);
  float step = get_param_num(argc, params, 3, 0);
  int result = 1;
  if (is_param_map(argc, params, 1)) {
    float value = get_param_num_field(argc, params, 1, "value");
    int changed = nk_slider_float(_ctx, min, &value, max, step);
    if (changed) {
      v_setreal(map_get(params[1].var_p, "value"), value);
    }
  } else {
    v_setstr(retval, "Invalid slider input");
    result = 0;
  }
  return result;
}

static int cmd_spacing(int argc, slib_par_t *params, var_t *retval) {
  nk_spacing(_ctx, get_param_int(argc, params, 0, 0));
  return 1;
}

static int cmd_tooltip(int argc, slib_par_t *params, var_t *retval) {
  const char *text = get_param_str(argc, params, 0, nullptr);
  if (text) {
    nk_tooltip(_ctx, text);
  }
  return 1;
}

static int cmd_treepop(int argc, slib_par_t *params, var_t *retval) {
  nk_tree_pop(_ctx);
  return 1;
}

static int cmd_treepush(int argc, slib_par_t *params, var_t *retval) {
  const char *typeStr = get_param_str(argc, params, 0, nullptr);
  const char *title = get_param_str(argc, params, 1, nullptr);
  enum nk_tree_type type = NK_TREE_NODE;

  if (typeStr != nullptr) {
    if (!strcasecmp(typeStr, "node")) {
      type = NK_TREE_NODE;
    } else if (!strcasecmp(typeStr, "tab")) {
      type = NK_TREE_TAB;
    }
  }
  if (title != nullptr) {
    enum nk_collapse_states state = NK_MINIMIZED;
    int id = 0;
    int open = nk_tree_push_hashed(_ctx, type, title, state, title, strlen(title), id);
    v_setint(retval, open);
  } else {
    v_setstr(retval, "Invalid treepush input");
  }
  return 1;
}

static int cmd_widgetbounds(int argc, slib_par_t *params, var_t *retval) {
  struct nk_rect rc = nk_widget_bounds(_ctx);
  v_toarray1(retval, 4);
  v_setint(v_elem(retval, 0), rc.x);
  v_setint(v_elem(retval, 1), rc.y);
  v_setint(v_elem(retval, 2), rc.w);
  v_setint(v_elem(retval, 3), rc.h);
  return 1;
}

static int cmd_widgetishovered(int argc, slib_par_t *params, var_t *retval) {
  v_setint(retval, nk_widget_is_hovered(_ctx));
  return 1;
}

static int cmd_windowbegin(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_ctx != nullptr) {
    nkp_process_events();
    nkbd_begin(_ctx);
    const char *title = get_param_str(argc, params, 0, "Untitled");
    struct nk_rect rc = get_param_rect(argc, params, 1);
    nk_flags flags = get_param_window_flags(argc, params, 5);
    v_setint(retval, nk_begin(_ctx, title, rc, flags));
    if ((flags & NK_WINDOW_TITLE) == 0) {
      nkp_set_window_title(title);
    }
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_windowend(int argc, slib_par_t *params, var_t *retval) {
  nkbd_end(_ctx, _focus_color);
  nk_end(_ctx);
  nkp_windowend();
  return 1;
}

static int cmd_windowgetbounds(int argc, slib_par_t *params, var_t *retval) {
  struct nk_rect rc = nk_window_get_bounds(_ctx);
  v_toarray1(retval, 4);
  v_setint(v_elem(retval, 0), rc.x);
  v_setint(v_elem(retval, 1), rc.y);
  v_setint(v_elem(retval, 2), rc.w);
  v_setint(v_elem(retval, 3), rc.h);
  return 1;
}

// nk.lineWidth(10)
static int cmd_line_width(int argc, slib_par_t *params, var_t *retval) {
  glLineWidth(get_param_int(argc, params, 0, 1));
  return 1;
}

// nk.pollEvents()
static int cmd_poll_events(int argc, slib_par_t *params, var_t *retval) {
  glfwPollEvents();
  return 1;
}

// nk.waitEvents(n)
static int cmd_wait_events(int argc, slib_par_t *params, var_t *retval) {
  int waitMillis = get_param_int(argc, params, 0, -1);
  if (waitMillis > 0) {
    glfwWaitEventsTimeout(waitMillis / 1000);
  } else {
    glfwWaitEvents();
  }
  return 1;
}

// nk.swapBuffers()
static int cmd_swap_buffers(int argc, slib_par_t *params, var_t *retval) {
  glfwSwapBuffers(_window);
  return 1;
}

FUNC_SIG lib_proc[] = {
  {1, 1,  "BUTTON", cmd_button},
  {1, 1,  "MENUITEM", cmd_menuitem},
  {6, 6,  "ARC", cmd_arc},
  {2, 2,  "CHECKBOX", cmd_checkbox},
  {4, 4,  "CIRCLE", cmd_circle},
  {1, 1,  "COLORPICKER", cmd_colorpicker},
  {1, 10, "COMBOBOX", cmd_combobox},
  {0, 0,  "CONTEXTUALEND", cmd_contextualend},
  {1, 3,  "CONTEXTUALITEM", cmd_contextualitem},
  {8, 8,  "CURVE", cmd_curve},
  {2, 2,  "EDIT", cmd_edit},
  {5, 5,  "ELLIPSE", cmd_ellipse},
  {2, 2,  "GROUPBEGIN", cmd_groupbegin},
  {0, 0,  "GROUPEND", cmd_groupend},
  {1, 10, "LABEL", cmd_label},
  {3, 4,  "LAYOUTROW", cmd_layoutrow},
  {1, 1,  "LAYOUTROWPUSH", cmd_layoutrowpush},
  {3, 3,  "LAYOUTROWBEGIN", cmd_layoutrowbegin},
  {0, 0,  "LAYOUTROWEND", cmd_layoutrowend},
  {2, 40, "LINE", cmd_line},
  {0, 0,  "MENUBARBEGIN", cmd_menubarbegin},
  {0, 0,  "MENUBAREND", cmd_menubarend},
  {0, 0,  "MENUEND", cmd_menuend},
  {2, 40, "POLYGON", cmd_polygon},
  {2, 3,  "PROGRESS", cmd_progress},
  {6, 6,  "PROPERTY", cmd_property},
  {2, 2,  "RADIO", cmd_radio},
  {8, 8,  "RECTMULTICOLOR", cmd_rectmulticolor},
  {2, 4,  "SELECTABLE", cmd_selectable},
  {4, 4,  "SLIDER", cmd_slider},
  {1, 1,  "SPACING", cmd_spacing},
  {5, 5,  "TEXT", cmd_text},
  {1, 1,  "TOOLTIP", cmd_tooltip},
  {0, 0,  "TREEPOP", cmd_treepop},
  {0, 0,  "WINDOWEND", cmd_windowend},
  {1, 1,  "LINEWIDTH", cmd_line_width},
  {0, 0,  "POLLEVENTS", cmd_poll_events},
  {0, 1,  "WAITEVENTS", cmd_wait_events},
  {0, 0,  "SWAPBUFFERS", cmd_swap_buffers}
};

FUNC_SIG lib_func[] = {
  {1, 1,  "BUTTON", cmd_button},
  {1, 1,  "MENUITEM", cmd_menuitem},
  {6, 6,  "CONTEXTUALBEGIN", cmd_contextualbegin},
  {4, 4,  "MENUBEGIN", cmd_menubegin},
  {2, 2,  "TREEPUSH", cmd_treepush},
  {0, 0,  "WIDGETBOUNDS", cmd_widgetbounds},
  {0, 0,  "WIDGETISHOVERED", cmd_widgetishovered},
  {2, 10, "WINDOWBEGIN", cmd_windowbegin},
  {0, 0,  "WINDOWGETBOUNDS", cmd_windowgetbounds}
};

SBLIB_API void sblib_devinit(const char *prog, int width, int height) {
  _bg_color = nk_black;
  _fg_color = nk_white;
  _focus_color = {95, 95, 95, 255};
  _line_thickness = 1;

  const char *name = strrchr(prog, '/');
  if (name == nullptr) {
    name = prog;
  } else {
    name++;
  }
  int len = strlen(name) + 16;
  char *title = new char[len];
  sprintf(title, "%s - SmallBASIC", name);
  _ctx = nkp_create_window(title, width, height);
  delete [] title;
}

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
    fprintf(stderr, "Nuklear: PROC index error [%d]\n", index);
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
    fprintf(stderr, "Nuklear: FUNC index error [%d]\n", index);
    result = 0;
  }
  return result;
}

SBLIB_API void sblib_close() {
  if (_ctx) {
    nkp_close();
  }
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
  return _isExit || glfwWindowShouldClose(_window) ? -2 : 0;
}

SBLIB_API void sblib_setcolor(long fg) {
  _fg_color = get_color(fg);
}

SBLIB_API void sblib_settextcolor(long fg, long bg) {
  _fg_color = get_color(fg);
  _bg_color = get_color(bg);
}

SBLIB_API void sblib_cls() {
  nk_colorf c = nk_color_cf(_bg_color);
  glClearColor(c.r, c.g, c.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void drawBegin(int mode) {
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0.0f, _width, _height, 0.0f, -1.0f, 1.0f);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glBegin(mode);
  nk_colorf c = nk_color_cf(_fg_color);
  glColor3f(c.r, c.g, c.b);
}

void drawEnd() {
  glEnd();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
}

// draw a line
SBLIB_API void sblib_line(int x1, int y1, int x2, int y2) {
  drawBegin(GL_LINES);
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  drawEnd();
}

// draw a pixel
SBLIB_API void sblib_setpixel(int x, int y) {
  drawBegin(GL_POINTS);
  glVertex2f(x, y);
  drawEnd();
}

// draw rectangle
SBLIB_API void sblib_rect(int x1, int y1, int x2, int y2, int fill) {
  drawBegin(fill ? GL_POLYGON : GL_LINE_LOOP);
  glVertex2f(x1, y1);
  glVertex2f(x2, y1);
  glVertex2f(x2, y2);
  glVertex2f(x1, y2);
  drawEnd();
}

SBLIB_API void sblib_ellipse(int xc, int yc, int xr, int yr, int fill) {
  // precalculate the sine and cosine
  double theta = (2.0 * M_PI) / ellipse_segments;
  float s = sin(theta);
  float c = cos(theta);
  float x = 1;
  float y = 0;

  drawBegin(fill ? GL_POLYGON : GL_LINE_LOOP);

  for (int i = 0; i < ellipse_segments; i++)  {
    // apply radius and offset
    glVertex2f(x * xr + xc, y * yr + yc);

    // apply the rotation matrix
    float t = x;
    x = c * x - s * y;
    y = s * t + c * y;
  }
  drawEnd();
}

SBLIB_API int sblib_has_window_ui(void) {
  // module creates a UI in a new window
  return 1;
}
