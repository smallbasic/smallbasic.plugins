// This file is part of SmallBASIC
//
// Plugin for raylib games library - https://www.raylib.com/
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2021 Chris Warren-Smith

#include "config.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic push
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wenum-compare"
#pragma GCC diagnostic ignored "-Wdouble-promotion"
#include <raylib/raylib/src/raylib.h>
#include <raygui/src/raygui.h>
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#include <raylib/src/extras/physac.h>
#include <GLFW/glfw3.h>
#include <cstring>

#include "robin-hood-hashing/src/include/robin_hood.h"
#include "include/var.h"
#include "include/module.h"
#include "include/param.h"
#include "physac.h"

robin_hood::unordered_map<int, AudioStream> _audioStream;
robin_hood::unordered_map<int, Font> _fontMap;
robin_hood::unordered_map<int, Image> _imageMap;
robin_hood::unordered_map<int, Matrix> _matrixMap;
robin_hood::unordered_map<int, Mesh> _meshMap;
robin_hood::unordered_map<int, Model> _modelMap;
robin_hood::unordered_map<int, ModelAnimation> _modelAnimationMap;
robin_hood::unordered_map<int, Music> _musicMap;
robin_hood::unordered_map<int, PhysicsBody> _physicsMap;
robin_hood::unordered_map<int, RenderTexture2D> _renderMap;
robin_hood::unordered_map<int, Sound> _soundMap;
robin_hood::unordered_map<int, Texture2D> _textureMap;
robin_hood::unordered_map<int, Wave> _waveMap;
int _nextId = 1;

PhysicsBody get_physics_body(var_p_t var) {
  PhysicsBody result;
  int id = var->v.fn.id;
  if (id != -1 && _physicsMap.find(id) != _physicsMap.end()) {
    result = _physicsMap.at(id);
  } else {
    result = nullptr;
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

static NPatchInfo get_param_npatch(int argc, slib_par_t *params, int n) {
  NPatchInfo result;
  if (is_param_map(argc, params, n) && is_map(map_get(params[n].var_p, "source"))) {
    var_p_t map = params[n].var_p;
    var_p_t source = map_get(map, "source");
    result.source.x = get_map_num(source, "x");
    result.source.y = get_map_num(source, "y");
    result.source.width = get_map_num(source, "width");
    result.source.height = get_map_num(source, "height");
    result.left = get_map_num(map, "left");
    result.top = get_map_num(map, "top");
    result.right = get_map_num(map, "right");
    result.bottom = get_map_num(map, "bottom");
    result.layout = get_map_num(map, "layout");
  } else {
    TraceLog(LOG_FATAL, "NPatchInfo not found");
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

static Vector2 *get_param_vec2_array(int argc, slib_par_t *params, int n) {
  static Vector2 data[10];
  Vector2 *result;
  if (is_param_array(argc, params, n)) {
    var_p_t array = params[n].var_p;
    int size = v_asize(array);
    for (int i = 0; i < size && i < 10; i++) {
      data[i] = get_array_elem_vec2(array, i);
    }
    result = (Vector2 *)&data;
  } else {
    result = nullptr;
  }
  return result;
}

static Vector3 *get_param_vec3_array(int argc, slib_par_t *params, int n) {
  static Vector3 data[10];
  Vector3 *result;
  if (is_param_array(argc, params, n)) {
    var_p_t array = params[n].var_p;
    int size = v_asize(array);
    for (int i = 0; i < size && i < 10; i++) {
      data[i] = get_array_elem_vec3(array, i);
    }
    result = (Vector3 *)&data;
  } else {
    result = nullptr;
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

static Vector4 get_param_vec4(int argc, slib_par_t *params, int n) {
  Vector4 result;
  if (is_param_map(argc, params, n)) {
    result.x = get_map_num(params[n].var_p, "x");
    result.y = get_map_num(params[n].var_p, "y");
    result.z = get_map_num(params[n].var_p, "z");
    result.w = get_map_num(params[n].var_p, "w");
  } else if (is_param_array(argc, params, n)) {
    result.x = get_array_elem_num(params[n].var_p, 0);
    result.y = get_array_elem_num(params[n].var_p, 1);
    result.z = get_array_elem_num(params[n].var_p, 2);
    result.w = get_array_elem_num(params[n].var_p, 3);
  } else {
    result.x = 0;
    result.y = 0;
    result.z = 0;
    result.w = 0;
  }
  return result;
}

static Shader get_param_shader(int argc, slib_par_t *params, int arg) {
  Shader result;
  if (is_param_map(argc, params, arg)) {
    result.id = get_id(params, arg);
    var_p_t var = map_get(params[arg].var_p, "locs");
    if (v_is_type(var, V_INT)) {
      result.locs = (int *)(var->v.i);
    } else {
      result.locs = nullptr;
    }
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
  } else if (is_param_map(argc, params, n)) {
    var_p_t map = params[n].var_p;
    result.min = get_map_vec3(map, "min");
    result.max = get_map_vec3(map, "max");
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

static Ray get_param_ray(int argc, slib_par_t *params, int n) {
  Ray result;
  if (is_param_map(argc, params, n)) {
    var_p_t map = params[n].var_p;
    result.position = get_map_vec3(map, "position");
    result.direction = get_map_vec3(map, "direction");
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
    result.projection = get_array_elem_num(array, 4);
  } else if (is_param_map(argc, params, n)) {
    var_p_t map = params[n].var_p;
    result.position = get_map_vec3(map, "position");
    result.target = get_map_vec3(map, "target");
    result.up = get_map_vec3(map, "up");
    result.fovy = get_map_num(map, "fovy");
    result.projection = get_map_num(map, "projection");
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

static int get_audiostream_id(int argc, slib_par_t *params, int arg, var_t *retval) {
  int result = 0;
  if (is_param_map(argc, params, arg)) {
    int id = get_id(params, arg);
    if (id != -1 && _audioStream.find(id) != _audioStream.end()) {
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "AudioStream not found");
  }
  return result;
}

static int get_font_id(int argc, slib_par_t *params, int arg, var_t *retval) {
  int result = -1;
  if (is_param_map(argc, params, arg)) {
    // the passed in variable is a map
    int id = get_id(params, arg);
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
    int id = get_id(params, arg);
    if (id != -1 && _imageMap.find(id) != _imageMap.end()) {
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "Image not found");
  }
  return result;
}

static int get_mesh_id(int argc, slib_par_t *params, int arg, var_t *retval) {
  int result = -1;
  if (is_param_map(argc, params, arg)) {
    int id = get_id(params, arg);
    if (id != -1 && _meshMap.find(id) != _meshMap.end()) {
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "Mesh not found");
  }
  return result;
}

static int get_texture_id(int argc, slib_par_t *params, int arg, var_t *retval) {
  int result = -1;
  if (is_param_map(argc, params, arg)) {
    int id = get_id(params, arg);
    if (id != -1 && _textureMap.find(id) != _textureMap.end()) {
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
    int id = get_id(params, arg);
    if (id != -1 && _modelMap.find(id) != _modelMap.end()) {
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "Model not found");
  }
  return result;
}

static int get_model_animation_id(int argc, slib_par_t *params, int arg, var_t *retval) {
  int result = -1;
  if (is_param_map(argc, params, arg)) {
    int id = get_id(params, arg);
    if (id != -1 && _modelAnimationMap.find(id) != _modelAnimationMap.end()) {
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "Model Animation not found");
  }
  return result;
}

static int get_physics_body_id(int argc, slib_par_t *params, int arg, var_t *retval) {
  int result = -1;
  if (is_param_map(argc, params, arg)) {
    int id = get_id(params, arg);
    if (id != -1 && _physicsMap.find(id) != _physicsMap.end()) {
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "PhysicsBody not found");
  }
  return result;
}

static int get_render_texture_id(int argc, slib_par_t *params, int arg, var_t *retval) {
  int result = 0;
  if (is_param_map(argc, params, arg)) {
    int id = get_id(params, arg);
    if (id != -1 && _renderMap.find(id) != _renderMap.end()) {
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "RenderTexture not found");
  }
  return result;
}

static int get_matrix_id(int argc, slib_par_t *params, int arg, var_t *retval) {
  int result = 0;
  if (is_param_map(argc, params, arg)) {
    int id = get_id(params, arg);
    if (id != -1 && _matrixMap.find(id) != _matrixMap.end()) {
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "Matrix not found");
  }
  return result;
}

static int get_music_id(int argc, slib_par_t *params, int arg, var_t *retval) {
  int result = 0;
  if (is_param_map(argc, params, arg)) {
    int id = get_id(params, arg);
    if (id != -1 && _musicMap.find(id) != _musicMap.end()) {
      _musicMap.at(id).looping = map_get_int(params[arg].var_p, "looping", 0);
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "Music not found");
  }
  return result;
}

static int get_wave_id(int argc, slib_par_t *params, int arg, var_t *retval) {
  int result = 0;
  if (is_param_map(argc, params, arg)) {
    int id = get_id(params, arg);
    if (id != -1 && _waveMap.find(id) != _waveMap.end()) {
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "Wave not found");
  }
  return result;
}

static int get_sound_id(int argc, slib_par_t *params, int arg, var_t *retval) {
  int result = 0;
  if (is_param_map(argc, params, arg)) {
    int id = get_id(params, arg);
    if (id != -1 && _soundMap.find(id) != _soundMap.end()) {
      result = id;
    }
  }
  if (result == -1) {
    error(retval, "Sound not found");
  }
  return result;
}

static void v_setrect(var_t *var, int width, int height, int id) {
  map_init_id(var, id);
  v_setint(map_add_var(var, "width", 0), width);
  v_setint(map_add_var(var, "height", 0), height);
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

static void v_setvec4(var_t *var, Vector4 &vec4) {
  map_init(var);
  v_setreal(map_add_var(var, "x", 0), vec4.x);
  v_setreal(map_add_var(var, "y", 0), vec4.y);
  v_setreal(map_add_var(var, "z", 0), vec4.z);
  v_setreal(map_add_var(var, "w", 0), vec4.w);
}

static void v_setboundingbox(var_t *var, BoundingBox &box) {
  map_init(var);
  v_setvec3(map_add_var(var, "min", 0), box.min);
  v_setvec3(map_add_var(var, "max", 0), box.max);
}

static void v_setcolor(var_t *var, Color &c) {
  var_int_t color = ((0x00000000) | (c.r << 24) | (c.g << 16) | (c.b << 8) | (c.a));
  v_setint(var, color);
}

static void v_setaudiostream(var_t *var, AudioStream &audioStream) {
  int id = ++_nextId;
  _audioStream[id] = audioStream;
  map_init_id(var, id);
  v_setint(map_add_var(var, "sampleRate", 0), audioStream.sampleRate);
  v_setint(map_add_var(var, "sampleSize", 0), audioStream.sampleSize);
  v_setint(map_add_var(var, "channels", 0), audioStream.channels);
}

static void v_setfont(var_t *var, Font &font) {
  auto id = ++_nextId;
  _fontMap[id] = font;
  map_init_id(var, id);
  v_setint(map_add_var(var, "baseSize", 0), font.baseSize);
  v_setint(map_add_var(var, "charsCount", 0), font.glyphCount);
}

static void v_setphysics(var_t *var, PhysicsBody &physics) {
  if (physics != NULL) {
    auto id = ++_nextId;
    _physicsMap[id] = physics;
    create(physics, var, id);
  } else {
    v_setint(var, 0);
  }
}

static void v_setshader(var_t *var, Shader &shader) {
  map_init_id(var, shader.id);
  v_setint(map_add_var(var, "locs", 0), (var_int_t)shader.locs);
}

static void v_settexture2d(var_t *var, Texture2D &texture) {
  int id = ++_nextId;
  _textureMap[id] = texture;
  v_setrect(var, texture.width, texture.height, id);
  v_setint(map_add_var(var, "id", 0), texture.id);
  v_setint(map_add_var(var, "mipmaps", 0), texture.mipmaps);
  v_setint(map_add_var(var, "format", 0), texture.format);
}

static void v_setimage(var_t *var, Image &image) {
  int id = ++_nextId;
  _imageMap[id] = image;
  v_setrect(var, image.width, image.height, id);
}

static void v_setmesh(var_t *var, Mesh &mesh) {
  int id = ++_nextId;
  _meshMap[id] = mesh;
  map_init_id(var, id);
  v_setint(map_add_var(var, "vertexCount", 0), mesh.vertexCount);
  v_setint(map_add_var(var, "triangleCount", 0), mesh.triangleCount);
}

static void v_setmatrix(var_t *var, Matrix &matrix) {
  int id = ++_nextId;
  _matrixMap[id] = matrix;
  map_init_id(var, id);
}

static void v_setmodel(var_t *var, Model &model) {
  auto id = ++_nextId;
  _modelMap[id] = model;
  map_init_id(var, id);
  v_setint(map_add_var(var, "meshCount", 0), model.meshCount);
  v_setint(map_add_var(var, "materialCount", 0), model.materialCount);
  v_setint(map_add_var(var, "boneCount", 0), model.boneCount);
}

static void v_setmusic(var_t *var, Music &music) {
  int id = ++_nextId;
  _musicMap[id] = music;
  map_init_id(var, id);
  v_setint(map_add_var(var, "frameCount", 0), music.frameCount);
  v_setint(map_add_var(var, "looping", 0), music.looping);
  v_setint(map_add_var(var, "ctxType", 0), music.ctxType);
}

static void v_setmodel_animation(var_t *var, ModelAnimation *anims, int animsCount) {
  v_toarray1(var, animsCount);
  for (int i = 0; i < animsCount; i++) {
    var_t *v_anim = v_elem(var, i);
    auto id = ++_nextId;
    _modelAnimationMap[id] = anims[i];
    map_init_id(v_anim, id);

    int frameCount = anims[i].frameCount;
    int boneCount = anims[i].boneCount;
    map_add_var(v_anim, "frameCount", frameCount);
    map_add_var(v_anim, "boneCount", boneCount);

    var_t *v_framePoses = map_add_var(v_anim, "framePoses", 0);
    v_tomatrix(v_framePoses, frameCount, boneCount);
    for (int frame = 0; frame < frameCount; frame++) {
      for (int bone = 0; bone < boneCount; bone++) {
        var_t *v_transform = v_elem(v_framePoses, ((boneCount * frame) + bone));
        map_init(v_transform);
        v_setvec3(map_add_var(v_transform, "translation", 0), anims[0].framePoses[frame][bone].translation);
        v_setvec3(map_add_var(v_transform, "scale", 0), anims[0].framePoses[frame][bone].scale);
      }
    }
  }
}

static void v_setraycollision(var_t *var, RayCollision &info) {
  map_init(var);
  v_setint(map_add_var(var, "hit", 0), info.hit);
  v_setint(map_add_var(var, "distance", 0), info.distance);
  v_setvec3(map_add_var(var, "point", 0), info.point);
  v_setvec3(map_add_var(var, "normal", 0), info.normal);
}

static void v_setray(var_t *var, Ray &ray) {
  map_init(var);
  v_setvec3(map_add_var(var, "position", 0), ray.position);
  v_setvec3(map_add_var(var, "direction", 0), ray.direction);
}

static void v_setsound(var_t *var, Sound &sound) {
  int id = ++_nextId;
  _soundMap[id] = sound;
  map_init_id(var, id);
  v_setint(map_add_var(var, "frameCount", 0), sound.frameCount);
}

static void v_setwave(var_t *var, Wave &wave) {
  int id = ++_nextId;
  _waveMap[id] = wave;
  map_init_id(var, id);
  v_setint(map_add_var(var, "frameCount", 0), wave.frameCount);
  v_setint(map_add_var(var, "sampleRate", 0), wave.sampleRate);
  v_setint(map_add_var(var, "sampleSize", 0), wave.sampleSize);
  v_setint(map_add_var(var, "channels", 0), wave.channels);
}

#include "proc.h"
#include "func.h"

static int cmd_setshadervalue(int argc, slib_par_t *params, var_t *retval) {
  auto shader = get_param_shader(argc, params, 0);
  auto uniformLoc = get_param_int(argc, params, 1, 0);
  auto uniformType = get_param_int(argc, params, 3, SHADER_UNIFORM_FLOAT);
  auto count = get_param_int(argc, params, 4, 1);
  Vector3 vec3;
  Vector2 vec2;
  float num;
  switch (uniformType) {
  case SHADER_UNIFORM_FLOAT:
    num = get_param_num(argc, params, 2, 0);
    SetShaderValueV(shader, uniformLoc, &num, uniformType, count);
    break;
  case SHADER_UNIFORM_VEC2:
    vec2 = get_param_vec2(argc, params, 2);
    SetShaderValueV(shader, uniformLoc, &vec2, uniformType, count);
    break;
  case SHADER_UNIFORM_VEC3:
    vec3 = get_param_vec3(argc, params, 2);
    SetShaderValueV(shader, uniformLoc, &vec3, uniformType, count);
    break;
  default:
    error(retval, "Uniform type not implemented");
    break;
  }
  return 1;
}

static int cmd_updatecamera(int argc, slib_par_t *params, var_t *retval) {
  auto camera = get_camera_3d(argc, params, 0);
  UpdateCamera(&camera);
  set_camera_3d(params[0].var_p, &camera);
  return 1;
}

static int cmd_loadmodelanimations(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, NULL);
  unsigned int animsCount = 0;
  auto anims = LoadModelAnimations(fileName, &animsCount);
  v_setmodel_animation(retval, anims, animsCount);
  RL_FREE(anims);
  return 1;
}

static int cmd_setmodeldiffusetexture(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int modelId = get_model_id(argc, params, 0, retval);
  int textureId = get_texture_id(argc, params, 1, retval);
  if (modelId != -1 && textureId != -1) {
    _modelMap.at(modelId).materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = _textureMap.at(textureId);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_loadrendertexture(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  auto renderId = ++_nextId;
  auto renderTexture = LoadRenderTexture(width, height);
  _renderMap[renderId] = renderTexture;
  map_init_id(retval, renderId);
  auto textureId = ++_nextId;
  _textureMap[textureId] = renderTexture.texture;
  var_p_t texture = map_add_var(retval, "texture", 0);
  v_setrect(texture, renderTexture.texture.width, renderTexture.texture.height, textureId);
  return 1;
}

static int cmd_loadshader(int argc, slib_par_t *params, var_t *retval) {
  auto vsFileName = get_param_str(argc, params, 0, NULL);
  auto fsFileName = get_param_str(argc, params, 1, NULL);
  int result;
  if (vsFileName[0] == '0' && vsFileName[1] == '\0') {
    Shader shader = LoadShader(0, fsFileName);
    v_setshader(retval, shader);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_meshboundingbox(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_mesh_id(argc, params, 0, retval);
  if (id != -1) {
    auto mesh = GetMeshBoundingBox(_meshMap.at(id));
    map_init(retval);
    v_setvec3(map_add_var(retval, "min", 0), mesh.min);
    v_setvec3(map_add_var(retval, "max", 0), mesh.max);
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
    UnloadImageColors(pixels);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_textformat(int argc, slib_par_t *params, var_t *retval) {
  v_setstr(retval, format_text(argc, params, 0));
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
  auto text = get_param_str(argc, params, 1, 0);
  auto alpha = get_param_num(argc, params, 2, 0);
  auto fnResult = GuiColorBarAlpha(bounds, text, alpha);
  v_setreal(retval, fnResult);
  return 1;
}

static int cmd_guicolorbarhue(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto text = get_param_str(argc, params, 1, 0);
  auto value = get_param_num(argc, params, 2, 0);
  auto fnResult = GuiColorBarHue(bounds, text, value);
  v_setreal(retval, fnResult);
  return 1;
}

static int cmd_guicolorpicker(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto text = get_param_str(argc, params, 1, 0);
  auto color = get_param_color(argc, params, 2);
  auto fnResult = GuiColorPicker(bounds, text, color);
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
  auto text = get_param_str(argc, params, 1, 0);
  auto spacing = get_param_num(argc, params, 2, 0);
  auto subdivs = get_param_int(argc, params, 3, 0);
  auto fnResult = GuiGrid(bounds, text, spacing, subdivs);
  v_setvec2(retval, fnResult);
  return 1;
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

static int cmd_guiscrollpanel(int argc, slib_par_t *params, var_t *retval) {
  auto bounds = get_param_rect(argc, params, 0);
  auto text = get_param_str(argc, params, 1, 0);
  auto content = get_param_rect(argc, params, 2);
  auto scroll = get_param_vec2(argc, params, 3);
  auto fnResult = GuiScrollPanel(bounds, text, content, &scroll);
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

static int cmd_guidisable(int argc, slib_par_t *params, var_t *retval) {
  GuiDisable();
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
  auto text = get_param_str(argc, params, 1, 0);
  GuiPanel(bounds, text);
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

static int cmd_updatephysics(int argc, slib_par_t *params, var_t *retval) {
  UpdatePhysics();
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
  auto delta = get_param_num(argc, params, 0, 0);
  SetPhysicsTimeStep(delta);
  return 1;
}

static int cmd_setphysbodyenabled(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    setEnabled(_physicsMap.at(id), get_param_int(argc, params, 1, 0) == 1);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_setphysbodyposition(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    setPosition(_physicsMap.at(id), get_param_vec2(argc, params, 1));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_setphysbodyvelocity(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    setVelocity(_physicsMap.at(id), get_param_vec2(argc, params, 1));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_setphysbodyforce(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    setForce(_physicsMap.at(id), get_param_vec2(argc, params, 1));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_setphysbodyangularvelocity(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    setAngularVelocity(_physicsMap.at(id), get_param_num(argc, params, 1, 0));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_setphysbodytorque(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    setTorque(_physicsMap.at(id), get_param_num(argc, params, 1, 0));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_setphysbodyorient(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    setOrient(_physicsMap.at(id), get_param_num(argc, params, 1, 0));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_setphysbodyinertia(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    setInertia(_physicsMap.at(id), get_param_num(argc, params, 1, 0));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_setphysbodyinverseinertia(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    setInverseInertia(_physicsMap.at(id), get_param_num(argc, params, 1, 0));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_setphysbodymass(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    setMass(_physicsMap.at(id), get_param_num(argc, params, 1, 0));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_setphysbodyinversemass(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    setInverseMass(_physicsMap.at(id), get_param_num(argc, params, 1, 0));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_setphysbodystaticfriction(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    setStaticFriction(_physicsMap.at(id), get_param_num(argc, params, 1, 0));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_setphysbodydynamicfriction(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    setDynamicFriction(_physicsMap.at(id), get_param_num(argc, params, 1, 0));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_setphysbodyrestitution(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    setRestitution(_physicsMap.at(id), get_param_num(argc, params, 1, 0));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_setphysbodyusegravity(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    setUseGravity(_physicsMap.at(id), get_param_int(argc, params, 1, 0) == 1);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_setphysbodyisgrounded(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    setIsGrounded(_physicsMap.at(id), get_param_int(argc, params, 1, 0) == 1);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_setphysbodyfreezeorient(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int id = get_physics_body_id(argc, params, 0, retval);
  if (id != -1) {
    setFreezeOrient(_physicsMap.at(id), get_param_int(argc, params, 1, 0) == 1);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static FUNC_SIG lib_func[] = {
#include "func-def.h"
  {1, 1, "MESHBOUNDINGBOX", cmd_meshboundingbox},
  {2, 2, "LOADSHADER", cmd_loadshader},
  {2, 2, "LOADRENDERTEXTURE", cmd_loadrendertexture},
  {1, 1, "LOADMODELANIMATIONS", cmd_loadmodelanimations},
  {1, 20,"TEXTFORMAT", cmd_textformat},
  {2, 2, "GUIBUTTON", cmd_guibutton},
  {3, 3, "GUICHECKBOX", cmd_guicheckbox},
  {3, 3, "GUICOLORBARALPHA", cmd_guicolorbaralpha},
  {3, 3, "GUICOLORBARHUE", cmd_guicolorbarhue},
  {3, 3, "GUICOLORPICKER", cmd_guicolorpicker},
  {3, 3, "GUICOMBOBOX", cmd_guicombobox},
  {4, 4, "GUIDROPDOWNBOX", cmd_guidropdownbox},
  {2, 2, "GUIGETSTYLE", cmd_guigetstyle},
  {4, 4, "GUIGRID", cmd_guigrid},
  {2, 2, "GUILABELBUTTON", cmd_guilabelbutton},
  {4, 4, "GUILISTVIEW", cmd_guilistview},
  {6, 6, "GUILISTVIEWEX", cmd_guilistviewex},
  {4, 4, "GUIMESSAGEBOX", cmd_guimessagebox},
  {6, 6, "GUIPROGRESSBAR", cmd_guiprogressbar},
  {4, 4, "GUISCROLLPANEL", cmd_guiscrollpanel},
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
  {0, 0, "PHYSICSSHAPETYPE", cmd_physicsshapetype},
};

static FUNC_SIG lib_proc[] = {
#include "proc-def.h"
  {4, 5, "SETSHADERVALUE", cmd_setshadervalue},
  {2, 2, "SETMODELDIFFUSETEXTURE", cmd_setmodeldiffusetexture},
  {1, 1, "UPDATECAMERA", cmd_updatecamera},
  {2, 2, "UPDATETEXTURE", cmd_updatetexture},
  {0, 0, "GUIDISABLE", cmd_guidisable},
  {2, 2, "GUIDUMMYREC", cmd_guidummyrec},
  {0, 0, "GUIENABLE", cmd_guienable},
  {1, 1, "GUIFADE", cmd_guifade},
  {2, 2, "GUIGROUPBOX", cmd_guigroupbox},
  {2, 2, "GUILABEL", cmd_guilabel},
  {2, 2, "GUILINE", cmd_guiline},
  {1, 1, "GUILOADSTYLE", cmd_guiloadstyle},
  {0, 0, "GUILOADSTYLEDEFAULT", cmd_guiloadstyledefault},
  {0, 0, "GUILOCK", cmd_guilock},
  {2, 2, "GUIPANEL", cmd_guipanel},
  {1, 1, "GUISETSTATE", cmd_guisetstate},
  {3, 3, "GUISETSTYLE", cmd_guisetstyle},
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
  {0, 0, "UPDATEPHYSICS", cmd_updatephysics},
  {2, 2, "SETPHYSICSBODYROTATION", cmd_setphysicsbodyrotation},
  {2, 2, "SETPHYSICSGRAVITY", cmd_setphysicsgravity},
  {1, 1, "SETPHYSICSTIMESTEP", cmd_setphysicstimestep},
  {2, 2, "SETPHYSICSBODYENABLED", cmd_setphysbodyenabled},
  {2, 2, "SETPHYSICSBODYPOSITION", cmd_setphysbodyposition},
  {2, 2, "SETPHYSICSBODYVELOCITY", cmd_setphysbodyvelocity},
  {2, 2, "SETPHYSICSBODYFORCE", cmd_setphysbodyforce},
  {2, 2, "SETPHYSICSBODYANGULARVELOCITY", cmd_setphysbodyangularvelocity},
  {2, 2, "SETPHYSICSBODYTORQUE", cmd_setphysbodytorque},
  {2, 2, "SETPHYSICSBODYORIENT", cmd_setphysbodyorient},
  {2, 2, "SETPHYSICSBODYINERTIA", cmd_setphysbodyinertia},
  {2, 2, "SETPHYSICSBODYINVERSEINERTIA", cmd_setphysbodyinverseinertia},
  {2, 2, "SETPHYSICSBODYMASS", cmd_setphysbodymass},
  {2, 2, "SETPHYSICSBODYINVERSEMASS", cmd_setphysbodyinversemass},
  {2, 2, "SETPHYSICSBODYSTATICFRICTION", cmd_setphysbodystaticfriction},
  {2, 2, "SETPHYSICSBODYDYNAMICFRICTION", cmd_setphysbodydynamicfriction},
  {2, 2, "SETPHYSICSBODYRESTITUTION", cmd_setphysbodyrestitution},
  {2, 2, "SETPHYSICSBODYUSEGRAVITY", cmd_setphysbodyusegravity},
  {2, 2, "SETPHYSICSBODYISGROUNDED", cmd_setphysbodyisgrounded},
  {2, 2, "SETPHYSICSBODYFREEZEORIENT", cmd_setphysbodyfreezeorient}
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
    fprintf(stderr, "Raylib: PROC index error [%d]\n", index);
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
    fprintf(stderr, "Raylib: FUNC index error [%d]\n", index);
    result = 0;
  }
  return result;
}

SBLIB_API void sblib_close(void) {
  _audioStream.clear();
  _fontMap.clear();
  _imageMap.clear();
  _matrixMap.clear();
  _meshMap.clear();
  _modelAnimationMap.clear();
  _modelMap.clear();
  _musicMap.clear();
  _renderMap.clear();
  _soundMap.clear();
  _textureMap.clear();
  _waveMap.clear();
}
