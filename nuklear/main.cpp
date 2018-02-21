// This file is part of SmallBASIC
//
// Plugin for nukler GUI library - https://github.com/vurtun/nuklear.git
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2018 Chris Warren-Smith

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_FONT_BAKING
#define NK_IMPLEMENTATION
#define NK_SDL_GL2_IMPLEMENTATION

#include "config.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "nuklear/nuklear.h"
#include "nuklear/nuklear_sdl_gl2.h"
#include "var.h"
#include "module.h"

SDL_Window *_window;
SDL_GLContext _glContext;
struct nk_context *_ctx;
bool _sdlExit;

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 480
#define LOOP_DELAY 120

int get_param_int(int param_count, slib_par_t *params, int n, int def) {
  int result;
  if (n >= 0 && n < param_count) {
    switch (params[n].var_p->type) {
    case V_INT:
      result = params[n].var_p->v.i;
      break;
    case V_NUM:
      result = params[n].var_p->v.n;
      break;
    default:
      result = def;
    }
  } else {
    result = def;
  }
  return result;
}

var_num_t get_param_num(int param_count, slib_par_t *params, int n, var_num_t def) {
  var_num_t result;
  var_p_t p = params[n].var_p;
  if (n >= 0 && n < param_count) {
    switch (params[n].var_p->type) {
    case V_INT:
      result = params[n].var_p->v.i;
      break;
    case V_NUM:
      result = params[n].var_p->v.n;
      break;
    default:
      result = def;
      break;
    }
  } else {
    result = def;
  }
  return result;
}

const char *get_param_str(int param_count, slib_par_t *params, int n, const char *def) {
  const char *result;
  static char buf[256];
  if (n >= 0 && n < param_count) {
    switch (params[n].var_p->type) {
    case V_STR:
      result = params[n].var_p->v.p.ptr;
      break;
    case V_INT:
      sprintf(buf, "%ld", params[n].var_p->v.i);
      result = buf;
      break;
    case V_NUM:
      sprintf(buf, "%f", params[n].var_p->v.n);
      result = buf;
      break;
    }
  } else {
    result = def;
  }
  return result;
}

void createWindow(const char *title, int width, int height) {
  SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  _window = SDL_CreateWindow(title,
                             SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
                             SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI);
  _glContext = SDL_GL_CreateContext(_window);
  glClearColor(0.10f, 0.18f, 0.24f, 1.0f);
  _ctx = nk_sdl_init(_window);
  struct nk_font_atlas *atlas;
  nk_sdl_font_stash_begin(&atlas);
  nk_sdl_font_stash_end();
}

typedef struct API {
  const char *name;
  int (*command)(int, slib_par_t *, var_t *retval);
} API;

int cmd_arc(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_button(int param_count, slib_par_t *params, var_t *retval) {
  const char *title = get_param_str(param_count, params, 0, NULL);
  int result;
  if (title != NULL) {
    result = nk_button_label(_ctx, title);
  } else {
    result = 0;
  }
  v_setint(retval, result);
  return param_count == 1;
}

int cmd_checkbox(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_circle(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_colorpicker(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_combobox(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_contextualbegin(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_contextualend(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_contextualitem(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_curve(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_draw(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_edit(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_ellipse(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_framebegin(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_frameend(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_groupbegin(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_groupend(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_image(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_init(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_keypressed(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_label(int param_count, slib_par_t *params, var_t *retval) {
  const char *label = get_param_str(param_count, params, 0, NULL);
  const char *position = get_param_str(param_count, params, 1, NULL);
  int result;
  if (label != NULL && position != NULL) {
    nk_flags alignment;
    if (strcasecmp(position, "left") == 0) {
      alignment = NK_TEXT_LEFT;
    } else if (strcasecmp(position, "centered") == 0) {
      alignment = NK_TEXT_CENTERED;
    } else if (strcasecmp(position, "right") == 0) {
      alignment = NK_TEXT_RIGHT;
    }
    nk_label(_ctx, label, alignment);
    result = 1;
  } else {
    v_setstr(retval, "Empty label and position");
    result = 0;
  }
  return result;
}

int cmd_layoutrow(int param_count, slib_par_t *params, var_t *retval) {
  const char *format = get_param_str(param_count, params, 0, NULL);
  var_num_t height = get_param_num(param_count, params, 1, 0);
  int cols = get_param_int(param_count, params, 2, 0);
  if (format != NULL) {
    if (strcasecmp(format, "dynamic") == 0) {
      nk_row_layout(_ctx, NK_DYNAMIC, height, cols, 0);
    } else if (strcasecmp(format, "static") == 0) {
      nk_row_layout(_ctx, NK_STATIC, height, cols, 0);
    }
  }
  return param_count == 3;
}

int cmd_line(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_menubegin(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_menuend(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_menuitem(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_menubarbegin(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_menubarend(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_mousemoved(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_mousepressed(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_mousereleased(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_polygon(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_progress(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_property(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_radio(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_rectmulticolor(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_selectable(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_slider(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_spacing(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_text(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_textinput(int param_count, slib_par_t *params, var_t *retval) {
  int len;
  char buf[256];
  const char *text = get_param_str(param_count, params, 0, NULL);
  strcpy(buf, text);
  nk_edit_string(_ctx, NK_EDIT_SIMPLE, buf, &len, sizeof(buf) - 1, nk_filter_float);
  buf[len] = 0;
  v_setstr(retval, buf);
  return 1;
}

int cmd_tooltip(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_treepop(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_treepush(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_wheelmoved(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_widgetbounds(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_widgetishovered(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_windowbegin(int param_count, slib_par_t *params, var_t *retval) {
  const char *title = get_param_str(param_count, params, 0, "Untitled");
  int x = get_param_int(param_count, params, 1, 50);
  int y = get_param_int(param_count, params, 2, 50);
  int w = get_param_int(param_count, params, 3, 180);
  int h = get_param_int(param_count, params, 4, 250);

  if (_window == NULL) {
    createWindow(title, WINDOW_WIDTH, WINDOW_HEIGHT);
  }

  SDL_Delay(LOOP_DELAY);
  SDL_Event evt;
  nk_input_begin(_ctx);
  while (SDL_PollEvent(&evt)) {
    if (evt.type == SDL_QUIT) {
      _sdlExit = true;
      break;
    }
    nk_sdl_handle_event(&evt);
  }
  nk_input_end(_ctx);

  nk_flags flags = NK_WINDOW_MINIMIZABLE;
  for (int i = 5; i < param_count; i++) {
    const char *flag = get_param_str(param_count, params, i, NULL);
    if (flag != NULL && flag[0] != '\0') {
      if (strcasecmp(flag, "border") == 0) {
        flags |= NK_WINDOW_BORDER;
      } else if (strcasecmp(flag, "movable") == 0) {
        flags |= NK_WINDOW_MOVABLE;
      } else if (strcasecmp(flag, "title") == 0) {
        flags |= NK_WINDOW_TITLE;
      } else if (strcasecmp(flag, "scalable") == 0) {
        flags |= NK_WINDOW_SCALABLE;
      } else if (strcasecmp(flag, "no_scrollbar") == 0) {
        flags |= NK_WINDOW_NO_SCROLLBAR;
      }
    }
  }
  int result = nk_begin(_ctx, title, nk_rect(x, y, w, h), flags);
  v_setint(retval, result);
  return 1;
}

int cmd_windowend(int param_count, slib_par_t *params, var_t *retval) {
  nk_end(_ctx);

  int width;
  int height;
  SDL_GetWindowSize(_window, &width, &height);
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT);
  /* IMPORTANT: `nk_sdl_render` modifies some global OpenGL state
   * with blending, scissor, face culling, depth test and viewport and
   * defaults everything back into a default state.
   * Make sure to either a.) save and restore or b.) reset your own state after
   * rendering the UI. */
  nk_sdl_render(NK_ANTI_ALIASING_ON);
  SDL_GL_SwapWindow(_window);
  return param_count == 0;
}

int cmd_windowgetbounds(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

API lib_proc[] = {
  {"ARC", cmd_arc},
  {"CHECKBOX", cmd_checkbox},
  {"CIRCLE", cmd_circle},
  {"COLORPICKER", cmd_colorpicker},
  {"COMBOBOX", cmd_combobox},
  {"CONTEXTUALEND", cmd_contextualend},
  {"CONTEXTUALITEM", cmd_contextualitem},
  {"CURVE", cmd_curve},
  {"DRAW", cmd_draw},
  {"EDIT", cmd_edit},
  {"ELLIPSE", cmd_ellipse},
  {"FRAMEBEGIN", cmd_framebegin},
  {"FRAMEEND", cmd_frameend},
  {"GROUPBEGIN", cmd_groupbegin},
  {"GROUPEND", cmd_groupend},
  {"IMAGE", cmd_image},
  {"INIT", cmd_init},
  {"KEYPRESSED", cmd_keypressed},
  {"LABEL", cmd_label},
  {"LAYOUTROW", cmd_layoutrow},
  {"LINE", cmd_line},
  {"MENUBARBEGIN", cmd_menubarbegin},
  {"MENUBAREND", cmd_menubarend},
  {"MENUEND", cmd_menuend},
  {"MENUITEM", cmd_menuitem},
  {"MOUSEMOVED", cmd_mousemoved},
  {"MOUSEPRESSED", cmd_mousepressed},
  {"MOUSERELEASED", cmd_mousereleased},
  {"POLYGON", cmd_polygon},
  {"PROGRESS", cmd_progress},
  {"PROPERTY", cmd_property},
  {"RADIO", cmd_radio},
  {"RECTMULTICOLOR", cmd_rectmulticolor},
  {"SELECTABLE", cmd_selectable},
  {"SLIDER", cmd_slider},
  {"SPACING", cmd_spacing},
  {"TEXT", cmd_text},
  {"TEXTINPUT", cmd_textinput},
  {"TOOLTIP", cmd_tooltip},
  {"TREEPOP", cmd_treepop},
  {"WHEELMOVED", cmd_wheelmoved},
  {"WINDOWEND", cmd_windowend},
};

API lib_func[] = {
  {"BUTTON", cmd_button},
  {"CONTEXTUALBEGIN", cmd_contextualbegin},
  {"MENUBEGIN", cmd_menubegin},
  {"TREEPUSH", cmd_treepush},
  {"WIDGETBOUNDS", cmd_widgetbounds},
  {"WIDGETISHOVERED", cmd_widgetishovered},
  {"WINDOWBEGIN", cmd_windowbegin},
  {"WINDOWGETBOUNDS", cmd_windowgetbounds}
};

int sblib_init(void) {
  _window = NULL;
  _glContext = NULL;
  _ctx = NULL;
  _sdlExit = false;
  return 1;
}

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

int sblib_proc_exec(int index, int param_count, slib_par_t *params, var_t *retval) {
  int result;
  if (index < sblib_proc_count()) {
    result = lib_proc[index].command(param_count, params, retval);
  } else {
    result = 0;
  }
  return result;
}

int sblib_func_exec(int index, int param_count, slib_par_t *params, var_t *retval) {
  int result;
  if (index < sblib_proc_count()) {
    result = lib_func[index].command(param_count, params, retval);
  } else {
    result = 0;
  }
  return result;
}

int sblib_events(int wait_flag) {
  return _sdlExit ? -2 : 0;
}

void sblib_close() {
  if (_ctx) {
    nk_sdl_shutdown();
    SDL_GL_DeleteContext(_glContext);
    SDL_DestroyWindow(_window);
    sblib_init();
  }
}
