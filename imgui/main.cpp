// This file is part of SmallBASIC
//
// Plugin for imgui - https://github.com/ocornut/imgui
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2020 Chris Warren-Smith

#include "config.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <unordered_map>

#include "include/var.h"
#include "include/var_map.h"
#include "include/module.h"
#include "include/param.h"

std::unordered_map<int, GLFWwindow *> _windowMap;
int _nextId = 1;
float _width, _height;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

using namespace ImGui;

static bool is_int_byref(int argc, slib_par_t *params, int n) {
  return (n < argc && params[n].byref &&
          (v_is_type(params[n].var_p, V_INT) || v_is_type(params[n].var_p, V_NUM)));
}

static ImVec2 get_param_vec2(int argc, slib_par_t *params, int n) {
  ImVec2 result;
  if (is_param_map(argc, params, n)) {
    result.x = get_map_num(params[n].var_p, "x");
    result.y = get_map_num(params[n].var_p, "y");
  } else if (is_param_array(argc, params, n)) {
    result.x = get_array_elem_num(params[n].var_p, 0);
    result.y = get_array_elem_num(params[n].var_p, 1);
  } else {
    result.x = 0.0;
    result.y = 0.0;
  }
  return result;
}

static ImVec4 get_param_vec4(int argc, slib_par_t *params, int n) {
  ImVec4 result;
  if (is_param_map(argc, params, n)) {
    result.x = get_map_num(params[n].var_p, "x");
    result.y = get_map_num(params[n].var_p, "y");
    result.z = get_map_num(params[n].var_p, "z");
    result.w = get_map_num(params[n].var_p, "w");
  } else if (is_param_array(argc, params, n)) {
    result.x = get_array_elem_num(params[n].var_p, 0);
    result.y = get_array_elem_num(params[n].var_p, 1);
    result.x = get_array_elem_num(params[n].var_p, 2);
    result.y = get_array_elem_num(params[n].var_p, 3);
  } else {
    result.x = 0.0;
    result.y = 0.0;
    result.z = 0.0;
    result.w = 0.0;
  }
  return result;
}

static void v_set_vec2(var_t *var, ImVec2 &vec2) {
  map_init(var);
  v_setint(map_add_var(var, "x", 0), vec2.x);
  v_setint(map_add_var(var, "y", 0), vec2.y);
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

static void close_window(GLFWwindow *window, int windowId) {
  if (window != nullptr) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    if (windowId != -1) {
      _windowMap.erase(windowId);
    }
  }
}

int cmd_acceptdragdroppayload(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    //auto type = get_param_int(argc, params, 0, 0);
    //auto flags = get_param_int(argc, params, 1, 0);
    //auto fnResult = AcceptDragDropPayload(type, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "AcceptDragDropPayload", 2);
  }
  return result;
}

static int cmd_arrowbutton(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto str_id = get_param_str(argc, params, 0, 0);
    auto dir = get_param_int(argc, params, 1, 0);
    auto fnResult = ArrowButton(str_id, dir);
    v_setint(retval, fnResult);
  } else {
    error(retval, "ArrowButton", 2);
  }
  return result;
}

static int cmd_begin(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc > 0 && argc < 4);
  if (result) {
    auto name = get_param_str(argc, params, 0, 0);
    auto p_open = get_param_int(argc, params, 1, 0) == 1;
    auto flags = get_param_int(argc, params, 2, 0);
    auto fnResult = Begin(name, &p_open, flags);
    v_setint(retval, fnResult);
  } else {
    error(retval, "Begin", 1, 3);
  }
  return result;
}

static int cmd_begin_fullscreen(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc > 0 && argc < 4);
  if (result) {
    const ImVec2 pos = ImVec2(0, 0);
    const ImVec2 size = ImVec2(_width, _height);
    SetNextWindowPos(pos);
    SetNextWindowSize(size);

    auto name = get_param_str(argc, params, 0, 0);
    auto p_open = get_param_int(argc, params, 1, 0) == 1;
    auto flags = get_param_int(argc, params, 2, 0) |
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;
    auto fnResult = Begin(name, &p_open, flags);
    v_setint(retval, fnResult);
  } else {
    error(retval, "BeginFullScreen", 1, 3);
  }
  return result;
}

static int cmd_beginchild(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    auto str_id = get_param_str(argc, params, 0, 0);
    auto size = get_param_vec2(argc, params, 1);
    auto border = get_param_int(argc, params, 2, 0);
    auto flags = get_param_int(argc, params, 3, 0);
    auto fnResult = BeginChild(str_id, size, border, flags);
    v_setint(retval, fnResult);
  } else {
    error(retval, "BeginChild", 4);
  }
  return result;
}

static int cmd_beginchildframe(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    auto id = get_param_int(argc, params, 0, 0);
    auto size = get_param_vec2(argc, params, 1);
    auto flags = get_param_int(argc, params, 2, 0);
    auto fnResult = BeginChildFrame(id, size, flags);
    v_setint(retval, fnResult);
  } else {
    error(retval, "BeginChildFrame", 3);
  }
  return result;
}

static int cmd_begincombo(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    auto label = get_param_str(argc, params, 0, 0);
    auto preview_value = get_param_str(argc, params, 1, NULL);
    auto flags = get_param_int(argc, params, 2, 0);
    auto fnResult = BeginCombo(label, preview_value, flags);
    v_setint(retval, fnResult);
  } else {
    error(retval, "BeginCombo", 3);
  }
  return result;
}

static int cmd_begindragdropsource(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto flags = get_param_int(argc, params, 0, 0);
    auto fnResult = BeginDragDropSource(flags);
    v_setint(retval, fnResult);
  } else {
    error(retval, "BeginDragDropSource", 1);
  }
  return result;
}

static int cmd_begindragdroptarget(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = BeginDragDropTarget();
    v_setint(retval, fnResult);
  } else {
    error(retval, "BeginDragDropTarget", 0);
  }
  return result;
}

static int cmd_beginmainmenubar(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = BeginMainMenuBar();
    v_setint(retval, fnResult);
  } else {
    error(retval, "BeginMainMenuBar", 0);
  }
  return result;
}

static int cmd_beginmenu(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto label = get_param_str(argc, params, 0, 0);
    auto enabled = get_param_int(argc, params, 1, 0);
    auto fnResult = BeginMenu(label, enabled);
    v_setint(retval, fnResult);
  } else {
    error(retval, "BeginMenu", 2);
  }
  return result;
}

static int cmd_beginmenubar(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = BeginMenuBar();
    v_setint(retval, fnResult);
  } else {
    error(retval, "BeginMenuBar", 0);
  }
  return result;
}

static int cmd_beginpopup(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto str_id = get_param_str(argc, params, 0, 0);
    auto flags = get_param_int(argc, params, 1, 0);
    auto fnResult = BeginPopup(str_id, flags);
    v_setint(retval, fnResult);
  } else {
    error(retval, "BeginPopup", 2);
  }
  return result;
}

static int cmd_beginpopupcontextitem(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto str_id = get_param_str(argc, params, 0, 0);
    auto popup_flags = get_param_int(argc, params, 1, 0);
    auto fnResult = BeginPopupContextItem(str_id, popup_flags);
    v_setint(retval, fnResult);
  } else {
    error(retval, "BeginPopupContextItem", 2);
  }
  return result;
}

static int cmd_beginpopupcontextvoid(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto str_id = get_param_str(argc, params, 0, 0);
    auto popup_flags = get_param_int(argc, params, 1, 0);
    auto fnResult = BeginPopupContextVoid(str_id, popup_flags);
    v_setint(retval, fnResult);
  } else {
    error(retval, "BeginPopupContextVoid", 2);
  }
  return result;
}

static int cmd_beginpopupcontextwindow(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto str_id = get_param_str(argc, params, 0, 0);
    auto popup_flags = get_param_int(argc, params, 1, 0);
    auto fnResult = BeginPopupContextWindow(str_id, popup_flags);
    v_setint(retval, fnResult);
  } else {
    error(retval, "BeginPopupContextWindow", 2);
  }
  return result;
}

static int cmd_beginpopupmodal(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    auto name = get_param_str(argc, params, 0, 0);
    bool p_open = (get_param_int(argc, params, 1, 0) == 1);
    auto flags = get_param_int(argc, params, 2, 0);
    auto fnResult = BeginPopupModal(name, &p_open, flags);
    v_setint(retval, fnResult);
  } else {
    error(retval, "BeginPopupModal", 3);
  }
  return result;
}

static int cmd_begintabbar(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto str_id = get_param_str(argc, params, 0, 0);
    auto flags = get_param_int(argc, params, 1, 0);
    auto fnResult = BeginTabBar(str_id, flags);
    v_setint(retval, fnResult);
  } else {
    error(retval, "BeginTabBar", 2);
  }
  return result;
}

static int cmd_begintabitem(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    auto label = get_param_str(argc, params, 0, 0);
    bool p_open = get_param_int(argc, params, 1, 0) == 1;
    auto flags = get_param_int(argc, params, 2, 0);
    auto fnResult = BeginTabItem(label, &p_open, flags);
    v_setint(retval, fnResult);
  } else {
    error(retval, "BeginTabItem", 3);
  }
  return result;
}

static int cmd_button(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1 || argc == 2);
  if (result) {
    auto label = get_param_str(argc, params, 0, 0);
    auto size = get_param_vec2(argc, params, 1);
    auto fnResult = Button(label, size);
    v_setint(retval, fnResult);
  } else {
    error(retval, "Button", 1, 2);
  }
  return result;
}

static int cmd_calcitemwidth(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = CalcItemWidth();
    v_setreal(retval, fnResult);
  } else {
    error(retval, "CalcItemWidth", 0);
  }
  return result;
}

static int cmd_calctextsize(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    auto text = get_param_str(argc, params, 0, 0);
    auto text_end = get_param_str(argc, params, 1, 0);
    bool hide_text_after_double_hash = get_param_int(argc, params, 2, 0) == 1;
    auto wrap_width = get_param_num(argc, params, 3, 0);
    auto fnResult = CalcTextSize(text, text_end, hide_text_after_double_hash, wrap_width);
    v_set_vec2(retval, fnResult);
  } else {
    error(retval, "CalcTextSize", 4);
  }
  return result;
}

static int cmd_checkbox(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2 && is_int_byref(argc, params, 1));
  if (result) {
    auto label = get_param_str(argc, params, 0, 0);
    bool value = get_param_int(argc, params, 1, 0) == 1;
    Checkbox(label, &value);
    v_setint(params[1].var_p, value);
  } else {
    error(retval, "Checkbox", 2);
  }
  return result;
}

int cmd_checkboxflags(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto flags = get_param_int(argc, params, 1, 0);
    //auto flags_value = get_param_int(argc, params, 2, 0);
    //auto fnResult = CheckboxFlags(label, flags, flags_value);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "CheckboxFlags", 3);
  }
  return result;
}

int cmd_collapsingheader(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto flags = get_param_int(argc, params, 1, 0);
    //auto fnResult = CollapsingHeader(label, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "CollapsingHeader", 2);
  }
  return result;
}

int cmd_colorbutton(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    //auto desc_id = get_param_int(argc, params, 0, 0);
    //auto col = get_param_int(argc, params, 1, 0);
    //auto flags = get_param_int(argc, params, 2, 0);
    //auto size = get_param_int(argc, params, 3, 0);
    //auto fnResult = ColorButton(desc_id, col, flags, size);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "ColorButton", 4);
  }
  return result;
}

int cmd_colorconvertfloat4tou32(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    //auto in = get_param_int(argc, params, 0, 0);
    //auto fnResult = ColorConvertFloat4ToU32(in);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "ColorConvertFloat4ToU32", 1);
  }
  return result;
}

int cmd_colorconvertu32tofloat4(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    //auto in = get_param_int(argc, params, 0, 0);
    //auto fnResult = ColorConvertU32ToFloat4(in);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "ColorConvertU32ToFloat4", 1);
  }
  return result;
}

static int cmd_coloredit3(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2 || argc == 3);
  if (result && is_param_array(argc, params, 1)) {
    auto label = get_param_str(argc, params, 0, 0);
    float col[3] = {
      get_array_elem_num(params[1].var_p, 0),
      get_array_elem_num(params[1].var_p, 1),
      get_array_elem_num(params[1].var_p, 2),
    };
    auto flags = get_param_int(argc, params, 2, 0);
    auto fnResult = ColorEdit3(label, col, flags);
    v_setint(retval, fnResult);
  } else {
    error(retval, "ColorEdit3", 2, 3);
  }
  return result;
}

int cmd_coloredit4(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2 || argc == 3);
  if (result && is_param_array(argc, params, 1)) {
    auto label = get_param_str(argc, params, 0, 0);
    float col[4] = {
      get_array_elem_num(params[1].var_p, 0),
      get_array_elem_num(params[1].var_p, 1),
      get_array_elem_num(params[1].var_p, 2),
      get_array_elem_num(params[1].var_p, 3),
    };
    auto flags = get_param_int(argc, params, 2, 0);
    auto fnResult = ColorEdit3(label, col, flags);
    v_setint(retval, fnResult);
  } else {
    error(retval, "ColorEdit4", 2, 3);
  }
  return result;
}

int cmd_colorpicker3(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto col = get_param_int(argc, params, 1, 0);
    //auto flags = get_param_int(argc, params, 2, 0);
    //auto fnResult = ColorPicker3(label, col, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "ColorPicker3", 3);
  }
  return result;
}

int cmd_colorpicker4(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto col = get_param_int(argc, params, 1, 0);
    //auto flags = get_param_int(argc, params, 2, 0);
    //auto ref_col = get_param_int(argc, params, 3, 0);
    //auto fnResult = ColorPicker4(label, col, flags, ref_col);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "ColorPicker4", 4);
  }
  return result;
}

int cmd_combo(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto current_item = get_param_int(argc, params, 1, 0);
    //auto items = get_param_int(argc, params, 2, 0);
    //auto items_count = get_param_int(argc, params, 3, 0);
    //auto popup_max_height_in_items = get_param_int(argc, params, 4, 0);
    //auto fnResult = Combo(label, current_item, items, items_count, popup_max_height_in_items);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "Combo", 5);
  }
  return result;
}

static int cmd_createcontext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    SetCurrentContext(CreateContext());
    v_setint(retval, 1);
  } else {
    error(retval, "CreateContext", 0);
  }
  return result;
}

int cmd_debugcheckversionanddatalayout(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 13);
  if (result) {
    //auto version_str = get_param_int(argc, params, 0, 0);
    //auto size_t = get_param_int(argc, params, 1, 0);
    //auto sz_io = get_param_int(argc, params, 2, 0);
    //auto size_t = get_param_int(argc, params, 3, 0);
    //auto sz_style = get_param_int(argc, params, 4, 0);
    //auto size_t = get_param_int(argc, params, 5, 0);
    //auto sz_vec2 = get_param_int(argc, params, 6, 0);
    //auto size_t = get_param_int(argc, params, 7, 0);
    //auto sz_vec4 = get_param_int(argc, params, 8, 0);
    //auto size_t = get_param_int(argc, params, 9, 0);
    //auto sz_drawvert = get_param_int(argc, params, 10, 0);
    //auto size_t = get_param_int(argc, params, 11, 0);
    //auto sz_drawidx = get_param_int(argc, params, 12, 0);
    //auto fnResult = DebugCheckVersionAndDataLayout(version_str, size_t, sz_io, size_t, sz_style, size_t, sz_vec2, size_t, sz_vec4, size_t, sz_drawvert, size_t, sz_drawidx);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "DebugCheckVersionAndDataLayout", 13);
  }
  return result;
}

int cmd_dragfloat(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto v_speed = get_param_int(argc, params, 2, 0);
    //auto v_min = get_param_int(argc, params, 3, 0);
    //auto v_max = get_param_int(argc, params, 4, 0);
    //auto format = get_param_int(argc, params, 5, 0);
    //auto flags = get_param_int(argc, params, 6, 0);
    //auto fnResult = DragFloat(label, v, v_speed, v_min, v_max, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "DragFloat", 7);
  }
  return result;
}

int cmd_dragfloat2(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto v_speed = get_param_int(argc, params, 2, 0);
    //auto v_min = get_param_int(argc, params, 3, 0);
    //auto v_max = get_param_int(argc, params, 4, 0);
    //auto format = get_param_int(argc, params, 5, 0);
    //auto flags = get_param_int(argc, params, 6, 0);
    //auto fnResult = DragFloat2(label, v, v_speed, v_min, v_max, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "DragFloat2", 7);
  }
  return result;
}

int cmd_dragfloat3(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto v_speed = get_param_int(argc, params, 2, 0);
    //auto v_min = get_param_int(argc, params, 3, 0);
    //auto v_max = get_param_int(argc, params, 4, 0);
    //auto format = get_param_int(argc, params, 5, 0);
    //auto flags = get_param_int(argc, params, 6, 0);
    //auto fnResult = DragFloat3(label, v, v_speed, v_min, v_max, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "DragFloat3", 7);
  }
  return result;
}

int cmd_dragfloat4(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto v_speed = get_param_int(argc, params, 2, 0);
    //auto v_min = get_param_int(argc, params, 3, 0);
    //auto v_max = get_param_int(argc, params, 4, 0);
    //auto format = get_param_int(argc, params, 5, 0);
    //auto flags = get_param_int(argc, params, 6, 0);
    //auto fnResult = DragFloat4(label, v, v_speed, v_min, v_max, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "DragFloat4", 7);
  }
  return result;
}

int cmd_dragfloatrange2(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 9);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v_current_min = get_param_int(argc, params, 1, 0);
    //auto v_current_max = get_param_int(argc, params, 2, 0);
    //auto v_speed = get_param_int(argc, params, 3, 0);
    //auto v_min = get_param_int(argc, params, 4, 0);
    //auto v_max = get_param_int(argc, params, 5, 0);
    //auto format = get_param_int(argc, params, 6, 0);
    //auto format_max = get_param_int(argc, params, 7, 0);
    //auto flags = get_param_int(argc, params, 8, 0);
    //auto fnResult = DragFloatRange2(label, v_current_min, v_current_max, v_speed, v_min, v_max, format, format_max, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "DragFloatRange2", 9);
  }
  return result;
}

int cmd_dragint(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto v_speed = get_param_int(argc, params, 2, 0);
    //auto v_min = get_param_int(argc, params, 3, 0);
    //auto v_max = get_param_int(argc, params, 4, 0);
    //auto format = get_param_int(argc, params, 5, 0);
    //auto flags = get_param_int(argc, params, 6, 0);
    //auto fnResult = DragInt(label, v, v_speed, v_min, v_max, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "DragInt", 7);
  }
  return result;
}

int cmd_dragint2(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto v_speed = get_param_int(argc, params, 2, 0);
    //auto v_min = get_param_int(argc, params, 3, 0);
    //auto v_max = get_param_int(argc, params, 4, 0);
    //auto format = get_param_int(argc, params, 5, 0);
    //auto flags = get_param_int(argc, params, 6, 0);
    //auto fnResult = DragInt2(label, v, v_speed, v_min, v_max, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "DragInt2", 7);
  }
  return result;
}

int cmd_dragint3(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto v_speed = get_param_int(argc, params, 2, 0);
    //auto v_min = get_param_int(argc, params, 3, 0);
    //auto v_max = get_param_int(argc, params, 4, 0);
    //auto format = get_param_int(argc, params, 5, 0);
    //auto flags = get_param_int(argc, params, 6, 0);
    //auto fnResult = DragInt3(label, v, v_speed, v_min, v_max, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "DragInt3", 7);
  }
  return result;
}

int cmd_dragint4(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto v_speed = get_param_int(argc, params, 2, 0);
    //auto v_min = get_param_int(argc, params, 3, 0);
    //auto v_max = get_param_int(argc, params, 4, 0);
    //auto format = get_param_int(argc, params, 5, 0);
    //auto flags = get_param_int(argc, params, 6, 0);
    //auto fnResult = DragInt4(label, v, v_speed, v_min, v_max, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "DragInt4", 7);
  }
  return result;
}

int cmd_dragintrange2(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 9);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v_current_min = get_param_int(argc, params, 1, 0);
    //auto v_current_max = get_param_int(argc, params, 2, 0);
    //auto v_speed = get_param_int(argc, params, 3, 0);
    //auto v_min = get_param_int(argc, params, 4, 0);
    //auto v_max = get_param_int(argc, params, 5, 0);
    //auto format = get_param_int(argc, params, 6, 0);
    //auto format_max = get_param_int(argc, params, 7, 0);
    //auto flags = get_param_int(argc, params, 8, 0);
    //auto fnResult = DragIntRange2(label, v_current_min, v_current_max, v_speed, v_min, v_max, format, format_max, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "DragIntRange2", 9);
  }
  return result;
}

int cmd_dragscalar(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 8);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto data_type = get_param_int(argc, params, 1, 0);
    //auto p_data = get_param_int(argc, params, 2, 0);
    //auto v_speed = get_param_int(argc, params, 3, 0);
    //auto p_min = get_param_int(argc, params, 4, 0);
    //auto p_max = get_param_int(argc, params, 5, 0);
    //auto format = get_param_int(argc, params, 6, 0);
    //auto flags = get_param_int(argc, params, 7, 0);
    //auto fnResult = DragScalar(label, data_type, p_data, v_speed, p_min, p_max, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "DragScalar", 8);
  }
  return result;
}

int cmd_dragscalarn(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 9);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto data_type = get_param_int(argc, params, 1, 0);
    //auto p_data = get_param_int(argc, params, 2, 0);
    //auto components = get_param_int(argc, params, 3, 0);
    //auto v_speed = get_param_int(argc, params, 4, 0);
    //auto p_min = get_param_int(argc, params, 5, 0);
    //auto p_max = get_param_int(argc, params, 6, 0);
    //auto format = get_param_int(argc, params, 7, 0);
    //auto flags = get_param_int(argc, params, 8, 0);
    //auto fnResult = DragScalarN(label, data_type, p_data, components, v_speed, p_min, p_max, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "DragScalarN", 9);
  }
  return result;
}

int cmd_getbackgrounddrawlist(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    //auto fnResult = GetBackgroundDrawList();
    //v_setint(retval, fnResult);
  } else {
    error(retval, "GetBackgroundDrawList", 0);
  }
  return result;
}

static int cmd_getclipboardtext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetClipboardText();
    v_setstr(retval, fnResult);
  } else {
    error(retval, "GetClipboardText", 0);
  }
  return result;
}

int cmd_getcoloru32(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    //auto idx = get_param_int(argc, params, 0, 0);
    //auto alpha_mul = get_param_int(argc, params, 1, 0);
    //auto fnResult = GetColorU32(idx, alpha_mul);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "GetColorU32", 2);
  }
  return result;
}

static int cmd_getcolumnindex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetColumnIndex();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetColumnIndex", 0);
  }
  return result;
}

static int cmd_getcolumnoffset(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto column_index = get_param_int(argc, params, 0, 0);
    auto fnResult = GetColumnOffset(column_index);
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetColumnOffset", 1);
  }
  return result;
}

static int cmd_getcolumnscount(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetColumnsCount();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetColumnsCount", 0);
  }
  return result;
}

static int cmd_getcolumnwidth(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto column_index = get_param_int(argc, params, 0, 0);
    auto fnResult = GetColumnWidth(column_index);
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetColumnWidth", 1);
  }
  return result;
}

static int cmd_getcontentregionavail(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetContentRegionAvail();
    v_set_vec2(retval, fnResult);
  } else {
    error(retval, "GetContentRegionAvail", 0);
  }
  return result;
}

static int cmd_getcontentregionmax(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetContentRegionMax();
    v_set_vec2(retval, fnResult);
  } else {
    error(retval, "GetContentRegionMax", 0);
  }
  return result;
}

int cmd_getcurrentcontext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    //auto fnResult = GetCurrentContext();
    //v_setint(retval, fnResult);
  } else {
    error(retval, "GetCurrentContext", 0);
  }
  return result;
}

static int cmd_getcursorpos(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetCursorPos();
    v_set_vec2(retval, fnResult);
  } else {
    error(retval, "GetCursorPos", 0);
  }
  return result;
}

static int cmd_getcursorposx(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetCursorPosX();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetCursorPosX", 0);
  }
  return result;
}

static int cmd_getcursorposy(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetCursorPosY();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetCursorPosY", 0);
  }
  return result;
}

static int cmd_getcursorscreenpos(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetCursorScreenPos();
    v_set_vec2(retval, fnResult);
  } else {
    error(retval, "GetCursorScreenPos", 0);
  }
  return result;
}

static int cmd_getcursorstartpos(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetCursorStartPos();
    v_set_vec2(retval, fnResult);
  } else {
    error(retval, "GetCursorStartPos", 0);
  }
  return result;
}

int cmd_getdragdroppayload(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    //auto fnResult = GetDragDropPayload();
    //v_setint(retval, fnResult);
  } else {
    error(retval, "GetDragDropPayload", 0);
  }
  return result;
}

int cmd_getdrawdata(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    //auto fnResult = GetDrawData();
    //v_setint(retval, fnResult);
  } else {
    error(retval, "GetDrawData", 0);
  }
  return result;
}

int cmd_getdrawlistshareddata(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    //auto fnResult = GetDrawListSharedData();
    //v_setint(retval, fnResult);
  } else {
    error(retval, "GetDrawListSharedData", 0);
  }
  return result;
}

int cmd_getfont(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    //auto fnResult = GetFont();
    //v_setint(retval, fnResult);
  } else {
    error(retval, "GetFont", 0);
  }
  return result;
}

static int cmd_getfontsize(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetFontSize();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetFontSize", 0);
  }
  return result;
}

static int cmd_getfonttexuvwhitepixel(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetFontTexUvWhitePixel();
    v_set_vec2(retval, fnResult);
  } else {
    error(retval, "GetFontTexUvWhitePixel", 0);
  }
  return result;
}

int cmd_getforegrounddrawlist(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    //auto fnResult = GetForegroundDrawList();
    //v_setint(retval, fnResult);
  } else {
    error(retval, "GetForegroundDrawList", 0);
  }
  return result;
}

static int cmd_getframecount(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetFrameCount();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetFrameCount", 0);
  }
  return result;
}

static int cmd_getframeheight(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetFrameHeight();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetFrameHeight", 0);
  }
  return result;
}

static int cmd_getframeheightwithspacing(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetFrameHeightWithSpacing();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetFrameHeightWithSpacing", 0);
  }
  return result;
}

static int cmd_getid(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto str_id = get_param_str(argc, params, 0, 0);
    auto fnResult = GetID(str_id);
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetID", 1);
  }
  return result;
}

static int cmd_getitemrectmax(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetItemRectMax();
    v_set_vec2(retval, fnResult);
  } else {
    error(retval, "GetItemRectMax", 0);
  }
  return result;
}

static int cmd_getitemrectmin(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetItemRectMin();
    v_set_vec2(retval, fnResult);
  } else {
    error(retval, "GetItemRectMin", 0);
  }
  return result;
}

static int cmd_getitemrectsize(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetItemRectSize();
    v_set_vec2(retval, fnResult);
  } else {
    error(retval, "GetItemRectSize", 0);
  }
  return result;
}

static int cmd_getkeyindex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto imgui_key = get_param_int(argc, params, 0, 0);
    auto fnResult = GetKeyIndex(imgui_key);
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetKeyIndex", 1);
  }
  return result;
}

int cmd_getkeypressedamount(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    //auto key_index = get_param_int(argc, params, 0, 0);
    //auto repeat_delay = get_param_int(argc, params, 1, 0);
    //auto rate = get_param_int(argc, params, 2, 0);
    //auto fnResult = GetKeyPressedAmount(key_index, repeat_delay, rate);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "GetKeyPressedAmount", 3);
  }
  return result;
}

static int cmd_getmousecursor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetMouseCursor();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetMouseCursor", 0);
  }
  return result;
}

int cmd_getmousedragdelta(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    //auto button = get_param_int(argc, params, 0, 0);
    //auto lock_threshold = get_param_int(argc, params, 1, 0);
    //auto fnResult = GetMouseDragDelta(button, lock_threshold);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "GetMouseDragDelta", 2);
  }
  return result;
}

static int cmd_getmousepos(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetMousePos();
    v_set_vec2(retval, fnResult);
  } else {
    error(retval, "GetMousePos", 0);
  }
  return result;
}

static int cmd_getmouseposonopeningcurrentpopup(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetMousePosOnOpeningCurrentPopup();
    v_set_vec2(retval, fnResult);
  } else {
    error(retval, "GetMousePosOnOpeningCurrentPopup", 0);
  }
  return result;
}

static int cmd_getscrollmaxx(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetScrollMaxX();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetScrollMaxX", 0);
  }
  return result;
}

static int cmd_getscrollmaxy(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetScrollMaxY();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetScrollMaxY", 0);
  }
  return result;
}

static int cmd_getscrollx(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetScrollX();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetScrollX", 0);
  }
  return result;
}

static int cmd_getscrolly(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetScrollY();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetScrollY", 0);
  }
  return result;
}

int cmd_getstatestorage(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    //auto fnResult = GetStateStorage();
    //v_setint(retval, fnResult);
  } else {
    error(retval, "GetStateStorage", 0);
  }
  return result;
}

int cmd_getstyle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    //auto fnResult = GetStyle();
    //v_setint(retval, fnResult);
  } else {
    error(retval, "GetStyle", 0);
  }
  return result;
}

static int cmd_getstylecolorname(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto idx = get_param_int(argc, params, 0, 0);
    auto fnResult = GetStyleColorName(idx);
    v_setstr(retval, fnResult);
  } else {
    error(retval, "GetStyleColorName", 1);
  }
  return result;
}

int cmd_getstylecolorvec4(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    //auto idx = get_param_int(argc, params, 0, 0);
    //auto fnResult = GetStyleColorVec4(idx);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "GetStyleColorVec4", 1);
  }
  return result;
}

static int cmd_gettextlineheight(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetTextLineHeight();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetTextLineHeight", 0);
  }
  return result;
}

static int cmd_gettextlineheightwithspacing(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetTextLineHeightWithSpacing();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetTextLineHeightWithSpacing", 0);
  }
  return result;
}

static int cmd_gettime(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetTime();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetTime", 0);
  }
  return result;
}

static int cmd_gettreenodetolabelspacing(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetTreeNodeToLabelSpacing();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetTreeNodeToLabelSpacing", 0);
  }
  return result;
}

static int cmd_getversion(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetVersion();
    v_setstr(retval, fnResult);
  } else {
    error(retval, "GetVersion", 0);
  }
  return result;
}

static int cmd_getwindowcontentregionmax(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetWindowContentRegionMax();
    v_set_vec2(retval, fnResult);
  } else {
    error(retval, "GetWindowContentRegionMax", 0);
  }
  return result;
}

static int cmd_getwindowcontentregionmin(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetWindowContentRegionMin();
    v_set_vec2(retval, fnResult);
  } else {
    error(retval, "GetWindowContentRegionMin", 0);
  }
  return result;
}

static int cmd_getwindowcontentregionwidth(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetWindowContentRegionWidth();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetWindowContentRegionWidth", 0);
  }
  return result;
}

int cmd_getwindowdrawlist(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    //auto fnResult = GetWindowDrawList();
    //v_set_vec2(retval, fnResult);
  } else {
    error(retval, "GetWindowDrawList", 0);
  }
  return result;
}

static int cmd_getwindowheight(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    v_setreal(retval, GetWindowHeight());
  } else {
    error(retval, "GetWindowHeight", 0);
  }
  return result;
}

static int cmd_getwindowpos(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetWindowPos();
    v_set_vec2(retval, fnResult);
  } else {
    error(retval, "GetWindowPos", 0);
  }
  return result;
}

static int cmd_getwindowsize(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetWindowSize();
    v_set_vec2(retval, fnResult);
  } else {
    error(retval, "GetWindowSize", 0);
  }
  return result;
}

static int cmd_getwindowwidth(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    v_setreal(retval, GetWindowWidth());
  } else {
    error(retval, "GetWindowWidth", 0);
  }
  return result;
}

int cmd_imagebutton(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    //auto user_texture_id = get_param_int(argc, params, 0, 0);
    //auto size = get_param_vec2(argc, params, 1);
    //auto uv0 = get_param_int(argc, params, 2, 0);
    //auto uv1 = get_param_int(argc, params, 3, 0);
    //auto frame_padding = get_param_int(argc, params, 4, 0);
    //auto bg_col = get_param_int(argc, params, 5, 0);
    //auto tint_col = get_param_int(argc, params, 6, 0);
    //auto fnResult = ImageButton(user_texture_id, size, uv0, uv1, frame_padding, bg_col, tint_col);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "ImageButton", 7);
  }
  return result;
}

int cmd_inputdouble(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto step = get_param_int(argc, params, 2, 0);
    //auto step_fast = get_param_int(argc, params, 3, 0);
    //auto format = get_param_int(argc, params, 4, 0);
    //auto flags = get_param_int(argc, params, 5, 0);
    //auto fnResult = InputDouble(label, v, step, step_fast, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "InputDouble", 6);
  }
  return result;
}

int cmd_inputfloat(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto step = get_param_int(argc, params, 2, 0);
    //auto step_fast = get_param_int(argc, params, 3, 0);
    //auto format = get_param_int(argc, params, 4, 0);
    //auto flags = get_param_int(argc, params, 5, 0);
    //auto fnResult = InputFloat(label, v, step, step_fast, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "InputFloat", 6);
  }
  return result;
}

int cmd_inputfloat2(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto format = get_param_int(argc, params, 2, 0);
    //auto flags = get_param_int(argc, params, 3, 0);
    //auto fnResult = InputFloat2(label, v, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "InputFloat2", 4);
  }
  return result;
}

int cmd_inputfloat3(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto format = get_param_int(argc, params, 2, 0);
    //auto flags = get_param_int(argc, params, 3, 0);
    //auto fnResult = InputFloat3(label, v, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "InputFloat3", 4);
  }
  return result;
}

int cmd_inputfloat4(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto format = get_param_int(argc, params, 2, 0);
    //auto flags = get_param_int(argc, params, 3, 0);
    //auto fnResult = InputFloat4(label, v, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "InputFloat4", 4);
  }
  return result;
}

int cmd_inputint(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto step = get_param_int(argc, params, 2, 0);
    //auto step_fast = get_param_int(argc, params, 3, 0);
    //auto flags = get_param_int(argc, params, 4, 0);
    //auto fnResult = InputInt(label, v, step, step_fast, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "InputInt", 5);
  }
  return result;
}

int cmd_inputint2(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto flags = get_param_int(argc, params, 2, 0);
    //auto fnResult = InputInt2(label, v, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "InputInt2", 3);
  }
  return result;
}

int cmd_inputint3(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto flags = get_param_int(argc, params, 2, 0);
    //auto fnResult = InputInt3(label, v, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "InputInt3", 3);
  }
  return result;
}

int cmd_inputint4(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto flags = get_param_int(argc, params, 2, 0);
    //auto fnResult = InputInt4(label, v, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "InputInt4", 3);
  }
  return result;
}

int cmd_inputscalar(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto data_type = get_param_int(argc, params, 1, 0);
    //auto p_data = get_param_int(argc, params, 2, 0);
    //auto p_step = get_param_int(argc, params, 3, 0);
    //auto p_step_fast = get_param_int(argc, params, 4, 0);
    //auto format = get_param_int(argc, params, 5, 0);
    //auto flags = get_param_int(argc, params, 6, 0);
    //auto fnResult = InputScalar(label, data_type, p_data, p_step, p_step_fast, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "InputScalar", 7);
  }
  return result;
}

int cmd_inputscalarn(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 8);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto data_type = get_param_int(argc, params, 1, 0);
    //auto p_data = get_param_int(argc, params, 2, 0);
    //auto components = get_param_int(argc, params, 3, 0);
    //auto p_step = get_param_int(argc, params, 4, 0);
    //auto p_step_fast = get_param_int(argc, params, 5, 0);
    //auto format = get_param_int(argc, params, 6, 0);
    //auto flags = get_param_int(argc, params, 7, 0);
    //auto fnResult = InputScalarN(label, data_type, p_data, components, p_step, p_step_fast, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "InputScalarN", 8);
  }
  return result;
}

int cmd_inputtext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto buf = get_param_int(argc, params, 1, 0);
    //auto size_t = get_param_int(argc, params, 2, 0);
    //auto buf_size = get_param_int(argc, params, 3, 0);
    //auto flags = get_param_int(argc, params, 4, 0);
    //auto callback = get_param_int(argc, params, 5, 0);
    //auto user_data = get_param_int(argc, params, 6, 0);
    //auto fnResult = InputText(label, buf, size_t, buf_size, flags, callback, user_data);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "InputText", 7);
  }
  return result;
}

int cmd_inputtextmultiline(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 8);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto buf = get_param_int(argc, params, 1, 0);
    //auto size_t = get_param_int(argc, params, 2, 0);
    //auto buf_size = get_param_int(argc, params, 3, 0);
    //auto size = get_param_int(argc, params, 4, 0);
    //auto flags = get_param_int(argc, params, 5, 0);
    //auto callback = get_param_int(argc, params, 6, 0);
    //auto user_data = get_param_int(argc, params, 7, 0);
    //auto fnResult = InputTextMultiline(label, buf, size_t, buf_size, size, flags, callback, user_data);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "InputTextMultiline", 8);
  }
  return result;
}

int cmd_inputtextwithhint(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 8);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto hint = get_param_int(argc, params, 1, 0);
    //auto buf = get_param_int(argc, params, 2, 0);
    //auto size_t = get_param_int(argc, params, 3, 0);
    //auto buf_size = get_param_int(argc, params, 4, 0);
    //auto flags = get_param_int(argc, params, 5, 0);
    //auto callback = get_param_int(argc, params, 6, 0);
    //auto user_data = get_param_int(argc, params, 7, 0);
    //auto fnResult = InputTextWithHint(label, hint, buf, size_t, buf_size, flags, callback, user_data);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "InputTextWithHint", 8);
  }
  return result;
}

int cmd_invisiblebutton(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    //auto str_id = get_param_str(argc, params, 0, 0);
    //auto size = get_param_vec2(argc, params, 1);
    //auto flags = get_param_int(argc, params, 2, 0);
    //auto fnResult = InvisibleButton(str_id, size, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "InvisibleButton", 3);
  }
  return result;
}

static int cmd_isanyitemactive(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsAnyItemActive();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsAnyItemActive", 0);
  }
  return result;
}

static int cmd_isanyitemfocused(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsAnyItemFocused();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsAnyItemFocused", 0);
  }
  return result;
}

static int cmd_isanyitemhovered(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsAnyItemHovered();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsAnyItemHovered", 0);
  }
  return result;
}

static int cmd_isanymousedown(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsAnyMouseDown();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsAnyMouseDown", 0);
  }
  return result;
}

static int cmd_isitemactivated(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsItemActivated();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsItemActivated", 0);
  }
  return result;
}

static int cmd_isitemactive(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsItemActive();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsItemActive", 0);
  }
  return result;
}

static int cmd_isitemclicked(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto mouse_button = get_param_int(argc, params, 0, 0);
    auto fnResult = IsItemClicked(mouse_button);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsItemClicked", 1);
  }
  return result;
}

static int cmd_isitemdeactivated(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsItemDeactivated();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsItemDeactivated", 0);
  }
  return result;
}

static int cmd_isitemdeactivatedafteredit(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsItemDeactivatedAfterEdit();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsItemDeactivatedAfterEdit", 0);
  }
  return result;
}

static int cmd_isitemedited(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsItemEdited();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsItemEdited", 0);
  }
  return result;
}

static int cmd_isitemfocused(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsItemFocused();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsItemFocused", 0);
  }
  return result;
}

static int cmd_isitemhovered(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto flags = get_param_int(argc, params, 0, 0);
    auto fnResult = IsItemHovered(flags);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsItemHovered", 1);
  }
  return result;
}

static int cmd_isitemtoggledopen(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsItemToggledOpen();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsItemToggledOpen", 0);
  }
  return result;
}

static int cmd_isitemvisible(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsItemVisible();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsItemVisible", 0);
  }
  return result;
}

static int cmd_iskeydown(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto user_key_index = get_param_int(argc, params, 0, 0);
    auto fnResult = IsKeyDown(user_key_index);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsKeyDown", 1);
  }
  return result;
}

static int cmd_iskeypressed(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto user_key_index = get_param_int(argc, params, 0, 0);
    auto repeat = get_param_int(argc, params, 1, 0);
    auto fnResult = IsKeyPressed(user_key_index, repeat);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsKeyPressed", 2);
  }
  return result;
}

static int cmd_iskeyreleased(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto user_key_index = get_param_int(argc, params, 0, 0);
    auto fnResult = IsKeyReleased(user_key_index);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsKeyReleased", 1);
  }
  return result;
}

static int cmd_ismouseclicked(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto button = get_param_int(argc, params, 0, 0);
    auto repeat = get_param_int(argc, params, 1, 0);
    auto fnResult = IsMouseClicked(button, repeat);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsMouseClicked", 2);
  }
  return result;
}

static int cmd_ismousedoubleclicked(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto button = get_param_int(argc, params, 0, 0);
    auto fnResult = IsMouseDoubleClicked(button);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsMouseDoubleClicked", 1);
  }
  return result;
}

static int cmd_ismousedown(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto button = get_param_int(argc, params, 0, 0);
    auto fnResult = IsMouseDown(button);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsMouseDown", 1);
  }
  return result;
}

static int cmd_ismousedragging(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto button = get_param_int(argc, params, 0, 0);
    auto lock_threshold = get_param_int(argc, params, 1, 0);
    auto fnResult = IsMouseDragging(button, lock_threshold);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsMouseDragging", 2);
  }
  return result;
}

static int cmd_ismousehoveringrect(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    auto r_min = get_param_vec2(argc, params, 0);
    auto r_max = get_param_vec2(argc, params, 1);
    auto clip = get_param_int(argc, params, 2, 0);
    auto fnResult = IsMouseHoveringRect(r_min, r_max, clip);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsMouseHoveringRect", 3);
  }
  return result;
}

static int cmd_ismouseposvalid(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto mouse_pos = get_param_vec2(argc, params, 0);
    auto fnResult = IsMousePosValid(&mouse_pos);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsMousePosValid", 1);
  }
  return result;
}

static int cmd_ismousereleased(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto button = get_param_int(argc, params, 0, 0);
    auto fnResult = IsMouseReleased(button);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsMouseReleased", 1);
  }
  return result;
}

static int cmd_ispopupopen(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto str_id = get_param_str(argc, params, 0, 0);
    auto flags = get_param_int(argc, params, 1, 0);
    auto fnResult = IsPopupOpen(str_id, flags);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsPopupOpen", 2);
  }
  return result;
}

static int cmd_isrectvisible(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto size = get_param_vec2(argc, params, 0);
    auto fnResult = IsRectVisible(size);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsRectVisible", 1);
  }
  return result;
}

static int cmd_iswindowappearing(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsWindowAppearing();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsWindowAppearing", 0);
  }
  return result;
}

static int cmd_iswindowcollapsed(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsWindowCollapsed();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsWindowCollapsed", 0);
  }
  return result;
}

static int cmd_iswindowfocused(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto flags = get_param_int(argc, params, 0, 0);
    auto fnResult = IsWindowFocused(flags);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsWindowFocused", 1);
  }
  return result;
}

static int cmd_iswindowhovered(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto flags = get_param_int(argc, params, 0, 0);
    auto fnResult = IsWindowHovered(flags);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsWindowHovered", 1);
  }
  return result;
}

int cmd_listbox(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto current_item = get_param_int(argc, params, 1, 0);
    //auto items = get_param_int(argc, params, 2, 0);
    //auto items_count = get_param_int(argc, params, 3, 0);
    //auto height_in_items = get_param_int(argc, params, 4, 0);
    //auto fnResult = ListBox(label, current_item, items, items_count, height_in_items);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "ListBox", 5);
  }
  return result;
}

int cmd_listboxheader(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto size = get_param_vec2(argc, params, 1);
    //auto fnResult = ListBoxHeader(label, size);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "ListBoxHeader", 2);
  }
  return result;
}

int cmd_menuitem(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto shortcut = get_param_int(argc, params, 1, 0);
    //auto selected = get_param_int(argc, params, 2, 0);
    //auto enabled = get_param_int(argc, params, 3, 0);
    //auto fnResult = MenuItem(label, shortcut, selected, enabled);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "MenuItem", 4);
  }
  return result;
}

int cmd_radiobutton(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto active = get_param_int(argc, params, 1, 0);
    //auto fnResult = RadioButton(label, active);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "RadioButton", 2);
  }
  return result;
}

int cmd_saveinisettingstomemory(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    //auto size_t = get_param_int(argc, params, 0, 0);
    //auto out_ini_size = get_param_int(argc, params, 1, 0);
    //auto fnResult = SaveIniSettingsToMemory(size_t, out_ini_size);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "SaveIniSettingsToMemory", 2);
  }
  return result;
}

int cmd_selectable(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto selected = get_param_int(argc, params, 1, 0);
    //auto flags = get_param_int(argc, params, 2, 0);
    //auto size = get_param_int(argc, params, 3, 0);
    //auto fnResult = Selectable(label, selected, flags, size);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "Selectable", 4);
  }
  return result;
}

int cmd_setdragdroppayload(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    //auto type = get_param_int(argc, params, 0, 0);
    //auto data = get_param_int(argc, params, 1, 0);
    //auto size_t = get_param_int(argc, params, 2, 0);
    //auto sz = get_param_int(argc, params, 3, 0);
    //auto cond = get_param_int(argc, params, 4, 0);
    //auto fnResult = SetDragDropPayload(type, data, size_t, sz, cond);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "SetDragDropPayload", 5);
  }
  return result;
}

int cmd_showstyleselector(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto fnResult = ShowStyleSelector(label);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "ShowStyleSelector", 1);
  }
  return result;
}

int cmd_sliderangle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v_rad = get_param_int(argc, params, 1, 0);
    //auto v_degrees_min = get_param_int(argc, params, 2, 0);
    //auto v_degrees_max = get_param_int(argc, params, 3, 0);
    //auto format = get_param_int(argc, params, 4, 0);
    //auto flags = get_param_int(argc, params, 5, 0);
    //auto fnResult = SliderAngle(label, v_rad, v_degrees_min, v_degrees_max, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "SliderAngle", 6);
  }
  return result;
}

static int cmd_sliderfloat(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc >= 4 && argc <= 6 && is_int_byref(argc, params, 1));
  if (result) {
    auto label = get_param_str(argc, params, 0, 0);
    float value = get_param_num(argc, params, 1, 0);
    float v_min = get_param_num(argc, params, 2, 0);
    float v_max = get_param_num(argc, params, 3, 0);
    auto format = get_param_str(argc, params, 4, "%.3f");
    auto flags = get_param_int(argc, params, 5, 0);
    SliderFloat(label, &value, v_min, v_max, format, flags);
    v_setreal(params[1].var_p, value);
  } else {
    error(retval, "SliderFloat", 4, 5);
  }
  return result;
}

int cmd_sliderfloat2(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto v_min = get_param_int(argc, params, 2, 0);
    //auto v_max = get_param_int(argc, params, 3, 0);
    //auto format = get_param_int(argc, params, 4, 0);
    //auto flags = get_param_int(argc, params, 5, 0);
    //auto fnResult = SliderFloat2(label, v, v_min, v_max, format, flags);
  } else {
    error(retval, "SliderFloat2", 6);
  }
  return result;
}

int cmd_sliderfloat3(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto v_min = get_param_int(argc, params, 2, 0);
    //auto v_max = get_param_int(argc, params, 3, 0);
    //auto format = get_param_int(argc, params, 4, 0);
    //auto flags = get_param_int(argc, params, 5, 0);
    //auto fnResult = SliderFloat3(label, v, v_min, v_max, format, flags);
  } else {
    error(retval, "SliderFloat3", 6);
  }
  return result;
}

int cmd_sliderfloat4(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto v_min = get_param_int(argc, params, 2, 0);
    //auto v_max = get_param_int(argc, params, 3, 0);
    //auto format = get_param_int(argc, params, 4, 0);
    //auto flags = get_param_int(argc, params, 5, 0);
    //auto fnResult = SliderFloat4(label, v, v_min, v_max, format, flags);
  } else {
    error(retval, "SliderFloat4", 6);
  }
  return result;
}

int cmd_sliderint(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto v_min = get_param_int(argc, params, 2, 0);
    //auto v_max = get_param_int(argc, params, 3, 0);
    //auto format = get_param_int(argc, params, 4, 0);
    //auto flags = get_param_int(argc, params, 5, 0);
    //auto fnResult = SliderInt(label, v, v_min, v_max, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "SliderInt", 6);
  }
  return result;
}

int cmd_sliderint2(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto v_min = get_param_int(argc, params, 2, 0);
    //auto v_max = get_param_int(argc, params, 3, 0);
    //auto format = get_param_int(argc, params, 4, 0);
    //auto flags = get_param_int(argc, params, 5, 0);
    //auto fnResult = SliderInt2(label, v, v_min, v_max, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "SliderInt2", 6);
  }
  return result;
}

int cmd_sliderint3(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto v_min = get_param_int(argc, params, 2, 0);
    //auto v_max = get_param_int(argc, params, 3, 0);
    //auto format = get_param_int(argc, params, 4, 0);
    //auto flags = get_param_int(argc, params, 5, 0);
    //auto fnResult = SliderInt3(label, v, v_min, v_max, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "SliderInt3", 6);
  }
  return result;
}

int cmd_sliderint4(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto v = get_param_int(argc, params, 1, 0);
    //auto v_min = get_param_int(argc, params, 2, 0);
    //auto v_max = get_param_int(argc, params, 3, 0);
    //auto format = get_param_int(argc, params, 4, 0);
    //auto flags = get_param_int(argc, params, 5, 0);
    //auto fnResult = SliderInt4(label, v, v_min, v_max, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "SliderInt4", 6);
  }
  return result;
}

int cmd_sliderscalar(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto data_type = get_param_int(argc, params, 1, 0);
    //auto p_data = get_param_int(argc, params, 2, 0);
    //auto p_min = get_param_int(argc, params, 3, 0);
    //auto p_max = get_param_int(argc, params, 4, 0);
    //auto format = get_param_int(argc, params, 5, 0);
    //auto flags = get_param_int(argc, params, 6, 0);
    //auto fnResult = SliderScalar(label, data_type, p_data, p_min, p_max, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "SliderScalar", 7);
  }
  return result;
}

int cmd_sliderscalarn(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 8);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto data_type = get_param_int(argc, params, 1, 0);
    //auto p_data = get_param_int(argc, params, 2, 0);
    //auto components = get_param_int(argc, params, 3, 0);
    //auto p_min = get_param_int(argc, params, 4, 0);
    //auto p_max = get_param_int(argc, params, 5, 0);
    //auto format = get_param_int(argc, params, 6, 0);
    //auto flags = get_param_int(argc, params, 7, 0);
    //auto fnResult = SliderScalarN(label, data_type, p_data, components, p_min, p_max, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "SliderScalarN", 8);
  }
  return result;
}

int cmd_smallbutton(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto fnResult = SmallButton(label);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "SmallButton", 1);
  }
  return result;
}

int cmd_tabitembutton(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto flags = get_param_int(argc, params, 1, 0);
    //auto fnResult = TabItemButton(label, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "TabItemButton", 2);
  }
  return result;
}

int cmd_treenode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto fnResult = TreeNode(label);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "TreeNode", 1);
  }
  return result;
}

int cmd_treenodeex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto flags = get_param_int(argc, params, 1, 0);
    //auto fnResult = TreeNodeEx(label, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "TreeNodeEx", 2);
  }
  return result;
}

int cmd_treenodeexv(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    //auto str_id = get_param_str(argc, params, 0, 0);
    //auto flags = get_param_int(argc, params, 1, 0);
    //auto fmt = get_param_int(argc, params, 2, 0);
    //auto va_list = get_param_int(argc, params, 3, 0);
    //auto args = get_param_int(argc, params, 4, 0);
    //auto format = get_param_int(argc, params, 5, 0);
    //auto printf = get_param_int(argc, params, 6, 0);
    // format_text(argc, params, 0)
    //auto fnResult = TreeNodeExV(str_id, flags, fmt, va_list, args, format, printf);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "TreeNodeExV", 7);
  }
  return result;
}

int cmd_treenodev(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    //auto str_id = get_param_str(argc, params, 0, 0);
    //auto fmt = get_param_int(argc, params, 1, 0);
    //auto va_list = get_param_int(argc, params, 2, 0);
    //auto args = get_param_int(argc, params, 3, 0);
    //auto format = get_param_int(argc, params, 4, 0);
    //auto printf = get_param_int(argc, params, 5, 0);
    // format_text(argc, params, 0)
    //auto fnResult = TreeNodeV(str_id, fmt, va_list, args, format, printf);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "TreeNodeV", 6);
  }
  return result;
}

int cmd_vsliderfloat(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto size = get_param_vec2(argc, params, 1);
    //auto v = get_param_int(argc, params, 2, 0);
    //auto v_min = get_param_int(argc, params, 3, 0);
    //auto v_max = get_param_int(argc, params, 4, 0);
    //auto format = get_param_int(argc, params, 5, 0);
    //auto flags = get_param_int(argc, params, 6, 0);
    //auto fnResult = VSliderFloat(label, size, v, v_min, v_max, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "VSliderFloat", 7);
  }
  return result;
}

int cmd_vsliderint(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto size = get_param_vec2(argc, params, 1);
    //auto v = get_param_int(argc, params, 2, 0);
    //auto v_min = get_param_int(argc, params, 3, 0);
    //auto v_max = get_param_int(argc, params, 4, 0);
    //auto format = get_param_int(argc, params, 5, 0);
    //auto flags = get_param_int(argc, params, 6, 0);
    //auto fnResult = VSliderInt(label, size, v, v_min, v_max, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "VSliderInt", 7);
  }
  return result;
}

int cmd_vsliderscalar(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 8);
  if (result) {
    //auto label = get_param_str(argc, params, 0, 0);
    //auto size = get_param_vec2(argc, params, 1);
    //auto data_type = get_param_int(argc, params, 2, 0);
    //auto p_data = get_param_int(argc, params, 3, 0);
    //auto p_min = get_param_int(argc, params, 4, 0);
    //auto p_max = get_param_int(argc, params, 5, 0);
    //auto format = get_param_int(argc, params, 6, 0);
    //auto flags = get_param_int(argc, params, 7, 0);
    //auto fnResult = VSliderScalar(label, size, data_type, p_data, p_min, p_max, format, flags);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "VSliderScalar", 8);
  }
  return result;
}

int cmd_aligntexttoframepadding(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    AlignTextToFramePadding();
  } else {
    error(retval, "AlignTextToFramePadding", 0);
  }
  return result;
}

static int cmd_begingroup(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    BeginGroup();
  } else {
    error(retval, "BeginGroup", 0);
  }
  return result;
}

static int cmd_begintooltip(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    BeginTooltip();
  } else {
    error(retval, "BeginTooltip", 0);
  }
  return result;
}

static int cmd_bullet(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    Bullet();
  } else {
    error(retval, "Bullet", 0);
  }
  return result;
}

static int cmd_bullettext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    BulletText(format_text(argc, params, 0));
  } else {
    error(retval, "BulletText", 3);
  }
  return result;
}

int cmd_calclistclipping(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto items_count = get_param_int(argc, params, 0, 0);
    // auto items_height = get_param_int(argc, params, 1, 0);
    // auto out_items_display_start = get_param_int(argc, params, 2, 0);
    // auto out_items_display_end = get_param_int(argc, params, 3, 0);
    // CalcListClipping(items_count, items_height, out_items_display_start, out_items_display_end);
  } else {
    error(retval, "CalcListClipping", 4);
  }
  return result;
}

static int cmd_capturekeyboardfromapp(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    bool want_capture_keyboard_value = get_param_int(argc, params, 0, 0) == 1;
    CaptureKeyboardFromApp(want_capture_keyboard_value);
  } else {
    error(retval, "CaptureKeyboardFromApp", 1);
  }
  return result;
}

static int cmd_capturemousefromapp(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    bool want_capture_mouse_value = get_param_int(argc, params, 0, 0) == 1;
    CaptureMouseFromApp(want_capture_mouse_value);
  } else {
    error(retval, "CaptureMouseFromApp", 1);
  }
  return result;
}

static int cmd_closecurrentpopup(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    CloseCurrentPopup();
  } else {
    error(retval, "CloseCurrentPopup", 0);
  }
  return result;
}

int cmd_colorconverthsvtorgb(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto h = get_param_int(argc, params, 0, 0);
    // auto s = get_param_int(argc, params, 1, 0);
    // auto v = get_param_int(argc, params, 2, 0);
    // auto out_r = get_param_int(argc, params, 3, 0);
    // auto out_g = get_param_int(argc, params, 4, 0);
    // auto out_b = get_param_int(argc, params, 5, 0);
    // ColorConvertHSVtoRGB(h, s, v, out_r, out_g, out_b);
  } else {
    error(retval, "ColorConvertHSVtoRGB", 6);
  }
  return result;
}

int cmd_colorconvertrgbtohsv(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto r = get_param_int(argc, params, 0, 0);
    // auto g = get_param_int(argc, params, 1, 0);
    // auto b = get_param_int(argc, params, 2, 0);
    // auto out_h = get_param_int(argc, params, 3, 0);
    // auto out_s = get_param_int(argc, params, 4, 0);
    // auto out_v = get_param_int(argc, params, 5, 0);
    // ColorConvertRGBtoHSV(r, g, b, out_h, out_s, out_v);
  } else {
    error(retval, "ColorConvertRGBtoHSV", 6);
  }
  return result;
}

static int cmd_columns(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    auto count = get_param_int(argc, params, 0, 0);
    auto id = get_param_str(argc, params, 1, 0);
    bool border = get_param_int(argc, params, 2, 0) == 1;
    Columns(count, id, border);
  } else {
    error(retval, "Columns", 3);
  }
  return result;
}

static int cmd_destroycontext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    DestroyContext();
  } else {
    error(retval, "DestroyContext", 1);
  }
  return result;
}

static int cmd_dummy(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto size = get_param_vec2(argc, params, 0);
    Dummy(size);
  } else {
    error(retval, "Dummy", 1);
  }
  return result;
}

static int cmd_end(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    End();
  } else {
    error(retval, "End", 0);
  }
  return result;
}

static int cmd_endchild(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndChild();
  } else {
    error(retval, "EndChild", 0);
  }
  return result;
}

static int cmd_endchildframe(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndChildFrame();
  } else {
    error(retval, "EndChildFrame", 0);
  }
  return result;
}

static int cmd_endcombo(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndCombo();
  } else {
    error(retval, "EndCombo", 0);
  }
  return result;
}

static int cmd_enddragdropsource(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndDragDropSource();
  } else {
    error(retval, "EndDragDropSource", 0);
  }
  return result;
}

static int cmd_enddragdroptarget(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndDragDropTarget();
  } else {
    error(retval, "EndDragDropTarget", 0);
  }
  return result;
}

static int cmd_endframe(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndFrame();
  } else {
    error(retval, "EndFrame", 0);
  }
  return result;
}

static int cmd_endgroup(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndGroup();
  } else {
    error(retval, "EndGroup", 0);
  }
  return result;
}

static int cmd_endmainmenubar(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndMainMenuBar();
  } else {
    error(retval, "EndMainMenuBar", 0);
  }
  return result;
}

static int cmd_endmenu(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndMenu();
  } else {
    error(retval, "EndMenu", 0);
  }
  return result;
}

static int cmd_endmenubar(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndMenuBar();
  } else {
    error(retval, "EndMenuBar", 0);
  }
  return result;
}

static int cmd_endpopup(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndPopup();
  } else {
    error(retval, "EndPopup", 0);
  }
  return result;
}

static int cmd_endtabbar(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndTabBar();
  } else {
    error(retval, "EndTabBar", 0);
  }
  return result;
}

static int cmd_endtabitem(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndTabItem();
  } else {
    error(retval, "EndTabItem", 0);
  }
  return result;
}

static int cmd_endtooltip(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndTooltip();
  } else {
    error(retval, "EndTooltip", 0);
  }
  return result;
}

int cmd_image(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto user_texture_id = get_param_int(argc, params, 0, 0);
    // auto size = get_param_int(argc, params, 1, 0);
    // auto uv0 = get_param_int(argc, params, 2, 0);
    // auto uv1 = get_param_int(argc, params, 3, 0);
    // auto tint_col = get_param_int(argc, params, 4, 0);
    // auto border_col = get_param_int(argc, params, 5, 0);
    // Image(user_texture_id, size, uv0, uv1, tint_col, border_col);
  } else {
    error(retval, "Image", 6);
  }
  return result;
}

static int cmd_indent(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto indent_w = get_param_int(argc, params, 0, 0);
    Indent(indent_w);
  } else {
    error(retval, "Indent", 1);
  }
  return result;
}

static int cmd_labeltext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    auto label = get_param_str(argc, params, 0, 0);
    LabelText(label, format_text(argc, params, 1));
  } else {
    error(retval, "LabelText", 4);
  }
  return result;
}

static int cmd_listboxfooter(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    ListBoxFooter();
  } else {
    error(retval, "ListBoxFooter", 0);
  }
  return result;
}

static int cmd_loadinisettingsfromdisk(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto ini_filename = get_param_str(argc, params, 0, 0);
    LoadIniSettingsFromDisk(ini_filename);
  } else {
    error(retval, "LoadIniSettingsFromDisk", 1);
  }
  return result;
}

int cmd_loadinisettingsfrommemory(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto ini_data = get_param_int(argc, params, 0, 0);
    // auto size_t = get_param_int(argc, params, 1, 0);
    // auto ini_size = get_param_int(argc, params, 2, 0);
    // LoadIniSettingsFromMemory(ini_data, size_t, ini_size);
  } else {
    error(retval, "LoadIniSettingsFromMemory", 3);
  }
  return result;
}

static int cmd_logbuttons(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    LogButtons();
  } else {
    error(retval, "LogButtons", 0);
  }
  return result;
}

static int cmd_logfinish(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    LogFinish();
  } else {
    error(retval, "LogFinish", 0);
  }
  return result;
}

static int cmd_logtext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    LogText(format_text(argc, params, 0));
  } else {
    error(retval, "LogText", 3);
  }
  return result;
}

static int cmd_logtoclipboard(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto auto_open_depth = get_param_int(argc, params, 0, 0);
    LogToClipboard(auto_open_depth);
  } else {
    error(retval, "LogToClipboard", 1);
  }
  return result;
}

static int cmd_logtofile(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto auto_open_depth = get_param_int(argc, params, 0, 0);
    auto filename = get_param_str(argc, params, 1, 0);
    LogToFile(auto_open_depth, filename);
  } else {
    error(retval, "LogToFile", 2);
  }
  return result;
}

static int cmd_logtotty(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto auto_open_depth = get_param_int(argc, params, 0, 0);
    LogToTTY(auto_open_depth);
  } else {
    error(retval, "LogToTTY", 1);
  }
  return result;
}

static int cmd_newframe(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    NewFrame();
  } else {
    error(retval, "NewFrame", 0);
  }
  return result;
}

int cmd_newline(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    NewLine();
  } else {
    error(retval, "NewLine", 0);
  }
  return result;
}

static int cmd_nextcolumn(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    NextColumn();
  } else {
    error(retval, "NextColumn", 0);
  }
  return result;
}

static int cmd_openpopup(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto str_id = get_param_str(argc, params, 0, 0);
    auto popup_flags = get_param_int(argc, params, 1, 0);
    OpenPopup(str_id, popup_flags);
  } else {
    error(retval, "OpenPopup", 2);
  }
  return result;
}

static int cmd_openpopuponitemclick(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto str_id = get_param_str(argc, params, 0, 0);
    auto popup_flags = get_param_int(argc, params, 1, 0);
    OpenPopupOnItemClick(str_id, popup_flags);
  } else {
    error(retval, "OpenPopupOnItemClick", 2);
  }
  return result;
}

int cmd_plothistogram(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 9);
  if (result) {
    // auto label = get_param_str(argc, params, 0, 0);
    // auto values = get_param_int(argc, params, 1, 0);
    // auto values_count = get_param_int(argc, params, 2, 0);
    // auto values_offset = get_param_int(argc, params, 3, 0);
    // auto overlay_text = get_param_int(argc, params, 4, 0);
    // auto scale_min = get_param_int(argc, params, 5, 0);
    // auto scale_max = get_param_int(argc, params, 6, 0);
    // auto graph_size = get_param_int(argc, params, 7, 0);
    // auto stride = get_param_int(argc, params, 8, 0);
    // PlotHistogram(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size, stride);
  } else {
    error(retval, "PlotHistogram", 9);
  }
  return result;
}

int cmd_plotlines(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 9);
  if (result) {
    // auto label = get_param_str(argc, params, 0, 0);
    // auto values = get_param_int(argc, params, 1, 0);
    // auto values_count = get_param_int(argc, params, 2, 0);
    // auto values_offset = get_param_int(argc, params, 3, 0);
    // auto overlay_text = get_param_int(argc, params, 4, 0);
    // auto scale_min = get_param_int(argc, params, 5, 0);
    // auto scale_max = get_param_int(argc, params, 6, 0);
    // auto graph_size = get_param_int(argc, params, 7, 0);
    // auto stride = get_param_int(argc, params, 8, 0);
    // PlotLines(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size, stride);
  } else {
    error(retval, "PlotLines", 9);
  }
  return result;
}

int cmd_popallowkeyboardfocus(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    PopAllowKeyboardFocus();
  } else {
    error(retval, "PopAllowKeyboardFocus", 0);
  }
  return result;
}

static int cmd_popbuttonrepeat(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    PopButtonRepeat();
  } else {
    error(retval, "PopButtonRepeat", 0);
  }
  return result;
}

static int cmd_popcliprect(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    PopClipRect();
  } else {
    error(retval, "PopClipRect", 0);
  }
  return result;
}

static int cmd_popfont(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    PopFont();
  } else {
    error(retval, "PopFont", 0);
  }
  return result;
}

static int cmd_popid(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    PopID();
  } else {
    error(retval, "PopID", 0);
  }
  return result;
}

static int cmd_popitemwidth(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    PopItemWidth();
  } else {
    error(retval, "PopItemWidth", 0);
  }
  return result;
}

static int cmd_popstylecolor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto count = get_param_int(argc, params, 0, 0);
    PopStyleColor(count);
  } else {
    error(retval, "PopStyleColor", 1);
  }
  return result;
}

static int cmd_popstylevar(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto count = get_param_int(argc, params, 0, 0);
    PopStyleVar(count);
  } else {
    error(retval, "PopStyleVar", 1);
  }
  return result;
}

static int cmd_poptextwrappos(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    PopTextWrapPos();
  } else {
    error(retval, "PopTextWrapPos", 0);
  }
  return result;
}

static int cmd_progressbar(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    auto fraction = get_param_int(argc, params, 0, 0);
    auto size_arg = get_param_vec2(argc, params, 1);
    auto overlay = get_param_str(argc, params, 2, "");
    ProgressBar(fraction, size_arg, overlay);
  } else {
    error(retval, "ProgressBar", 3);
  }
  return result;
}

static int cmd_pushallowkeyboardfocus(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto allow_keyboard_focus = get_param_int(argc, params, 0, 0);
    PushAllowKeyboardFocus(allow_keyboard_focus);
  } else {
    error(retval, "PushAllowKeyboardFocus", 1);
  }
  return result;
}

static int cmd_pushbuttonrepeat(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto repeat = get_param_int(argc, params, 0, 0);
    PushButtonRepeat(repeat);
  } else {
    error(retval, "PushButtonRepeat", 1);
  }
  return result;
}

int cmd_pushcliprect(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto clip_rect_min = get_param_int(argc, params, 0, 0);
    // auto clip_rect_max = get_param_int(argc, params, 1, 0);
    // auto intersect_with_current_clip_rect = get_param_int(argc, params, 2, 0);
    // PushClipRect(clip_rect_min, clip_rect_max, intersect_with_current_clip_rect);
  } else {
    error(retval, "PushClipRect", 3);
  }
  return result;
}

int cmd_pushfont(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    //auto font = get_param_int(argc, params, 0, 0);
    //PushFont(font);
  } else {
    error(retval, "PushFont", 1);
  }
  return result;
}

static int cmd_pushid(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto str_id = get_param_str(argc, params, 0, 0);
    PushID(str_id);
  } else {
    error(retval, "PushID", 1);
  }
  return result;
}

static int cmd_pushitemwidth(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto item_width = get_param_int(argc, params, 0, 0);
    PushItemWidth(item_width);
  } else {
    error(retval, "PushItemWidth", 1);
  }
  return result;
}

static int cmd_pushstylecolor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto idx = get_param_int(argc, params, 0, 0);
    auto col = get_param_int(argc, params, 1, 0);
    PushStyleColor(idx, col);
  } else {
    error(retval, "PushStyleColor", 2);
  }
  return result;
}

static int cmd_pushstylevar(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto idx = get_param_int(argc, params, 0, 0);
    auto val = get_param_int(argc, params, 1, 0);
    PushStyleVar(idx, val);
  } else {
    error(retval, "PushStyleVar", 2);
  }
  return result;
}

static int cmd_pushtextwrappos(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto wrap_local_pos_x = get_param_int(argc, params, 0, 0);
    PushTextWrapPos(wrap_local_pos_x);
  } else {
    error(retval, "PushTextWrapPos", 1);
  }
  return result;
}

static int cmd_render(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0 || argc == 1);
  if (result) {
    Render();
    GLFWwindow *window = get_window(argc, params);
    if (window != nullptr) {
      int display_w, display_h;
      glfwGetFramebufferSize(window, &display_w, &display_h);
      glViewport(0, 0, display_w, display_h);
      glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
      glClear(GL_COLOR_BUFFER_BIT);
    }
    ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
    if (window != nullptr) {
      glfwSwapBuffers(window);
    }
  } else {
    error(retval, "Render", 1);
  }
  return result;
}

static int cmd_resetmousedragdelta(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto button = get_param_int(argc, params, 0, 0);
    ResetMouseDragDelta(button);
  } else {
    error(retval, "ResetMouseDragDelta", 1);
  }
  return result;
}

static int cmd_sameline(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0 || argc == 1 || argc == 2);
  if (result) {
    auto offset_from_start_x = get_param_int(argc, params, 0, 0);
    auto spacing = get_param_int(argc, params, 1, 0);
    SameLine(offset_from_start_x, spacing);
  } else {
    error(retval, "SameLine", 0, 2);
  }
  return result;
}

int cmd_saveinisettingstodisk(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto ini_filename = get_param_int(argc, params, 0, 0);
    // SaveIniSettingsToDisk(ini_filename);
  } else {
    error(retval, "SaveIniSettingsToDisk", 1);
  }
  return result;
}

static int cmd_separator(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    Separator();
  } else {
    error(retval, "Separator", 0);
  }
  return result;
}

int cmd_setallocatorfunctions(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 8);
  if (result) {
    // auto alloc_func = get_param_int(argc, params, 0, 0);
    // auto size_t = get_param_int(argc, params, 1, 0);
    // auto sz = get_param_int(argc, params, 2, 0);
    // auto user_data = get_param_int(argc, params, 3, 0);
    // auto free_func = get_param_int(argc, params, 4, 0);
    // auto ptr = get_param_int(argc, params, 5, 0);
    // auto user_data = get_param_int(argc, params, 6, 0);
    // auto user_data = get_param_int(argc, params, 7, 0);
    // SetAllocatorFunctions(alloc_func, size_t, sz, user_data, free_func, ptr, user_data, user_data);
  } else {
    error(retval, "SetAllocatorFunctions", 8);
  }
  return result;
}

static int cmd_setclipboardtext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto text = get_param_str(argc, params, 0, 0);
    SetClipboardText(text);
  } else {
    error(retval, "SetClipboardText", 1);
  }
  return result;
}

static int cmd_setcoloreditoptions(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto flags = get_param_int(argc, params, 0, 0);
    SetColorEditOptions(flags);
  } else {
    error(retval, "SetColorEditOptions", 1);
  }
  return result;
}

static int cmd_setcolumnoffset(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto column_index = get_param_int(argc, params, 0, 0);
    auto offset_x = get_param_int(argc, params, 1, 0);
    SetColumnOffset(column_index, offset_x);
  } else {
    error(retval, "SetColumnOffset", 2);
  }
  return result;
}

static int cmd_setcolumnwidth(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto column_index = get_param_int(argc, params, 0, 0);
    auto width = get_param_int(argc, params, 1, 0);
    SetColumnWidth(column_index, width);
  } else {
    error(retval, "SetColumnWidth", 2);
  }
  return result;
}

int cmd_setcurrentcontext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    //auto ctx = get_param_int(argc, params, 0, 0);
    //SetCurrentContext(ctx);
  } else {
    error(retval, "SetCurrentContext", 1);
  }
  return result;
}

static int cmd_setcursorpos(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto local_pos = get_param_vec2(argc, params, 0);
    SetCursorPos(local_pos);
  } else {
    error(retval, "SetCursorPos", 1);
  }
  return result;
}

static int cmd_setcursorposx(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto local_x = get_param_int(argc, params, 0, 0);
    SetCursorPosX(local_x);
  } else {
    error(retval, "SetCursorPosX", 1);
  }
  return result;
}

static int cmd_setcursorposy(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto local_y = get_param_int(argc, params, 0, 0);
    SetCursorPosY(local_y);
  } else {
    error(retval, "SetCursorPosY", 1);
  }
  return result;
}

static int cmd_setcursorscreenpos(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto pos = get_param_vec2(argc, params, 0);
    SetCursorScreenPos(pos);
  } else {
    error(retval, "SetCursorScreenPos", 1);
  }
  return result;
}

int cmd_setitemallowoverlap(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    SetItemAllowOverlap();
  } else {
    error(retval, "SetItemAllowOverlap", 0);
  }
  return result;
}

int cmd_setitemdefaultfocus(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    SetItemDefaultFocus();
  } else {
    error(retval, "SetItemDefaultFocus", 0);
  }
  return result;
}

static int cmd_setkeyboardfocushere(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto offset = get_param_int(argc, params, 0, 0);
    SetKeyboardFocusHere(offset);
  } else {
    error(retval, "SetKeyboardFocusHere", 1);
  }
  return result;
}

static int cmd_setmousecursor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto cursor_type = get_param_int(argc, params, 0, 0);
    SetMouseCursor(cursor_type);
  } else {
    error(retval, "SetMouseCursor", 1);
  }
  return result;
}

static int cmd_setnextitemopen(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto is_open = get_param_int(argc, params, 0, 0);
    auto cond = get_param_int(argc, params, 1, 0);
    SetNextItemOpen(is_open, cond);
  } else {
    error(retval, "SetNextItemOpen", 2);
  }
  return result;
}

static int cmd_setnextitemwidth(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto item_width = get_param_int(argc, params, 0, 0);
    SetNextItemWidth(item_width);
  } else {
    error(retval, "SetNextItemWidth", 1);
  }
  return result;
}

static int cmd_setnextwindowbgalpha(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto alpha = get_param_num(argc, params, 0, 0);
    SetNextWindowBgAlpha(alpha);
  } else {
    error(retval, "SetNextWindowBgAlpha", 1);
  }
  return result;
}

static int cmd_setnextwindowcollapsed(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    bool collapsed = get_param_int(argc, params, 0, 0) == 1;
    auto cond = get_param_int(argc, params, 1, 0);
    SetNextWindowCollapsed(collapsed, cond);
  } else {
    error(retval, "SetNextWindowCollapsed", 2);
  }
  return result;
}

static int cmd_setnextwindowcontentsize(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto size = get_param_vec2(argc, params, 0);
    SetNextWindowContentSize(size);
  } else {
    error(retval, "SetNextWindowContentSize", 1);
  }
  return result;
}

static int cmd_setnextwindowfocus(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    SetNextWindowFocus();
  } else {
    error(retval, "SetNextWindowFocus", 0);
  }
  return result;
}

static int cmd_setnextwindowpos(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc >= 1 && argc <= 3);
  if (result) {
    auto pos = get_param_vec2(argc, params, 0);
    auto cond = get_param_int(argc, params, 1, 0);
    auto pivot = get_param_vec2(argc, params, 2);
    SetNextWindowPos(pos, cond, pivot);
  } else {
    error(retval, "SetNextWindowPos", 1, 3);
  }
  return result;
}

static int cmd_setnextwindowsize(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1 || argc == 2);
  if (result) {
    auto size = get_param_vec2(argc, params, 0);
    auto cond = get_param_int(argc, params, 1, 0);
    SetNextWindowSize(size, cond);
  } else {
    error(retval, "SetNextWindowSize", 1, 2);
  }
  return result;
}

static int cmd_setnextwindowsizeconstraints(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto size_min = get_param_vec2(argc, params, 0);
    auto size_max = get_param_vec2(argc, params, 1);
    SetNextWindowSizeConstraints(size_min, size_max);
  } else {
    error(retval, "SetNextWindowSizeConstraints", 2);
  }
  return result;
}

static int cmd_setscrollfromposx(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto local_x = get_param_int(argc, params, 0, 0);
    auto center_x_ratio = get_param_int(argc, params, 1, 0);
    SetScrollFromPosX(local_x, center_x_ratio);
  } else {
    error(retval, "SetScrollFromPosX", 2);
  }
  return result;
}

static int cmd_setscrollfromposy(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto local_y = get_param_int(argc, params, 0, 0);
    auto center_y_ratio = get_param_int(argc, params, 1, 0);
    SetScrollFromPosY(local_y, center_y_ratio);
  } else {
    error(retval, "SetScrollFromPosY", 2);
  }
  return result;
}

static int cmd_setscrollherex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto center_x_ratio = get_param_int(argc, params, 0, 0);
    SetScrollHereX(center_x_ratio);
  } else {
    error(retval, "SetScrollHereX", 1);
  }
  return result;
}

static int cmd_setscrollherey(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto center_y_ratio = get_param_int(argc, params, 0, 0);
    SetScrollHereY(center_y_ratio);
  } else {
    error(retval, "SetScrollHereY", 1);
  }
  return result;
}

static int cmd_setscrollx(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto scroll_x = get_param_int(argc, params, 0, 0);
    SetScrollX(scroll_x);
  } else {
    error(retval, "SetScrollX", 1);
  }
  return result;
}

static int cmd_setscrolly(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto scroll_y = get_param_int(argc, params, 0, 0);
    SetScrollY(scroll_y);
  } else {
    error(retval, "SetScrollY", 1);
  }
  return result;
}

int cmd_setstatestorage(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto storage = get_param_int(argc, params, 0, 0);
    // SetStateStorage(storage);
  } else {
    error(retval, "SetStateStorage", 1);
  }
  return result;
}

static int cmd_settabitemclosed(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto tab_or_docked_window_label = get_param_str(argc, params, 0, "");
    SetTabItemClosed(tab_or_docked_window_label);
  } else {
    error(retval, "SetTabItemClosed", 1);
  }
  return result;
}

static int cmd_settooltip(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    SetTooltip(format_text(argc, params, 0));
  } else {
    error(retval, "SetTooltip", 3);
  }
  return result;
}

static int cmd_setwindowcollapsed(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto collapsed = get_param_int(argc, params, 0, 0);
    bool cond = get_param_int(argc, params, 1, 0) == 1;
    SetWindowCollapsed(collapsed, cond);
  } else {
    error(retval, "SetWindowCollapsed", 2);
  }
  return result;
}

static int cmd_setwindowfocus(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    SetWindowFocus();
  } else {
    error(retval, "SetWindowFocus", 0);
  }
  return result;
}

static int cmd_setwindowfontscale(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto scale = get_param_int(argc, params, 0, 0);
    SetWindowFontScale(scale);
  } else {
    error(retval, "SetWindowFontScale", 1);
  }
  return result;
}

static int cmd_setwindowpos(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto pos = get_param_vec2(argc, params, 0);
    auto cond = get_param_int(argc, params, 1, 0);
    SetWindowPos(pos, cond);
  } else {
    error(retval, "SetWindowPos", 2);
  }
  return result;
}

static int cmd_setwindowsize(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto size = get_param_vec2(argc, params, 0);
    auto cond = get_param_int(argc, params, 1, 0);
    SetWindowSize(size, cond);
  } else {
    error(retval, "SetWindowSize", 2);
  }
  return result;
}

static int cmd_showaboutwindow(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    bool p_open = (get_param_int(argc, params, 0, 0) == 1);
    ShowAboutWindow(&p_open);
  } else {
    error(retval, "ShowAboutWindow", 1);
  }
  return result;
}

static int cmd_showmetricswindow(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    bool p_open = get_param_int(argc, params, 0, 0) == 1;
    ShowMetricsWindow(&p_open);
  } else {
    error(retval, "ShowMetricsWindow", 1);
  }
  return result;
}

static int cmd_showstyleeditor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    ShowStyleEditor();
  } else {
    error(retval, "ShowStyleEditor", 0);
  }
  return result;
}

static int cmd_spacing(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    Spacing();
  } else {
    error(retval, "Spacing", 0);
  }
  return result;
}

static int cmd_stylecolorsclassic(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    StyleColorsClassic();
  } else {
    error(retval, "StyleColorsClassic", 0);
  }
  return result;
}

static int cmd_stylecolorsdark(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    StyleColorsDark();
  } else {
    error(retval, "StyleColorsDark", 0);
  }
  return result;
}

static int cmd_stylecolorslight(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    StyleColorsLight();
  } else {
    error(retval, "StyleColorsLight", 0);
  }
  return result;
}

static int cmd_text(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc > 0);
  if (result) {
    Text(format_text(argc, params, 0));
  } else {
    error(retval, "Text", 1, 9);
  }
  return result;
}

static int cmd_textcolored(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc > 0);
  if (result) {
    auto col = get_param_vec4(argc, params, 0);
    TextColored(col, format_text(argc, params, 1));
  } else {
    error(retval, "TextColored", 1, 9);
  }
  return result;
}

static int cmd_textdisabled(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc > 0);
  if (result) {
    TextDisabled(format_text(argc, params, 0));
  } else {
    error(retval, "TextDisabled", 1, 9);
  }
  return result;
}

static int cmd_textunformatted(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1 || argc == 2);
  if (result) {
    auto text = get_param_str(argc, params, 0, NULL);
    auto text_end = get_param_str(argc, params, 1, NULL);
    TextUnformatted(text, text_end);
  } else {
    error(retval, "TextUnformatted", 1, 2);
  }
  return result;
}

static int cmd_textwrapped(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc > 0);
  if (result) {
    TextWrapped(format_text(argc, params, 0));
  } else {
    error(retval, "TextWrapped", 1, 9);
  }
  return result;
}

static int cmd_treepop(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    TreePop();
  } else {
    error(retval, "TreePop", 0);
  }
  return result;
}

static int cmd_treepush(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0 || argc == 1);
  if (result) {
    auto str_id = get_param_str(argc, params, 0, NULL);
    TreePush(str_id);
  } else {
    error(retval, "TreePush", 0, 1);
  }
  return result;
}

static int cmd_unindent(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto indent_w = get_param_num(argc, params, 0, 0);
    Unindent(indent_w);
  } else {
    error(retval, "Unindent", 1);
  }
  return result;
}

int cmd_value(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto prefix = get_param_int(argc, params, 0, 0);
    // auto b = get_param_int(argc, params, 1, 0);
    // Value(prefix, b);
  } else {
    error(retval, "Value", 2);
  }
  return result;
}

static void error_callback(int error, const char* description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// if not imgui.init() then throw "error"
static int cmd_init(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    glfwSetErrorCallback(error_callback);
    result = glfwInit();
    if (result) {
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    }
    v_setint(retval, result);
  } else {
    error(retval, "Init", 2);
  }
  return result;
}

static void window_size_callback(GLFWwindow* window, int width, int height) {
  _width = width;
  _height = height;
}

// window = imgui.create_window(640, 480, "Hello World")
static int cmd_create_window(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    int width = get_param_int(argc, params, 0, 640);
    int height = get_param_int(argc, params, 1, 480);
    const char *title = get_param_str(argc, params, 2, "SmallBASIC");
    GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    result = window != nullptr;
    if (result) {
      glfwMakeContextCurrent(window);
      glfwSwapInterval(1);
      gladLoadGL((GLADloadfunc) glfwGetProcAddress);
      glfwSetErrorCallback(error_callback);
      glfwSetWindowSizeCallback(window, window_size_callback);

      result = ++_nextId;
      _windowMap[result] = window;
      _width = width;
      _height = height;

      GLenum error = glGetError();
      if (error != GL_NO_ERROR) {
        char message[100];
        snprintf(message, sizeof(message), "GL Error %d", error);
        v_setstr(retval, message);
        result = 0;
      } else {
        v_setint(retval, result);

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        CreateContext();
        ImGuiIO& io = GetIO(); (void)io;

        // Setup Dear ImGui style
        StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 130");
      }
    } else {
      char message[100];
      snprintf(message, sizeof(message), "Failed to create window [%d X %d]", width, height);
      v_setstr(retval, message);
    }
  } else {
    error(retval, "create_window", 3);
  }
  return result;
}

// n = imgui.window_should_close(window)
static int cmd_window_should_close(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  GLFWwindow *window = get_window(argc, params);
  if (result && window != nullptr) {
    if (glfwWindowShouldClose(window)) {
      close_window(window, get_param_int(argc, params, 0, -1));
      v_setint(retval, 1);
    } else {
      v_setint(retval, 0);
    }
  } else {
    error(retval, "window_should_close", 1);
  }
  return result;
}

static int cmd_framerate(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    v_setreal(retval, GetIO().Framerate);
  } else {
    error(retval, "Framerate", 0);
  }
  return result;
}

// imgui.poll_events()
static int cmd_poll_events(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    glfwPollEvents();
  } else {
    error(retval, "poll_events", 0);
  }
  return result;
}

// imgui.wait_events(n)
static int cmd_wait_events(int argc, slib_par_t *params, var_t *retval) {
  int waitMillis = get_param_int(argc, params, 0, -1);
  if (waitMillis > 0) {
    glfwWaitEventsTimeout(waitMillis / 1000);
  } else {
    glfwWaitEvents();
  }
  return argc < 2;
}

static int cmd_close_window(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    close_window(get_window(argc, params), get_param_int(argc, params, 0, -1));
  } else {
    error(retval, "close_window", 1);
  }
  return result;
}

API lib_func[] = {
  //{"ACCEPTDRAGDROPPAYLOAD", cmd_acceptdragdroppayload},
  {"ARROWBUTTON", cmd_arrowbutton},
  {"BEGIN", cmd_begin},
  {"BEGINFULLSCREEN", cmd_begin_fullscreen},
  {"BEGINCHILD", cmd_beginchild},
  {"BEGINCHILDFRAME", cmd_beginchildframe},
  {"BEGINCOMBO", cmd_begincombo},
  {"BEGINDRAGDROPSOURCE", cmd_begindragdropsource},
  {"BEGINDRAGDROPTARGET", cmd_begindragdroptarget},
  {"BEGINMAINMENUBAR", cmd_beginmainmenubar},
  {"BEGINMENU", cmd_beginmenu},
  {"BEGINMENUBAR", cmd_beginmenubar},
  {"BEGINPOPUP", cmd_beginpopup},
  {"BEGINPOPUPCONTEXTITEM", cmd_beginpopupcontextitem},
  {"BEGINPOPUPCONTEXTVOID", cmd_beginpopupcontextvoid},
  {"BEGINPOPUPCONTEXTWINDOW", cmd_beginpopupcontextwindow},
  {"BEGINPOPUPMODAL", cmd_beginpopupmodal},
  {"BEGINTABBAR", cmd_begintabbar},
  {"BEGINTABITEM", cmd_begintabitem},
  {"BUTTON", cmd_button},
  {"CALCITEMWIDTH", cmd_calcitemwidth},
  {"CALCTEXTSIZE", cmd_calctextsize},
  //{"COLLAPSINGHEADER", cmd_collapsingheader},
  //{"COLORBUTTON", cmd_colorbutton},
  //{"COLORCONVERTFLOAT4TOU32", cmd_colorconvertfloat4tou32},
  //{"COLORCONVERTU32TOFLOAT4", cmd_colorconvertu32tofloat4},
  {"COLOREDIT3", cmd_coloredit3},
  {"COLOREDIT4", cmd_coloredit4},
  //{"COLORPICKER3", cmd_colorpicker3},
  //{"COLORPICKER4", cmd_colorpicker4},
  //{"COMBO", cmd_combo},
  {"CREATECONTEXT", cmd_createcontext},
  //{"DEBUGCHECKVERSIONANDDATALAYOUT", cmd_debugcheckversionanddatalayout},
  //{"DRAGFLOAT", cmd_dragfloat},
  //{"DRAGFLOAT2", cmd_dragfloat2},
  //{"DRAGFLOAT3", cmd_dragfloat3},
  //{"DRAGFLOAT4", cmd_dragfloat4},
  //{"DRAGFLOATRANGE2", cmd_dragfloatrange2},
  //{"DRAGINT", cmd_dragint},
  //{"DRAGINT2", cmd_dragint2},
  //{"DRAGINT3", cmd_dragint3},
  //{"DRAGINT4", cmd_dragint4},
  //{"DRAGINTRANGE2", cmd_dragintrange2},
  //{"DRAGSCALAR", cmd_dragscalar},
  //{"DRAGSCALARN", cmd_dragscalarn},
  //{"GETBACKGROUNDDRAWLIST", cmd_getbackgrounddrawlist},
  {"GETCLIPBOARDTEXT", cmd_getclipboardtext},
  //{"GETCOLORU32", cmd_getcoloru32},
  {"GETCOLUMNINDEX", cmd_getcolumnindex},
  {"GETCOLUMNOFFSET", cmd_getcolumnoffset},
  {"GETCOLUMNSCOUNT", cmd_getcolumnscount},
  {"GETCOLUMNWIDTH", cmd_getcolumnwidth},
  {"GETCONTENTREGIONAVAIL", cmd_getcontentregionavail},
  {"GETCONTENTREGIONMAX", cmd_getcontentregionmax},
  //{"GETCURRENTCONTEXT", cmd_getcurrentcontext},
  {"GETCURSORPOS", cmd_getcursorpos},
  {"GETCURSORPOSX", cmd_getcursorposx},
  {"GETCURSORPOSY", cmd_getcursorposy},
  {"GETCURSORSCREENPOS", cmd_getcursorscreenpos},
  {"GETCURSORSTARTPOS", cmd_getcursorstartpos},
  //{"GETDRAGDROPPAYLOAD", cmd_getdragdroppayload},
  //{"GETDRAWDATA", cmd_getdrawdata},
  //{"GETDRAWLISTSHAREDDATA", cmd_getdrawlistshareddata},
  //{"GETFONT", cmd_getfont},
  {"GETFONTSIZE", cmd_getfontsize},
  {"GETFONTTEXUVWHITEPIXEL", cmd_getfonttexuvwhitepixel},
  //{"GETFOREGROUNDDRAWLIST", cmd_getforegrounddrawlist},
  {"GETFRAMECOUNT", cmd_getframecount},
  {"GETFRAMEHEIGHT", cmd_getframeheight},
  {"GETFRAMEHEIGHTWITHSPACING", cmd_getframeheightwithspacing},
  {"GETID", cmd_getid},
  {"GETITEMRECTMAX", cmd_getitemrectmax},
  {"GETITEMRECTMIN", cmd_getitemrectmin},
  {"GETITEMRECTSIZE", cmd_getitemrectsize},
  {"GETKEYINDEX", cmd_getkeyindex},
  //{"GETKEYPRESSEDAMOUNT", cmd_getkeypressedamount},
  {"GETMOUSECURSOR", cmd_getmousecursor},
  //{"GETMOUSEDRAGDELTA", cmd_getmousedragdelta},
  {"GETMOUSEPOS", cmd_getmousepos},
  {"GETMOUSEPOSONOPENINGCURRENTPOPUP", cmd_getmouseposonopeningcurrentpopup},
  {"GETSCROLLMAXX", cmd_getscrollmaxx},
  {"GETSCROLLMAXY", cmd_getscrollmaxy},
  {"GETSCROLLX", cmd_getscrollx},
  {"GETSCROLLY", cmd_getscrolly},
  //{"GETSTATESTORAGE", cmd_getstatestorage},
  //{"GETSTYLE", cmd_getstyle},
  {"GETSTYLECOLORNAME", cmd_getstylecolorname},
  //{"GETSTYLECOLORVEC4", cmd_getstylecolorvec4},
  {"GETTEXTLINEHEIGHT", cmd_gettextlineheight},
  {"GETTEXTLINEHEIGHTWITHSPACING", cmd_gettextlineheightwithspacing},
  {"GETTIME", cmd_gettime},
  {"GETTREENODETOLABELSPACING", cmd_gettreenodetolabelspacing},
  {"GETVERSION", cmd_getversion},
  {"GETWINDOWCONTENTREGIONMAX", cmd_getwindowcontentregionmax},
  {"GETWINDOWCONTENTREGIONMIN", cmd_getwindowcontentregionmin},
  {"GETWINDOWCONTENTREGIONWIDTH", cmd_getwindowcontentregionwidth},
  {"GETWINDOWDRAWLIST", cmd_getwindowdrawlist},
  {"GETWINDOWHEIGHT", cmd_getwindowheight},
  {"GETWINDOWPOS", cmd_getwindowpos},
  {"GETWINDOWSIZE", cmd_getwindowsize},
  {"GETWINDOWWIDTH", cmd_getwindowwidth},
  //{"IMAGEBUTTON", cmd_imagebutton},
  //{"INPUTDOUBLE", cmd_inputdouble},
  //{"INPUTFLOAT", cmd_inputfloat},
  //{"INPUTFLOAT2", cmd_inputfloat2},
  //{"INPUTFLOAT3", cmd_inputfloat3},
  //{"INPUTFLOAT4", cmd_inputfloat4},
  //{"INPUTINT", cmd_inputint},
  //{"INPUTINT2", cmd_inputint2},
  //{"INPUTINT3", cmd_inputint3},
  //{"INPUTINT4", cmd_inputint4},
  //{"INPUTSCALAR", cmd_inputscalar},
  //{"INPUTSCALARN", cmd_inputscalarn},
  //{"INPUTTEXT", cmd_inputtext},
  //{"INPUTTEXTMULTILINE", cmd_inputtextmultiline},
  //{"INPUTTEXTWITHHINT", cmd_inputtextwithhint},
  //{"INVISIBLEBUTTON", cmd_invisiblebutton},
  {"ISANYITEMACTIVE", cmd_isanyitemactive},
  {"ISANYITEMFOCUSED", cmd_isanyitemfocused},
  {"ISANYITEMHOVERED", cmd_isanyitemhovered},
  {"ISANYMOUSEDOWN", cmd_isanymousedown},
  {"ISITEMACTIVATED", cmd_isitemactivated},
  {"ISITEMACTIVE", cmd_isitemactive},
  {"ISITEMCLICKED", cmd_isitemclicked},
  {"ISITEMDEACTIVATED", cmd_isitemdeactivated},
  {"ISITEMDEACTIVATEDAFTEREDIT", cmd_isitemdeactivatedafteredit},
  {"ISITEMEDITED", cmd_isitemedited},
  {"ISITEMFOCUSED", cmd_isitemfocused},
  {"ISITEMHOVERED", cmd_isitemhovered},
  {"ISITEMTOGGLEDOPEN", cmd_isitemtoggledopen},
  {"ISITEMVISIBLE", cmd_isitemvisible},
  {"ISKEYDOWN", cmd_iskeydown},
  {"ISKEYPRESSED", cmd_iskeypressed},
  {"ISKEYRELEASED", cmd_iskeyreleased},
  {"ISMOUSECLICKED", cmd_ismouseclicked},
  {"ISMOUSEDOUBLECLICKED", cmd_ismousedoubleclicked},
  {"ISMOUSEDOWN", cmd_ismousedown},
  {"ISMOUSEDRAGGING", cmd_ismousedragging},
  {"ISMOUSEHOVERINGRECT", cmd_ismousehoveringrect},
  {"ISMOUSEPOSVALID", cmd_ismouseposvalid},
  {"ISMOUSERELEASED", cmd_ismousereleased},
  {"ISPOPUPOPEN", cmd_ispopupopen},
  {"ISRECTVISIBLE", cmd_isrectvisible},
  {"ISWINDOWAPPEARING", cmd_iswindowappearing},
  {"ISWINDOWCOLLAPSED", cmd_iswindowcollapsed},
  {"ISWINDOWFOCUSED", cmd_iswindowfocused},
  {"ISWINDOWHOVERED", cmd_iswindowhovered},
  //{"LISTBOX", cmd_listbox},
  //{"LISTBOXHEADER", cmd_listboxheader},
  //{"MENUITEM", cmd_menuitem},
  //{"RADIOBUTTON", cmd_radiobutton},
  //{"SAVEINISETTINGSTOMEMORY", cmd_saveinisettingstomemory},
  //{"SELECTABLE", cmd_selectable},
  //{"SETDRAGDROPPAYLOAD", cmd_setdragdroppayload},
  //{"SHOWSTYLESELECTOR", cmd_showstyleselector},
  //{"SLIDERANGLE", cmd_sliderangle},
  //{"SLIDERINT", cmd_sliderint},
  //{"SLIDERINT2", cmd_sliderint2},
  //{"SLIDERINT3", cmd_sliderint3},
  //{"SLIDERINT4", cmd_sliderint4},
  //{"SLIDERSCALAR", cmd_sliderscalar},
  //{"SLIDERSCALARN", cmd_sliderscalarn},
  //{"SMALLBUTTON", cmd_smallbutton},
  //{"TABITEMBUTTON", cmd_tabitembutton},
  //{"TREENODE", cmd_treenode},
  //{"TREENODEEX", cmd_treenodeex},
  //{"TREENODEEXV", cmd_treenodeexv},
  //{"TREENODEV", cmd_treenodev},
  //{"VSLIDERFLOAT", cmd_vsliderfloat},
  //{"VSLIDERINT", cmd_vsliderint},
  //{"VSLIDERSCALAR", cmd_vsliderscalar},

  {"CREATE_WINDOW", cmd_create_window},
  {"INIT", cmd_init},
  {"WINDOW_SHOULD_CLOSE", cmd_window_should_close},
  {"FRAMERATE", cmd_framerate},
};

API lib_proc[] = {
  {"ALIGNTEXTTOFRAMEPADDING", cmd_aligntexttoframepadding},
  {"BEGINGROUP", cmd_begingroup},
  {"BEGINTOOLTIP", cmd_begintooltip},
  {"BULLET", cmd_bullet},
  {"BULLETTEXT", cmd_bullettext},
  // {"CALCLISTCLIPPING", cmd_calclistclipping},
  {"CAPTUREKEYBOARDFROMAPP", cmd_capturekeyboardfromapp},
  {"CAPTUREMOUSEFROMAPP", cmd_capturemousefromapp},
  {"CHECKBOX", cmd_checkbox},
  //{"CHECKBOXFLAGS", cmd_checkboxflags},
  {"CLOSECURRENTPOPUP", cmd_closecurrentpopup},
  // {"COLORCONVERTHSVTORGB", cmd_colorconverthsvtorgb},
  // {"COLORCONVERTRGBTOHSV", cmd_colorconvertrgbtohsv},
  {"COLUMNS", cmd_columns},
  {"DESTROYCONTEXT", cmd_destroycontext},
  {"DUMMY", cmd_dummy},
  {"END", cmd_end},
  {"ENDCHILD", cmd_endchild},
  {"ENDCHILDFRAME", cmd_endchildframe},
  {"ENDCOMBO", cmd_endcombo},
  {"ENDDRAGDROPSOURCE", cmd_enddragdropsource},
  {"ENDDRAGDROPTARGET", cmd_enddragdroptarget},
  {"ENDFRAME", cmd_endframe},
  {"ENDGROUP", cmd_endgroup},
  {"ENDMAINMENUBAR", cmd_endmainmenubar},
  {"ENDMENU", cmd_endmenu},
  {"ENDMENUBAR", cmd_endmenubar},
  {"ENDPOPUP", cmd_endpopup},
  {"ENDTABBAR", cmd_endtabbar},
  {"ENDTABITEM", cmd_endtabitem},
  {"ENDTOOLTIP", cmd_endtooltip},
  // {"IMAGE", cmd_image},
  {"INDENT", cmd_indent},
  {"LABELTEXT", cmd_labeltext},
  {"LISTBOXFOOTER", cmd_listboxfooter},
  {"LOADINISETTINGSFROMDISK", cmd_loadinisettingsfromdisk},
  // {"LOADINISETTINGSFROMMEMORY", cmd_loadinisettingsfrommemory},
  {"LOGBUTTONS", cmd_logbuttons},
  {"LOGFINISH", cmd_logfinish},
  {"LOGTEXT", cmd_logtext},
  {"LOGTOCLIPBOARD", cmd_logtoclipboard},
  {"LOGTOFILE", cmd_logtofile},
  {"LOGTOTTY", cmd_logtotty},
  {"NEWFRAME", cmd_newframe},
  {"NEWLINE", cmd_newline},
  {"NEXTCOLUMN", cmd_nextcolumn},
  {"OPENPOPUP", cmd_openpopup},
  {"OPENPOPUPONITEMCLICK", cmd_openpopuponitemclick},
  // {"PLOTHISTOGRAM", cmd_plothistogram},
  // {"PLOTLINES", cmd_plotlines},
  {"POPALLOWKEYBOARDFOCUS", cmd_popallowkeyboardfocus},
  {"POPBUTTONREPEAT", cmd_popbuttonrepeat},
  {"POPCLIPRECT", cmd_popcliprect},
  {"POPFONT", cmd_popfont},
  {"POPID", cmd_popid},
  {"POPITEMWIDTH", cmd_popitemwidth},
  {"POPSTYLECOLOR", cmd_popstylecolor},
  {"POPSTYLEVAR", cmd_popstylevar},
  {"POPTEXTWRAPPOS", cmd_poptextwrappos},
  {"PROGRESSBAR", cmd_progressbar},
  {"PUSHALLOWKEYBOARDFOCUS", cmd_pushallowkeyboardfocus},
  {"PUSHBUTTONREPEAT", cmd_pushbuttonrepeat},
  // {"PUSHCLIPRECT", cmd_pushcliprect},
  // {"PUSHFONT", cmd_pushfont},
  {"PUSHID", cmd_pushid},
  {"PUSHITEMWIDTH", cmd_pushitemwidth},
  {"PUSHSTYLECOLOR", cmd_pushstylecolor},
  {"PUSHSTYLEVAR", cmd_pushstylevar},
  {"PUSHTEXTWRAPPOS", cmd_pushtextwrappos},
  {"RENDER", cmd_render},
  {"RESETMOUSEDRAGDELTA", cmd_resetmousedragdelta},
  {"SAMELINE", cmd_sameline},
  // {"SAVEINISETTINGSTODISK", cmd_saveinisettingstodisk},
  {"SEPARATOR", cmd_separator},
  // {"SETALLOCATORFUNCTIONS", cmd_setallocatorfunctions},
  {"SETCLIPBOARDTEXT", cmd_setclipboardtext},
  {"SETCOLOREDITOPTIONS", cmd_setcoloreditoptions},
  {"SETCOLUMNOFFSET", cmd_setcolumnoffset},
  {"SETCOLUMNWIDTH", cmd_setcolumnwidth},
  // {"SETCURRENTCONTEXT", cmd_setcurrentcontext},
  {"SETCURSORPOS", cmd_setcursorpos},
  {"SETCURSORPOSX", cmd_setcursorposx},
  {"SETCURSORPOSY", cmd_setcursorposy},
  {"SETCURSORSCREENPOS", cmd_setcursorscreenpos},
  {"SETITEMALLOWOVERLAP", cmd_setitemallowoverlap},
  {"SETITEMDEFAULTFOCUS", cmd_setitemdefaultfocus},
  {"SETKEYBOARDFOCUSHERE", cmd_setkeyboardfocushere},
  {"SETMOUSECURSOR", cmd_setmousecursor},
  {"SETNEXTITEMOPEN", cmd_setnextitemopen},
  {"SETNEXTITEMWIDTH", cmd_setnextitemwidth},
  {"SETNEXTWINDOWBGALPHA", cmd_setnextwindowbgalpha},
  {"SETNEXTWINDOWCOLLAPSED", cmd_setnextwindowcollapsed},
  {"SETNEXTWINDOWCONTENTSIZE", cmd_setnextwindowcontentsize},
  {"SETNEXTWINDOWFOCUS", cmd_setnextwindowfocus},
  {"SETNEXTWINDOWPOS", cmd_setnextwindowpos},
  {"SETNEXTWINDOWSIZE", cmd_setnextwindowsize},
  {"SETNEXTWINDOWSIZECONSTRAINTS", cmd_setnextwindowsizeconstraints},
  {"SETSCROLLFROMPOSX", cmd_setscrollfromposx},
  {"SETSCROLLFROMPOSY", cmd_setscrollfromposy},
  {"SETSCROLLHEREX", cmd_setscrollherex},
  {"SETSCROLLHEREY", cmd_setscrollherey},
  {"SETSCROLLX", cmd_setscrollx},
  {"SETSCROLLY", cmd_setscrolly},
  // {"SETSTATESTORAGE", cmd_setstatestorage},
  {"SETTABITEMCLOSED", cmd_settabitemclosed},
  {"SETTOOLTIP", cmd_settooltip},
  {"SETWINDOWCOLLAPSED", cmd_setwindowcollapsed},
  {"SETWINDOWFOCUS", cmd_setwindowfocus},
  {"SETWINDOWFONTSCALE", cmd_setwindowfontscale},
  {"SETWINDOWPOS", cmd_setwindowpos},
  {"SETWINDOWSIZE", cmd_setwindowsize},
  {"SHOWABOUTWINDOW", cmd_showaboutwindow},
  {"SHOWMETRICSWINDOW", cmd_showmetricswindow},
  {"SHOWSTYLEEDITOR", cmd_showstyleeditor},
  {"SLIDERFLOAT", cmd_sliderfloat},
  //{"SLIDERFLOAT2", cmd_sliderfloat2},
  //{"SLIDERFLOAT3", cmd_sliderfloat3},
  //{"SLIDERFLOAT4", cmd_sliderfloat4},
  {"SPACING", cmd_spacing},
  {"STYLECOLORSCLASSIC", cmd_stylecolorsclassic},
  {"STYLECOLORSDARK", cmd_stylecolorsdark},
  {"STYLECOLORSLIGHT", cmd_stylecolorslight},
  {"TEXT", cmd_text},
  {"TEXTCOLORED", cmd_textcolored},
  {"TEXTDISABLED", cmd_textdisabled},
  {"TEXTUNFORMATTED", cmd_textunformatted},
  {"TEXTWRAPPED", cmd_textwrapped},
  {"TREEPOP", cmd_treepop},
  {"TREEPUSH", cmd_treepush},
  {"UNINDENT", cmd_unindent},
  // {"VALUE", cmd_value},

  {"POLL_EVENTS", cmd_poll_events},
  {"WAIT_EVENTS", cmd_wait_events},
  {"CLOSE_WINDOW", cmd_close_window},
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
    fprintf(stderr, "ImGui: PROC index error [%d]\n", index);
    result = 0;
  }
  return result;
}

int sblib_func_exec(int index, int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (index < sblib_func_count()) {
    result = lib_func[index].command(argc, params, retval);
  } else {
    fprintf(stderr, "ImGui: FUNC index error [%d]\n", index);
    result = 0;
  }
  return result;
}

int sblib_events(int wait_flag, int *w, int *h) {
  *w = _width;
  *h = _height;
  return 0;
}

void sblib_close(void) {
  _windowMap.clear();
}
