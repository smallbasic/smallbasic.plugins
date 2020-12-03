// This file is part of SmallBASIC
//
// Plugin for raylib games library - https://www.raylib.com/
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2020 Chris Warren-Smith

#include "config.h"

#include <raylib/raylib/src/raylib.h>
#include <raygui/src/raygui.h>
#include <raylib/src/physac.h>
#include <GLFW/glfw3.h>
#include <cstring>
#include <unordered_map>

#include "include/var.h"
#include "include/var_map.h"
#include "include/module.h"
#include "include/param.h"

std::unordered_map<int, Font> _fontMap;
std::unordered_map<int, Image> _imageMap;
std::unordered_map<int, Model> _modelMap;
std::unordered_map<int, Music> _musicMap;
std::unordered_map<int, PhysicsBody> _physicsMap;
std::unordered_map<int, RenderTexture2D> _renderMap;
std::unordered_map<int, Sound> _soundMap;
std::unordered_map<int, Texture2D> _textureMap;
int _nextId = 1;
const char *mapID = "_ID";

static bool is_array(var_p_t var, uint32_t size) {
  return var != nullptr && v_is_type(var, V_ARRAY) && v_asize(var) == size;
}

static bool is_map(var_p_t var) {
  return var != nullptr && v_is_type(var, V_MAP);
}

static Color get_param_color(int argc, slib_par_t *params, int n) {
  Color result;
  if (n >= 0 && n < argc && params[n].var_p->type == V_INT) {
    var_int_t c = params[n].var_p->v.i;
    uint8_t r = (c & 0xff000000) >> 24;
    uint8_t g = (c & 0xff0000) >> 16;
    uint8_t b = (c & 0xff00) >> 8;
    uint8_t a = (c & 0xff);
    result = {r, g, b, a};
  } else if (is_param_array(argc, params, n)) {
    result.r = get_array_elem_num(params[n].var_p, 0);
    result.g = get_array_elem_num(params[n].var_p, 1);
    result.b = get_array_elem_num(params[n].var_p, 2);
    if (v_asize(params[n].var_p) == 4) {
      result.a = get_array_elem_num(params[n].var_p, 3);
    } else {
      result.a = 255;
    }
  } if (is_param_map(argc, params, n)) {
    result.r = get_map_num(params[n].var_p, "r");
    result.g = get_map_num(params[n].var_p, "g");
    result.b = get_map_num(params[n].var_p, "b");
    result.a = get_map_num(params[n].var_p, "a");
  }
  return result;
}

static Rectangle get_param_rect(int argc, slib_par_t *params, int n) {
  Rectangle result;
  if (is_param_map(argc, params, n)) {
    result.x = get_map_num(params[n].var_p, "x");
    result.y = get_map_num(params[n].var_p, "y");
    result.width = get_map_num(params[n].var_p, "width");
    result.height = get_map_num(params[n].var_p, "height");
  } else if (is_param_array(argc, params, n)) {
    result.x = get_array_elem_num(params[n].var_p, 0);
    result.y = get_array_elem_num(params[n].var_p, 1);
    result.width = get_array_elem_num(params[n].var_p, 2);
    result.height = get_array_elem_num(params[n].var_p, 3);
  }
  return result;
}

static Vector2 get_param_vec2(int argc, slib_par_t *params, int n) {
  Vector2 result;
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

static Vector2 get_array_elem_vec2(var_p_t array, int index) {
  Vector2 result;
  int size = v_asize(array);
  if (index >= 0 && index < size) {
    var_p_t elem = v_elem(array, index);
    if (elem->type == V_ARRAY) {
      result.x = get_array_elem_num(elem, 0);
      result.y = get_array_elem_num(elem, 1);
    }
  }
  return result;
}

static Vector2 get_map_vec2(var_p_t map, const char *name) {
  Vector2 result;
  var_p_t array = map_get(map, name);
  if (is_array(array, 2)) {
    result.x = get_array_elem_num(array, 0);
    result.y = get_array_elem_num(array, 1);
  } else if (is_map(array)) {
    var_p_t var = map_get(map, name);
    result.x = get_map_num(var, "x");
    result.y = get_map_num(var, "y");
  } else {
    TraceLog(LOG_FATAL, "Vector2 not found");
  }
  return result;
}

static Camera2D get_camera_2d(int argc, slib_par_t *params, int n) {
  Camera2D result;
  if (is_param_array(argc, params, n)) {
    var_p_t array = params[n].var_p;
    result.offset = get_array_elem_vec2(array, 0);
    result.target = get_array_elem_vec2(array, 1);
    result.rotation = get_array_elem_num(array, 2);
    result.zoom = get_array_elem_num(array, 3);
  } else if (is_param_map(argc, params, n)) {
    var_p_t map = params[n].var_p;
    result.offset = get_map_vec2(map, "offset");
    result.target = get_map_vec2(map, "target");
    result.rotation = get_map_num(map, "rotation");
    result.zoom = get_map_num(map, "zoom");
  } else {
    TraceLog(LOG_FATAL, "Camera2D not found");
  }
  return result;
}

static Vector3 get_param_vec3(int argc, slib_par_t *params, int n) {
  Vector3 result;
  if (is_param_map(argc, params, n)) {
    result.x = get_map_num(params[n].var_p, "x");
    result.y = get_map_num(params[n].var_p, "y");
    result.z = get_map_num(params[n].var_p, "z");
  } else if (is_param_array(argc, params, n)) {
    result.x = get_array_elem_num(params[n].var_p, 0);
    result.y = get_array_elem_num(params[n].var_p, 1);
    result.z = get_array_elem_num(params[n].var_p, 2);
  } else {
    result.x = 0;
    result.y = 0;
    result.z = 0;
  }
  return result;
}

static Vector3 get_array_elem_vec3(var_p_t array, int index) {
  Vector3 result;
  int size = v_asize(array);
  if (index >= 0 && index < size) {
    var_p_t elem = v_elem(array, index);
    if (elem->type == V_ARRAY) {
      result.x = get_array_elem_num(elem, 0);
      result.y = get_array_elem_num(elem, 1);
      result.z = get_array_elem_num(elem, 2);
    }
  }
  return result;
}

static Vector3 get_map_vec3(var_p_t map, const char *name) {
  Vector3 result;
  var_p_t array = map_get(map, name);
  if (is_array(array, 3)) {
    result.x = get_array_elem_num(array, 0);
    result.y = get_array_elem_num(array, 1);
    result.z = get_array_elem_num(array, 2);
  } else if (is_map(array)) {
    result.x = get_map_num(array, "x");
    result.y = get_map_num(array, "y");
    result.z = get_map_num(array, "z");
  } else {
    TraceLog(LOG_FATAL, "Vector3 not found");
  }
  return result;
}

static Camera3D get_camera_3d(int argc, slib_par_t *params, int n) {
  Camera3D result;
  if (is_param_array(argc, params, n)) {
    var_p_t array = params[n].var_p;
    result.position = get_array_elem_vec3(array, 0);
    result.target = get_array_elem_vec3(array, 1);
    result.up = get_array_elem_vec3(array, 2);
    result.fovy = get_array_elem_num(array, 3);
    result.type = get_array_elem_num(array, 4);
  } else if (is_param_map(argc, params, n)) {
    var_p_t map = params[n].var_p;
    result.position = get_map_vec3(map, "position");
    result.target = get_map_vec3(map, "target");
    result.up = get_map_vec3(map, "up");
    result.fovy = get_map_num(map, "fovy");
    result.type = get_map_num(map, "type");
  } else {
    TraceLog(LOG_FATAL, "Camera3D not found");
  }
  return result;
}

static void set_camera_3d(var_p_t var, Camera3D *camera) {
  var_p_t v_position;
  var_p_t v_target;
  var_p_t v_up;

  if (is_array(var, 5)) {
    v_position = v_elem(var, 0);
    v_target = v_elem(var, 1);
    v_up = v_elem(var, 2);
  } else if (v_is_type(var, V_MAP)) {
    v_position = map_get(var, "position");
    v_target = map_get(var, "target");
    v_up = map_get(var, "up");
  } else {
    v_position = nullptr;
    v_target = nullptr;
    v_up = nullptr;
  }
  if (is_array(v_position, 3)) {
    v_setreal(v_elem(v_position, 0), camera->position.x);
    v_setreal(v_elem(v_position, 1), camera->position.y);
    v_setreal(v_elem(v_position, 2), camera->position.z);
  } else {
    TraceLog(LOG_FATAL, "Camera3D position not found");
  }
  if (is_array(v_target, 3)) {
    v_setreal(v_elem(v_target, 0), camera->target.x);
    v_setreal(v_elem(v_target, 1), camera->target.y);
    v_setreal(v_elem(v_target, 2), camera->target.z);
  } else {
    TraceLog(LOG_FATAL, "Camera3D target not found");
  }
  if (is_array(v_up, 3)) {
    v_setreal(v_elem(v_up, 0), camera->up.x);
    v_setreal(v_elem(v_up, 1), camera->up.y);
    v_setreal(v_elem(v_up, 2), camera->up.z);
  } else {
    TraceLog(LOG_FATAL, "Camera3D up not found");
  }
}

static Shader get_param_shader(int argc, slib_par_t *params, int n) {
  Shader result;
  if (is_param_map(argc, params, n)) {
    var_p_t var = map_get(params[n].var_p, "locs");
    if (v_is_type(var, V_INT)) {
      result.locs = (int *)(var->v.i);
    } else {
      result.locs = nullptr;
    }
    result.id = map_get_int(params[n].var_p, mapID, -1);
  } else {
    TraceLog(LOG_FATAL, "Shader not found");
  }
  return result;
}

static BoundingBox get_param_bounding_box(int argc, slib_par_t *params, int n) {
  BoundingBox result;
  if (is_param_array(argc, params, n)) {
    var_p_t array = params[n].var_p;
    result.min = get_array_elem_vec3(array, 0);
    result.max = get_array_elem_vec3(array, 1);
  } else {
    result.min.x = 0;
    result.min.y = 0;
    result.min.z = 0;
    result.max.x = 0;
    result.max.y = 0;
    result.max.z = 0;
  }
  return result;
}

static int get_font_id(int argc, slib_par_t *params, int arg, var_t *retval) {
  int result = -1;
  if (is_param_map(argc, params, arg)) {
    // the passed in variable is a map
    int id = map_get_int(params[arg].var_p, mapID, -1);
    if (id != -1 && _fontMap.find(id) != _fontMap.end()) {
      // the map contained an ID field with a live value
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "Font not found");
  }
  return result;
}

static int get_image_id(int argc, slib_par_t *params, int arg, var_t *retval) {
  int result = -1;
  if (is_param_map(argc, params, arg)) {
    // the passed in variable is a map
    int id = map_get_int(params[arg].var_p, mapID, -1);
    if (id != -1 && _imageMap.find(id) != _imageMap.end()) {
      // the map contained an ID field with a live value
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "Image not found");
  }
  return result;
}

static int get_texture_id(int argc, slib_par_t *params, int arg, var_t *retval) {
  int result = -1;
  if (is_param_map(argc, params, arg)) {
    // the passed in variable is a map
    int id = map_get_int(params[arg].var_p, mapID, -1);
    if (id != -1 && _textureMap.find(id) != _textureMap.end()) {
      // the map contained an ID field with a live value
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "Texture not found");
  }
  return result;
}

static int get_model_id(int argc, slib_par_t *params, int arg, var_t *retval) {
  int result = -1;
  if (is_param_map(argc, params, arg)) {
    // the passed in variable is a map
    int id = map_get_int(params[arg].var_p, mapID, -1);
    if (id != -1 && _modelMap.find(id) != _modelMap.end()) {
      // the map contained an ID field with a live value
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "Model not found");
  }
  return result;
}

static int get_physics_body_id(int argc, slib_par_t *params, int arg, var_t *retval) {
  int result = -1;
  if (is_param_map(argc, params, arg)) {
    // the passed in variable is a map
    int id = map_get_int(params[arg].var_p, mapID, -1);
    if (id != -1 && _physicsMap.find(id) != _physicsMap.end()) {
      // the map contained an ID field with a live value
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "PhysicsBody not found");
  }
  return result;
}

static int get_render_texture_id(int argc, slib_par_t *params, int n) {
  int result = 0;
  if (is_param_map(argc, params, n)) {
    // the passed in variable is a map
    int id = map_get_int(params[n].var_p, mapID, -1);
    if (id != -1 && _renderMap.find(id) != _renderMap.end()) {
      // the map contained an ID field with a live value
      result = id;
    }
  }
  return result;
}

static void v_setcolor(var_t *var, Color &c) {
  var_int_t color = ((0x00000000) | (c.r << 24) | (c.g << 16) | (c.b << 8) | (c.a));
  v_setint(var, color);
}

static void v_setfont(var_t *var, Font &font, int id) {
  map_init(var);
  v_setint(map_add_var(var, "baseSize", 0), font.baseSize);
  v_setint(map_add_var(var, "charsCount", 0), font.charsCount);
  v_setint(map_add_var(var, mapID, 0), id);
}

static void v_setrect(var_t *var, int width, int height, int id) {
  map_init(var);
  v_setint(map_add_var(var, "width", 0), width);
  v_setint(map_add_var(var, "height", 0), height);
  v_setint(map_add_var(var, mapID, 0), id);
}

static void v_setrect(var_t *var, Rectangle &rect) {
  map_init(var);
  v_setreal(map_add_var(var, "x", 0), rect.x);
  v_setreal(map_add_var(var, "y", 0), rect.y);
  v_setreal(map_add_var(var, "width", 0), rect.width);
  v_setreal(map_add_var(var, "height", 0), rect.height);
}

static void v_setvec2(var_t *var, Vector2 &vec2) {
  map_init(var);
  v_setreal(map_add_var(var, "x", 0), vec2.x);
  v_setreal(map_add_var(var, "y", 0), vec2.y);
}

static void v_setvec3(var_t *var, Vector3 &vec3) {
  map_init(var);
  v_setreal(map_add_var(var, "x", 0), vec3.x);
  v_setreal(map_add_var(var, "y", 0), vec3.y);
  v_setreal(map_add_var(var, "z", 0), vec3.z);
}

static void v_setphysics(var_t *var, PhysicsBody &physics) {
  map_init(var);
  //v_setreal(map_add_var(var, "x", 0), vec2.x);
  //v_setreal(map_add_var(var, "y", 0), vec2.y);
}

static int cmd_changedirectory(int argc, slib_par_t *params, var_t *retval) {
  auto dir = get_param_str(argc, params, 0, NULL);
  auto fnResult = ChangeDirectory(dir);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_checkcollisionboxes(int argc, slib_par_t *params, var_t *retval) {
  auto box1 = get_param_bounding_box(argc, params, 0);
  auto box2 = get_param_bounding_box(argc, params, 1);
  auto fnResult = CheckCollisionBoxes(box1, box2);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_checkcollisionboxsphere(int argc, slib_par_t *params, var_t *retval) {
  auto box = get_param_bounding_box(argc, params, 0);
  auto center = get_param_vec3(argc, params, 1);
  auto radius = get_param_num(argc, params, 2, 0);
  auto fnResult = CheckCollisionBoxSphere(box, center, radius);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_checkcollisioncirclerec(int argc, slib_par_t *params, var_t *retval) {
  auto center = get_param_vec2(argc, params, 0);
  auto radius = get_param_num(argc, params, 1, 0);
  auto rec = get_param_rect(argc, params, 2);
  auto fnResult = CheckCollisionCircleRec(center, radius, rec);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_checkcollisioncircles(int argc, slib_par_t *params, var_t *retval) {
  auto center1 = get_param_vec2(argc, params, 0);
  auto radius1 = get_param_num(argc, params, 1, 0);
  auto center2 = get_param_vec2(argc, params, 2);
  auto radius2 = get_param_num(argc, params, 3, 0);
  auto fnResult = CheckCollisionCircles(center1, radius1, center2, radius2);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_checkcollisionpointcircle(int argc, slib_par_t *params, var_t *retval) {
  auto point = get_param_vec2(argc, params, 0);
  auto center = get_param_vec2(argc, params, 1);
  auto radius = get_param_num(argc, params, 2, 0);
  auto fnResult = CheckCollisionPointCircle(point, center, radius);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_checkcollisionpointrec(int argc, slib_par_t *params, var_t *retval) {
  auto point = get_param_vec2(argc, params, 0);
  auto rec = get_param_rect(argc, params, 1);
  auto fnResult = CheckCollisionPointRec(point, rec);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_checkcollisionpointtriangle(int argc, slib_par_t *params, var_t *retval) {
  auto point = get_param_vec2(argc, params, 0);
  auto p1 = get_param_vec2(argc, params, 1);
  auto p2 = get_param_vec2(argc, params, 2);
  auto p3 = get_param_vec2(argc, params, 3);
  auto fnResult = CheckCollisionPointTriangle(point, p1, p2, p3);
  v_setint(retval, fnResult);
  return 1;
}

int cmd_checkcollisionraybox(int argc, slib_par_t *params, var_t *retval) {
  //auto ray = get_param_int(argc, params, 0, 0);
  //auto box = get_param_int(argc, params, 1, 0);
  //auto fnResult = CheckCollisionRayBox(ray, box);
  //v_setint(retval, fnResult);
  return 1;
}

int cmd_checkcollisionraysphere(int argc, slib_par_t *params, var_t *retval) {
  //auto ray = get_param_int(argc, params, 0, 0);
  //auto center = get_param_int(argc, params, 1, 0);
  //auto radius = get_param_int(argc, params, 2, 0);
  //auto fnResult = CheckCollisionRaySphere(ray, center, radius);
  //v_setint(retval, fnResult);
  return 1;
}

int cmd_checkcollisionraysphereex(int argc, slib_par_t *params, var_t *retval) {
  //auto ray = get_param_int(argc, params, 0, 0);
  //auto center = get_param_int(argc, params, 1, 0);
  //auto radius = get_param_int(argc, params, 2, 0);
  //auto collisionPoint = get_param_int(argc, params, 3, 0);
  //auto fnResult = CheckCollisionRaySphereEx(ray, center, radius, collisionPoint);
  //v_setint(retval, fnResult);
  return 1;
}

static int cmd_checkcollisionrecs(int argc, slib_par_t *params, var_t *retval) {
  auto rec1 = get_param_rect(argc, params, 0);
  auto rec2 = get_param_rect(argc, params, 1);
  auto fnResult = CheckCollisionRecs(rec1, rec2);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_checkcollisionspheres(int argc, slib_par_t *params, var_t *retval) {
  auto centerA = get_param_vec3(argc, params, 0);
  auto radiusA = get_param_num(argc, params, 1, 0);
  auto centerB = get_param_vec3(argc, params, 2);
  auto radiusB = get_param_num(argc, params, 3, 0);
  auto fnResult = CheckCollisionSpheres(centerA, radiusA, centerB, radiusB);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_coloralpha(int argc, slib_par_t *params, var_t *retval) {
  auto color = get_param_color(argc, params, 0);
  auto alpha = get_param_num(argc, params, 1, 0);
  auto fnResult = ColorAlpha(color, alpha);
  v_setcolor(retval, fnResult);
  return 1;
}

static int cmd_coloralphablend(int argc, slib_par_t *params, var_t *retval) {
  auto dst = get_param_color(argc, params, 0);
  auto src = get_param_color(argc, params, 1);
  auto tint = get_param_color(argc, params, 2);
  auto fnResult = ColorAlphaBlend(dst, src, tint);
  v_setcolor(retval, fnResult);
  return 1;
}

static int cmd_colorfromhsv(int argc, slib_par_t *params, var_t *retval) {
  auto hue = get_param_num(argc, params, 0, 0);
  auto saturation = get_param_num(argc, params, 1, 0);
  auto value = get_param_num(argc, params, 2, 0);
  auto fnResult = ColorFromHSV(hue, saturation, value);
  v_setcolor(retval, fnResult);
  return 1;
}

static int cmd_colortohsv(int argc, slib_par_t *params, var_t *retval) {
  auto color = get_param_color(argc, params, 0);
  auto fnResult = ColorToHSV(color);
  v_setvec3(retval, fnResult);
  return 1;
}

int cmd_compressdata(int argc, slib_par_t *params, var_t *retval) {
  // auto data = get_param_str(argc, params, 0, NULL);
  // auto dataLength = get_param_str(argc, params, 1, NULL);
  // auto compDataLength = get_param_str(argc, params, 2, NULL);
  // auto fnResult = CompressData(data, dataLength, compDataLength);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_decompressdata(int argc, slib_par_t *params, var_t *retval) {
  // auto compData = get_param_str(argc, params, 0, NULL);
  // auto compDataLength = get_param_str(argc, params, 1, NULL);
  // auto dataLength = get_param_str(argc, params, 2, NULL);
  // auto fnResult = DecompressData(compData, compDataLength, dataLength);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_fade(int argc, slib_par_t *params, var_t *retval) {
  auto color = get_param_color(argc, params, 0);
  auto alpha = get_param_num(argc, params, 1, 0);
  auto fnResult = Fade(color, alpha);
  v_setcolor(retval, fnResult);
  return 1;
}

static int cmd_genimagecellular(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  auto tileSize = get_param_int(argc, params, 2, 0);
  auto image = GenImageCellular(width, height, tileSize);
  auto id = ++_nextId;
  _imageMap[id] = image;
  v_setrect(retval, image.width, image.height, id);
  return 1;
}

static int cmd_genimagechecked(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  auto checksX = get_param_int(argc, params, 2, 0);
  auto checksY = get_param_int(argc, params, 3, 0);
  auto col1 = get_param_color(argc, params, 4);
  auto col2 = get_param_color(argc, params, 5);
  auto image = GenImageChecked(width, height, checksX, checksY, col1, col2);
  auto id = ++_nextId;
  _imageMap[id] = image;
  v_setrect(retval, image.width, image.height, id);
  return 1;
}

static int cmd_genimagecolor(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  auto color = get_param_color(argc, params, 2);
  auto image = GenImageColor(width, height, color);
  auto id = ++_nextId;
  _imageMap[id] = image;
  v_setrect(retval, image.width, image.height, id);
  return 1;
}

int cmd_genimagefontatlas(int argc, slib_par_t *params, var_t *retval) {
  // auto chars = get_param_str(argc, params, 0, NULL);
  // auto recs = get_param_str(argc, params, 1, NULL);
  // auto charsCount = get_param_str(argc, params, 2, NULL);
  // auto fontSize = get_param_int(argc, params, 3, NULL);
  // auto padding = get_param_str(argc, params, 4, NULL);
  // auto packMethod = get_param_str(argc, params, 5, NULL);
  // auto fnResult = GenImageFontAtlas(chars, recs, charsCount, fontSize, padding, packMethod);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_genimagegradienth(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  auto left = get_param_color(argc, params, 2);
  auto right = get_param_color(argc, params, 3);
  auto image = GenImageGradientH(width, height, left, right);
  auto id = ++_nextId;
  _imageMap[id] = image;
  v_setrect(retval, image.width, image.height, id);
  return 1;
}

static int cmd_genimagegradientradial(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  auto density = get_param_int(argc, params, 2, 0);
  auto inner = get_param_color(argc, params, 3);
  auto outer = get_param_color(argc, params, 4);
  auto image = GenImageGradientRadial(width, height, density, inner, outer);
  auto id = ++_nextId;
  _imageMap[id] = image;
  v_setrect(retval, image.width, image.height, id);
  return 1;
}

static int cmd_genimagegradientv(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  auto top = get_param_color(argc, params, 2);
  auto bottom = get_param_color(argc, params, 3);
  auto image = GenImageGradientV(width, height, top, bottom);
  auto id = ++_nextId;
  _imageMap[id] = image;
  v_setrect(retval, image.width, image.height, id);
  return 1;
}

static int cmd_genimageperlinnoise(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  auto offsetX = get_param_int(argc, params, 2, 0);
  auto offsetY = get_param_int(argc, params, 3, 0);
  auto scale = get_param_num(argc, params, 4, 0);
  auto image = GenImagePerlinNoise(width, height, offsetX, offsetY, scale);
  auto id = ++_nextId;
  _imageMap[id] = image;
  v_setrect(retval, image.width, image.height, id);
  return 1;
}

static int cmd_genimagewhitenoise(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  auto factor = get_param_int(argc, params, 2, 0);
  auto image = GenImageWhiteNoise(width, height, factor);
  auto id = ++_nextId;
  _imageMap[id] = image;
  v_setrect(retval, image.width, image.height, id);
  return 1;
}

int cmd_genmeshcube(int argc, slib_par_t *params, var_t *retval) {
  // auto width = get_param_str(argc, params, 0, NULL);
  // auto height = get_param_str(argc, params, 1, NULL);
  // auto length = get_param_str(argc, params, 2, NULL);
  // auto fnResult = GenMeshCube(width, height, length);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_genmeshcubicmap(int argc, slib_par_t *params, var_t *retval) {
  // auto cubicmap = get_param_str(argc, params, 0, NULL);
  // auto cubeSize = get_param_str(argc, params, 1, NULL);
  // auto fnResult = GenMeshCubicmap(cubicmap, cubeSize);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_genmeshcylinder(int argc, slib_par_t *params, var_t *retval) {
  // auto radius = get_param_num(argc, params, 0, NULL);
  // auto height = get_param_str(argc, params, 1, NULL);
  // auto slices = get_param_str(argc, params, 2, NULL);
  // auto fnResult = GenMeshCylinder(radius, height, slices);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_genmeshheightmap(int argc, slib_par_t *params, var_t *retval) {
  // auto heightmap = get_param_str(argc, params, 0, NULL);
  // auto size = get_param_str(argc, params, 1, NULL);
  // auto fnResult = GenMeshHeightmap(heightmap, size);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_genmeshhemisphere(int argc, slib_par_t *params, var_t *retval) {
  // auto radius = get_param_num(argc, params, 0, NULL);
  // auto rings = get_param_str(argc, params, 1, NULL);
  // auto slices = get_param_str(argc, params, 2, NULL);
  // auto fnResult = GenMeshHemiSphere(radius, rings, slices);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_genmeshknot(int argc, slib_par_t *params, var_t *retval) {
  // auto radius = get_param_num(argc, params, 0, NULL);
  // auto size = get_param_str(argc, params, 1, NULL);
  // auto radSeg = get_param_str(argc, params, 2, NULL);
  // auto sides = get_param_str(argc, params, 3, NULL);
  // auto fnResult = GenMeshKnot(radius, size, radSeg, sides);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_genmeshplane(int argc, slib_par_t *params, var_t *retval) {
  // auto width = get_param_str(argc, params, 0, NULL);
  // auto length = get_param_str(argc, params, 1, NULL);
  // auto resX = get_param_str(argc, params, 2, NULL);
  // auto resZ = get_param_str(argc, params, 3, NULL);
  // auto fnResult = GenMeshPlane(width, length, resX, resZ);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_genmeshpoly(int argc, slib_par_t *params, var_t *retval) {
  // auto sides = get_param_str(argc, params, 0, NULL);
  // auto radius = get_param_num(argc, params, 1, NULL);
  // auto fnResult = GenMeshPoly(sides, radius);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_genmeshsphere(int argc, slib_par_t *params, var_t *retval) {
  // auto radius = get_param_num(argc, params, 0, NULL);
  // auto rings = get_param_str(argc, params, 1, NULL);
  // auto slices = get_param_str(argc, params, 2, NULL);
  // auto fnResult = GenMeshSphere(radius, rings, slices);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_genmeshtorus(int argc, slib_par_t *params, var_t *retval) {
  // auto radius = get_param_num(argc, params, 0, NULL);
  // auto size = get_param_str(argc, params, 1, NULL);
  // auto radSeg = get_param_str(argc, params, 2, NULL);
  // auto sides = get_param_str(argc, params, 3, NULL);
  // auto fnResult = GenMeshTorus(radius, size, radSeg, sides);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_gentexturebrdf(int argc, slib_par_t *params, var_t *retval) {
  // auto shader = get_param_str(argc, params, 0, NULL);
  // auto size = get_param_str(argc, params, 1, NULL);
  // auto fnResult = GenTextureBRDF(shader, size);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_gentexturecubemap(int argc, slib_par_t *params, var_t *retval) {
  // auto shader = get_param_str(argc, params, 0, NULL);
  // auto panorama = get_param_str(argc, params, 1, NULL);
  // auto size = get_param_str(argc, params, 2, NULL);
  // auto format = get_param_str(argc, params, 3, NULL);
  // auto fnResult = GenTextureCubemap(shader, panorama, size, format);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_gentextureirradiance(int argc, slib_par_t *params, var_t *retval) {
  // auto shader = get_param_str(argc, params, 0, NULL);
  // auto cubemap = get_param_str(argc, params, 1, NULL);
  // auto size = get_param_str(argc, params, 2, NULL);
  // auto fnResult = GenTextureIrradiance(shader, cubemap, size);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_gentextureprefilter(int argc, slib_par_t *params, var_t *retval) {
  // auto shader = get_param_str(argc, params, 0, NULL);
  // auto cubemap = get_param_str(argc, params, 1, NULL);
  // auto size = get_param_str(argc, params, 2, NULL);
  // auto fnResult = GenTexturePrefilter(shader, cubemap, size);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_getcameramatrix(int argc, slib_par_t *params, var_t *retval) {
  // auto camera = get_camera_3d(argc, params, 0, NULL);
  // auto fnResult = GetCameraMatrix(camera);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_getcameramatrix2d(int argc, slib_par_t *params, var_t *retval) {
  // auto camera = get_camera_3d(argc, params, 0, NULL);
  // auto fnResult = GetCameraMatrix2D(camera);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_getclipboardtext(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetClipboardText();
  v_setstr(retval, fnResult);
  return 1;
}

int cmd_getcodepoints(int argc, slib_par_t *params, var_t *retval) {
  // auto text = get_param_str(argc, params, 0, NULL);
  // auto count = get_param_str(argc, params, 1, NULL);
  // auto fnResult = GetCodepoints(text, count);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_getcodepointscount(int argc, slib_par_t *params, var_t *retval) {
  // auto text = get_param_str(argc, params, 0, NULL);
  // auto fnResult = GetCodepointsCount(text);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_getcollisionrayground(int argc, slib_par_t *params, var_t *retval) {
  // auto ray = get_param_str(argc, params, 0, NULL);
  // auto groundHeight = get_param_str(argc, params, 1, NULL);
  // auto fnResult = GetCollisionRayGround(ray, groundHeight);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_getcollisionraymodel(int argc, slib_par_t *params, var_t *retval) {
  // auto ray = get_param_str(argc, params, 0, NULL);
  // auto model = get_param_str(argc, params, 1, NULL);
  // auto fnResult = GetCollisionRayModel(ray, model);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_getcollisionraytriangle(int argc, slib_par_t *params, var_t *retval) {
  // auto ray = get_param_str(argc, params, 0, NULL);
  // auto p1 = get_param_str(argc, params, 1, NULL);
  // auto p2 = get_param_str(argc, params, 2, NULL);
  // auto p3 = get_param_str(argc, params, 3, NULL);
  // auto fnResult = GetCollisionRayTriangle(ray, p1, p2, p3);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_getcollisionrec(int argc, slib_par_t *params, var_t *retval) {
  auto rec1 = get_param_rect(argc, params, 0);
  auto rec2 = get_param_rect(argc, params, 1);
  Rectangle rect = GetCollisionRec(rec1, rec2);
  v_setrect(retval, rect);
  return 1;
}

static int cmd_getcolor(int argc, slib_par_t *params, var_t *retval) {
  auto hexValue = get_param_int(argc, params, 0, 0);
  auto fnResult = GetColor(hexValue);
  v_setcolor(retval, fnResult);
  return 1;
}

int cmd_getdroppedfiles(int argc, slib_par_t *params, var_t *retval) {
  // auto count = get_param_str(argc, params, 0, NULL);
  // auto fnResult = GetDroppedFiles(count);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_getfontdefault(int argc, slib_par_t *params, var_t *retval) {
  // auto fnResult = GetFontDefault();
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_getfps(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetFPS();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_getframetime(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetFrameTime();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_getgamepadaxiscount(int argc, slib_par_t *params, var_t *retval) {
  auto gamepad = get_param_int(argc, params, 0, 0);
  auto fnResult = GetGamepadAxisCount(gamepad);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_getgamepadaxismovement(int argc, slib_par_t *params, var_t *retval) {
  auto gamepad = get_param_int(argc, params, 0, 0);
  auto axis = get_param_int(argc, params, 1, 0);
  auto fnResult = GetGamepadAxisMovement(gamepad, axis);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_getgamepadbuttonpressed(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetGamepadButtonPressed();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_getgamepadname(int argc, slib_par_t *params, var_t *retval) {
  auto gamepad = get_param_int(argc, params, 0, 0);
  auto fnResult = GetGamepadName(gamepad);
  v_setstr(retval, fnResult);
  return 1;
}

static int cmd_getgesturedetected(int argc, slib_par_t *params, var_t *retval) {
  v_setint(retval, GetGestureDetected());
  return 1;
}

static int cmd_getgesturedragangle(int argc, slib_par_t *params, var_t *retval) {
  v_setreal(retval, GetGestureDragAngle());
  return 1;
}

int cmd_getgesturedragvector(int argc, slib_par_t *params, var_t *retval) {
  // auto fnResult = GetGestureDragVector();
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_getgestureholdduration(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetGestureHoldDuration();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_getgesturepinchangle(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetGesturePinchAngle();
  v_setint(retval, fnResult);
  return 1;
}

int cmd_getgesturepinchvector(int argc, slib_par_t *params, var_t *retval) {
  // auto fnResult = GetGesturePinchVector();
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_getglyphindex(int argc, slib_par_t *params, var_t *retval) {
  // auto font = get_param_str(argc, params, 0, NULL);
  // auto codepoint = get_param_str(argc, params, 1, NULL);
  // auto fnResult = GetGlyphIndex(font, codepoint);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_getimagealphaborder(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // auto threshold = get_param_str(argc, params, 1, NULL);
  // auto fnResult = GetImageAlphaBorder(image, threshold);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_getimagedata(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_image_id(argc, params, 0, retval);
  if (id != -1) {
    auto fnResult = GetImageData(_imageMap.at(id));
    v_setint(retval, (var_int_t)fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_getimagedatanormalized(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // auto fnResult = GetImageDataNormalized(image);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_getimagepalette(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // auto maxPaletteSize = get_param_str(argc, params, 1, NULL);
  // auto extractCount = get_param_str(argc, params, 2, NULL);
  // auto fnResult = GetImagePalette(image, maxPaletteSize, extractCount);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_getkeypressed(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetKeyPressed();
  v_setint(retval, fnResult);
  return 1;
}

int cmd_getmatrixmodelview(int argc, slib_par_t *params, var_t *retval) {
  // auto fnResult = GetMatrixModelview();
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_getmatrixprojection(int argc, slib_par_t *params, var_t *retval) {
  // auto fnResult = GetMatrixProjection();
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_getmonitorcount(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetMonitorCount();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_getmonitorheight(int argc, slib_par_t *params, var_t *retval) {
  auto monitor = get_param_int(argc, params, 0, 0);
  auto fnResult = GetMonitorHeight(monitor);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_getmonitorname(int argc, slib_par_t *params, var_t *retval) {
  auto monitor = get_param_int(argc, params, 0, 0);
  auto fnResult = GetMonitorName(monitor);
  v_setstr(retval, fnResult);
  return 1;
}

static int cmd_getmonitorphysicalheight(int argc, slib_par_t *params, var_t *retval) {
  auto monitor = get_param_int(argc, params, 0, 0);
  auto fnResult = GetMonitorPhysicalHeight(monitor);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_getmonitorphysicalwidth(int argc, slib_par_t *params, var_t *retval) {
  auto monitor = get_param_int(argc, params, 0, 0);
  auto fnResult = GetMonitorPhysicalWidth(monitor);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_getmonitorrefreshrate(int argc, slib_par_t *params, var_t *retval) {
  auto monitor = get_param_int(argc, params, 0, 0);
  auto fnResult = GetMonitorRefreshRate(monitor);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_getmonitorwidth(int argc, slib_par_t *params, var_t *retval) {
  auto monitor = get_param_int(argc, params, 0, 0);
  auto fnResult = GetMonitorWidth(monitor);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_getmousecursor(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetMouseCursor();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_getmouseposition(int argc, slib_par_t *params, var_t *retval) {
  auto position = GetMousePosition();
  map_init(retval);
  v_setint(map_add_var(retval, "x", 0), position.x);
  v_setint(map_add_var(retval, "y", 0), position.y);
  return 1;
}

int cmd_getmouseray(int argc, slib_par_t *params, var_t *retval) {
  // auto mousePosition = get_param_str(argc, params, 0, NULL);
  // auto camera = get_camera_3d(argc, params, 1, NULL);
  // auto fnResult = GetMouseRay(mousePosition, camera);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_getmousewheelmove(int argc, slib_par_t *params, var_t *retval) {
  v_setreal(retval, GetMouseWheelMove());
  return 1;
}

static int cmd_getmousex(int argc, slib_par_t *params, var_t *retval) {
  v_setint(retval, GetMouseX());
  return 1;
}

static int cmd_getmousey(int argc, slib_par_t *params, var_t *retval) {
  v_setint(retval, GetMouseY());
  return 1;
}

static int cmd_getmusictimelength(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    auto fnResult = GetMusicTimeLength(music);
    v_setint(retval, fnResult);
    result = 1;
  } else {

    result = 0;
  }
  return result;
}

static int cmd_getmusictimeplayed(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    auto fnResult = GetMusicTimePlayed(music);
    v_setint(retval, fnResult);
    result = 1;
  } else {

    result = 0;
  }
  return result;
}

int cmd_getnextcodepoint(int argc, slib_par_t *params, var_t *retval) {
  // auto text = get_param_str(argc, params, 0, NULL);
  // auto bytesProcessed = get_param_str(argc, params, 1, NULL);
  // auto fnResult = GetNextCodepoint(text, bytesProcessed);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_getpixelcolor(int argc, slib_par_t *params, var_t *retval) {
  // auto srcPtr = get_param_str(argc, params, 0, NULL);
  // auto format = get_param_str(argc, params, 1, NULL);
  // auto fnResult = GetPixelColor(srcPtr, format);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_getpixeldatasize(int argc, slib_par_t *params, var_t *retval) {
  // auto width = get_param_str(argc, params, 0, NULL);
  // auto height = get_param_str(argc, params, 1, NULL);
  // auto format = get_param_str(argc, params, 2, NULL);
  // auto fnResult = GetPixelDataSize(width, height, format);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_getprevdirectorypath(int argc, slib_par_t *params, var_t *retval) {
  auto dirPath = get_param_str(argc, params, 0, NULL);
  auto fnResult = GetPrevDirectoryPath(dirPath);
  v_setstr(retval, fnResult);
  return 1;
}

static int cmd_getrandomvalue(int argc, slib_par_t *params, var_t *retval) {
  auto min = get_param_int(argc, params, 0, 0);
  auto max = get_param_int(argc, params, 1, 0);
  auto fnResult = GetRandomValue(min, max);
  v_setint(retval, fnResult);
  return 1;
}

int cmd_getscreendata(int argc, slib_par_t *params, var_t *retval) {
  // auto fnResult = GetScreenData();
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_getscreenheight(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetScreenHeight();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_getscreentoworld2d(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec2(argc, params, 0);
  auto camera = get_camera_2d(argc, params, 1);
  auto fnResult = GetScreenToWorld2D(position, camera);
  v_setvec2(retval, fnResult);
  return 1;
}

static int cmd_getscreenwidth(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetScreenWidth();
  v_setint(retval, fnResult);
  return 1;
}

int cmd_getshaderdefault(int argc, slib_par_t *params, var_t *retval) {
  // auto fnResult = GetShaderDefault();
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_getshaderlocation(int argc, slib_par_t *params, var_t *retval) {
  auto shader = get_param_shader(argc, params, 0);
  auto uniformName = get_param_str(argc, params, 1, NULL);
  auto fnResult = GetShaderLocation(shader, uniformName);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_getshaderlocationattrib(int argc, slib_par_t *params, var_t *retval) {
  auto shader = get_param_shader(argc, params, 0);
  auto attribName = get_param_str(argc, params, 1, NULL);
  auto fnResult = GetShaderLocationAttrib(shader, attribName);
  v_setint(retval, fnResult);
  return 1;
}

int cmd_getshapestexture(int argc, slib_par_t *params, var_t *retval) {
  // auto fnResult = GetShapesTexture();
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_getshapestexturerec(int argc, slib_par_t *params, var_t *retval) {
  // auto fnResult = GetShapesTextureRec();
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_getsoundsplaying(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetSoundsPlaying();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_gettexturedata(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_texture_id(argc, params, 0, retval);
  if (id != -1) {
    Image image = GetTextureData(_textureMap.at(id));
    id = ++_nextId;
    _imageMap[id] = image;
    v_setrect(retval, image.width, image.height, id);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_gettexturedefault(int argc, slib_par_t *params, var_t *retval) {
  // auto fnResult = GetTextureDefault();
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_gettime(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetTime();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_gettouchpointscount(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetTouchPointsCount();
  v_setint(retval, fnResult);
  return 1;
}

int cmd_gettouchposition(int argc, slib_par_t *params, var_t *retval) {
  // auto index = get_param_str(argc, params, 0, NULL);
  // auto fnResult = GetTouchPosition(index);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_gettouchx(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetTouchX();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_gettouchy(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetTouchY();
  v_setint(retval, fnResult);
  return 1;
}

int cmd_getwavedata(int argc, slib_par_t *params, var_t *retval) {
  // auto wave = get_param_str(argc, params, 0, NULL);
  // auto fnResult = GetWaveData(wave);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_getwindowposition(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetWindowPosition();
  v_setvec2(retval, fnResult);
  return 1;
}

int cmd_getwindowscaledpi(int argc, slib_par_t *params, var_t *retval) {
  // auto fnResult = GetWindowScaleDPI();
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_getworkingdirectory(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetWorkingDirectory();
  v_setstr(retval, fnResult);
  return 1;
}

int cmd_getworldtoscreen(int argc, slib_par_t *params, var_t *retval) {
  // auto position = get_param_vec3(argc, params, 0);
  // auto camera = get_camera_3d(argc, params, 1, NULL);
  // auto fnResult = GetWorldToScreen(position, camera);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_getworldtoscreen2d(int argc, slib_par_t *params, var_t *retval) {
  // auto position = get_param_vec3(argc, params, 0);
  // auto camera = get_camera_3d(argc, params, 1, NULL);
  // auto fnResult = GetWorldToScreen2D(position, camera);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_getworldtoscreenex(int argc, slib_par_t *params, var_t *retval) {
  // auto position = get_param_vec3(argc, params, 0);
  // auto camera = get_camera_3d(argc, params, 1, NULL);
  // auto width = get_param_str(argc, params, 2, NULL);
  // auto height = get_param_str(argc, params, 3, NULL);
  // auto fnResult = GetWorldToScreenEx(position, camera, width, height);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_imagecopy(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // auto fnResult = ImageCopy(image);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_imagefromimage(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // auto rec = get_param_str(argc, params, 1, NULL);
  // auto fnResult = ImageFromImage(image, rec);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_imagetext(int argc, slib_par_t *params, var_t *retval) {
  // auto text = get_param_str(argc, params, 0, NULL);
  // auto fontSize = get_param_int(argc, params, 1, NULL);
  // auto color = get_param_color(argc, params, 2);
  // auto fnResult = ImageText(text, fontSize, color);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_imagetextex(int argc, slib_par_t *params, var_t *retval) {
  // auto font = get_param_str(argc, params, 0, NULL);
  // auto text = get_param_str(argc, params, 1, NULL);
  // auto fontSize = get_param_int(argc, params, 2, NULL);
  // auto spacing = get_param_str(argc, params, 3, NULL);
  // auto tint = get_param_color(argc, params, 4);
  // auto fnResult = ImageTextEx(font, text, fontSize, spacing, tint);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_initaudiostream(int argc, slib_par_t *params, var_t *retval) {
  // auto sampleRate = get_param_str(argc, params, 0, NULL);
  // auto sampleSize = get_param_str(argc, params, 1, NULL);
  // auto channels = get_param_str(argc, params, 2, NULL);
  // auto fnResult = InitAudioStream(sampleRate, sampleSize, channels);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_isaudiodeviceready(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsAudioDeviceReady();
  v_setint(retval, fnResult);
  return 1;
}

int cmd_isaudiostreamplaying(int argc, slib_par_t *params, var_t *retval) {
  //auto stream = get_param_int(argc, params, 0, 0);
  //auto fnResult = IsAudioStreamPlaying(stream);
  //v_setint(retval, fnResult);
  return 1;
}

int cmd_isaudiostreamprocessed(int argc, slib_par_t *params, var_t *retval) {
  //auto stream = get_param_str(argc, params, 0, NULL);
  //auto fnResult = IsAudioStreamProcessed(stream);
  //v_setint(retval, fnResult);
  return 1;
}

static int cmd_iscursorhidden(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsCursorHidden();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_iscursoronscreen(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsCursorOnScreen();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_isfiledropped(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsFileDropped();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_isfileextension(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, NULL);
  auto ext = get_param_str(argc, params, 1, NULL);
  auto fnResult = IsFileExtension(fileName, ext);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_isgamepadavailable(int argc, slib_par_t *params, var_t *retval) {
  auto gamepad = get_param_int(argc, params, 0, 0);
  auto fnResult = IsGamepadAvailable(gamepad);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_isgamepadbuttondown(int argc, slib_par_t *params, var_t *retval) {
  auto gamepad = get_param_int(argc, params, 0, 0);
  auto button = get_param_int(argc, params, 1, 0);
  auto fnResult = IsGamepadButtonDown(gamepad, button);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_isgamepadbuttonpressed(int argc, slib_par_t *params, var_t *retval) {
  auto gamepad = get_param_int(argc, params, 0, 0);
  auto button = get_param_int(argc, params, 1, 0);
  auto fnResult = IsGamepadButtonPressed(gamepad, button);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_isgamepadbuttonreleased(int argc, slib_par_t *params, var_t *retval) {
  auto gamepad = get_param_int(argc, params, 0, 0);
  auto button = get_param_int(argc, params, 1, 0);
  auto fnResult = IsGamepadButtonReleased(gamepad, button);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_isgamepadbuttonup(int argc, slib_par_t *params, var_t *retval) {
  auto gamepad = get_param_int(argc, params, 0, 0);
  auto button = get_param_int(argc, params, 1, 0);
  auto fnResult = IsGamepadButtonUp(gamepad, button);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_isgamepadname(int argc, slib_par_t *params, var_t *retval) {
  auto gamepad = get_param_int(argc, params, 0, 0);
  auto name = get_param_str(argc, params, 1, NULL);
  auto fnResult = IsGamepadName(gamepad, name);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_isgesturedetected(int argc, slib_par_t *params, var_t *retval) {
  auto gesture = get_param_int(argc, params, 0, 0);
  auto fnResult = IsGestureDetected(gesture);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_iskeydown(int argc, slib_par_t *params, var_t *retval) {
  auto key = get_param_int(argc, params, 0, 0);
  auto fnResult = IsKeyDown(key);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_iskeypressed(int argc, slib_par_t *params, var_t *retval) {
  auto key = get_param_int(argc, params, 0, 0);
  auto fnResult = IsKeyPressed(key);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_iskeyreleased(int argc, slib_par_t *params, var_t *retval) {
  auto key = get_param_int(argc, params, 0, 0);
  auto fnResult = IsKeyReleased(key);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_iskeyup(int argc, slib_par_t *params, var_t *retval) {
  auto key = get_param_int(argc, params, 0, 0);
  auto fnResult = IsKeyUp(key);
  v_setint(retval, fnResult);
  return 1;
}

int cmd_ismodelanimationvalid(int argc, slib_par_t *params, var_t *retval) {
  //auto model = get_param_str(argc, params, 0, NULL);
  //auto anim = get_param_str(argc, params, 1, NULL);
  //auto fnResult = IsModelAnimationValid(model, anim);
  //v_setint(retval, fnResult);
  return 1;
}

static int cmd_ismousebuttondown(int argc, slib_par_t *params, var_t *retval) {
  auto button = get_param_int(argc, params, 0, 0);
  auto fnResult = IsMouseButtonDown(button);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_ismousebuttonpressed(int argc, slib_par_t *params, var_t *retval) {
  auto button = get_param_int(argc, params, 0, 0);
  auto fnResult = IsMouseButtonPressed(button);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_ismousebuttonreleased(int argc, slib_par_t *params, var_t *retval) {
  auto button = get_param_int(argc, params, 0, 0);
  auto fnResult = IsMouseButtonReleased(button);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_ismousebuttonup(int argc, slib_par_t *params, var_t *retval) {
  auto button = get_param_int(argc, params, 0, 0);
  auto fnResult = IsMouseButtonUp(button);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_ismusicplaying(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    auto fnResult = IsMusicPlaying(music);
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_issoundplaying(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    auto fnResult = IsSoundPlaying(sound);
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_isvrsimulatorready(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsVrSimulatorReady();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_iswindowfocused(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsWindowFocused();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_iswindowfullscreen(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsWindowFullscreen();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_iswindowhidden(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsWindowHidden();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_iswindowmaximized(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsWindowMaximized();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_iswindowminimized(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsWindowMinimized();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_iswindowready(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsWindowReady();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_iswindowresized(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsWindowResized();
  v_setint(retval, fnResult);
  return 1;
}

int cmd_loadfiledata(int argc, slib_par_t *params, var_t *retval) {
  // auto fileName = get_param_str(argc, params, 0, NULL);
  // auto bytesRead = get_param_str(argc, params, 1, NULL);
  // auto fnResult = LoadFileData(fileName, bytesRead);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_loadfiletext(int argc, slib_par_t *params, var_t *retval) {
  // auto fileName = get_param_str(argc, params, 0, NULL);
  // auto fnResult = LoadFileText(fileName);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_loadfont(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, NULL);
  Font font = LoadFont(fileName);
  auto id = ++_nextId;
  _fontMap[id] = font;
  v_setfont(retval, font, id);
  return 1;
}

int cmd_loadfontdata(int argc, slib_par_t *params, var_t *retval) {
  // auto fileData = get_param_str(argc, params, 0, NULL);
  // auto dataSize = get_param_str(argc, params, 1, NULL);
  // auto fontSize = get_param_int(argc, params, 2, NULL);
  // auto fontChars = get_param_str(argc, params, 3, NULL);
  // auto charsCount = get_param_str(argc, params, 4, NULL);
  // auto type = get_param_str(argc, params, 5, NULL);
  // auto fnResult = LoadFontData(fileData, dataSize, fontSize, fontChars, charsCount, type);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_loadfontex(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, NULL);
  auto fontSize = get_param_int(argc, params, 1, 0);
  // char *fontChars = 0; // get_param_str(argc, params, 2, NULL);
  auto charsCount = get_param_int(argc, params, 3, 0);
  auto font = LoadFontEx(fileName, fontSize, 0, charsCount);
  auto id = ++_nextId;
  _fontMap[id] = font;
  v_setfont(retval, font, id);
  return 1;
}

int cmd_loadfontfromimage(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // auto key = get_param_str(argc, params, 1, NULL);
  // auto firstChar = get_param_str(argc, params, 2, NULL);
  // auto fnResult = LoadFontFromImage(image, key, firstChar);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_loadfontfrommemory(int argc, slib_par_t *params, var_t *retval) {
  // auto fileType = get_param_str(argc, params, 0, NULL);
  // auto fileData = get_param_str(argc, params, 1, NULL);
  // auto dataSize = get_param_str(argc, params, 2, NULL);
  // auto fontSize = get_param_int(argc, params, 3, NULL);
  // auto fontChars = get_param_str(argc, params, 4, NULL);
  // auto charsCount = get_param_str(argc, params, 5, NULL);
  // auto fnResult = LoadFontFromMemory(fileType, fileData, dataSize, fontSize, fontChars, charsCount);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_loadimage(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, NULL);
  auto image = LoadImage(fileName);
  auto id = ++_nextId;
  _imageMap[id] = image;
  v_setrect(retval, image.width, image.height, id);
  return 1;
}

int cmd_loadimageanim(int argc, slib_par_t *params, var_t *retval) {
  // auto fileName = get_param_str(argc, params, 0, NULL);
  // auto frames = get_param_str(argc, params, 1, NULL);
  // auto fnResult = LoadImageAnim(fileName, frames);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_loadimagefrommemory(int argc, slib_par_t *params, var_t *retval) {
  // auto fileType = get_param_str(argc, params, 0, NULL);
  // auto fileData = get_param_str(argc, params, 1, NULL);
  // auto dataSize = get_param_str(argc, params, 2, NULL);
  // auto fnResult = LoadImageFromMemory(fileType, fileData, dataSize);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_loadimageraw(int argc, slib_par_t *params, var_t *retval) {
  // auto fileName = get_param_str(argc, params, 0, NULL);
  // auto width = get_param_str(argc, params, 1, NULL);
  // auto height = get_param_str(argc, params, 2, NULL);
  // auto format = get_param_str(argc, params, 3, NULL);
  // auto headerSize = get_param_str(argc, params, 4, NULL);
  // auto fnResult = LoadImageRaw(fileName, width, height, format, headerSize);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_loadmaterialdefault(int argc, slib_par_t *params, var_t *retval) {
  // auto fnResult = LoadMaterialDefault();
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_loadmaterials(int argc, slib_par_t *params, var_t *retval) {
  // auto fileName = get_param_str(argc, params, 0, NULL);
  // auto materialCount = get_param_str(argc, params, 1, NULL);
  // auto fnResult = LoadMaterials(fileName, materialCount);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_loadmeshes(int argc, slib_par_t *params, var_t *retval) {
  // auto fileName = get_param_str(argc, params, 0, NULL);
  // auto meshCount = get_param_str(argc, params, 1, NULL);
  // auto fnResult = LoadMeshes(fileName, meshCount);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_loadmodel(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, NULL);
  auto model = LoadModel(fileName);
  auto id = ++_nextId;
  _modelMap[id] = model;

  map_init(retval);
  v_setint(map_add_var(retval, "meshCount", 0), model.meshCount);
  v_setint(map_add_var(retval, "materialCount", 0), model.materialCount);
  v_setint(map_add_var(retval, "boneCount", 0), model.boneCount);
  v_setint(map_add_var(retval, mapID, 0), id);
  return 1;
}

int cmd_loadmodelanimations(int argc, slib_par_t *params, var_t *retval) {
  // auto fileName = get_param_str(argc, params, 0, NULL);
  // auto animsCount = get_param_str(argc, params, 1, NULL);
  // auto fnResult = LoadModelAnimations(fileName, animsCount);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_loadmodelfrommesh(int argc, slib_par_t *params, var_t *retval) {
  // auto mesh = get_param_str(argc, params, 0, NULL);
  // auto fnResult = LoadModelFromMesh(mesh);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_loadmusicstream(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, NULL);
  int fnResult = ++_nextId;
  _musicMap[fnResult] = LoadMusicStream(fileName);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_loadrendertexture(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  auto renderId = ++_nextId;
  auto renderTexture = LoadRenderTexture(width, height);
  _renderMap[renderId] = renderTexture;

  map_init(retval);
  map_add_var(retval, mapID, renderId);

  auto textureId = ++_nextId;
  _textureMap[textureId] = renderTexture.texture;
  var_p_t texture = map_add_var(retval, "texture", 0);
  v_setrect(texture, renderTexture.texture.width, renderTexture.texture.height, textureId);
  return 1;
}

static int cmd_loadshader(int argc, slib_par_t *params, var_t *retval) {
  auto vsFileName = get_param_str(argc, params, 0, NULL);
  auto fsFileName = get_param_str(argc, params, 1, NULL);
  Shader shader;
  int result;
  if (vsFileName[0] == '0' && vsFileName[1] == '\0') {
    shader = LoadShader(0, fsFileName);
    map_init(retval);
    v_setint(map_add_var(retval, "locs", 0), (var_int_t)shader.locs);
    v_setint(map_add_var(retval, mapID, 0), shader.id);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_loadshadercode(int argc, slib_par_t *params, var_t *retval) {
  // auto vsCode = get_param_str(argc, params, 0, NULL);
  // auto fsCode = get_param_str(argc, params, 1, NULL);
  // auto fnResult = LoadShaderCode(vsCode, fsCode);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_loadsound(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, NULL);
  int fnResult = ++_nextId;
  _soundMap[fnResult] = LoadSound(fileName);
  v_setint(retval, fnResult);
  return 1;
}

int cmd_loadsoundfromwave(int argc, slib_par_t *params, var_t *retval) {
  // auto wave = get_param_str(argc, params, 0, NULL);
  // auto fnResult = LoadSoundFromWave(wave);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_loadstoragevalue(int argc, slib_par_t *params, var_t *retval) {
  // auto position = get_param_vec3(argc, params, 0);
  // auto fnResult = LoadStorageValue(position);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_loadtexture(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, NULL);
  Texture2D texture = LoadTexture(fileName);
  int id = ++_nextId;
  _textureMap[id] = texture;
  v_setrect(retval, texture.width, texture.height, id);
  return 1;
}

int cmd_loadtexturecubemap(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // auto layoutType = get_param_str(argc, params, 1, NULL);
  // auto fnResult = LoadTextureCubemap(image, layoutType);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_loadtexturefromimage(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_image_id(argc, params, 0, retval);
  if (id != -1) {
    auto texture = LoadTextureFromImage(_imageMap.at(id));
    id = ++_nextId;
    _textureMap[id] = texture;
    v_setrect(retval, texture.width, texture.height, id);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_loadwave(int argc, slib_par_t *params, var_t *retval) {
  // auto fileName = get_param_str(argc, params, 0, NULL);
  // auto fnResult = LoadWave(fileName);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_loadwavefrommemory(int argc, slib_par_t *params, var_t *retval) {
  // auto fileType = get_param_str(argc, params, 0, NULL);
  // auto fileData = get_param_str(argc, params, 1, NULL);
  // auto dataSize = get_param_str(argc, params, 2, NULL);
  // auto fnResult = LoadWaveFromMemory(fileType, fileData, dataSize);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_measuretext(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, NULL);
  auto fontSize = get_param_int(argc, params, 1, 0);
  auto fnResult = MeasureText(text, fontSize);
  v_setint(retval, fnResult);
  return 1;
}

int cmd_measuretextex(int argc, slib_par_t *params, var_t *retval) {
  // auto font = get_param_str(argc, params, 0, NULL);
  // auto text = get_param_str(argc, params, 1, NULL);
  // auto fontSize = get_param_int(argc, params, 2, NULL);
  // auto spacing = get_param_str(argc, params, 3, NULL);
  // auto fnResult = MeasureTextEx(font, text, fontSize, spacing);
  // v_setint(retval, fnResult);
  return 1;
}

int cmd_meshboundingbox(int argc, slib_par_t *params, var_t *retval) {
  // auto mesh = get_param_str(argc, params, 0, NULL);
  // auto fnResult = MeshBoundingBox(mesh);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_textformat(int argc, slib_par_t *params, var_t *retval) {
  v_setstr(retval, format_text(argc, params, 0));
  return 1;
}

int cmd_wavecopy(int argc, slib_par_t *params, var_t *retval) {
  // auto wave = get_param_str(argc, params, 0, NULL);
  // auto fnResult = WaveCopy(wave);
  // v_setint(retval, fnResult);
  return 1;
}

static int cmd_windowshouldclose(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = WindowShouldClose();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_beginblendmode(int argc, slib_par_t *params, var_t *retval) {
  auto mode = get_param_int(argc, params, 0, 0);
  BeginBlendMode(mode);
  return 1;
}

static int cmd_begindrawing(int argc, slib_par_t *params, var_t *retval) {
  BeginDrawing();
  return 1;
}

static int cmd_beginmode2d(int argc, slib_par_t *params, var_t *retval) {
  BeginMode2D(get_camera_2d(argc, params, 0));
  return 1;
}

static int cmd_beginmode3d(int argc, slib_par_t *params, var_t *retval) {
  BeginMode3D(get_camera_3d(argc, params, 0));
  return 1;
}

static int cmd_beginscissormode(int argc, slib_par_t *params, var_t *retval) {
  auto x = get_param_int(argc, params, 0, 0);
  auto y = get_param_int(argc, params, 1, 0);
  auto width = get_param_int(argc, params, 2, 0);
  auto height = get_param_int(argc, params, 3, 0);
  BeginScissorMode(x, y, width, height);
  return 1;
}

static int cmd_beginshadermode(int argc, slib_par_t *params, var_t *retval) {
  BeginShaderMode(get_param_shader(argc, params, 0));
  return 1;
}

static int cmd_begintexturemode(int argc, slib_par_t *params, var_t *retval) {
  int id = get_render_texture_id(argc, params, 0);
  if (id != -1) {
    BeginTextureMode(_renderMap.at(id));
  }
  return 1;
}

static int cmd_beginvrdrawing(int argc, slib_par_t *params, var_t *retval) {
  BeginVrDrawing();
  return 1;
}

static int cmd_clearbackground(int argc, slib_par_t *params, var_t *retval) {
  auto color = get_param_color(argc, params, 0);
  ClearBackground(color);
  return 1;
}

static int cmd_cleardirectoryfiles(int argc, slib_par_t *params, var_t *retval) {
  ClearDirectoryFiles();
  return 1;
}

static int cmd_cleardroppedfiles(int argc, slib_par_t *params, var_t *retval) {
  ClearDroppedFiles();
  return 1;
}

static int cmd_closeaudiodevice(int argc, slib_par_t *params, var_t *retval) {
  CloseAudioDevice();
  return 1;
}

int cmd_closeaudiostream(int argc, slib_par_t *params, var_t *retval) {
  // auto stream = get_param_str(argc, params, 0, NULL);
  // CloseAudioStream(stream);
  return 1;
}

static int cmd_closevrsimulator(int argc, slib_par_t *params, var_t *retval) {
  CloseVrSimulator();
  return 1;
}

static int cmd_closewindow(int argc, slib_par_t *params, var_t *retval) {
  CloseWindow();
  return 1;
}

static int cmd_disablecursor(int argc, slib_par_t *params, var_t *retval) {
  DisableCursor();
  return 1;
}

int cmd_drawbillboard(int argc, slib_par_t *params, var_t *retval) {
  // auto camera = get_camera_3d(argc, params, 0, NULL);
  // auto texture = get_param_str(argc, params, 1, NULL);
  // auto center = get_param_str(argc, params, 2, NULL);
  // auto size = get_param_str(argc, params, 3, NULL);
  // auto tint = get_param_color(argc, params, 4);
  // DrawBillboard(camera, texture, center, size, tint);
  return 1;
}

int cmd_drawbillboardrec(int argc, slib_par_t *params, var_t *retval) {
  // auto camera = get_camera_3d(argc, params, 0, NULL);
  // auto texture = get_param_str(argc, params, 1, NULL);
  // auto sourceRec = get_param_str(argc, params, 2, NULL);
  // auto center = get_param_str(argc, params, 3, NULL);
  // auto size = get_param_str(argc, params, 4, NULL);
  // auto tint = get_param_color(argc, params, 5);
  // DrawBillboardRec(camera, texture, sourceRec, center, size, tint);
  return 1;
}

static int cmd_drawboundingbox(int argc, slib_par_t *params, var_t *retval) {
  auto box = get_param_bounding_box(argc, params, 0);
  auto color = get_param_color(argc, params, 1);
  DrawBoundingBox(box, color);
  return 1;
}

static int cmd_drawcircle(int argc, slib_par_t *params, var_t *retval) {
  auto centerX = get_param_int(argc, params, 0, 0);
  auto centerY = get_param_int(argc, params, 1, 0);
  auto radius = get_param_num(argc, params, 2, 0);
  auto color = get_param_color(argc, params, 3);
  DrawCircle(centerX, centerY, radius, color);
  return 1;
}

static int cmd_drawcircle3d(int argc, slib_par_t *params, var_t *retval) {
  auto center = get_param_vec3(argc, params, 0);
  auto radius = get_param_num(argc, params, 1, 0);
  auto rotationAxis = get_param_vec3(argc, params, 2);
  auto rotationAngle = get_param_num(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawCircle3D(center, radius, rotationAxis, rotationAngle, color);
  return 1;
}

static int cmd_drawcirclegradient(int argc, slib_par_t *params, var_t *retval) {
  auto centerX = get_param_int(argc, params, 0, 0);
  auto centerY = get_param_int(argc, params, 1, 0);
  auto radius = get_param_num(argc, params, 2, 0);
  auto color1 = get_param_color(argc, params, 3);
  auto color2 = get_param_color(argc, params, 4);
  DrawCircleGradient(centerX, centerY, radius, color1, color2);
  return 1;
}

static int cmd_drawcirclelines(int argc, slib_par_t *params, var_t *retval) {
  auto centerX = get_param_int(argc, params, 0, 0);
  auto centerY = get_param_int(argc, params, 1, 0);
  auto radius = get_param_num(argc, params, 2, 0);
  auto color = get_param_color(argc, params, 3);
  DrawCircleLines(centerX, centerY, radius, color);
  return 1;
}

static int cmd_drawcirclesector(int argc, slib_par_t *params, var_t *retval) {
  auto center = get_param_vec2(argc, params, 0);
  auto radius = get_param_num(argc, params, 1, 0);
  auto startAngle = get_param_int(argc, params, 2, 0);
  auto endAngle = get_param_int(argc, params, 3, 0);
  auto segments = get_param_int(argc, params, 4, 0);
  auto color = get_param_color(argc, params, 5);
  DrawCircleSector(center, radius, startAngle, endAngle, segments, color);
  return 1;
}

static int cmd_drawcirclesectorlines(int argc, slib_par_t *params, var_t *retval) {
  auto center = get_param_vec2(argc, params, 0);
  auto radius = get_param_num(argc, params, 1, 0);
  auto startAngle = get_param_int(argc, params, 2, 0);
  auto endAngle = get_param_int(argc, params, 3, 0);
  auto segments = get_param_int(argc, params, 4, 0);
  auto color = get_param_color(argc, params, 5);
  DrawCircleSectorLines(center, radius, startAngle, endAngle, segments, color);
  return 1;
}

static int cmd_drawcirclev(int argc, slib_par_t *params, var_t *retval) {
  auto center = get_param_vec2(argc, params, 0);
  auto radius = get_param_num(argc, params, 1, 0);
  auto color = get_param_color(argc, params, 2);
  DrawCircleV(center, radius, color);
  return 1;
}

static int cmd_drawcube(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec3(argc, params, 0);
  auto width = get_param_num(argc, params, 1, 0);
  auto height = get_param_num(argc, params, 2, 0);
  auto length = get_param_num(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawCube(position, width, height, length, color);
  return 1;
}

int cmd_drawcubetexture(int argc, slib_par_t *params, var_t *retval) {
  // auto texture = get_param_str(argc, params, 0, NULL);
  // auto position = get_param_vec3(argc, params, 1, NULL);
  // auto width = get_param_str(argc, params, 2, NULL);
  // auto height = get_param_str(argc, params, 3, NULL);
  // auto length = get_param_str(argc, params, 4, NULL);
  // auto color = get_param_color(argc, params, 5);
  // DrawCubeTexture(texture, position, width, height, length, color);
  return 1;
}

static int cmd_drawcubev(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec3(argc, params, 0);
  auto size = get_param_vec3(argc, params, 1);
  auto color = get_param_color(argc, params, 2);
  DrawCubeV(position, size, color);
  return 1;
}

static int cmd_drawcubewires(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec3(argc, params, 0);
  auto width = get_param_num(argc, params, 1, 0);
  auto height = get_param_num(argc, params, 2, 0);
  auto length = get_param_num(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawCubeWires(position, width, height, length, color);
  return 1;
}

static int cmd_drawcubewiresv(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec3(argc, params, 0);
  auto size = get_param_vec3(argc, params, 1);
  auto color = get_param_color(argc, params, 2);
  DrawCubeWiresV(position, size, color);
  return 1;
}

static int cmd_drawcylinder(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec3(argc, params, 0);
  auto radiusTop = get_param_num(argc, params, 1, 0);
  auto radiusBottom = get_param_num(argc, params, 2, 0);
  auto height = get_param_num(argc, params, 3, 0);
  auto slices = get_param_int(argc, params, 4, 0);
  auto color = get_param_color(argc, params, 5);
  DrawCylinder(position, radiusTop, radiusBottom, height, slices, color);
  return 1;
}

static int cmd_drawcylinderwires(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec3(argc, params, 0);
  auto radiusTop = get_param_num(argc, params, 1, 0);
  auto radiusBottom = get_param_num(argc, params, 2, 0);
  auto height = get_param_num(argc, params, 3, 0);
  auto slices = get_param_int(argc, params, 4, 0);
  auto color = get_param_color(argc, params, 5);
  DrawCylinderWires(position, radiusTop, radiusBottom, height, slices, color);
  return 1;
}

static int cmd_drawellipse(int argc, slib_par_t *params, var_t *retval) {
  auto centerX = get_param_int(argc, params, 0, 0);
  auto centerY = get_param_int(argc, params, 1, 0);
  auto radiusH = get_param_num(argc, params, 2, 0);
  auto radiusV = get_param_num(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawEllipse(centerX, centerY, radiusH, radiusV, color);
  return 1;
}

static int cmd_drawellipselines(int argc, slib_par_t *params, var_t *retval) {
  auto centerX = get_param_int(argc, params, 0, 0);
  auto centerY = get_param_int(argc, params, 1, 0);
  auto radiusH = get_param_num(argc, params, 2, 0);
  auto radiusV = get_param_num(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawEllipseLines(centerX, centerY, radiusH, radiusV, color);
  return 1;
}

static int cmd_drawfps(int argc, slib_par_t *params, var_t *retval) {
  auto posX = get_param_int(argc, params, 0, 0);
  auto posY = get_param_int(argc, params, 1, 0);
  DrawFPS(posX, posY);
  return 1;
}

static int cmd_drawgizmo(int argc, slib_par_t *params, var_t *retval) {
  DrawGizmo(get_param_vec3(argc, params, 0));
  return 1;
}

static int cmd_drawgrid(int argc, slib_par_t *params, var_t *retval) {
  auto slices = get_param_int(argc, params, 0, 0);
  auto spacing = get_param_num(argc, params, 1, 0);
  DrawGrid(slices, spacing);
  return 1;
}

static int cmd_drawline(int argc, slib_par_t *params, var_t *retval) {
  auto startPosX = get_param_int(argc, params, 0, 0);
  auto startPosY = get_param_int(argc, params, 1, 0);
  auto endPosX = get_param_int(argc, params, 2, 0);
  auto endPosY = get_param_int(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawLine(startPosX, startPosY, endPosX, endPosY, color);
  return 1;
}

static int cmd_drawline3d(int argc, slib_par_t *params, var_t *retval) {
  auto startPos = get_param_vec3(argc, params, 0);
  auto endPos = get_param_vec3(argc, params, 1);
  auto color = get_param_color(argc, params, 2);
  DrawLine3D(startPos, endPos, color);
  return 1;
}

static int cmd_drawlinebezier(int argc, slib_par_t *params, var_t *retval) {
  auto startPos = get_param_vec2(argc, params, 0);
  auto endPos = get_param_vec2(argc, params, 1);
  auto thick = get_param_num(argc, params, 2, 0);
  auto color = get_param_color(argc, params, 3);
  DrawLineBezier(startPos, endPos, thick, color);
  return 1;
}

static int cmd_drawlineex(int argc, slib_par_t *params, var_t *retval) {
  auto startPos = get_param_vec2(argc, params, 0);
  auto endPos = get_param_vec2(argc, params, 1);
  auto thick = get_param_num(argc, params, 2, 0);
  auto color = get_param_color(argc, params, 3);
  DrawLineEx(startPos, endPos, thick, color);
  return 1;
}

static int cmd_drawlinestrip(int argc, slib_par_t *params, var_t *retval) {
  auto points = get_param_vec2(argc, params, 0);
  auto numPoints = get_param_int(argc, params, 1, 0);
  auto color = get_param_color(argc, params, 2);
  DrawLineStrip(&points, numPoints, color);
  return 1;
}

static int cmd_drawlinev(int argc, slib_par_t *params, var_t *retval) {
  auto startPos = get_param_vec2(argc, params, 0);
  auto endPos = get_param_vec2(argc, params, 1);
  auto color = get_param_color(argc, params, 2);
  DrawLineV(startPos, endPos, color);
  return 1;
}

static int cmd_drawmodel(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_model_id(argc, params, 0, retval);
  if (id != -1) {
    auto position = get_param_vec3(argc, params, 1);
    auto scale = get_param_num(argc, params, 2, 0);
    auto tint = get_param_color(argc, params, 3);
    DrawModel(_modelMap.at(id), position, scale, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_drawmodelex(int argc, slib_par_t *params, var_t *retval) {
  // auto model = get_param_str(argc, params, 0, NULL);
  // auto position = get_param_vec3(argc, params, 1, NULL);
  // auto rotationAxis = get_param_str(argc, params, 2, NULL);
  // auto rotationAngle = get_param_str(argc, params, 3, NULL);
  // auto scale = get_param_str(argc, params, 4, NULL);
  // auto tint = get_param_color(argc, params, 5);
  // DrawModelEx(model, position, rotationAxis, rotationAngle, scale, tint);
  return 1;
}

int cmd_drawmodelwires(int argc, slib_par_t *params, var_t *retval) {
  // auto model = get_param_str(argc, params, 0, NULL);
  // auto position = get_param_vec3(argc, params, 1, NULL);
  // auto scale = get_param_str(argc, params, 2, NULL);
  // auto tint = get_param_color(argc, params, 3);
  // DrawModelWires(model, position, scale, tint);
  return 1;
}

int cmd_drawmodelwiresex(int argc, slib_par_t *params, var_t *retval) {
  // auto model = get_param_str(argc, params, 0, NULL);
  // auto position = get_param_vec3(argc, params, 1, NULL);
  // auto rotationAxis = get_param_str(argc, params, 2, NULL);
  // auto rotationAngle = get_param_str(argc, params, 3, NULL);
  // auto scale = get_param_str(argc, params, 4, NULL);
  // auto tint = get_param_color(argc, params, 5);
  // DrawModelWiresEx(model, position, rotationAxis, rotationAngle, scale, tint);
  return 1;
}

static int cmd_drawpixel(int argc, slib_par_t *params, var_t *retval) {
  auto posX = get_param_int(argc, params, 0, 0);
  auto posY = get_param_int(argc, params, 1, 0);
  auto color = get_param_color(argc, params, 2);
  DrawPixel(posX, posY, color);
  return 1;
}

static int cmd_drawpixelv(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec2(argc, params, 0);
  auto color = get_param_color(argc, params, 1);
  DrawPixelV(position, color);
  return 1;
}

static int cmd_drawplane(int argc, slib_par_t *params, var_t *retval) {
  auto centerPos = get_param_vec3(argc, params, 0);
  auto size = get_param_vec2(argc, params, 1);
  auto color = get_param_color(argc, params, 2);
  DrawPlane(centerPos, size, color);
  return 1;
}

static int cmd_drawpoint3d(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec3(argc, params, 0);
  auto color = get_param_color(argc, params, 1);
  DrawPoint3D(position, color);
  return 1;
}

static int cmd_drawpoly(int argc, slib_par_t *params, var_t *retval) {
  auto center = get_param_vec2(argc, params, 0);
  auto sides = get_param_int(argc, params, 1, 0);
  auto radius = get_param_num(argc, params, 2, 0);
  auto rotation = get_param_num(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawPoly(center, sides, radius, rotation, color);
  return 1;
}

static int cmd_drawpolylines(int argc, slib_par_t *params, var_t *retval) {
  auto center = get_param_vec2(argc, params, 0);
  auto sides = get_param_int(argc, params, 1, 0);
  auto radius = get_param_num(argc, params, 2, 0);
  auto rotation = get_param_num(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawPolyLines(center, sides, radius, rotation, color);
  return 1;
}

int cmd_drawray(int argc, slib_par_t *params, var_t *retval) {
  // auto ray = get_param_str(argc, params, 0, NULL);
  // auto color = get_param_color(argc, params, 1);
  // DrawRay(ray, color);
  return 1;
}

static int cmd_drawrectangle(int argc, slib_par_t *params, var_t *retval) {
  auto posX = get_param_int(argc, params, 0, 0);
  auto posY = get_param_int(argc, params, 1, 0);
  auto width = get_param_int(argc, params, 2, 0);
  auto height = get_param_int(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawRectangle(posX, posY, width, height, color);
  return 1;
}

static int cmd_drawrectanglegradientex(int argc, slib_par_t *params, var_t *retval) {
  auto rec = get_param_rect(argc, params, 0);
  auto col1 = get_param_color(argc, params, 1);
  auto col2 = get_param_color(argc, params, 2);
  auto col3 = get_param_color(argc, params, 3);
  auto col4 = get_param_color(argc, params, 4);
  DrawRectangleGradientEx(rec, col1, col2, col3, col4);
  return 1;
}

static int cmd_drawrectanglegradienth(int argc, slib_par_t *params, var_t *retval) {
  auto posX = get_param_int(argc, params, 0, 0);
  auto posY = get_param_int(argc, params, 1, 0);
  auto width = get_param_int(argc, params, 2, 0);
  auto height = get_param_int(argc, params, 3, 0);
  auto color1 = get_param_color(argc, params, 4);
  auto color2 = get_param_color(argc, params, 5);
  DrawRectangleGradientH(posX, posY, width, height, color1, color2);
  return 1;
}

static int cmd_drawrectanglegradientv(int argc, slib_par_t *params, var_t *retval) {
  auto posX = get_param_int(argc, params, 0, 0);
  auto posY = get_param_int(argc, params, 1, 0);
  auto width = get_param_int(argc, params, 2, 0);
  auto height = get_param_int(argc, params, 3, 0);
  auto color1 = get_param_color(argc, params, 4);
  auto color2 = get_param_color(argc, params, 5);
  DrawRectangleGradientV(posX, posY, width, height, color1, color2);
  return 1;
}

static int cmd_drawrectanglelines(int argc, slib_par_t *params, var_t *retval) {
  auto posX = get_param_int(argc, params, 0, 0);
  auto posY = get_param_int(argc, params, 1, 0);
  auto width = get_param_int(argc, params, 2, 0);
  auto height = get_param_int(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawRectangleLines(posX, posY, width, height, color);
  return 1;
}

static int cmd_drawrectanglelinesex(int argc, slib_par_t *params, var_t *retval) {
  auto rec = get_param_rect(argc, params, 0);
  auto lineThick = get_param_int(argc, params, 1, 0);
  auto color = get_param_color(argc, params, 2);
  DrawRectangleLinesEx(rec, lineThick, color);
  return 1;
}

static int cmd_drawrectanglepro(int argc, slib_par_t *params, var_t *retval) {
  auto rec = get_param_rect(argc, params, 0);
  auto origin = get_param_vec2(argc, params, 1);
  auto rotation = get_param_num(argc, params, 2, 0);
  auto color = get_param_color(argc, params, 3);
  DrawRectanglePro(rec, origin, rotation, color);
  return 1;
}

static int cmd_drawrectanglerec(int argc, slib_par_t *params, var_t *retval) {
  auto rec = get_param_rect(argc, params, 0);
  auto color = get_param_color(argc, params, 1);
  DrawRectangleRec(rec, color);
  return 1;
}

static int cmd_drawrectanglerounded(int argc, slib_par_t *params, var_t *retval) {
  auto rec = get_param_rect(argc, params, 0);
  auto roundness = get_param_num(argc, params, 1, 0);
  auto segments = get_param_int(argc, params, 2, 0);
  auto color = get_param_color(argc, params, 3);
  DrawRectangleRounded(rec, roundness, segments, color);
  return 1;
}

static int cmd_drawrectangleroundedlines(int argc, slib_par_t *params, var_t *retval) {
  auto rec = get_param_rect(argc, params, 0);
  auto roundness = get_param_num(argc, params, 1, 0);
  auto segments = get_param_int(argc, params, 2, 0);
  auto lineThick = get_param_int(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawRectangleRoundedLines(rec, roundness, segments, lineThick, color);
  return 1;
}

static int cmd_drawrectanglev(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec2(argc, params, 0);
  auto size = get_param_vec2(argc, params, 1);
  auto color = get_param_color(argc, params, 2);
  DrawRectangleV(position, size, color);
  return 1;
}

static int cmd_drawring(int argc, slib_par_t *params, var_t *retval) {
  auto center = get_param_vec2(argc, params, 0);
  auto innerRadius = get_param_num(argc, params, 1, 0);
  auto outerRadius = get_param_num(argc, params, 2, 0);
  auto startAngle = get_param_int(argc, params, 3, 0);
  auto endAngle = get_param_int(argc, params, 4, 0);
  auto segments = get_param_int(argc, params, 5, 0);
  auto color = get_param_color(argc, params, 6);
  DrawRing(center, innerRadius, outerRadius, startAngle, endAngle, segments, color);
  return 1;
}

static int cmd_drawringlines(int argc, slib_par_t *params, var_t *retval) {
  auto center = get_param_vec2(argc, params, 0);
  auto innerRadius = get_param_num(argc, params, 1, 0);
  auto outerRadius = get_param_num(argc, params, 2, 0);
  auto startAngle = get_param_int(argc, params, 3, 0);
  auto endAngle = get_param_int(argc, params, 4, 0);
  auto segments = get_param_int(argc, params, 5, 0);
  auto color = get_param_color(argc, params, 6);
  DrawRingLines(center, innerRadius, outerRadius, startAngle, endAngle, segments, color);
  return 1;
}

static int cmd_drawsphere(int argc, slib_par_t *params, var_t *retval) {
  auto centerPos = get_param_vec3(argc, params, 0);
  auto radius = get_param_num(argc, params, 1, 0);
  auto color = get_param_color(argc, params, 2);
  DrawSphere(centerPos, radius, color);
  return 1;
}

static int cmd_drawsphereex(int argc, slib_par_t *params, var_t *retval) {
  auto centerPos = get_param_vec3(argc, params, 0);
  auto radius = get_param_num(argc, params, 1, 0);
  auto rings = get_param_int(argc, params, 2, 0);
  auto slices = get_param_int(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawSphereEx(centerPos, radius, rings, slices, color);
  return 1;
}

static int cmd_drawspherewires(int argc, slib_par_t *params, var_t *retval) {
  auto centerPos = get_param_vec3(argc, params, 0);
  auto radius = get_param_num(argc, params, 1, 0);
  auto rings = get_param_int(argc, params, 2, 0);
  auto slices = get_param_int(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawSphereWires(centerPos, radius, rings, slices, color);
  return 1;
}

static int cmd_drawtext(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, NULL);
  auto posX = get_param_int(argc, params, 1, 0);
  auto posY = get_param_int(argc, params, 2, 0);
  auto fontSize = get_param_int(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawText(text, posX, posY, fontSize, color);
  return 1;
}

int cmd_drawtextcodepoint(int argc, slib_par_t *params, var_t *retval) {
  // auto font = get_param_str(argc, params, 0, NULL);
  // auto codepoint = get_param_str(argc, params, 1, NULL);
  // auto position = get_param_vec3(argc, params, 2, NULL);
  // auto scale = get_param_str(argc, params, 3, NULL);
  // auto tint = get_param_color(argc, params, 4);
  // DrawTextCodepoint(font, codepoint, position, scale, tint);
  return 1;
}

static int cmd_drawtextex(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_font_id(argc, params, 0, retval);
  if (id != -1) {
    auto text = get_param_str(argc, params, 1, NULL);
    auto position = get_param_vec2(argc, params, 2);
    auto fontSize = get_param_num(argc, params, 3, 0);
    auto spacing = get_param_num(argc, params, 4, 0);
    auto tint = get_param_color(argc, params, 5);
    DrawTextEx(_fontMap[id], text, position, fontSize, spacing, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_drawtextrec(int argc, slib_par_t *params, var_t *retval) {
  // auto font = get_param_str(argc, params, 0, NULL);
  // auto text = get_param_str(argc, params, 1, NULL);
  // auto rec = get_param_str(argc, params, 2, NULL);
  // auto fontSize = get_param_int(argc, params, 3, NULL);
  // auto spacing = get_param_str(argc, params, 4, NULL);
  // auto wordWrap = get_param_str(argc, params, 5, NULL);
  // auto tint = get_param_color(argc, params, 6);
  // DrawTextRec(font, text, rec, fontSize, spacing, wordWrap, tint);
  return 1;
}

int cmd_drawtextrecex(int argc, slib_par_t *params, var_t *retval) {
  // auto font = get_param_str(argc, params, 0, NULL);
  // auto text = get_param_str(argc, params, 1, NULL);
  // auto rec = get_param_str(argc, params, 2, NULL);
  // auto fontSize = get_param_int(argc, params, 3, NULL);
  // auto spacing = get_param_str(argc, params, 4, NULL);
  // auto wordWrap = get_param_str(argc, params, 5, NULL);
  // auto tint = get_param_color(argc, params, 6);
  // auto selectStart = get_param_str(argc, params, 7, NULL);
  // auto selectLength = get_param_str(argc, params, 8, NULL);
  // auto selectTint = get_param_str(argc, params, 9, NULL);
  // auto selectBackTint = get_param_str(argc, params, 10, NULL);
  // DrawTextRecEx(font, text, rec, fontSize, spacing, wordWrap, tint, selectStart, selectLength, selectTint, selectBackTint);
  return 1;
}

static int cmd_drawtexture(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_texture_id(argc, params, 0, retval);
  if (id != -1) {
    auto posX = get_param_int(argc, params, 1, 0);
    auto posY = get_param_int(argc, params, 2, 0);
    auto tint = get_param_color(argc, params, 3);
    DrawTexture(_textureMap.at(id), posX, posY, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_drawtextureex(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_texture_id(argc, params, 0, retval);
  if (id != -1) {
    auto position = get_param_vec2(argc, params, 1);
    auto rotation = get_param_num(argc, params, 2, 0);
    auto scale = get_param_num(argc, params, 3, 0);
    auto tint = get_param_color(argc, params, 4);
    DrawTextureEx(_textureMap.at(id), position, rotation, scale, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_drawtexturenpatch(int argc, slib_par_t *params, var_t *retval) {
  // auto texture = get_param_str(argc, params, 0, NULL);
  // auto nPatchInfo = get_param_str(argc, params, 1, NULL);
  // auto destRec = get_param_str(argc, params, 2, NULL);
  // auto origin = get_param_str(argc, params, 3, NULL);
  // auto rotation = get_param_str(argc, params, 4, NULL);
  // auto tint = get_param_color(argc, params, 5);
  // DrawTextureNPatch(texture, nPatchInfo, destRec, origin, rotation, tint);
  return 1;
}

static int cmd_drawtexturepro(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_texture_id(argc, params, 0, retval);
  if (id != -1) {
    auto sourceRec = get_param_rect(argc, params, 1);
    auto destRec = get_param_rect(argc, params, 2);
    auto origin = get_param_vec2(argc, params, 3);
    auto rotation = get_param_num(argc, params, 4, 0);
    auto tint = get_param_color(argc, params, 5);
    DrawTexturePro(_textureMap.at(id), sourceRec, destRec, origin, rotation, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_drawtexturequad(int argc, slib_par_t *params, var_t *retval) {
  // auto texture = get_param_str(argc, params, 0, NULL);
  // auto tiling = get_param_str(argc, params, 1, NULL);
  // auto offset = get_param_str(argc, params, 2, NULL);
  // auto quad = get_param_str(argc, params, 3, NULL);
  // auto tint = get_param_color(argc, params, 4);
  // DrawTextureQuad(texture, tiling, offset, quad, tint);
  return 1;
}

static int cmd_drawtexturerec(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (is_param_map(argc, params, 0)) {
    int id = map_get_int(params[0].var_p, mapID, -1);
    if (id != -1 && _textureMap.find(id) != _textureMap.end()) {
      auto sourceRec = get_param_rect(argc, params, 1);
      auto position = get_param_vec2(argc, params, 2);
      auto tint = get_param_color(argc, params, 3);
      DrawTextureRec(_textureMap.at(id), sourceRec, position, tint);
    }
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_drawtexturetiled(int argc, slib_par_t *params, var_t *retval) {
  // auto texture = get_param_str(argc, params, 0, NULL);
  // auto sourceRec = get_param_str(argc, params, 1, NULL);
  // auto destRec = get_param_str(argc, params, 2, NULL);
  // auto origin = get_param_str(argc, params, 3, NULL);
  // auto rotation = get_param_str(argc, params, 4, NULL);
  // auto scale = get_param_str(argc, params, 5, NULL);
  // auto tint = get_param_color(argc, params, 6);
  // DrawTextureTiled(texture, sourceRec, destRec, origin, rotation, scale, tint);
  return 1;
}

int cmd_drawtexturev(int argc, slib_par_t *params, var_t *retval) {
  //auto texture = get_param_str(argc, params, 0, NULL);
  //auto position = get_param_vec3(argc, params, 1, NULL);
  //auto tint = get_param_color(argc, params, 2, NULL);
  //DrawTextureV(texture, position, tint);
  return 1;
}

static int cmd_drawtriangle(int argc, slib_par_t *params, var_t *retval) {
  auto v1 = get_param_vec2(argc, params, 0);
  auto v2 = get_param_vec2(argc, params, 1);
  auto v3 = get_param_vec2(argc, params, 2);
  auto color = get_param_color(argc, params, 3);
  DrawTriangle(v1, v2, v3, color);
  return 1;
}

static int cmd_drawtriangle3d(int argc, slib_par_t *params, var_t *retval) {
  auto v1 = get_param_vec3(argc, params, 0);
  auto v2 = get_param_vec3(argc, params, 1);
  auto v3 = get_param_vec3(argc, params, 2);
  auto color = get_param_color(argc, params, 3);
  DrawTriangle3D(v1, v2, v3, color);
  return 1;
}

static int cmd_drawtrianglefan(int argc, slib_par_t *params, var_t *retval) {
  auto points = get_param_vec2(argc, params, 0);
  auto numPoints = get_param_int(argc, params, 1, 0);
  auto color = get_param_color(argc, params, 2);
  DrawTriangleFan(&points, numPoints, color);
  return 1;
}

static int cmd_drawtrianglelines(int argc, slib_par_t *params, var_t *retval) {
  auto v1 = get_param_vec2(argc, params, 0);
  auto v2 = get_param_vec2(argc, params, 1);
  auto v3 = get_param_vec2(argc, params, 2);
  auto color = get_param_color(argc, params, 3);
  DrawTriangleLines(v1, v2, v3, color);
  return 1;
}

int cmd_drawtrianglestrip(int argc, slib_par_t *params, var_t *retval) {
  // auto points = get_param_str(argc, params, 0, NULL);
  // auto pointsCount = get_param_str(argc, params, 1, NULL);
  // auto color = get_param_color(argc, params, 2);
  // DrawTriangleStrip(points, pointsCount, color);
  return 1;
}

static int cmd_drawtrianglestrip3d(int argc, slib_par_t *params, var_t *retval) {
  auto points = get_param_vec3(argc, params, 0);
  auto pointsCount = get_param_int(argc, params, 1, 0);
  auto color = get_param_color(argc, params, 2);
  DrawTriangleStrip3D(&points, pointsCount, color);
  return 1;
}

static int cmd_enablecursor(int argc, slib_par_t *params, var_t *retval) {
  EnableCursor();
  return 1;
}

static int cmd_endblendmode(int argc, slib_par_t *params, var_t *retval) {
  EndBlendMode();
  return 1;
}

static int cmd_enddrawing(int argc, slib_par_t *params, var_t *retval) {
  EndDrawing();
  return 1;
}

static int cmd_endmode2d(int argc, slib_par_t *params, var_t *retval) {
  EndMode2D();
  return 1;
}

static int cmd_endmode3d(int argc, slib_par_t *params, var_t *retval) {
  EndMode3D();
  return 1;
}

static int cmd_endscissormode(int argc, slib_par_t *params, var_t *retval) {
  EndScissorMode();
  return 1;
}

static int cmd_endshadermode(int argc, slib_par_t *params, var_t *retval) {
  EndShaderMode();
  return 1;
}

static int cmd_endtexturemode(int argc, slib_par_t *params, var_t *retval) {
  EndTextureMode();
  return 1;
}

static int cmd_endvrdrawing(int argc, slib_par_t *params, var_t *retval) {
  EndVrDrawing();
  return 1;
}

static int cmd_exportimage(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_image_id(argc, params, 0, retval);
  if (id != -1) {
    auto fileName = get_param_str(argc, params, 1, NULL);
    ExportImage(_imageMap.at(id), fileName);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_exportimageascode(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // auto fileName = get_param_str(argc, params, 1, NULL);
  // ExportImageAsCode(image, fileName);
  return 1;
}

int cmd_exportmesh(int argc, slib_par_t *params, var_t *retval) {
  // auto mesh = get_param_str(argc, params, 0, NULL);
  // auto fileName = get_param_str(argc, params, 1, NULL);
  // ExportMesh(mesh, fileName);
  return 1;
}

int cmd_exportwave(int argc, slib_par_t *params, var_t *retval) {
  // auto wave = get_param_str(argc, params, 0, NULL);
  // auto fileName = get_param_str(argc, params, 1, NULL);
  // ExportWave(wave, fileName);
  return 1;
}

int cmd_exportwaveascode(int argc, slib_par_t *params, var_t *retval) {
  // auto wave = get_param_str(argc, params, 0, NULL);
  // auto fileName = get_param_str(argc, params, 1, NULL);
  // ExportWaveAsCode(wave, fileName);
  return 1;
}

int cmd_gentexturemipmaps(int argc, slib_par_t *params, var_t *retval) {
  // auto texture = get_param_str(argc, params, 0, NULL);
  // GenTextureMipmaps(texture);
  return 1;
}

static int cmd_getwindowhandle(int argc, slib_par_t *params, var_t *retval) {
  GetWindowHandle();
  return 1;
}

static int cmd_hidecursor(int argc, slib_par_t *params, var_t *retval) {
  HideCursor();
  return 1;
}

int cmd_imagealphaclear(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // auto color = get_param_color(argc, params, 1);
  // auto threshold = get_param_str(argc, params, 2, NULL);
  // ImageAlphaClear(image, color, threshold);
  return 1;
}

int cmd_imagealphacrop(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // auto threshold = get_param_str(argc, params, 1, NULL);
  // ImageAlphaCrop(image, threshold);
  return 1;
}

int cmd_imagealphamask(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // auto alphaMask = get_param_str(argc, params, 1, NULL);
  // ImageAlphaMask(image, alphaMask);
  return 1;
}

int cmd_imagealphapremultiply(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // ImageAlphaPremultiply(&image);
  return 1;
}

int cmd_imageclearbackground(int argc, slib_par_t *params, var_t *retval) {
  // auto dst = get_param_str(argc, params, 0, NULL);
  // auto color = get_param_color(argc, params, 1);
  // ImageClearBackground(dst, color);
  return 1;
}

static int cmd_imagecolorbrightness(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_image_id(argc, params, 0, retval);
  if (id != -1) {
    auto brightness = get_param_int(argc, params, 1, 0);
    ImageColorBrightness(&_imageMap.at(id), brightness);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_imagecolorcontrast(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_image_id(argc, params, 0, retval);
  if (id != -1) {
    auto contrast = get_param_int(argc, params, 1, 0);
    ImageColorContrast(&_imageMap.at(id), contrast);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_imagecolorgrayscale(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_image_id(argc, params, 0, retval);
  if (id != -1) {
    ImageColorGrayscale(&_imageMap.at(id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_imagecolorinvert(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_image_id(argc, params, 0, retval);
  if (id != -1) {
    ImageColorInvert(&_imageMap.at(id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_imagecolorreplace(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // auto color = get_param_color(argc, params, 1);
  // auto replace = get_param_str(argc, params, 2, NULL);
  // ImageColorReplace(image, color, replace);
  return 1;
}

static int cmd_imagecolortint(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_image_id(argc, params, 0, retval);
  if (id != -1) {
    auto color = get_param_color(argc, params, 1);
    ImageColorTint(&_imageMap.at(id), color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_imagecrop(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // auto crop = get_param_str(argc, params, 1, NULL);
  // ImageCrop(&image, crop);
  return 1;
}

int cmd_imagedither(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // auto rBpp = get_param_str(argc, params, 1, NULL);
  // auto gBpp = get_param_str(argc, params, 2, NULL);
  // auto bBpp = get_param_str(argc, params, 3, NULL);
  // auto aBpp = get_param_str(argc, params, 4, NULL);
  // ImageDither(image, rBpp, gBpp, bBpp, aBpp);
  return 1;
}

int cmd_imagedraw(int argc, slib_par_t *params, var_t *retval) {
  // auto dst = get_param_str(argc, params, 0, NULL);
  // auto src = get_param_str(argc, params, 1, NULL);
  // auto srcRec = get_param_str(argc, params, 2, NULL);
  // auto dstRec = get_param_str(argc, params, 3, NULL);
  // auto tint = get_param_color(argc, params, 4);
  // ImageDraw(dst, src, srcRec, dstRec, tint);
  return 1;
}

static int cmd_imagedrawcircle(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_image_id(argc, params, 0, retval);
  if (id != -1) {
    auto centerX = get_param_int(argc, params, 1, 0);
    auto centerY = get_param_int(argc, params, 2, 0);
    auto radius = get_param_int(argc, params, 3, 0);
    auto color = get_param_color(argc, params, 4);
    ImageDrawCircle(&_imageMap.at(id), centerX, centerY, radius, color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_imagedrawcirclev(int argc, slib_par_t *params, var_t *retval) {
  // auto dst = get_param_str(argc, params, 0, NULL);
  // auto center = get_param_str(argc, params, 1, NULL);
  // auto radius = get_param_num(argc, params, 2, NULL);
  // auto color = get_param_color(argc, params, 3);
  // ImageDrawCircleV(dst, center, radius, color);
  return 1;
}

int cmd_imagedrawline(int argc, slib_par_t *params, var_t *retval) {
  // auto dst = get_param_str(argc, params, 0, NULL);
  // auto startPosX = get_param_str(argc, params, 1, NULL);
  // auto startPosY = get_param_str(argc, params, 2, NULL);
  // auto endPosX = get_param_str(argc, params, 3, NULL);
  // auto endPosY = get_param_str(argc, params, 4, NULL);
  // auto color = get_param_color(argc, params, 5);
  // ImageDrawLine(dst, startPosX, startPosY, endPosX, endPosY, color);
  return 1;
}

int cmd_imagedrawlinev(int argc, slib_par_t *params, var_t *retval) {
  // auto dst = get_param_str(argc, params, 0, NULL);
  // auto start = get_param_str(argc, params, 1, NULL);
  // auto end = get_param_str(argc, params, 2, NULL);
  // auto color = get_param_color(argc, params, 3);
  // ImageDrawLineV(dst, start, end, color);
  return 1;
}

int cmd_imagedrawpixel(int argc, slib_par_t *params, var_t *retval) {
  // auto dst = get_param_str(argc, params, 0, NULL);
  // auto posX = get_param_str(argc, params, 1, NULL);
  // auto posY = get_param_str(argc, params, 2, NULL);
  // auto color = get_param_color(argc, params, 3);
  // ImageDrawPixel(dst, posX, posY, color);
  return 1;
}

int cmd_imagedrawpixelv(int argc, slib_par_t *params, var_t *retval) {
  // auto dst = get_param_str(argc, params, 0, NULL);
  // auto position = get_param_vec3(argc, params, 1, NULL);
  // auto color = get_param_color(argc, params, 2);
  // ImageDrawPixelV(dst, position, color);
  return 1;
}

int cmd_imagedrawrectangle(int argc, slib_par_t *params, var_t *retval) {
  // auto dst = get_param_str(argc, params, 0, NULL);
  // auto posX = get_param_str(argc, params, 1, NULL);
  // auto posY = get_param_str(argc, params, 2, NULL);
  // auto width = get_param_str(argc, params, 3, NULL);
  // auto height = get_param_str(argc, params, 4, NULL);
  // auto color = get_param_color(argc, params, 5);
  // ImageDrawRectangle(dst, posX, posY, width, height, color);
  return 1;
}

int cmd_imagedrawrectanglelines(int argc, slib_par_t *params, var_t *retval) {
  // auto dst = get_param_str(argc, params, 0, NULL);
  // auto rec = get_param_str(argc, params, 1, NULL);
  // auto thick = get_param_str(argc, params, 2, NULL);
  // auto color = get_param_color(argc, params, 3);
  // ImageDrawRectangleLines(dst, rec, thick, color);
  return 1;
}

static int cmd_imagedrawrectanglerec(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_image_id(argc, params, 0, retval);
  if (id != -1) {
    auto rec = get_param_rect(argc, params, 1);
    auto color = get_param_color(argc, params, 2);
    ImageDrawRectangleRec(&_imageMap.at(id), rec, color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_imagedrawrectanglev(int argc, slib_par_t *params, var_t *retval) {
  // auto dst = get_param_str(argc, params, 0, NULL);
  // auto position = get_param_vec3(argc, params, 1, NULL);
  // auto size = get_param_str(argc, params, 2, NULL);
  // auto color = get_param_color(argc, params, 3);
  // ImageDrawRectangleV(dst, position, size, color);
  return 1;
}

int cmd_imagedrawtext(int argc, slib_par_t *params, var_t *retval) {
  // auto dst = get_param_str(argc, params, 0, NULL);
  // auto text = get_param_str(argc, params, 1, NULL);
  // auto posX = get_param_str(argc, params, 2, NULL);
  // auto posY = get_param_str(argc, params, 3, NULL);
  // auto fontSize = get_param_int(argc, params, 4, NULL);
  // auto color = get_param_color(argc, params, 5);
  // ImageDrawText(dst, text, posX, posY, fontSize, color);
  return 1;
}

int cmd_imagedrawtextex(int argc, slib_par_t *params, var_t *retval) {
  // auto dst = get_param_str(argc, params, 0, NULL);
  // auto font = get_param_str(argc, params, 1, NULL);
  // auto text = get_param_str(argc, params, 2, NULL);
  // auto position = get_param_vec3(argc, params, 3, NULL);
  // auto fontSize = get_param_int(argc, params, 4, NULL);
  // auto spacing = get_param_str(argc, params, 5, NULL);
  // auto tint = get_param_color(argc, params, 6);
  // ImageDrawTextEx(dst, font, text, position, fontSize, spacing, tint);
  return 1;
}

static int cmd_imagefliphorizontal(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_image_id(argc, params, 0, retval);
  if (id != -1) {
    ImageFlipHorizontal(&_imageMap.at(id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_imageflipvertical(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_image_id(argc, params, 0, retval);
  if (id != -1) {
    ImageFlipVertical(&_imageMap.at(id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_imageformat(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_image_id(argc, params, 0, retval);
  if (id != -1) {
    auto newFormat = get_param_int(argc, params, 1, 0);
    ImageFormat(&_imageMap.at(id), newFormat);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_imagemipmaps(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // ImageMipmaps(image);
  return 1;
}

int cmd_imageresize(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // auto newWidth = get_param_str(argc, params, 1, NULL);
  // auto newHeight = get_param_str(argc, params, 2, NULL);
  // ImageResize(image, newWidth, newHeight);
  return 1;
}

int cmd_imageresizecanvas(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // auto newWidth = get_param_str(argc, params, 1, NULL);
  // auto newHeight = get_param_str(argc, params, 2, NULL);
  // auto offsetX = get_param_str(argc, params, 3, NULL);
  // auto offsetY = get_param_str(argc, params, 4, NULL);
  // auto fill = get_param_str(argc, params, 5, NULL);
  // ImageResizeCanvas(image, newWidth, newHeight, offsetX, offsetY, fill);
  return 1;
}

int cmd_imageresizenn(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // auto newWidth = get_param_str(argc, params, 1, NULL);
  // auto newHeight = get_param_str(argc, params, 2, NULL);
  // ImageResizeNN(image, newWidth, newHeight);
  return 1;
}

int cmd_imagerotateccw(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // ImageRotateCCW(image);
  return 1;
}

int cmd_imagerotatecw(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // ImageRotateCW(image);
  return 1;
}

int cmd_imagetopot(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // auto fill = get_param_str(argc, params, 1, NULL);
  // ImageToPOT(image, fill);
  return 1;
}

static int cmd_initaudiodevice(int argc, slib_par_t *params, var_t *retval) {
  InitAudioDevice();
  return 1;
}

static int cmd_initvrsimulator(int argc, slib_par_t *params, var_t *retval) {
  InitVrSimulator();
  return 1;
}

static int cmd_initwindow(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 640);
  auto height = get_param_int(argc, params, 1, 480);
  auto title = get_param_str(argc, params, 2, NULL);
  InitWindow(width, height, title);
  return 1;
}

static int cmd_maximizewindow(int argc, slib_par_t *params, var_t *retval) {
  MaximizeWindow();
  return 1;
}

int cmd_meshbinormals(int argc, slib_par_t *params, var_t *retval) {
  // auto mesh = get_param_str(argc, params, 0, NULL);
  // MeshBinormals(mesh);
  return 1;
}

int cmd_meshnormalssmooth(int argc, slib_par_t *params, var_t *retval) {
  // auto mesh = get_param_str(argc, params, 0, NULL);
  // MeshNormalsSmooth(mesh);
  return 1;
}

int cmd_meshtangents(int argc, slib_par_t *params, var_t *retval) {
  // auto mesh = get_param_str(argc, params, 0, NULL);
  // MeshTangents(mesh);
  return 1;
}

static int cmd_openurl(int argc, slib_par_t *params, var_t *retval) {
  auto url = get_param_str(argc, params, 0, NULL);
  OpenURL(url);
  return 1;
}

int cmd_pauseaudiostream(int argc, slib_par_t *params, var_t *retval) {
  // auto stream = get_param_str(argc, params, 0, NULL);
  // PauseAudioStream(stream);
  return 1;
}

static int cmd_pausemusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    PauseMusicStream(music);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_pausesound(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    PauseSound(sound);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_playaudiostream(int argc, slib_par_t *params, var_t *retval) {
  // auto stream = get_param_str(argc, params, 0, NULL);
  // PlayAudioStream(stream);
  return 1;
}

static int cmd_playmusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    PlayMusicStream(music);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_playsound(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    PlaySound(sound);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_playsoundmulti(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    PlaySoundMulti(sound);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_restorewindow(int argc, slib_par_t *params, var_t *retval) {
  RestoreWindow();
  return 1;
}

int cmd_resumeaudiostream(int argc, slib_par_t *params, var_t *retval) {
  // auto stream = get_param_str(argc, params, 0, NULL);
  // ResumeAudioStream(stream);
  return 1;
}

static int cmd_resumemusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    ResumeMusicStream(music);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_resumesound(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    ResumeSound(sound);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_savefiledata(int argc, slib_par_t *params, var_t *retval) {
  // auto fileName = get_param_str(argc, params, 0, NULL);
  // auto data = get_param_str(argc, params, 1, NULL);
  // auto bytesToWrite = get_param_str(argc, params, 2, NULL);
  // SaveFileData(fileName, data, bytesToWrite);
  return 1;
}

int cmd_savefiletext(int argc, slib_par_t *params, var_t *retval) {
  // auto fileName = get_param_str(argc, params, 0, NULL);
  // auto text = get_param_str(argc, params, 1, NULL);
  // SaveFileText(fileName, text);
  return 1;
}

int cmd_savestoragevalue(int argc, slib_par_t *params, var_t *retval) {
  // auto position = get_param_vec3(argc, params, 0);
  // auto value = get_param_num(argc, params, 1, 0);
  // SaveStorageValue(position, value);
  return 1;
}

int cmd_setaudiostreambuffersizedefault(int argc, slib_par_t *params, var_t *retval) {
  // auto size = get_param_str(argc, params, 0, NULL);
  // SetAudioStreamBufferSizeDefault(size);
  return 1;
}

int cmd_setaudiostreampitch(int argc, slib_par_t *params, var_t *retval) {
  // auto stream = get_param_str(argc, params, 0, NULL);
  // auto pitch = get_param_str(argc, params, 1, NULL);
  // SetAudioStreamPitch(stream, pitch);
  return 1;
}

int cmd_setaudiostreamvolume(int argc, slib_par_t *params, var_t *retval) {
  // auto stream = get_param_str(argc, params, 0, NULL);
  // auto volume = get_param_str(argc, params, 1, NULL);
  // SetAudioStreamVolume(stream, volume);
  return 1;
}

int cmd_setcameraaltcontrol(int argc, slib_par_t *params, var_t *retval) {
  // auto altKey = get_param_str(argc, params, 0, NULL);
  // SetCameraAltControl(altKey);
  return 1;
}

static int cmd_setcameramode(int argc, slib_par_t *params, var_t *retval) {
  auto mode = get_param_int(argc, params, 1, 0);
  SetCameraMode(get_camera_3d(argc, params, 0), mode);
  return 1;
}

int cmd_setcameramovecontrols(int argc, slib_par_t *params, var_t *retval) {
  // auto frontKey = get_param_str(argc, params, 0, NULL);
  // auto backKey = get_param_str(argc, params, 1, NULL);
  // auto rightKey = get_param_str(argc, params, 2, NULL);
  // auto leftKey = get_param_str(argc, params, 3, NULL);
  // auto upKey = get_param_str(argc, params, 4, NULL);
  // auto downKey = get_param_str(argc, params, 5, NULL);
  // SetCameraMoveControls(frontKey, backKey, rightKey, leftKey, upKey, downKey);
  return 1;
}

int cmd_setcamerapancontrol(int argc, slib_par_t *params, var_t *retval) {
  // auto panKey = get_param_str(argc, params, 0, NULL);
  // SetCameraPanControl(panKey);
  return 1;
}

int cmd_setcamerasmoothzoomcontrol(int argc, slib_par_t *params, var_t *retval) {
  // auto szKey = get_param_str(argc, params, 0, NULL);
  // SetCameraSmoothZoomControl(szKey);
  return 1;
}

static int cmd_setclipboardtext(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, NULL);
  SetClipboardText(text);
  return 1;
}

static int cmd_setconfigflags(int argc, slib_par_t *params, var_t *retval) {
  auto flags = get_param_int(argc, params, 0, 0);
  SetConfigFlags(flags);
  return 1;
}

static int cmd_setexitkey(int argc, slib_par_t *params, var_t *retval) {
  auto key = get_param_int(argc, params, 0, 0);
  SetExitKey(key);
  return 1;
}

int cmd_setgesturesenabled(int argc, slib_par_t *params, var_t *retval) {
  // auto gestureFlags = get_param_str(argc, params, 0, NULL);
  // SetGesturesEnabled(gestureFlags);
  return 1;
}

static int cmd_setmastervolume(int argc, slib_par_t *params, var_t *retval) {
  auto volume = get_param_num(argc, params, 0, 0);
  SetMasterVolume(volume);
  return 1;
}

int cmd_setmaterialtexture(int argc, slib_par_t *params, var_t *retval) {
  // auto material = get_param_str(argc, params, 0, NULL);
  // auto mapType = get_param_str(argc, params, 1, NULL);
  // auto texture = get_param_str(argc, params, 2, NULL);
  // SetMaterialTexture(material, mapType, texture);
  return 1;
}

int cmd_setmatrixmodelview(int argc, slib_par_t *params, var_t *retval) {
  // auto view = get_param_str(argc, params, 0, NULL);
  // SetMatrixModelview(view);
  return 1;
}

int cmd_setmatrixprojection(int argc, slib_par_t *params, var_t *retval) {
  // auto proj = get_param_str(argc, params, 0, NULL);
  // SetMatrixProjection(proj);
  return 1;
}

static int cmd_setmodeldiffusetexture(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int modelId = get_model_id(argc, params, 0, retval);
  int textureId = get_texture_id(argc, params, 1, retval);
  if (modelId != -1 && textureId != -1) {
    _modelMap.at(modelId).materials[0].maps[MAP_DIFFUSE].texture = _textureMap.at(textureId);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_setmodelmeshmaterial(int argc, slib_par_t *params, var_t *retval) {
  // auto model = get_param_str(argc, params, 0, NULL);
  // auto meshId = get_param_str(argc, params, 1, NULL);
  // auto materialId = get_param_str(argc, params, 2, NULL);
  // SetModelMeshMaterial(model, meshId, materialId);
  return 1;
}

static int cmd_setmousecursor(int argc, slib_par_t *params, var_t *retval) {
  auto cursor = get_param_int(argc, params, 0, 0);
  SetMouseCursor(cursor);
  return 1;
}

static int cmd_setmouseoffset(int argc, slib_par_t *params, var_t *retval) {
  auto offsetX = get_param_int(argc, params, 0, 0);
  auto offsetY = get_param_int(argc, params, 1, 0);
  SetMouseOffset(offsetX, offsetY);
  return 1;
}

static int cmd_setmouseposition(int argc, slib_par_t *params, var_t *retval) {
  auto x = get_param_int(argc, params, 0, 0);
  auto y = get_param_int(argc, params, 1, 0);
  SetMousePosition(x, y);
  return 1;
}

static int cmd_setmousescale(int argc, slib_par_t *params, var_t *retval) {
  auto scaleX = get_param_num(argc, params, 0, 0);
  auto scaleY = get_param_num(argc, params, 1, 0);
  SetMouseScale(scaleX, scaleY);
  return 1;
}

static int cmd_setmusicpitch(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    auto pitch = get_param_int(argc, params, 1, 0);
    SetMusicPitch(music, pitch);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_setmusicvolume(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    auto volume = get_param_int(argc, params, 1, 0);
    SetMusicVolume(music, volume);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_setpixelcolor(int argc, slib_par_t *params, var_t *retval) {
  // auto dstPtr = get_param_str(argc, params, 0, NULL);
  // auto color = get_param_color(argc, params, 1);
  // auto format = get_param_str(argc, params, 2, NULL);
  // SetPixelColor(dstPtr, color, format);
  return 1;
}

int cmd_setshadervaluematrix(int argc, slib_par_t *params, var_t *retval) {
  // auto shader = get_param_shader(argc, params, 0);
  // auto uniformLoc = get_param_str(argc, params, 1, NULL);
  // auto mat = get_param_str(argc, params, 2, NULL);
  // SetShaderValueMatrix(shader, uniformLoc, mat);
  return 1;
}

int cmd_setshadervaluetexture(int argc, slib_par_t *params, var_t *retval) {
  // auto shader = get_param_shader(argc, params, 0);
  // auto uniformLoc = get_param_str(argc, params, 1, NULL);
  // auto texture = get_param_str(argc, params, 2, NULL);
  // SetShaderValueTexture(shader, uniformLoc, texture);
  return 1;
}

static int cmd_setshadervaluev(int argc, slib_par_t *params, var_t *retval) {
  auto shader = get_param_shader(argc, params, 0);
  auto uniformLoc = get_param_int(argc, params, 1, 0);
  auto uniformType = get_param_int(argc, params, 3, UNIFORM_FLOAT);
  auto count = get_param_int(argc, params, 4, 1);
  Vector3 vec3;
  Vector2 vec2;
  float num;
  switch (uniformType) {
  case UNIFORM_FLOAT:
    num = get_param_num(argc, params, 2, 0);
    SetShaderValueV(shader, uniformLoc, &num, uniformType, count);
    break;
  case UNIFORM_VEC2:
    vec2 = get_param_vec2(argc, params, 2);
    SetShaderValueV(shader, uniformLoc, &vec2, uniformType, count);
    break;
  case UNIFORM_VEC3:
    vec3 = get_param_vec3(argc, params, 2);
    SetShaderValueV(shader, uniformLoc, &vec3, uniformType, count);
    break;
  default:
    error(retval, "Uniform type not implemented");
    break;
  }
  return 1;
}

int cmd_setshapestexture(int argc, slib_par_t *params, var_t *retval) {
  // auto texture = get_param_str(argc, params, 0, NULL);
  // auto source = get_param_str(argc, params, 1, NULL);
  // SetShapesTexture(texture, source);
  return 1;
}

static int cmd_setsoundpitch(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    auto pitch = get_param_int(argc, params, 1, 0);
    SetSoundPitch(sound, pitch);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_setsoundvolume(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    auto volume = get_param_int(argc, params, 1, 0);
    SetSoundVolume(sound, volume);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_settargetfps(int argc, slib_par_t *params, var_t *retval) {
  auto fps = get_param_int(argc, params, 0, 50);
  SetTargetFPS(fps);
  return 1;
}

int cmd_settexturefilter(int argc, slib_par_t *params, var_t *retval) {
  // auto texture = get_param_str(argc, params, 0, NULL);
  // auto filterMode = get_param_str(argc, params, 1, NULL);
  // SetTextureFilter(texture, filterMode);
  return 1;
}

int cmd_settexturewrap(int argc, slib_par_t *params, var_t *retval) {
  // auto texture = get_param_str(argc, params, 0, NULL);
  // auto wrapMode = get_param_str(argc, params, 1, NULL);
  // SetTextureWrap(texture, wrapMode);
  return 1;
}

int cmd_settracelogcallback(int argc, slib_par_t *params, var_t *retval) {
  // auto callback = get_param_str(argc, params, 0, NULL);
  // SetTraceLogCallback(callback);
  return 1;
}

static int cmd_settracelogexit(int argc, slib_par_t *params, var_t *retval) {
  auto logType = get_param_int(argc, params, 0, 0);
  SetTraceLogExit(logType);
  return 1;
}

static int cmd_settraceloglevel(int argc, slib_par_t *params, var_t *retval) {
  auto logType = get_param_int(argc, params, 0, 0);
  SetTraceLogLevel(logType);
  return 1;
}

int cmd_setvrconfiguration(int argc, slib_par_t *params, var_t *retval) {
  // auto info = get_param_str(argc, params, 0, NULL);
  // auto distortion = get_param_str(argc, params, 1, NULL);
  // SetVrConfiguration(info, distortion);
  return 1;
}

int cmd_setwindowicon(int argc, slib_par_t *params, var_t *retval) {
  // auto image = get_param_str(argc, params, 0, NULL);
  // SetWindowIcon(image);
  return 1;
}

static int cmd_setwindowminsize(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  SetWindowMinSize(width, height);
  return 1;
}

static int cmd_setwindowmonitor(int argc, slib_par_t *params, var_t *retval) {
  auto monitor = get_param_int(argc, params, 0, 0);
  SetWindowMonitor(monitor);
  return 1;
}

static int cmd_setwindowposition(int argc, slib_par_t *params, var_t *retval) {
  auto x = get_param_int(argc, params, 0, 0);
  auto y = get_param_int(argc, params, 1, 0);
  SetWindowPosition(x, y);
  return 1;
}

static int cmd_setwindowsize(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 640);
  auto height = get_param_int(argc, params, 1, 480);
  SetWindowSize(width, height);
  return 1;
}

static int cmd_setwindowtitle(int argc, slib_par_t *params, var_t *retval) {
  auto title = get_param_str(argc, params, 0, NULL);
  SetWindowTitle(title);
  return 1;
}

static int cmd_showcursor(int argc, slib_par_t *params, var_t *retval) {
  ShowCursor();
  return 1;
}

int cmd_stopaudiostream(int argc, slib_par_t *params, var_t *retval) {
  // auto stream = get_param_str(argc, params, 0, NULL);
  // StopAudioStream(stream);
  return 1;
}

static int cmd_stopmusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    StopMusicStream(music);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_stopsound(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    StopSound(sound);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_stopsoundmulti(int argc, slib_par_t *params, var_t *retval) {
  StopSoundMulti();
  return 1;
}

static int cmd_takescreenshot(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, NULL);
  TakeScreenshot(fileName);
  return 1;
}

static int cmd_togglefullscreen(int argc, slib_par_t *params, var_t *retval) {
  ToggleFullscreen();
  return 1;
}

static int cmd_togglevrmode(int argc, slib_par_t *params, var_t *retval) {
  ToggleVrMode();
  return 1;
}

static int cmd_tracelog(int argc, slib_par_t *params, var_t *retval) {
  auto logType = get_param_int(argc, params, 0, 0);
  TraceLog(logType, format_text(argc, params, 1));
  return 1;
}

static int cmd_unloadfont(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_font_id(argc, params, 0, retval);
  if (id != -1) {
    UnloadFont(_fontMap.at(id));
    _fontMap.erase(id);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_unloadimage(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_image_id(argc, params, 0, retval);
  if (id != -1) {
    UnloadImage(_imageMap.at(id));
    _imageMap.erase(id);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_unloadmaterial(int argc, slib_par_t *params, var_t *retval) {
  // auto material = get_param_str(argc, params, 0, NULL);
  // UnloadMaterial(material);
  return 1;
}

int cmd_unloadmesh(int argc, slib_par_t *params, var_t *retval) {
  // auto mesh = get_param_str(argc, params, 0, NULL);
  // UnloadMesh(mesh);
  return 1;
}

static int cmd_unloadmodel(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_model_id(argc, params, 0, retval);
  if (id != -1) {
    UnloadModel(_modelMap.at(id));
    _modelMap.erase(id);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_unloadmodelanimation(int argc, slib_par_t *params, var_t *retval) {
  // auto anim = get_param_str(argc, params, 0, NULL);
  // UnloadModelAnimation(anim);
  return 1;
}

static int cmd_unloadmusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto id = get_param_int(argc, params, 0, 0);
    auto music = _musicMap.at(id);
    UnloadMusicStream(music);
    _musicMap.erase(id);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_unloadrendertexture(int argc, slib_par_t *params, var_t *retval) {
  int id = get_render_texture_id(argc, params, 0);
  if (id != -1) {
    UnloadRenderTexture(_renderMap.at(id));
    _renderMap.erase(id);
  }
  return 1;
}

static int cmd_unloadshader(int argc, slib_par_t *params, var_t *retval) {
  UnloadShader(get_param_shader(argc, params, 0));
  return 1;
}

static int cmd_unloadsound(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto id = get_param_int(argc, params, 0, 0);
    auto sound = _soundMap.at(id);
    UnloadSound(sound);
    _soundMap.erase(id);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_unloadtexture(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (is_param_map(argc, params, 0)) {
    int id = map_get_int(params[0].var_p, mapID, -1);
    if (id != -1 && _textureMap.find(id) != _textureMap.end()) {
      UnloadTexture(_textureMap.at(id));
      _textureMap.erase(id);
      result = 1;
    } else {
      result = 0;
    }
  } else {
    result = 0;
  }
  return result;
}

int cmd_unloadwave(int argc, slib_par_t *params, var_t *retval) {
  // auto wave = get_param_str(argc, params, 0, NULL);
  // UnloadWave(wave);
  return 1;
}

int cmd_updateaudiostream(int argc, slib_par_t *params, var_t *retval) {
  // auto stream = get_param_str(argc, params, 0, NULL);
  // auto data = get_param_str(argc, params, 1, NULL);
  // auto samplesCount = get_param_str(argc, params, 2, NULL);
  // UpdateAudioStream(stream, data, samplesCount);
  return 1;
}

static int cmd_updatecamera(int argc, slib_par_t *params, var_t *retval) {
  auto camera = get_camera_3d(argc, params, 0);
  UpdateCamera(&camera);
  set_camera_3d(params[0].var_p, &camera);
  return 1;
}

int cmd_updatemodelanimation(int argc, slib_par_t *params, var_t *retval) {
  // auto model = get_param_str(argc, params, 0, NULL);
  // auto anim = get_param_str(argc, params, 1, NULL);
  // auto frame = get_param_str(argc, params, 2, NULL);
  // UpdateModelAnimation(model, anim, frame);
  return 1;
}

static int cmd_updatemusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    UpdateMusicStream(music);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_updatesound(int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (_soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    //auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    //auto data = get_param_int(argc, params, 1, 0);
    //auto samplesCount = get_param_int(argc, params, 2, 0);
    //UpdateSound(sound, data, samplesCount);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_updatetexture(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_texture_id(argc, params, 0, retval);
  if (id != -1 && params[1].var_p->type == V_INT) {
    Color *pixels = (Color *)params[1].var_p->v.i;
    UpdateTexture(_textureMap.at(id), pixels);

    // cleanup for rl.GetImageData(img)
    free(pixels);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int cmd_updatetexturerec(int argc, slib_par_t *params, var_t *retval) {
  // auto texture = get_param_str(argc, params, 0, NULL);
  // auto rec = get_param_str(argc, params, 1, NULL);
  // auto pixels = get_param_str(argc, params, 2, NULL);
  // UpdateTextureRec(texture, rec, pixels);
  return 1;
}

int cmd_updatevrtracking(int argc, slib_par_t *params, var_t *retval) {
  // auto camera = get_camera_3d(argc, params, 0, NULL);
  // UpdateVrTracking(camera);
  return 1;
}

int cmd_wavecrop(int argc, slib_par_t *params, var_t *retval) {
  // auto wave = get_param_str(argc, params, 0, NULL);
  // auto initSample = get_param_str(argc, params, 1, NULL);
  // auto finalSample = get_param_str(argc, params, 2, NULL);
  // WaveCrop(wave, initSample, finalSample);
  return 1;
}

int cmd_waveformat(int argc, slib_par_t *params, var_t *retval) {
  // auto wave = get_param_str(argc, params, 0, NULL);
  // auto sampleRate = get_param_str(argc, params, 1, NULL);
  // auto sampleSize = get_param_str(argc, params, 2, NULL);
  // auto channels = get_param_str(argc, params, 3, NULL);
  // WaveFormat(wave, sampleRate, sampleSize, channels);
  return 1;
}

static int cmd_guibutton(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto text = get_param_str(argc, params, 1, 0);
  auto fnResult = GuiButton(bounds, text);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_guicheckbox(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto text = get_param_str(argc, params, 1, 0);
  auto checked = get_param_int(argc, params, 2, 0);
  auto fnResult = GuiCheckBox(bounds, text, checked);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_guicolorbaralpha(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto alpha = get_param_num(argc, params, 1, 0);
  auto fnResult = GuiColorBarAlpha(bounds, alpha);
  v_setreal(retval, fnResult);
  return 1;
}

static int cmd_guicolorbarhue(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto value = get_param_num(argc, params, 1, 0);
  auto fnResult = GuiColorBarHue(bounds, value);
  v_setreal(retval, fnResult);
  return 1;
}

static int cmd_guicolorpicker(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto color = get_param_color(argc, params, 1);
  auto fnResult = GuiColorPicker(bounds, color);
  v_setcolor(retval, fnResult);
  return 1;
}

static int cmd_guicombobox(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto text = get_param_str(argc, params, 1, 0);
  auto active = get_param_int(argc, params, 2, 0);
  auto fnResult = GuiComboBox(bounds, text, active);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_guidropdownbox(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto text = get_param_str(argc, params, 1, 0);
  auto active = get_param_int(argc, params, 2, 0);
  bool editMode = get_param_int(argc, params, 3, 0) == 1;
  auto fnResult = GuiDropdownBox(bounds, text, &active, editMode);
  v_setint(retval, fnResult);
  return set_param_int(argc, params, 2, active, retval);
}

static int cmd_guigetstyle(int argc, slib_par_t *params, var_t *retval) {
  auto control = get_param_int(argc, params, 0, 0);
  auto property = get_param_int(argc, params, 1, 0);
  auto fnResult = GuiGetStyle(control, property);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_guigrid(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto spacing = get_param_num(argc, params, 1, 0);
  auto subdivs = get_param_int(argc, params, 2, 0);
  auto fnResult = GuiGrid(bounds, spacing, subdivs);
  v_setvec2(retval, fnResult);
  return 1;
}

static int cmd_guiimagebutton(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_texture_id(argc, params, 2, retval);
  if (id != -1) {
    auto bounds = get_param_rect(argc, params, 0);
    auto text = get_param_str(argc, params, 1, 0);
    auto fnResult = GuiImageButton(bounds, text, _textureMap.at(id));
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_guiimagebuttonex(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_texture_id(argc, params, 2, retval);
  if (id != -1) {
    auto bounds = get_param_rect(argc, params, 0);
    auto text = get_param_str(argc, params, 1, 0);
    auto texSource = get_param_rect(argc, params, 3);
    auto fnResult = GuiImageButtonEx(bounds, text, _textureMap.at(id), texSource);
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_guilabelbutton(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto text = get_param_str(argc, params, 1, 0);
  auto fnResult = GuiLabelButton(bounds, text);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_guilistview(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto text = get_param_str(argc, params, 1, 0);
  auto scrollIndex = get_param_int(argc, params, 2, 0);
  bool active = get_param_int(argc, params, 3, 0) == 1;
  auto fnResult = GuiListView(bounds, text, &scrollIndex, active);
  v_setint(retval, fnResult);
  return set_param_int(argc, params, 2, scrollIndex, retval);
}

static int cmd_guilistviewex(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto text = get_param_str(argc, params, 1, 0);
  auto count = get_param_int(argc, params, 2, 0);
  auto focus = get_param_int(argc, params, 3, 0);
  auto scrollIndex = get_param_int(argc, params, 4, 0);
  bool active = get_param_int(argc, params, 5, 0) == 1;
  auto fnResult = GuiListViewEx(bounds, &text, count, &focus, &scrollIndex, active);
  v_setint(retval, fnResult);
  return set_param_int(argc, params, 3, focus, retval) && set_param_int(argc, params, 4, scrollIndex, retval);
}

static int cmd_guimessagebox(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto title = get_param_str(argc, params, 1, 0);
  auto message = get_param_str(argc, params, 2, NULL);
  auto buttons = get_param_str(argc, params, 3, 0);
  auto fnResult = GuiMessageBox(bounds, title, message, buttons);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_guiprogressbar(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto textLeft = get_param_str(argc, params, 1, 0);
  auto textRight = get_param_str(argc, params, 2, 0);
  auto value = get_param_num(argc, params, 3, 0);
  auto minValue = get_param_num(argc, params, 4, 0);
  auto maxValue = get_param_num(argc, params, 5, 0);
  auto fnResult = GuiProgressBar(bounds, textLeft, textRight, value, minValue, maxValue);
  v_setreal(retval, fnResult);
  return 1;
}

static int cmd_guiscrollbar(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto value = get_param_num(argc, params, 1, 0);
  auto minValue = get_param_num(argc, params, 2, 0);
  auto maxValue = get_param_num(argc, params, 3, 0);
  auto fnResult = GuiScrollBar(bounds, value, minValue, maxValue);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_guiscrollpanel(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto content = get_param_rect(argc, params, 1);
  auto scroll = get_param_vec2(argc, params, 2);
  auto fnResult = GuiScrollPanel(bounds, content, &scroll);
  v_setrect(retval, fnResult);
  return 1;
}

static int cmd_guislider(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto textLeft = get_param_str(argc, params, 1, 0);
  auto textRight = get_param_str(argc, params, 2, 0);
  auto value = get_param_num(argc, params, 3, 0);
  auto minValue = get_param_num(argc, params, 4, 0);
  auto maxValue = get_param_num(argc, params, 5, 0);
  auto fnResult = GuiSlider(bounds, textLeft, textRight, value, minValue, maxValue);
  v_setreal(retval, fnResult);
  return 1;
}

static int cmd_guisliderbar(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto textLeft = get_param_str(argc, params, 1, 0);
  auto textRight = get_param_str(argc, params, 2, 0);
  auto value = get_param_num(argc, params, 3, 0);
  auto minValue = get_param_num(argc, params, 4, 0);
  auto maxValue = get_param_num(argc, params, 5, 0);
  auto fnResult = GuiSliderBar(bounds, textLeft, textRight, value, minValue, maxValue);
  v_setreal(retval, fnResult);
  return 1;
}

static int cmd_guispinner(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto text = get_param_str(argc, params, 1, 0);
  auto value = get_param_int(argc, params, 2, 0);
  auto minValue = get_param_num(argc, params, 3, 0);
  auto maxValue = get_param_num(argc, params, 4, 0);
  auto editMode = get_param_int(argc, params, 5, 0);
  auto fnResult = GuiSpinner(bounds, text, &value, minValue, maxValue, editMode);
  v_setint(retval, fnResult);
  return set_param_int(argc, params, 2, value, retval);
}

static int cmd_guitextbox(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto text = get_param_str(argc, params, 1, 0);
  auto textSize = get_param_int(argc, params, 2, 0);
  auto editMode = get_param_int(argc, params, 3, 0);
  auto fnResult = GuiTextBox(bounds, (char *)text, textSize, editMode);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_guitextboxmulti(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto text = get_param_str(argc, params, 1, 0);
  auto textSize = get_param_int(argc, params, 2, 0);
  auto editMode = get_param_int(argc, params, 3, 0);
  auto fnResult = GuiTextBoxMulti(bounds, (char *)text, textSize, editMode);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_guitextinputbox(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto title = get_param_str(argc, params, 1, 0);
  auto message = get_param_str(argc, params, 2, NULL);
  auto buttons = get_param_str(argc, params, 3, 0);
  auto text = get_param_str(argc, params, 4, 0);
  auto fnResult = GuiTextInputBox(bounds, title, message, buttons, (char *)text);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_guitoggle(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto text = get_param_str(argc, params, 1, 0);
  bool active = get_param_int(argc, params, 2, 0) == 1;
  auto fnResult = GuiToggle(bounds, text, active);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_guitogglegroup(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto text = get_param_str(argc, params, 1, 0);
  bool active = get_param_int(argc, params, 2, 0) == 1;
  auto fnResult = GuiToggleGroup(bounds, text, active);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_guivaluebox(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto text = get_param_str(argc, params, 1, 0);
  auto value = get_param_int(argc, params, 2, 0);
  auto minValue = get_param_int(argc, params, 3, 0);
  auto maxValue = get_param_int(argc, params, 4, 0);
  bool editMode = get_param_int(argc, params, 5, 0) == 1;
  auto fnResult = GuiValueBox(bounds, text, &value, minValue, maxValue, editMode);
  v_setint(retval, fnResult);
  return set_param_int(argc, params, 2, value, retval);
}

static int cmd_guiwindowbox(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto title = get_param_str(argc, params, 1, NULL);
  auto fnResult = GuiWindowBox(bounds, title);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_guicleartooltip(int argc, slib_par_t *params, var_t *retval) {
  GuiClearTooltip();
  return 1;
}

static int cmd_guidisable(int argc, slib_par_t *params, var_t *retval) {
  GuiDisable();
  return 1;
}

static int cmd_guidisabletooltip(int argc, slib_par_t *params, var_t *retval) {
  GuiDisableTooltip();
  return 1;
}

static int cmd_guidummyrec(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto text = get_param_str(argc, params, 1, 0);
  GuiDummyRec(bounds, text);
  return 1;
}

static int cmd_guienable(int argc, slib_par_t *params, var_t *retval) {
  GuiEnable();
  return 1;
}

static int cmd_guienabletooltip(int argc, slib_par_t *params, var_t *retval) {
  GuiEnableTooltip();
  return 1;
}

static int cmd_guifade(int argc, slib_par_t *params, var_t *retval) {
  auto alpha = get_param_num(argc, params, 0, 0);
  GuiFade(alpha);
  return 1;
}

static int cmd_guigroupbox(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto text = get_param_str(argc, params, 1, 0);
  GuiGroupBox(bounds, text);
  return 1;
}

static int cmd_guilabel(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto text = get_param_str(argc, params, 1, 0);
  GuiLabel(bounds, text);
  return 1;
}

static int cmd_guiline(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto text = get_param_str(argc, params, 1, 0);
  GuiLine(bounds, text);
  return 1;
}

static int cmd_guiloadstyle(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  GuiLoadStyle(fileName);
  return 1;
}

static int cmd_guiloadstyledefault(int argc, slib_par_t *params, var_t *retval) {
  GuiLoadStyleDefault();
  return 1;
}

static int cmd_guilock(int argc, slib_par_t *params, var_t *retval) {
  GuiLock();
  return 1;
}

static int cmd_guipanel(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  GuiPanel(bounds);
  return 1;
}

static int cmd_guisetstate(int argc, slib_par_t *params, var_t *retval) {
  auto state = get_param_int(argc, params, 0, 0);
  GuiSetState(state);
  return 1;
}

static int cmd_guisetstyle(int argc, slib_par_t *params, var_t *retval) {
  auto control = get_param_int(argc, params, 0, 0);
  auto property = get_param_int(argc, params, 1, 0);
  auto value = get_param_int(argc, params, 2, 0);
  GuiSetStyle(control, property, value);
  return 1;
}

static int cmd_guisettooltip(int argc, slib_par_t *params, var_t *retval) {
  auto tooltip = get_param_str(argc, params, 0, 0);
  GuiSetTooltip(tooltip);
  return 1;
}

static int cmd_guistatusbar(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto text = get_param_str(argc, params, 1, 0);
  GuiStatusBar(bounds, text);
  return 1;
}

static int cmd_guiunlock(int argc, slib_par_t *params, var_t *retval) {
  GuiUnlock();
  return 1;
}

static int cmd_poll_events(int argc, slib_par_t *params, var_t *retval) {
  glfwPollEvents();
  return 1;
}

static int cmd_wait_events(int argc, slib_par_t *params, var_t *retval) {
  int waitMillis = get_param_int(argc, params, 0, -1);
  if (waitMillis > 0) {
    glfwWaitEventsTimeout(waitMillis / 1000);
  } else {
    glfwWaitEvents();
  }
  return 1;
}

static int cmd_createphysicsbodycircle(int argc, slib_par_t *params, var_t *retval) {
  auto pos = get_param_vec2(argc, params, 0);
  auto radius = get_param_num(argc, params, 1, 0);
  auto density = get_param_num(argc, params, 2, 0);
  auto fnResult = CreatePhysicsBodyCircle(pos, radius, density);
  v_setphysics(retval, fnResult);
  return 1;
}

static int cmd_createphysicsbodypolygon(int argc, slib_par_t *params, var_t *retval) {
  auto pos = get_param_vec2(argc, params, 0);
  auto radius = get_param_num(argc, params, 1, 0);
  auto sides = get_param_int(argc, params, 2, 0);
  auto density = get_param_num(argc, params, 3, 0);
  auto fnResult = CreatePhysicsBodyPolygon(pos, radius, sides, density);
  v_setphysics(retval, fnResult);
  return 1;
}

static int cmd_createphysicsbodyrectangle(int argc, slib_par_t *params, var_t *retval) {
  auto pos = get_param_vec2(argc, params, 0);
  auto width = get_param_num(argc, params, 1, 0);
  auto height = get_param_num(argc, params, 2, 0);
  auto density = get_param_num(argc, params, 3, 0);
  auto fnResult = CreatePhysicsBodyRectangle(pos, width, height, density);
  v_setphysics(retval, fnResult);
  return 1;
}

static int cmd_getphysicsbodiescount(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetPhysicsBodiesCount();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_getphysicsbody(int argc, slib_par_t *params, var_t *retval) {
  auto index = get_param_int(argc, params, 0, 0);
  auto fnResult = GetPhysicsBody(index);
  v_setphysics(retval, fnResult);
  return 1;
}

static int cmd_getphysicsshapetype(int argc, slib_par_t *params, var_t *retval) {
  auto index = get_param_int(argc, params, 0, 0);
  auto fnResult = GetPhysicsShapeType(index);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_getphysicsshapevertex(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    auto vertex = get_param_int(argc, params, 1, 0);
    auto fnResult = GetPhysicsShapeVertex(_physicsMap.at(id), vertex);
    v_setvec2(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_getphysicsshapeverticescount(int argc, slib_par_t *params, var_t *retval) {
  auto index = get_param_int(argc, params, 0, 0);
  auto fnResult = GetPhysicsShapeVerticesCount(index);
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_isphysicsenabled(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsPhysicsEnabled();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_physicsshapetype(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = PhysicsShapeType();
  v_setint(retval, fnResult);
  return 1;
}

static int cmd_closephysics(int argc, slib_par_t *params, var_t *retval) {
  ClosePhysics();
  return 1;
}

static int cmd_destroyphysicsbody(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    DestroyPhysicsBody(_physicsMap.at(id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_initphysics(int argc, slib_par_t *params, var_t *retval) {
  InitPhysics();
  return 1;
}

static int cmd_physicsaddforce(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    auto force = get_param_vec2(argc, params, 1);
    PhysicsAddForce(_physicsMap.at(id), force);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_physicsaddtorque(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    auto amount = get_param_num(argc, params, 1, 0);
    PhysicsAddTorque(_physicsMap.at(id), amount);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_physicsshatter(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    auto position = get_param_vec2(argc, params, 1);
    auto force = get_param_num(argc, params, 2, 0);
    PhysicsShatter(_physicsMap.at(id), position, force);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_resetphysics(int argc, slib_par_t *params, var_t *retval) {
  ResetPhysics();
  return 1;
}

static int cmd_runphysicsstep(int argc, slib_par_t *params, var_t *retval) {
  RunPhysicsStep();
  return 1;
}

static int cmd_setphysicsbodyrotation(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    auto radians = get_param_num(argc, params, 1, 0);
    SetPhysicsBodyRotation(_physicsMap.at(id), radians);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_setphysicsgravity(int argc, slib_par_t *params, var_t *retval) {
  auto x = get_param_num(argc, params, 0, 0);
  auto y = get_param_num(argc, params, 1, 0);
  SetPhysicsGravity(x, y);
  return 1;
}

static int cmd_setphysicstimestep(int argc, slib_par_t *params, var_t *retval) {
  auto delta = get_param_int(argc, params, 0, 0);
  SetPhysicsTimeStep(delta);
  return 1;
}

FUNC_SIG lib_func[] = {
  {1, 1, "CHANGEDIRECTORY", cmd_changedirectory},
  {2, 2, "CHECKCOLLISIONBOXES", cmd_checkcollisionboxes},
  {3, 3, "CHECKCOLLISIONBOXSPHERE", cmd_checkcollisionboxsphere},
  {3, 3, "CHECKCOLLISIONCIRCLEREC", cmd_checkcollisioncirclerec},
  {4, 4, "CHECKCOLLISIONCIRCLES", cmd_checkcollisioncircles},
  {3, 3, "CHECKCOLLISIONPOINTCIRCLE", cmd_checkcollisionpointcircle},
  {2, 2, "CHECKCOLLISIONPOINTREC", cmd_checkcollisionpointrec},
  {4, 4, "CHECKCOLLISIONPOINTTRIANGLE", cmd_checkcollisionpointtriangle},
  //{2, 2, "CHECKCOLLISIONRAYBOX", cmd_checkcollisionraybox},
  //{3, 3, "CHECKCOLLISIONRAYSPHERE", cmd_checkcollisionraysphere},
  //{4, 4, "CHECKCOLLISIONRAYSPHEREEX", cmd_checkcollisionraysphereex},
  {2, 2, "CHECKCOLLISIONRECS", cmd_checkcollisionrecs},
  {4, 4, "CHECKCOLLISIONSPHERES", cmd_checkcollisionspheres},
  {2, 2, "COLORALPHA", cmd_coloralpha},
  {3, 3, "COLORALPHABLEND", cmd_coloralphablend},
  {3, 3, "COLORFROMHSV", cmd_colorfromhsv},
  {1, 1, "COLORTOHSV", cmd_colortohsv},
  //{3, 3, "COMPRESSDATA", cmd_compressdata},
  //{3, 3, "DECOMPRESSDATA", cmd_decompressdata},
  {2, 2, "FADE", cmd_fade},
  {3, 3, "GENIMAGECELLULAR", cmd_genimagecellular},
  {6, 6, "GENIMAGECHECKED", cmd_genimagechecked},
  {3, 3, "GENIMAGECOLOR", cmd_genimagecolor},
  //{6, 6, "GENIMAGEFONTATLAS", cmd_genimagefontatlas},
  {4, 4, "GENIMAGEGRADIENTH", cmd_genimagegradienth},
  {5, 5, "GENIMAGEGRADIENTRADIAL", cmd_genimagegradientradial},
  {4, 4, "GENIMAGEGRADIENTV", cmd_genimagegradientv},
  {5, 5, "GENIMAGEPERLINNOISE", cmd_genimageperlinnoise},
  {3, 3, "GENIMAGEWHITENOISE", cmd_genimagewhitenoise},
  //{3, 3, "GENMESHCUBE", cmd_genmeshcube},
  //{2, 2, "GENMESHCUBICMAP", cmd_genmeshcubicmap},
  //{3, 3, "GENMESHCYLINDER", cmd_genmeshcylinder},
  //{2, 2, "GENMESHHEIGHTMAP", cmd_genmeshheightmap},
  //{3, 3, "GENMESHHEMISPHERE", cmd_genmeshhemisphere},
  //{4, 4, "GENMESHKNOT", cmd_genmeshknot},
  //{4, 4, "GENMESHPLANE", cmd_genmeshplane},
  //{2, 2, "GENMESHPOLY", cmd_genmeshpoly},
  //{3, 3, "GENMESHSPHERE", cmd_genmeshsphere},
  //{4, 4, "GENMESHTORUS", cmd_genmeshtorus},
  //{2, 2, "GENTEXTUREBRDF", cmd_gentexturebrdf},
  //{4, 4, "GENTEXTURECUBEMAP", cmd_gentexturecubemap},
  //{3, 3, "GENTEXTUREIRRADIANCE", cmd_gentextureirradiance},
  //{3, 3, "GENTEXTUREPREFILTER", cmd_gentextureprefilter},
  //{1, 1, "GETCAMERAMATRIX", cmd_getcameramatrix},
  //{1, 1, "GETCAMERAMATRIX2D", cmd_getcameramatrix2d},
  {0, 0, "GETCLIPBOARDTEXT", cmd_getclipboardtext},
  //{2, 2, "GETCODEPOINTS", cmd_getcodepoints},
  //{1, 1, "GETCODEPOINTSCOUNT", cmd_getcodepointscount},
  //{2, 2, "GETCOLLISIONRAYGROUND", cmd_getcollisionrayground},
  //{2, 2, "GETCOLLISIONRAYMODEL", cmd_getcollisionraymodel},
  //{4, 4, "GETCOLLISIONRAYTRIANGLE", cmd_getcollisionraytriangle},
  {2, 2, "GETCOLLISIONREC", cmd_getcollisionrec},
  {1, 1, "GETCOLOR", cmd_getcolor},
  //{1, 1, "GETDROPPEDFILES", cmd_getdroppedfiles},
  //{0, 0, "GETFONTDEFAULT", cmd_getfontdefault},
  {0, 0, "GETFPS", cmd_getfps},
  {0, 0, "GETFRAMETIME", cmd_getframetime},
  {1, 1, "GETGAMEPADAXISCOUNT", cmd_getgamepadaxiscount},
  {2, 2, "GETGAMEPADAXISMOVEMENT", cmd_getgamepadaxismovement},
  {0, 0, "GETGAMEPADBUTTONPRESSED", cmd_getgamepadbuttonpressed},
  {1, 1, "GETGAMEPADNAME", cmd_getgamepadname},
  {0, 0, "GETGESTUREDETECTED", cmd_getgesturedetected},
  {0, 0, "GETGESTUREDRAGANGLE", cmd_getgesturedragangle},
  //{0, 0, "GETGESTUREDRAGVECTOR", cmd_getgesturedragvector},
  {0, 0, "GETGESTUREHOLDDURATION", cmd_getgestureholdduration},
  {0, 0, "GETGESTUREPINCHANGLE", cmd_getgesturepinchangle},
  //{0, 0, "GETGESTUREPINCHVECTOR", cmd_getgesturepinchvector},
  //{2, 2, "GETGLYPHINDEX", cmd_getglyphindex},
  //{2, 2, "GETIMAGEALPHABORDER", cmd_getimagealphaborder},
  {1, 1, "GETIMAGEDATA", cmd_getimagedata},
  //{1, 1, "GETIMAGEDATANORMALIZED", cmd_getimagedatanormalized},
  //{3, 3, "GETIMAGEPALETTE", cmd_getimagepalette},
  {0, 0, "GETKEYPRESSED", cmd_getkeypressed},
  //{0, 0, "GETMATRIXMODELVIEW", cmd_getmatrixmodelview},
  //{0, 0, "GETMATRIXPROJECTION", cmd_getmatrixprojection},
  {0, 0, "GETMONITORCOUNT", cmd_getmonitorcount},
  {1, 1, "GETMONITORHEIGHT", cmd_getmonitorheight},
  {1, 1, "GETMONITORNAME", cmd_getmonitorname},
  {1, 1, "GETMONITORPHYSICALHEIGHT", cmd_getmonitorphysicalheight},
  {1, 1, "GETMONITORPHYSICALWIDTH", cmd_getmonitorphysicalwidth},
  {1, 1, "GETMONITORREFRESHRATE", cmd_getmonitorrefreshrate},
  {1, 1, "GETMONITORWIDTH", cmd_getmonitorwidth},
  {0, 0, "GETMOUSECURSOR", cmd_getmousecursor},
  {0, 0, "GETMOUSEPOSITION", cmd_getmouseposition},
  //{2, 2, "GETMOUSERAY", cmd_getmouseray},
  {0, 0, "GETMOUSEWHEELMOVE", cmd_getmousewheelmove},
  {0, 0, "GETMOUSEX", cmd_getmousex},
  {0, 0, "GETMOUSEY", cmd_getmousey},
  {1, 1, "GETMUSICTIMELENGTH", cmd_getmusictimelength},
  {1, 1, "GETMUSICTIMEPLAYED", cmd_getmusictimeplayed},
  //{2, 2, "GETNEXTCODEPOINT", cmd_getnextcodepoint},
  //{2, 2, "GETPIXELCOLOR", cmd_getpixelcolor},
  //{3, 3, "GETPIXELDATASIZE", cmd_getpixeldatasize},
  {1, 1, "GETPREVDIRECTORYPATH", cmd_getprevdirectorypath},
  {2, 2, "GETRANDOMVALUE", cmd_getrandomvalue},
  //{0, 0, "GETSCREENDATA", cmd_getscreendata},
  {0, 0, "GETSCREENHEIGHT", cmd_getscreenheight},
  {2, 2, "GETSCREENTOWORLD2D", cmd_getscreentoworld2d},
  {0, 0, "GETSCREENWIDTH", cmd_getscreenwidth},
  //{0, 0, "GETSHADERDEFAULT", cmd_getshaderdefault},
  {2, 2, "GETSHADERLOCATION", cmd_getshaderlocation},
  {2, 2, "GETSHADERLOCATIONATTRIB", cmd_getshaderlocationattrib},
  //{0, 0, "GETSHAPESTEXTURE", cmd_getshapestexture},
  //{0, 0, "GETSHAPESTEXTUREREC", cmd_getshapestexturerec},
  {0, 0, "GETSOUNDSPLAYING", cmd_getsoundsplaying},
  {1, 1, "GETTEXTUREDATA", cmd_gettexturedata},
  //{0, 0, "GETTEXTUREDEFAULT", cmd_gettexturedefault},
  {0, 0, "GETTIME", cmd_gettime},
  {0, 0, "GETTOUCHPOINTSCOUNT", cmd_gettouchpointscount},
  //{1, 1, "GETTOUCHPOSITION", cmd_gettouchposition},
  {0, 0, "GETTOUCHX", cmd_gettouchx},
  {0, 0, "GETTOUCHY", cmd_gettouchy},
  //{1, 1, "GETWAVEDATA", cmd_getwavedata},
  {0, 0, "GETWINDOWPOSITION", cmd_getwindowposition},
  //{0, 0, "GETWINDOWSCALEDPI", cmd_getwindowscaledpi},
  {0, 0, "GETWORKINGDIRECTORY", cmd_getworkingdirectory},
  //{2, 2, "GETWORLDTOSCREEN", cmd_getworldtoscreen},
  //{2, 2, "GETWORLDTOSCREEN2D", cmd_getworldtoscreen2d},
  //{4, 4, "GETWORLDTOSCREENEX", cmd_getworldtoscreenex},
  //{1, 1, "IMAGECOPY", cmd_imagecopy},
  //{2, 2, "IMAGEFROMIMAGE", cmd_imagefromimage},
  //{3, 3, "IMAGETEXT", cmd_imagetext},
  //{5, 5, "IMAGETEXTEX", cmd_imagetextex},
  //{3, 3, "INITAUDIOSTREAM", cmd_initaudiostream},
  {0, 0, "ISAUDIODEVICEREADY", cmd_isaudiodeviceready},
  //{1, 1, "ISAUDIOSTREAMPLAYING", cmd_isaudiostreamplaying},
  //{1, 1, "ISAUDIOSTREAMPROCESSED", cmd_isaudiostreamprocessed},
  {0, 0, "ISCURSORHIDDEN", cmd_iscursorhidden},
  {0, 0, "ISCURSORONSCREEN", cmd_iscursoronscreen},
  {0, 0, "ISFILEDROPPED", cmd_isfiledropped},
  {2, 2, "ISFILEEXTENSION", cmd_isfileextension},
  {1, 1, "ISGAMEPADAVAILABLE", cmd_isgamepadavailable},
  {2, 2, "ISGAMEPADBUTTONDOWN", cmd_isgamepadbuttondown},
  {2, 2, "ISGAMEPADBUTTONPRESSED", cmd_isgamepadbuttonpressed},
  {2, 2, "ISGAMEPADBUTTONRELEASED", cmd_isgamepadbuttonreleased},
  {2, 2, "ISGAMEPADBUTTONUP", cmd_isgamepadbuttonup},
  {2, 2, "ISGAMEPADNAME", cmd_isgamepadname},
  {1, 1, "ISGESTUREDETECTED", cmd_isgesturedetected},
  {1, 1, "ISKEYDOWN", cmd_iskeydown},
  {1, 1, "ISKEYPRESSED", cmd_iskeypressed},
  {1, 1, "ISKEYRELEASED", cmd_iskeyreleased},
  {1, 1, "ISKEYUP", cmd_iskeyup},
  //{2, 2, "ISMODELANIMATIONVALID", cmd_ismodelanimationvalid},
  {1, 1, "ISMOUSEBUTTONDOWN", cmd_ismousebuttondown},
  {1, 1, "ISMOUSEBUTTONPRESSED", cmd_ismousebuttonpressed},
  {1, 1, "ISMOUSEBUTTONRELEASED", cmd_ismousebuttonreleased},
  {1, 1, "ISMOUSEBUTTONUP", cmd_ismousebuttonup},
  {1, 1, "ISMUSICPLAYING", cmd_ismusicplaying},
  {1, 1, "ISSOUNDPLAYING", cmd_issoundplaying},
  {0, 0, "ISVRSIMULATORREADY", cmd_isvrsimulatorready},
  {0, 0, "ISWINDOWFOCUSED", cmd_iswindowfocused},
  {0, 0, "ISWINDOWFULLSCREEN", cmd_iswindowfullscreen},
  {0, 0, "ISWINDOWHIDDEN", cmd_iswindowhidden},
  {0, 0, "ISWINDOWMAXIMIZED", cmd_iswindowmaximized},
  {0, 0, "ISWINDOWMINIMIZED", cmd_iswindowminimized},
  {0, 0, "ISWINDOWREADY", cmd_iswindowready},
  {0, 0, "ISWINDOWRESIZED", cmd_iswindowresized},
  //{2, 2, "LOADFILEDATA", cmd_loadfiledata},
  //{1, 1, "LOADFILETEXT", cmd_loadfiletext},
  {1, 1, "LOADFONT", cmd_loadfont},
  //{6, 6, "LOADFONTDATA", cmd_loadfontdata},
  {4, 4, "LOADFONTEX", cmd_loadfontex},
  //{3, 3, "LOADFONTFROMIMAGE", cmd_loadfontfromimage},
  //{6, 6, "LOADFONTFROMMEMORY", cmd_loadfontfrommemory},
  {1, 1, "LOADIMAGE", cmd_loadimage},
  //{2, 2, "LOADIMAGEANIM", cmd_loadimageanim},
  //{3, 3, "LOADIMAGEFROMMEMORY", cmd_loadimagefrommemory},
  //{5, 5, "LOADIMAGERAW", cmd_loadimageraw},
  //{0, 0, "LOADMATERIALDEFAULT", cmd_loadmaterialdefault},
  //{2, 2, "LOADMATERIALS", cmd_loadmaterials},
  //{2, 2, "LOADMESHES", cmd_loadmeshes},
  {1, 1, "LOADMODEL", cmd_loadmodel},
  //{2, 2, "LOADMODELANIMATIONS", cmd_loadmodelanimations},
  //{1, 1, "LOADMODELFROMMESH", cmd_loadmodelfrommesh},
  {1, 1, "LOADMUSICSTREAM", cmd_loadmusicstream},
  {2, 2, "LOADRENDERTEXTURE", cmd_loadrendertexture},
  {2, 2, "LOADSHADER", cmd_loadshader},
  //{2, 2, "LOADSHADERCODE", cmd_loadshadercode},
  {1, 1, "LOADSOUND", cmd_loadsound},
  {1, 1, "LOADSOUNDFROMWAVE", cmd_loadsoundfromwave},
  //{1, 1, "LOADSTORAGEVALUE", cmd_loadstoragevalue},
  {1, 1, "LOADTEXTURE", cmd_loadtexture},
  //{2, 2, "LOADTEXTURECUBEMAP", cmd_loadtexturecubemap},
  {1, 1, "LOADTEXTUREFROMIMAGE", cmd_loadtexturefromimage},
  //{1, 1, "LOADWAVE", cmd_loadwave},
  //{3, 3, "LOADWAVEFROMMEMORY", cmd_loadwavefrommemory},
  {2, 2, "MEASURETEXT", cmd_measuretext},
  //{4, 4, "MEASURETEXTEX", cmd_measuretextex},
  //{1, 1, "MESHBOUNDINGBOX", cmd_meshboundingbox},
  {1, 1, "TEXTFORMAT", cmd_textformat},
  //{1, 1, "WAVECOPY", cmd_wavecopy},
  {0, 0, "WINDOWSHOULDCLOSE", cmd_windowshouldclose},
  {2, 2, "GUIBUTTON", cmd_guibutton},
  {3, 3, "GUICHECKBOX", cmd_guicheckbox},
  {2, 2, "GUICOLORBARALPHA", cmd_guicolorbaralpha},
  {2, 2, "GUICOLORBARHUE", cmd_guicolorbarhue},
  {2, 2, "GUICOLORPICKER", cmd_guicolorpicker},
  {3, 3, "GUICOMBOBOX", cmd_guicombobox},
  {4, 4, "GUIDROPDOWNBOX", cmd_guidropdownbox},
  {2, 2, "GUIGETSTYLE", cmd_guigetstyle},
  {3, 3, "GUIGRID", cmd_guigrid},
  {2, 2, "GUIIMAGEBUTTON", cmd_guiimagebutton},
  {2, 2, "GUIIMAGEBUTTONEX", cmd_guiimagebuttonex},
  {2, 2, "GUILABELBUTTON", cmd_guilabelbutton},
  {4, 4, "GUILISTVIEW", cmd_guilistview},
  {6, 6, "GUILISTVIEWEX", cmd_guilistviewex},
  {4, 4, "GUIMESSAGEBOX", cmd_guimessagebox},
  {6, 6, "GUIPROGRESSBAR", cmd_guiprogressbar},
  {4, 4, "GUISCROLLBAR", cmd_guiscrollbar},
  {3, 3, "GUISCROLLPANEL", cmd_guiscrollpanel},
  {6, 6, "GUISLIDER", cmd_guislider},
  {6, 6, "GUISLIDERBAR", cmd_guisliderbar},
  {6, 6, "GUISPINNER", cmd_guispinner},
  {4, 4, "GUITEXTBOX", cmd_guitextbox},
  {4, 4, "GUITEXTBOXMULTI", cmd_guitextboxmulti},
  {5, 5, "GUITEXTINPUTBOX", cmd_guitextinputbox},
  {3, 3, "GUITOGGLE", cmd_guitoggle},
  {3, 3, "GUITOGGLEGROUP", cmd_guitogglegroup},
  {6, 6, "GUIVALUEBOX", cmd_guivaluebox},
  {2, 2, "GUIWINDOWBOX", cmd_guiwindowbox},
  {3, 3, "CREATEPHYSICSBODYCIRCLE", cmd_createphysicsbodycircle},
  {4, 4, "CREATEPHYSICSBODYPOLYGON", cmd_createphysicsbodypolygon},
  {4, 4, "CREATEPHYSICSBODYRECTANGLE", cmd_createphysicsbodyrectangle},
  {0, 0, "GETPHYSICSBODIESCOUNT", cmd_getphysicsbodiescount},
  {1, 1, "GETPHYSICSBODY", cmd_getphysicsbody},
  {1, 1, "GETPHYSICSSHAPETYPE", cmd_getphysicsshapetype},
  {2, 2, "GETPHYSICSSHAPEVERTEX", cmd_getphysicsshapevertex},
  {1, 1, "GETPHYSICSSHAPEVERTICESCOUNT", cmd_getphysicsshapeverticescount},
  {0, 0, "ISPHYSICSENABLED", cmd_isphysicsenabled},
  {0, 0, "PHYSICSSHAPETYPE", cmd_physicsshapetype},
};

FUNC_SIG lib_proc[] = {
  {1, 1, "BEGINBLENDMODE", cmd_beginblendmode},
  {0, 0, "BEGINDRAWING", cmd_begindrawing},
  {1, 1, "BEGINMODE2D", cmd_beginmode2d},
  {1, 1, "BEGINMODE3D", cmd_beginmode3d},
  {4, 4, "BEGINSCISSORMODE", cmd_beginscissormode},
  {1, 1, "BEGINSHADERMODE", cmd_beginshadermode},
  {1, 1, "BEGINTEXTUREMODE", cmd_begintexturemode},
  {0, 0, "BEGINVRDRAWING", cmd_beginvrdrawing},
  {1, 1, "CLEARBACKGROUND", cmd_clearbackground},
  {0, 0, "CLEARDIRECTORYFILES", cmd_cleardirectoryfiles},
  {0, 0, "CLEARDROPPEDFILES", cmd_cleardroppedfiles},
  {0, 0, "CLOSEAUDIODEVICE", cmd_closeaudiodevice},
  //{1, 1, "CLOSEAUDIOSTREAM", cmd_closeaudiostream},
  {0, 0, "CLOSEVRSIMULATOR", cmd_closevrsimulator},
  {0, 0, "CLOSEWINDOW", cmd_closewindow},
  {0, 0, "DISABLECURSOR", cmd_disablecursor},
  //{5, 5, "DRAWBILLBOARD", cmd_drawbillboard},
  //{6, 6, "DRAWBILLBOARDREC", cmd_drawbillboardrec},
  {2, 2, "DRAWBOUNDINGBOX", cmd_drawboundingbox},
  {4, 4, "DRAWCIRCLE", cmd_drawcircle},
  {5, 5, "DRAWCIRCLE3D", cmd_drawcircle3d},
  {5, 5, "DRAWCIRCLEGRADIENT", cmd_drawcirclegradient},
  {4, 4, "DRAWCIRCLELINES", cmd_drawcirclelines},
  {6, 6, "DRAWCIRCLESECTOR", cmd_drawcirclesector},
  {6, 6, "DRAWCIRCLESECTORLINES", cmd_drawcirclesectorlines},
  {3, 3, "DRAWCIRCLEV", cmd_drawcirclev},
  {5, 5, "DRAWCUBE", cmd_drawcube},
  //{6, 6, "DRAWCUBETEXTURE", cmd_drawcubetexture},
  {3, 3, "DRAWCUBEV", cmd_drawcubev},
  {5, 5, "DRAWCUBEWIRES", cmd_drawcubewires},
  {3, 3, "DRAWCUBEWIRESV", cmd_drawcubewiresv},
  {6, 6, "DRAWCYLINDER", cmd_drawcylinder},
  {6, 6, "DRAWCYLINDERWIRES", cmd_drawcylinderwires},
  {5, 5, "DRAWELLIPSE", cmd_drawellipse},
  {5, 5, "DRAWELLIPSELINES", cmd_drawellipselines},
  {2, 2, "DRAWFPS", cmd_drawfps},
  {1, 1, "DRAWGIZMO", cmd_drawgizmo},
  {2, 2, "DRAWGRID", cmd_drawgrid},
  {5, 5, "DRAWLINE", cmd_drawline},
  {3, 3, "DRAWLINE3D", cmd_drawline3d},
  {4, 4, "DRAWLINEBEZIER", cmd_drawlinebezier},
  {4, 4, "DRAWLINEEX", cmd_drawlineex},
  {3, 3, "DRAWLINESTRIP", cmd_drawlinestrip},
  {3, 3, "DRAWLINEV", cmd_drawlinev},
  {4, 4, "DRAWMODEL", cmd_drawmodel},
  //{6, 6, "DRAWMODELEX", cmd_drawmodelex},
  //{4, 4, "DRAWMODELWIRES", cmd_drawmodelwires},
  //{6, 6, "DRAWMODELWIRESEX", cmd_drawmodelwiresex},
  {3, 3, "DRAWPIXEL", cmd_drawpixel},
  {2, 2, "DRAWPIXELV", cmd_drawpixelv},
  {3, 3, "DRAWPLANE", cmd_drawplane},
  {2, 2, "DRAWPOINT3D", cmd_drawpoint3d},
  {5, 5, "DRAWPOLY", cmd_drawpoly},
  {5, 5, "DRAWPOLYLINES", cmd_drawpolylines},
  //{2, 2, "DRAWRAY", cmd_drawray},
  {5, 5, "DRAWRECTANGLE", cmd_drawrectangle},
  {5, 5, "DRAWRECTANGLEGRADIENTEX", cmd_drawrectanglegradientex},
  {6, 6, "DRAWRECTANGLEGRADIENTH", cmd_drawrectanglegradienth},
  {6, 6, "DRAWRECTANGLEGRADIENTV", cmd_drawrectanglegradientv},
  {5, 5, "DRAWRECTANGLELINES", cmd_drawrectanglelines},
  {3, 3, "DRAWRECTANGLELINESEX", cmd_drawrectanglelinesex},
  {4, 4, "DRAWRECTANGLEPRO", cmd_drawrectanglepro},
  {2, 2, "DRAWRECTANGLEREC", cmd_drawrectanglerec},
  {4, 4, "DRAWRECTANGLEROUNDED", cmd_drawrectanglerounded},
  {5, 5, "DRAWRECTANGLEROUNDEDLINES", cmd_drawrectangleroundedlines},
  {3, 3, "DRAWRECTANGLEV", cmd_drawrectanglev},
  {7, 7, "DRAWRING", cmd_drawring},
  {7, 7, "DRAWRINGLINES", cmd_drawringlines},
  {3, 3, "DRAWSPHERE", cmd_drawsphere},
  {5, 5, "DRAWSPHEREEX", cmd_drawsphereex},
  {5, 5, "DRAWSPHEREWIRES", cmd_drawspherewires},
  {5, 5, "DRAWTEXT", cmd_drawtext},
  //{5, 5, "DRAWTEXTCODEPOINT", cmd_drawtextcodepoint},
  {6, 6, "DRAWTEXTEX", cmd_drawtextex},
  //{7, 7, "DRAWTEXTREC", cmd_drawtextrec},
  //{11, 11, "DRAWTEXTRECEX", cmd_drawtextrecex},
  {4, 4, "DRAWTEXTURE", cmd_drawtexture},
  {5, 5, "DRAWTEXTUREEX", cmd_drawtextureex},
  //{6, 6, "DRAWTEXTURENPATCH", cmd_drawtexturenpatch},
  {6, 6, "DRAWTEXTUREPRO", cmd_drawtexturepro},
  //{5, 5, "DRAWTEXTUREQUAD", cmd_drawtexturequad},
  {4, 4, "DRAWTEXTUREREC", cmd_drawtexturerec},
  //{7, 7, "DRAWTEXTURETILED", cmd_drawtexturetiled},
  //{3, 3, "DRAWTEXTUREV", cmd_drawtexturev},
  {4, 4, "DRAWTRIANGLE", cmd_drawtriangle},
  {4, 4, "DRAWTRIANGLE3D", cmd_drawtriangle3d},
  {3, 3, "DRAWTRIANGLEFAN", cmd_drawtrianglefan},
  {4, 4, "DRAWTRIANGLELINES", cmd_drawtrianglelines},
  {3, 3, "DRAWTRIANGLESTRIP", cmd_drawtrianglestrip},
  {3, 3, "DRAWTRIANGLESTRIP3D", cmd_drawtrianglestrip3d},
  {0, 0, "ENABLECURSOR", cmd_enablecursor},
  {0, 0, "ENDBLENDMODE", cmd_endblendmode},
  {0, 0, "ENDDRAWING", cmd_enddrawing},
  {0, 0, "ENDMODE2D", cmd_endmode2d},
  {0, 0, "ENDMODE3D", cmd_endmode3d},
  {0, 0, "ENDSCISSORMODE", cmd_endscissormode},
  {0, 0, "ENDSHADERMODE", cmd_endshadermode},
  {0, 0, "ENDTEXTUREMODE", cmd_endtexturemode},
  {0, 0, "ENDVRDRAWING", cmd_endvrdrawing},
  {2, 2, "EXPORTIMAGE", cmd_exportimage},
  //{2, 2, "EXPORTIMAGEASCODE", cmd_exportimageascode},
  //{2, 2, "EXPORTMESH", cmd_exportmesh},
  //{2, 2, "EXPORTWAVE", cmd_exportwave},
  //{2, 2, "EXPORTWAVEASCODE", cmd_exportwaveascode},
  //{1, 1, "GENTEXTUREMIPMAPS", cmd_gentexturemipmaps},
  {0, 0, "GETWINDOWHANDLE", cmd_getwindowhandle},
  {0, 0, "HIDECURSOR", cmd_hidecursor},
  //{3, 3, "IMAGEALPHACLEAR", cmd_imagealphaclear},
  //{2, 2, "IMAGEALPHACROP", cmd_imagealphacrop},
  //{2, 2, "IMAGEALPHAMASK", cmd_imagealphamask},
  //{1, 1, "IMAGEALPHAPREMULTIPLY", cmd_imagealphapremultiply},
  //{2, 2, "IMAGECLEARBACKGROUND", cmd_imageclearbackground},
  {2, 2, "IMAGECOLORBRIGHTNESS", cmd_imagecolorbrightness},
  {2, 2, "IMAGECOLORCONTRAST", cmd_imagecolorcontrast},
  {1, 1, "IMAGECOLORGRAYSCALE", cmd_imagecolorgrayscale},
  {1, 1, "IMAGECOLORINVERT", cmd_imagecolorinvert},
  //{3, 3, "IMAGECOLORREPLACE", cmd_imagecolorreplace},
  {2, 2, "IMAGECOLORTINT", cmd_imagecolortint},
  //{2, 2, "IMAGECROP", cmd_imagecrop},
  //{5, 5, "IMAGEDITHER", cmd_imagedither},
  //{5, 5, "IMAGEDRAW", cmd_imagedraw},
  {5, 5, "IMAGEDRAWCIRCLE", cmd_imagedrawcircle},
  //{4, 4, "IMAGEDRAWCIRCLEV", cmd_imagedrawcirclev},
  //{6, 6, "IMAGEDRAWLINE", cmd_imagedrawline},
  //{4, 4, "IMAGEDRAWLINEV", cmd_imagedrawlinev},
  //{4, 4, "IMAGEDRAWPIXEL", cmd_imagedrawpixel},
  //{3, 3, "IMAGEDRAWPIXELV", cmd_imagedrawpixelv},
  //{6, 6, "IMAGEDRAWRECTANGLE", cmd_imagedrawrectangle},
  //{4, 4, "IMAGEDRAWRECTANGLELINES", cmd_imagedrawrectanglelines},
  {3, 3, "IMAGEDRAWRECTANGLEREC", cmd_imagedrawrectanglerec},
  //{4, 4, "IMAGEDRAWRECTANGLEV", cmd_imagedrawrectanglev},
  //{6, 6, "IMAGEDRAWTEXT", cmd_imagedrawtext},
  //{7, 7, "IMAGEDRAWTEXTEX", cmd_imagedrawtextex},
  {1, 1, "IMAGEFLIPHORIZONTAL", cmd_imagefliphorizontal},
  {1, 1, "IMAGEFLIPVERTICAL", cmd_imageflipvertical},
  {2, 2, "IMAGEFORMAT", cmd_imageformat},
  //{1, 1, "IMAGEMIPMAPS", cmd_imagemipmaps},
  //{3, 3, "IMAGERESIZE", cmd_imageresize},
  //{6, 6, "IMAGERESIZECANVAS", cmd_imageresizecanvas},
  //{3, 3, "IMAGERESIZENN", cmd_imageresizenn},
  //{1, 1, "IMAGEROTATECCW", cmd_imagerotateccw},
  //{1, 1, "IMAGEROTATECW", cmd_imagerotatecw},
  //{2, 2, "IMAGETOPOT", cmd_imagetopot},
  {0, 0, "INITAUDIODEVICE", cmd_initaudiodevice},
  {0, 0, "INITVRSIMULATOR", cmd_initvrsimulator},
  {3, 3, "INITWINDOW", cmd_initwindow},
  {0, 0, "MAXIMIZEWINDOW", cmd_maximizewindow},
  //{1, 1, "MESHBINORMALS", cmd_meshbinormals},
  //{1, 1, "MESHNORMALSSMOOTH", cmd_meshnormalssmooth},
  //{1, 1, "MESHTANGENTS", cmd_meshtangents},
  {1, 1, "OPENURL", cmd_openurl},
  //{1, 1, "PAUSEAUDIOSTREAM", cmd_pauseaudiostream},
  {1, 1, "PAUSEMUSICSTREAM", cmd_pausemusicstream},
  {1, 1, "PAUSESOUND", cmd_pausesound},
  //{1, 1, "PLAYAUDIOSTREAM", cmd_playaudiostream},
  {1, 1, "PLAYMUSICSTREAM", cmd_playmusicstream},
  {1, 1, "PLAYSOUND", cmd_playsound},
  {1, 1, "PLAYSOUNDMULTI", cmd_playsoundmulti},
  {0, 0, "RESTOREWINDOW", cmd_restorewindow},
  //{1, 1, "RESUMEAUDIOSTREAM", cmd_resumeaudiostream},
  {1, 1, "RESUMEMUSICSTREAM", cmd_resumemusicstream},
  {1, 1, "RESUMESOUND", cmd_resumesound},
  //{3, 3, "SAVEFILEDATA", cmd_savefiledata},
  //{2, 2, "SAVEFILETEXT", cmd_savefiletext},
  //{2, 2, "SAVESTORAGEVALUE", cmd_savestoragevalue},
  //{1, 1, "SETAUDIOSTREAMBUFFERSIZEDEFAULT", cmd_setaudiostreambuffersizedefault},
  //{2, 2, "SETAUDIOSTREAMPITCH", cmd_setaudiostreampitch},
  //{2, 2, "SETAUDIOSTREAMVOLUME", cmd_setaudiostreamvolume},
  //{1, 1, "SETCAMERAALTCONTROL", cmd_setcameraaltcontrol},
  {2, 2, "SETCAMERAMODE", cmd_setcameramode},
  //{6, 6, "SETCAMERAMOVECONTROLS", cmd_setcameramovecontrols},
  //{1, 1, "SETCAMERAPANCONTROL", cmd_setcamerapancontrol},
  //{1, 1, "SETCAMERASMOOTHZOOMCONTROL", cmd_setcamerasmoothzoomcontrol},
  {1, 1, "SETCLIPBOARDTEXT", cmd_setclipboardtext},
  {1, 1, "SETCONFIGFLAGS", cmd_setconfigflags},
  {1, 1, "SETEXITKEY", cmd_setexitkey},
  //{1, 1, "SETGESTURESENABLED", cmd_setgesturesenabled},
  {1, 1, "SETMASTERVOLUME", cmd_setmastervolume},
  //{3, 3, "SETMATERIALTEXTURE", cmd_setmaterialtexture},
  //{1, 1, "SETMATRIXMODELVIEW", cmd_setmatrixmodelview},
  //{1, 1, "SETMATRIXPROJECTION", cmd_setmatrixprojection},
  //{3, 3, "SETMODELMESHMATERIAL", cmd_setmodelmeshmaterial},
  {1, 1, "SETMOUSECURSOR", cmd_setmousecursor},
  {2, 2, "SETMOUSEOFFSET", cmd_setmouseoffset},
  {2, 2, "SETMODELDIFFUSETEXTURE", cmd_setmodeldiffusetexture},
  {2, 2, "SETMOUSEPOSITION", cmd_setmouseposition},
  {2, 2, "SETMOUSESCALE", cmd_setmousescale},
  {2, 2, "SETMUSICPITCH", cmd_setmusicpitch},
  {2, 2, "SETMUSICVOLUME", cmd_setmusicvolume},
  //{3, 3, "SETPIXELCOLOR", cmd_setpixelcolor},
  //{4, 4, "SETSHADERVALUE", cmd_setshadervalue},
  //{3, 3, "SETSHADERVALUEMATRIX", cmd_setshadervaluematrix},
  //{3, 3, "SETSHADERVALUETEXTURE", cmd_setshadervaluetexture},
  {4, 5, "SETSHADERVALUEV", cmd_setshadervaluev},
  //{2, 2, "SETSHAPESTEXTURE", cmd_setshapestexture},
  {2, 2, "SETSOUNDPITCH", cmd_setsoundpitch},
  {2, 2, "SETSOUNDVOLUME", cmd_setsoundvolume},
  {1, 1, "SETTARGETFPS", cmd_settargetfps},
  //{2, 2, "SETTEXTUREFILTER", cmd_settexturefilter},
  //{2, 2, "SETTEXTUREWRAP", cmd_settexturewrap},
  //{1, 1, "SETTRACELOGCALLBACK", cmd_settracelogcallback},
  {1, 1, "SETTRACELOGEXIT", cmd_settracelogexit},
  {1, 1, "SETTRACELOGLEVEL", cmd_settraceloglevel},
  //{2, 2, "SETVRCONFIGURATION", cmd_setvrconfiguration},
  //{1, 1, "SETWINDOWICON", cmd_setwindowicon},
  {2, 2, "SETWINDOWMINSIZE", cmd_setwindowminsize},
  {1, 1, "SETWINDOWMONITOR", cmd_setwindowmonitor},
  {2, 2, "SETWINDOWPOSITION", cmd_setwindowposition},
  {2, 2, "SETWINDOWSIZE", cmd_setwindowsize},
  {1, 1, "SETWINDOWTITLE", cmd_setwindowtitle},
  {0, 0, "SHOWCURSOR", cmd_showcursor},
  //{1, 1, "STOPAUDIOSTREAM", cmd_stopaudiostream},
  {1, 1, "STOPMUSICSTREAM", cmd_stopmusicstream},
  {1, 1, "STOPSOUND", cmd_stopsound},
  {0, 0, "STOPSOUNDMULTI", cmd_stopsoundmulti},
  {1, 1, "TAKESCREENSHOT", cmd_takescreenshot},
  {0, 0, "TOGGLEFULLSCREEN", cmd_togglefullscreen},
  {0, 0, "TOGGLEVRMODE", cmd_togglevrmode},
  {2, 2, "TRACELOG", cmd_tracelog},
  {1, 1, "UNLOADFONT", cmd_unloadfont},
  {1, 1, "UNLOADIMAGE", cmd_unloadimage},
  //{1, 1, "UNLOADMATERIAL", cmd_unloadmaterial},
  //{1, 1, "UNLOADMESH", cmd_unloadmesh},
  {1, 1, "UNLOADMODEL", cmd_unloadmodel},
  //{1, 1, "UNLOADMODELANIMATION", cmd_unloadmodelanimation},
  {1, 1, "UNLOADMUSICSTREAM", cmd_unloadmusicstream},
  {1, 1, "UNLOADRENDERTEXTURE", cmd_unloadrendertexture},
  {1, 1, "UNLOADSHADER", cmd_unloadshader},
  {1, 1, "UNLOADSOUND", cmd_unloadsound},
  {1, 1, "UNLOADTEXTURE", cmd_unloadtexture},
  //{1, 1, "UNLOADWAVE", cmd_unloadwave},
  //{3, 3, "UPDATEAUDIOSTREAM", cmd_updateaudiostream},
  {1, 1, "UPDATECAMERA", cmd_updatecamera},
  //{3, 3, "UPDATEMODELANIMATION", cmd_updatemodelanimation},
  {1, 1, "UPDATEMUSICSTREAM", cmd_updatemusicstream},
  //{3, 3, "UPDATESOUND", cmd_updatesound},
  {2, 2, "UPDATETEXTURE", cmd_updatetexture},
  //{3, 3, "UPDATETEXTUREREC", cmd_updatetexturerec},
  //{1, 1, "UPDATEVRTRACKING", cmd_updatevrtracking},
  //{3, 3, "WAVECROP", cmd_wavecrop},
  //{4, 4, "WAVEFORMAT", cmd_waveformat},
  {0, 0, "GUICLEARTOOLTIP", cmd_guicleartooltip},
  {0, 0, "GUIDISABLE", cmd_guidisable},
  {0, 0, "GUIDISABLETOOLTIP", cmd_guidisabletooltip},
  {2, 2, "GUIDUMMYREC", cmd_guidummyrec},
  {0, 0, "GUIENABLE", cmd_guienable},
  {0, 0, "GUIENABLETOOLTIP", cmd_guienabletooltip},
  {1, 1, "GUIFADE", cmd_guifade},
  {2, 2, "GUIGROUPBOX", cmd_guigroupbox},
  {2, 2, "GUILABEL", cmd_guilabel},
  {2, 2, "GUILINE", cmd_guiline},
  {1, 1, "GUILOADSTYLE", cmd_guiloadstyle},
  {0, 0, "GUILOADSTYLEDEFAULT", cmd_guiloadstyledefault},
  {0, 0, "GUILOCK", cmd_guilock},
  {1, 1, "GUIPANEL", cmd_guipanel},
  {1, 1, "GUISETSTATE", cmd_guisetstate},
  {3, 3, "GUISETSTYLE", cmd_guisetstyle},
  {1, 1, "GUISETTOOLTIP", cmd_guisettooltip},
  {2, 2, "GUISTATUSBAR", cmd_guistatusbar},
  {0, 0, "GUIUNLOCK", cmd_guiunlock},
  {0, 0, "POLLEVENTS", cmd_poll_events},
  {0, 1, "WAITEVENTS", cmd_wait_events},
  {0, 0, "CLOSEPHYSICS", cmd_closephysics},
  {1, 1, "DESTROYPHYSICSBODY", cmd_destroyphysicsbody},
  {0, 0, "INITPHYSICS", cmd_initphysics},
  {2, 2, "PHYSICSADDFORCE", cmd_physicsaddforce},
  {2, 2, "PHYSICSADDTORQUE", cmd_physicsaddtorque},
  {3, 3, "PHYSICSSHATTER", cmd_physicsshatter},
  {0, 0, "RESETPHYSICS", cmd_resetphysics},
  {0, 0, "RUNPHYSICSSTEP", cmd_runphysicsstep},
  {2, 2, "SETPHYSICSBODYROTATION", cmd_setphysicsbodyrotation},
  {2, 2, "SETPHYSICSGRAVITY", cmd_setphysicsgravity},
  {1, 1, "SETPHYSICSTIMESTEP", cmd_setphysicstimestep},
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
    strcpy(proc_name, lib_proc[index]._name);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int sblib_func_getname(int index, char *proc_name) {
  int result;
  if (index < sblib_func_count()) {
    strcpy(proc_name, lib_func[index]._name);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int sblib_proc_exec(int index, int argc, slib_par_t *params, var_t *retval) {
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
    fprintf(stderr, "Raylib: PROC index error [%d]\n", index);
    result = 0;
  }
  return result;
}

int sblib_func_exec(int index, int argc, slib_par_t *params, var_t *retval) {
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
    fprintf(stderr, "Raylib: FUNC index error [%d]\n", index);
    result = 0;
  }
  return result;
}

void sblib_close(void) {
  _fontMap.clear();
  _imageMap.clear();
  _modelMap.clear();
  _musicMap.clear();
  _renderMap.clear();
  _soundMap.clear();
  _textureMap.clear();
}
