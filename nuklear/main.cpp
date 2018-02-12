// This file is part of SmallBASIC
//
// Plugin for nukler GUI library - https://github.com/vurtun/nuklear.git
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2018 Chris Warren-Smith

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_FONT_BAKING
#define NK_IMPLEMENTATION

#include "nuklear/nuklear.h"
#include "nuklear/nuklear_sdl_gl2.h"
#include "var.h"
#include "module.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

SDL_Window *win;
SDL_GLContext glContext;
int win_width, win_height;

// GUI
struct nk_context *ctx;
struct nk_colorf bg;

typedef struct API {
  const char *name;
  int (*command)(int, slib_par_t *, var_t *retval);
} API;

int cmd_arc(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
}

int cmd_button(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
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
  return 0;
}

int cmd_layoutrow(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
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
  return 0;
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

  SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  win = SDL_CreateWindow("Demo",
                         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI);
  glContext = SDL_GL_CreateContext(win);
  SDL_GetWindowSize(win, &win_width, &win_height);
  
  // GUI
  ctx = nk_sdl_init(win);

  /* Load Fonts: if none of these are loaded a default font will be used  */
  /* Load Cursor: if you uncomment cursor loading please hide the cursor */
  struct nk_font_atlas *atlas;
  nk_sdl_font_stash_begin(&atlas);

  /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
  /*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16, 0);*/
  /*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
  /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);*/
  /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
  /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
  nk_sdl_font_stash_end();
  /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
  /*nk_style_set_font(ctx, &roboto->handle)*/

  /*set_style(ctx, THEME_WHITE);*/
  /*set_style(ctx, THEME_RED);*/
  /*set_style(ctx, THEME_BLUE);*/
  /*set_style(ctx, THEME_DARK);*/

  bg.r = 0.10f;
  bg.g = 0.18f;
  bg.b = 0.24f;
  bg.a = 1.0f;

  return 0;
}

int cmd_windowend(int param_count, slib_par_t *params, var_t *retval) {
  return 0;
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
  SDL_GetWindowSize(win, &win_width, &win_height);
  glViewport(0, 0, win_width, win_height);
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(bg.r, bg.g, bg.b, bg.a);
  /* IMPORTANT: `nk_sdl_render` modifies some global OpenGL state
   * with blending, scissor, face culling, depth test and viewport and
   * defaults everything back into a default state.
   * Make sure to either a.) save and restore or b.) reset your own state after
   * rendering the UI. */
  nk_sdl_render(NK_ANTI_ALIASING_ON);
  SDL_GL_SwapWindow(win);

  int result = 0;
  SDL_Event evt;
  nk_input_begin(ctx);
  while (SDL_PollEvent(&evt)) {
    if (evt.type == SDL_QUIT) {
      result = -2;
      break;
    }
    nk_sdl_handle_event(&evt);
  }
  nk_input_end(ctx);
  
  return result;
}

void sblib_close() {
  nk_sdl_shutdown();
  SDL_GL_DeleteContext(glContext);
  SDL_DestroyWindow(win);

}
