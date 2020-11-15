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
#include <cstring>
#include <unordered_map>

#include "include/var.h"
#include "include/var_map.h"
#include "include/module.h"
#include "include/param.h"

std::unordered_map<int, Image> _imageMap;
std::unordered_map<int, Model> _modelMap;
std::unordered_map<int, Music> _musicMap;
std::unordered_map<int, RenderTexture2D> _renderMap;
std::unordered_map<int, Sound> _soundMap;
std::unordered_map<int, Texture2D> _textureMap;
int _nextId = 1;
const char *mapID = "_ID";

void error_image(var_p_t var) {
  error(var, "Image not found");
}

void error_texture(var_p_t var) {
  error(var, "Texture not found");
}

void error_model(var_p_t var) {
  error(var, "Model not found");
}

bool is_array(var_p_t var, uint32_t size) {
  return v_is_type(var, V_ARRAY) && v_asize(var) == size;
}

var_int_t get_color_int(Color c) {
  return ((0x00000000) | (c.r << 24) | (c.g << 16) | (c.b << 8) | (c.a));
}

Color get_param_color(int argc, slib_par_t *params, int n) {
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
    result.a = 255;
  }
  return result;
}

Rectangle get_param_rect(int argc, slib_par_t *params, int n) {
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

Vector2 get_param_vec2(int argc, slib_par_t *params, int n) {
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

Vector3 get_param_vec3(int argc, slib_par_t *params, int n) {
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

Vector3 get_array_elem_vec3(var_p_t array, int index) {
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

Camera3D get_camera_3d(int argc, slib_par_t *params, int n) {
  Camera3D result;
  if (is_param_array(argc, params, n)) {
    var_p_t array = params[n].var_p;
    result.position = get_array_elem_vec3(array, 0);
    result.target = get_array_elem_vec3(array, 1);
    result.up = get_array_elem_vec3(array, 2);
    result.fovy = get_array_elem_num(array, 3);
    result.type = get_array_elem_num(array, 4);
  }
  return result;
}

Shader get_param_shader(int argc, slib_par_t *params, int n) {
  Shader result;
  if (is_param_map(argc, params, n)) {
    var_p_t var = map_get(params[n].var_p, "locs");
    if (v_is_type(var, V_INT)) {
      result.locs = (int *)(var->v.i);
    } else {
      result.locs = nullptr;
    }
    result.id = map_get_int(params[n].var_p, mapID, -1);
  }

  return result;
}

BoundingBox get_param_bounding_box(int argc, slib_par_t *params, int n) {
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

int get_image_id(int argc, slib_par_t *params, int n) {
  int result = -1;
  if (is_param_map(argc, params, n)) {
    // the passed in variable is a map
    int id = map_get_int(params[n].var_p, mapID, -1);
    if (id != -1 && _imageMap.find(id) != _imageMap.end()) {
      // the map contained an ID field with a live value
      result = id;
    }
  }
  return result;
}

int get_texture_id(int argc, slib_par_t *params, int n) {
  int result = -1;
  if (is_param_map(argc, params, n)) {
    // the passed in variable is a map
    int id = map_get_int(params[n].var_p, mapID, -1);
    if (id != -1 && _textureMap.find(id) != _textureMap.end()) {
      // the map contained an ID field with a live value
      result = id;
    }
  }
  return result;
}

int get_model_id(int argc, slib_par_t *params, int n) {
  int result = -1;
  if (is_param_map(argc, params, n)) {
    // the passed in variable is a map
    int id = map_get_int(params[n].var_p, mapID, -1);
    if (id != -1 && _modelMap.find(id) != _modelMap.end()) {
      // the map contained an ID field with a live value
      result = id;
    }
  }
  return result;
}

int get_render_texture_id(int argc, slib_par_t *params, int n) {
  int result = -1;
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

void create_rectangle(var_t *var, int width, int height, int id) {
  map_init(var);
  v_setint(map_add_var(var, "width", 0), width);
  v_setint(map_add_var(var, "height", 0), height);
  v_setint(map_add_var(var, mapID, 0), id);
}

void create_rectangle(var_t *var, Rectangle &rect) {
  map_init(var);
  v_setreal(map_add_var(var, "x", 0), rect.x);
  v_setreal(map_add_var(var, "y", 0), rect.y);
  v_setreal(map_add_var(var, "width", 0), rect.width);
  v_setreal(map_add_var(var, "height", 0), rect.height);
}

void create_vec2(var_t *var, float x, float y) {
  map_init(var);
  v_setreal(map_add_var(var, "x", 0), x);
  v_setreal(map_add_var(var, "y", 0), y);
}

int cmd_changedirectory(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto dir = get_param_str(argc, params, 0, NULL);
    auto fnResult = ChangeDirectory(dir);
    v_setint(retval, fnResult);
  } else {
    error(retval, "ChangeDirectory", 1);
  }
  return result;
}

int cmd_checkcollisionboxes(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto box1 = get_param_bounding_box(argc, params, 0);
    auto box2 = get_param_bounding_box(argc, params, 1);
    auto fnResult = CheckCollisionBoxes(box1, box2);
    v_setint(retval, fnResult);
  } else {
    error(retval, "CheckCollisionBoxes", 2);
  }
  return result;
}

int cmd_checkcollisionboxsphere(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    auto box = get_param_bounding_box(argc, params, 0);
    auto center = get_param_vec3(argc, params, 1);
    auto radius = get_param_num(argc, params, 2, 0);
    auto fnResult = CheckCollisionBoxSphere(box, center, radius);
    v_setint(retval, fnResult);
  } else {
    error(retval, "CheckCollisionBoxSphere", 3);
  }
  return result;
}

int cmd_checkcollisioncirclerec(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    auto center = get_param_vec2(argc, params, 0);
    auto radius = get_param_num(argc, params, 1, 0);
    auto rec = get_param_rect(argc, params, 2);
    auto fnResult = CheckCollisionCircleRec(center, radius, rec);
    v_setint(retval, fnResult);
  } else {
    error(retval, "CheckCollisionCircleRec", 3);
  }
  return result;
}

int cmd_checkcollisioncircles(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    auto center1 = get_param_vec2(argc, params, 0);
    auto radius1 = get_param_num(argc, params, 1, 0);
    auto center2 = get_param_vec2(argc, params, 2);
    auto radius2 = get_param_num(argc, params, 3, 0);
    auto fnResult = CheckCollisionCircles(center1, radius1, center2, radius2);
    v_setint(retval, fnResult);
  } else {
    error(retval, "CheckCollisionCircles", 4);
  }
  return result;
}

int cmd_checkcollisionpointcircle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    auto point = get_param_vec2(argc, params, 0);
    auto center = get_param_vec2(argc, params, 1);
    auto radius = get_param_num(argc, params, 2, 0);
    auto fnResult = CheckCollisionPointCircle(point, center, radius);
    v_setint(retval, fnResult);
  } else {
    error(retval, "CheckCollisionPointCircle", 3);
  }
  return result;
}

int cmd_checkcollisionpointrec(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto point = get_param_vec2(argc, params, 0);
    auto rec = get_param_rect(argc, params, 1);
    auto fnResult = CheckCollisionPointRec(point, rec);
    v_setint(retval, fnResult);
  } else {
    error(retval, "CheckCollisionPointRec", 2);
  }
  return result;
}

int cmd_checkcollisionpointtriangle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    auto point = get_param_vec2(argc, params, 0);
    auto p1 = get_param_vec2(argc, params, 1);
    auto p2 = get_param_vec2(argc, params, 2);
    auto p3 = get_param_vec2(argc, params, 3);
    auto fnResult = CheckCollisionPointTriangle(point, p1, p2, p3);
    v_setint(retval, fnResult);
  } else {
    error(retval, "CheckCollisionPointTriangle", 4);
  }
  return result;
}

int cmd_checkcollisionraybox(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    //auto ray = get_param_int(argc, params, 0, 0);
    //auto box = get_param_int(argc, params, 1, 0);
    //auto fnResult = CheckCollisionRayBox(ray, box);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "CheckCollisionRayBox", 2);
  }
  return result;
}

int cmd_checkcollisionraysphere(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    //auto ray = get_param_int(argc, params, 0, 0);
    //auto center = get_param_int(argc, params, 1, 0);
    //auto radius = get_param_int(argc, params, 2, 0);
    //auto fnResult = CheckCollisionRaySphere(ray, center, radius);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "CheckCollisionRaySphere", 2);
  }
  return result;
}

int cmd_checkcollisionraysphereex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    //auto ray = get_param_int(argc, params, 0, 0);
    //auto center = get_param_int(argc, params, 1, 0);
    //auto radius = get_param_int(argc, params, 2, 0);
    //auto collisionPoint = get_param_int(argc, params, 3, 0);
    //auto fnResult = CheckCollisionRaySphereEx(ray, center, radius, collisionPoint);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "CheckCollisionRaySphereEx", 4);
  }
  return result;
}

int cmd_checkcollisionrecs(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto rec1 = get_param_rect(argc, params, 0);
    auto rec2 = get_param_rect(argc, params, 1);
    auto fnResult = CheckCollisionRecs(rec1, rec2);
    v_setint(retval, fnResult);
  } else {
    error(retval, "CheckCollisionRecs", 2);
  }
  return result;
}

int cmd_checkcollisionspheres(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    auto centerA = get_param_vec3(argc, params, 0);
    auto radiusA = get_param_num(argc, params, 1, 0);
    auto centerB = get_param_vec3(argc, params, 2);
    auto radiusB = get_param_num(argc, params, 3, 0);
    auto fnResult = CheckCollisionSpheres(centerA, radiusA, centerB, radiusB);
    v_setint(retval, fnResult);
  } else {
    error(retval, "CheckCollisionSpheres", 4);
  }
  return result;
}

int cmd_codepointtoutf8(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto codepoint = get_param_str(argc, params, 0, NULL);
    // auto byteLength = get_param_str(argc, params, 1, NULL);
    // auto fnResult = CodepointToUtf8(codepoint, byteLength);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "CodepointToUtf8", 2);
  }
  return result;
}

int cmd_coloralpha(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto color = get_param_color(argc, params, 0, NULL);
    // auto alpha = get_param_num(argc, params, 1, NULL);
    // auto fnResult = ColorAlpha(color, alpha);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "ColorAlpha", 2);
  }
  return result;
}

int cmd_coloralphablend(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto src = get_param_str(argc, params, 1, NULL);
    // auto tint = get_param_str(argc, params, 2, NULL);
    // auto fnResult = ColorAlphaBlend(dst, src, tint);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "ColorAlphaBlend", 3);
  }
  return result;
}

int cmd_colorfromhsv(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto hue = get_param_str(argc, params, 0, NULL);
    // auto saturation = get_param_str(argc, params, 1, NULL);
    // auto value = get_param_str(argc, params, 2, NULL);
    // auto fnResult = ColorFromHSV(hue, saturation, value);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "ColorFromHSV", 3);
  }
  return result;
}

int cmd_colorfromnormalized(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto normalized = get_param_str(argc, params, 0, NULL);
    // auto fnResult = ColorFromNormalized(normalized);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "ColorFromNormalized", 1);
  }
  return result;
}

int cmd_colornormalize(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto color = get_param_color(argc, params, 0, NULL);
    // auto fnResult = ColorNormalize(color);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "ColorNormalize", 1);
  }
  return result;
}

int cmd_colortohsv(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto color = get_param_color(argc, params, 0, NULL);
    // auto fnResult = ColorToHSV(color);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "ColorToHSV", 1);
  }
  return result;
}

int cmd_colortoint(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto color = get_param_color(argc, params, 0, NULL);
    // auto fnResult = ColorToInt(color);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "ColorToInt", 1);
  }
  return result;
}

int cmd_compressdata(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto data = get_param_str(argc, params, 0, NULL);
    // auto dataLength = get_param_str(argc, params, 1, NULL);
    // auto compDataLength = get_param_str(argc, params, 2, NULL);
    // auto fnResult = CompressData(data, dataLength, compDataLength);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "CompressData", 3);
  }
  return result;
}

int cmd_decompressdata(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto compData = get_param_str(argc, params, 0, NULL);
    // auto compDataLength = get_param_str(argc, params, 1, NULL);
    // auto dataLength = get_param_str(argc, params, 2, NULL);
    // auto fnResult = DecompressData(compData, compDataLength, dataLength);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "DecompressData", 3);
  }
  return result;
}

int cmd_directoryexists(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto dirPath = get_param_str(argc, params, 0, NULL);
    auto fnResult = DirectoryExists(dirPath);
    v_setint(retval, fnResult);
  } else {
    error(retval, "DirectoryExists", 1);
  }
  return result;
}

int cmd_fade(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto color = get_param_color(argc, params, 0);
    auto alpha = get_param_num(argc, params, 1, 0);
    v_setint(retval, get_color_int(Fade(color, alpha)));
  } else {
    error(retval, "Fade", 1);
  }
  return result;
}

int cmd_fileexists(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto fileName = get_param_str(argc, params, 0, NULL);
    // auto fnResult = FileExists(fileName);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "FileExists", 1);
  }
  return result;
}

int cmd_genimagecellular(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    auto width = get_param_int(argc, params, 0, 0);
    auto height = get_param_int(argc, params, 1, 0);
    auto tileSize = get_param_int(argc, params, 2, 0);
    auto image = GenImageCellular(width, height, tileSize);
    auto id = ++_nextId;
    _imageMap[id] = image;
    create_rectangle(retval, image.width, image.height, id);
  } else {
    error(retval, "GenImageCellular", 3);
  }
  return result;
}

int cmd_genimagechecked(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    auto width = get_param_int(argc, params, 0, 0);
    auto height = get_param_int(argc, params, 1, 0);
    auto checksX = get_param_int(argc, params, 2, 0);
    auto checksY = get_param_int(argc, params, 3, 0);
    auto col1 = get_param_color(argc, params, 4);
    auto col2 = get_param_color(argc, params, 5);
    auto image = GenImageChecked(width, height, checksX, checksY, col1, col2);
    auto id = ++_nextId;
    _imageMap[id] = image;
    create_rectangle(retval, image.width, image.height, id);
  } else {
    error(retval, "GenImageChecked", 6);
  }
  return result;
}

int cmd_genimagecolor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    auto width = get_param_int(argc, params, 0, 0);
    auto height = get_param_int(argc, params, 1, 0);
    auto color = get_param_color(argc, params, 2);
    auto image = GenImageColor(width, height, color);
    auto id = ++_nextId;
    _imageMap[id] = image;
    create_rectangle(retval, image.width, image.height, id);
  } else {
    error(retval, "GenImageColor", 3);
  }
  return result;
}

int cmd_genimagefontatlas(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto chars = get_param_str(argc, params, 0, NULL);
    // auto recs = get_param_str(argc, params, 1, NULL);
    // auto charsCount = get_param_str(argc, params, 2, NULL);
    // auto fontSize = get_param_str(argc, params, 3, NULL);
    // auto padding = get_param_str(argc, params, 4, NULL);
    // auto packMethod = get_param_str(argc, params, 5, NULL);
    // auto fnResult = GenImageFontAtlas(chars, recs, charsCount, fontSize, padding, packMethod);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GenImageFontAtlas", 6);
  }
  return result;
}

int cmd_genimagegradienth(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    auto width = get_param_int(argc, params, 0, 0);
    auto height = get_param_int(argc, params, 1, 0);
    auto left = get_param_color(argc, params, 2);
    auto right = get_param_color(argc, params, 3);
    auto image = GenImageGradientH(width, height, left, right);
    auto id = ++_nextId;
    _imageMap[id] = image;
    create_rectangle(retval, image.width, image.height, id);
  } else {
    error(retval, "GenImageGradientH", 4);
  }
  return result;
}

int cmd_genimagegradientradial(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    auto width = get_param_int(argc, params, 0, 0);
    auto height = get_param_int(argc, params, 1, 0);
    auto density = get_param_int(argc, params, 2, 0);
    auto inner = get_param_color(argc, params, 3);
    auto outer = get_param_color(argc, params, 4);
    auto image = GenImageGradientRadial(width, height, density, inner, outer);
    auto id = ++_nextId;
    _imageMap[id] = image;
    create_rectangle(retval, image.width, image.height, id);
  } else {
    error(retval, "GenImageGradientRadial", 5);
  }
  return result;
}

int cmd_genimagegradientv(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    auto width = get_param_int(argc, params, 0, 0);
    auto height = get_param_int(argc, params, 1, 0);
    auto top = get_param_color(argc, params, 2);
    auto bottom = get_param_color(argc, params, 3);
    auto image = GenImageGradientV(width, height, top, bottom);
    auto id = ++_nextId;
    _imageMap[id] = image;
    create_rectangle(retval, image.width, image.height, id);
  } else {
    error(retval, "GenImageGradientV", 4);
  }
  return result;
}

int cmd_genimageperlinnoise(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    auto width = get_param_int(argc, params, 0, 0);
    auto height = get_param_int(argc, params, 1, 0);
    auto offsetX = get_param_int(argc, params, 2, 0);
    auto offsetY = get_param_int(argc, params, 3, 0);
    auto scale = get_param_num(argc, params, 4, 0);
    auto image = GenImagePerlinNoise(width, height, offsetX, offsetY, scale);
    auto id = ++_nextId;
    _imageMap[id] = image;
    create_rectangle(retval, image.width, image.height, id);
  } else {
    error(retval, "GenImagePerlinNoise", 5);
  }
  return result;
}

int cmd_genimagewhitenoise(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    auto width = get_param_int(argc, params, 0, 0);
    auto height = get_param_int(argc, params, 1, 0);
    auto factor = get_param_int(argc, params, 2, 0);
    auto image = GenImageWhiteNoise(width, height, factor);
    auto id = ++_nextId;
    _imageMap[id] = image;
    create_rectangle(retval, image.width, image.height, id);
  } else {
    error(retval, "GenImageWhiteNoise", 3);
  }
  return result;
}

int cmd_genmeshcube(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto width = get_param_str(argc, params, 0, NULL);
    // auto height = get_param_str(argc, params, 1, NULL);
    // auto length = get_param_str(argc, params, 2, NULL);
    // auto fnResult = GenMeshCube(width, height, length);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GenMeshCube", 3);
  }
  return result;
}

int cmd_genmeshcubicmap(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto cubicmap = get_param_str(argc, params, 0, NULL);
    // auto cubeSize = get_param_str(argc, params, 1, NULL);
    // auto fnResult = GenMeshCubicmap(cubicmap, cubeSize);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GenMeshCubicmap", 2);
  }
  return result;
}

int cmd_genmeshcylinder(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto radius = get_param_num(argc, params, 0, NULL);
    // auto height = get_param_str(argc, params, 1, NULL);
    // auto slices = get_param_str(argc, params, 2, NULL);
    // auto fnResult = GenMeshCylinder(radius, height, slices);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GenMeshCylinder", 3);
  }
  return result;
}

int cmd_genmeshheightmap(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto heightmap = get_param_str(argc, params, 0, NULL);
    // auto size = get_param_str(argc, params, 1, NULL);
    // auto fnResult = GenMeshHeightmap(heightmap, size);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GenMeshHeightmap", 3);
  }
  return result;
}

int cmd_genmeshhemisphere(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto radius = get_param_num(argc, params, 0, NULL);
    // auto rings = get_param_str(argc, params, 1, NULL);
    // auto slices = get_param_str(argc, params, 2, NULL);
    // auto fnResult = GenMeshHemiSphere(radius, rings, slices);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GenMeshHemiSphere", 3);
  }
  return result;
}

int cmd_genmeshknot(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto radius = get_param_num(argc, params, 0, NULL);
    // auto size = get_param_str(argc, params, 1, NULL);
    // auto radSeg = get_param_str(argc, params, 2, NULL);
    // auto sides = get_param_str(argc, params, 3, NULL);
    // auto fnResult = GenMeshKnot(radius, size, radSeg, sides);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GenMeshKnot", 4);
  }
  return result;
}

int cmd_genmeshplane(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto width = get_param_str(argc, params, 0, NULL);
    // auto length = get_param_str(argc, params, 1, NULL);
    // auto resX = get_param_str(argc, params, 2, NULL);
    // auto resZ = get_param_str(argc, params, 3, NULL);
    // auto fnResult = GenMeshPlane(width, length, resX, resZ);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GenMeshPlane", 4);
  }
  return result;
}

int cmd_genmeshpoly(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto sides = get_param_str(argc, params, 0, NULL);
    // auto radius = get_param_num(argc, params, 1, NULL);
    // auto fnResult = GenMeshPoly(sides, radius);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GenMeshPoly", 2);
  }
  return result;
}

int cmd_genmeshsphere(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto radius = get_param_num(argc, params, 0, NULL);
    // auto rings = get_param_str(argc, params, 1, NULL);
    // auto slices = get_param_str(argc, params, 2, NULL);
    // auto fnResult = GenMeshSphere(radius, rings, slices);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GenMeshSphere", 3);
  }
  return result;
}

int cmd_genmeshtorus(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto radius = get_param_num(argc, params, 0, NULL);
    // auto size = get_param_str(argc, params, 1, NULL);
    // auto radSeg = get_param_str(argc, params, 2, NULL);
    // auto sides = get_param_str(argc, params, 3, NULL);
    // auto fnResult = GenMeshTorus(radius, size, radSeg, sides);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GenMeshTorus", 4);
  }
  return result;
}

int cmd_gentexturebrdf(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto shader = get_param_str(argc, params, 0, NULL);
    // auto size = get_param_str(argc, params, 1, NULL);
    // auto fnResult = GenTextureBRDF(shader, size);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GenTextureBRDF", 2);
  }
  return result;
}

int cmd_gentexturecubemap(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto shader = get_param_str(argc, params, 0, NULL);
    // auto panorama = get_param_str(argc, params, 1, NULL);
    // auto size = get_param_str(argc, params, 2, NULL);
    // auto format = get_param_str(argc, params, 3, NULL);
    // auto fnResult = GenTextureCubemap(shader, panorama, size, format);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GenTextureCubemap", 4);
  }
  return result;
}

int cmd_gentextureirradiance(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto shader = get_param_str(argc, params, 0, NULL);
    // auto cubemap = get_param_str(argc, params, 1, NULL);
    // auto size = get_param_str(argc, params, 2, NULL);
    // auto fnResult = GenTextureIrradiance(shader, cubemap, size);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GenTextureIrradiance", 3);
  }
  return result;
}

int cmd_gentextureprefilter(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto shader = get_param_str(argc, params, 0, NULL);
    // auto cubemap = get_param_str(argc, params, 1, NULL);
    // auto size = get_param_str(argc, params, 2, NULL);
    // auto fnResult = GenTexturePrefilter(shader, cubemap, size);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GenTexturePrefilter", 3);
  }
  return result;
}

int cmd_getcameramatrix(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto camera = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetCameraMatrix(camera);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetCameraMatrix", 1);
  }
  return result;
}

int cmd_getcameramatrix2d(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto camera = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetCameraMatrix2D(camera);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetCameraMatrix2D", 1);
  }
  return result;
}

int cmd_getclipboardtext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetClipboardText();
    v_setstr(retval, fnResult);
  } else {
    error(retval, "GetClipboardText", 0);
  }
  return result;
}

int cmd_getcodepoints(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto text = get_param_str(argc, params, 0, NULL);
    // auto count = get_param_str(argc, params, 1, NULL);
    // auto fnResult = GetCodepoints(text, count);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetCodepoints", 2);
  }
  return result;
}

int cmd_getcodepointscount(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto text = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetCodepointsCount(text);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetCodepointsCount", 1);
  }
  return result;
}

int cmd_getcollisionrayground(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto ray = get_param_str(argc, params, 0, NULL);
    // auto groundHeight = get_param_str(argc, params, 1, NULL);
    // auto fnResult = GetCollisionRayGround(ray, groundHeight);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetCollisionRayGround", 2);
  }
  return result;
}

int cmd_getcollisionraymodel(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto ray = get_param_str(argc, params, 0, NULL);
    // auto model = get_param_str(argc, params, 1, NULL);
    // auto fnResult = GetCollisionRayModel(ray, model);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetCollisionRayModel", 2);
  }
  return result;
}

int cmd_getcollisionraytriangle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto ray = get_param_str(argc, params, 0, NULL);
    // auto p1 = get_param_str(argc, params, 1, NULL);
    // auto p2 = get_param_str(argc, params, 2, NULL);
    // auto p3 = get_param_str(argc, params, 3, NULL);
    // auto fnResult = GetCollisionRayTriangle(ray, p1, p2, p3);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetCollisionRayTriangle", 4);
  }
  return result;
}

int cmd_getcollisionrec(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto rec1 = get_param_rect(argc, params, 0);
    auto rec2 = get_param_rect(argc, params, 1);
    Rectangle rect = GetCollisionRec(rec1, rec2);
    create_rectangle(retval, rect);
  } else {
    error(retval, "GetCollisionRec", 2);
  }
  return result;
}

int cmd_getcolor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto hexValue = get_param_int(argc, params, 0, 0);
    v_setint(retval, get_color_int(GetColor(hexValue)));
  } else {
    error(retval, "GetColor", 1);
  }
  return result;
}

int cmd_getdirectoryfiles(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto dirPath = get_param_str(argc, params, 0, NULL);
    // auto count = get_param_str(argc, params, 1, NULL);
    // auto fnResult = GetDirectoryFiles(dirPath, count);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetDirectoryFiles", 2);
  }
  return result;
}

int cmd_getdirectorypath(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto filePath = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetDirectoryPath(filePath);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetDirectoryPath", 1);
  }
  return result;
}

int cmd_getdroppedfiles(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto count = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetDroppedFiles(count);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetDroppedFiles", 1);
  }
  return result;
}

int cmd_getfileextension(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto fileName = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetFileExtension(fileName);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetFileExtension", 1);
  }
  return result;
}

int cmd_getfilemodtime(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto fileName = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetFileModTime(fileName);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetFileModTime", 1);
  }
  return result;
}

int cmd_getfilename(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto filePath = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetFileName(filePath);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetFileName", 1);
  }
  return result;
}

int cmd_getfilenamewithoutext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto filePath = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetFileNameWithoutExt(filePath);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetFileNameWithoutExt", 1);
  }
  return result;
}

int cmd_getfontdefault(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetFontDefault();
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetFontDefault", 0);
  }
  return result;
}

int cmd_getfps(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetFPS();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetFPS", 0);
  }
  return result;
}

int cmd_getframetime(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetFrameTime();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetFrameTime", 0);
  }
  return result;
}

int cmd_getgamepadaxiscount(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto gamepad = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetGamepadAxisCount(gamepad);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetGamepadAxisCount", 1);
  }
  return result;
}

int cmd_getgamepadaxismovement(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto gamepad = get_param_str(argc, params, 0, NULL);
    // auto axis = get_param_str(argc, params, 1, NULL);
    // auto fnResult = GetGamepadAxisMovement(gamepad, axis);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetGamepadAxisMovement", 2);
  }
  return result;
}

int cmd_getgamepadbuttonpressed(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetGamepadButtonPressed();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetGamepadButtonPressed", 0);
  }
  return result;
}

int cmd_getgamepadname(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto gamepad = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetGamepadName(gamepad);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetGamepadName", 1);
  }
  return result;
}

int cmd_getgesturedetected(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    v_setint(retval, GetGestureDetected());
  } else {
    error(retval, "GetGestureDetected", 0);
  }
  return result;
}

int cmd_getgesturedragangle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    v_setreal(retval, GetGestureDragAngle());
  } else {
    error(retval, "GetGestureDragAngle", 0);
  }
  return result;
}

int cmd_getgesturedragvector(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetGestureDragVector();
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetGestureDragVector", 0);
  }
  return result;
}

int cmd_getgestureholdduration(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetGestureHoldDuration();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetGestureHoldDuration", 0);
  }
  return result;
}

int cmd_getgesturepinchangle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetGesturePinchAngle();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetGesturePinchAngle", 0);
  }
  return result;
}

int cmd_getgesturepinchvector(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetGesturePinchVector();
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetGesturePinchVector", 0);
  }
  return result;
}

int cmd_getglyphindex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto font = get_param_str(argc, params, 0, NULL);
    // auto codepoint = get_param_str(argc, params, 1, NULL);
    // auto fnResult = GetGlyphIndex(font, codepoint);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetGlyphIndex", 2);
  }
  return result;
}

int cmd_getimagealphaborder(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // auto threshold = get_param_str(argc, params, 1, NULL);
    // auto fnResult = GetImageAlphaBorder(image, threshold);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetImageAlphaBorder", 2);
  }
  return result;
}

int cmd_getimagedata(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    int id = get_image_id(argc, params, 0);
    if (id != -1) {
      auto fnResult = GetImageData(_imageMap.at(id));
      v_setint(retval, (var_int_t)fnResult);
    } else {
      error_image(retval);
      result = 0;
    }
  } else {
    error(retval, "GetImageData", 1);
  }
  return result;
}

int cmd_getimagedatanormalized(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetImageDataNormalized(image);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetImageDataNormalized", 1);
  }
  return result;
}

int cmd_getimagepalette(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // auto maxPaletteSize = get_param_str(argc, params, 1, NULL);
    // auto extractCount = get_param_str(argc, params, 2, NULL);
    // auto fnResult = GetImagePalette(image, maxPaletteSize, extractCount);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetImagePalette", 3);
  }
  return result;
}

int cmd_getkeypressed(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetKeyPressed();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetKeyPressed", 0);
  }
  return result;
}

int cmd_getmatrixmodelview(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetMatrixModelview();
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetMatrixModelview", 0);
  }
  return result;
}

int cmd_getmatrixprojection(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetMatrixProjection();
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetMatrixProjection", 0);
  }
  return result;
}

int cmd_getmonitorcount(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetMonitorCount();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetMonitorCount", 0);
  }
  return result;
}

int cmd_getmonitorheight(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto monitor = get_param_int(argc, params, 0, 0);
    auto fnResult = GetMonitorHeight(monitor);
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetMonitorHeight", 1);
  }
  return result;
}

int cmd_getmonitorname(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto monitor = get_param_int(argc, params, 0, 0);
    auto fnResult = GetMonitorName(monitor);
    v_setstr(retval, fnResult);
  } else {
    error(retval, "GetMonitorName", 1);
  }
  return result;
}

int cmd_getmonitorphysicalheight(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto monitor = get_param_int(argc, params, 0, 0);
    auto fnResult = GetMonitorPhysicalHeight(monitor);
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetMonitorPhysicalHeight", 1);
  }
  return result;
}

int cmd_getmonitorphysicalwidth(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto monitor = get_param_int(argc, params, 0, 0);
    auto fnResult = GetMonitorPhysicalWidth(monitor);
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetMonitorPhysicalWidth", 1);
  }
  return result;
}

int cmd_getmonitorrefreshrate(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto monitor = get_param_int(argc, params, 0, 0);
    auto fnResult = GetMonitorRefreshRate(monitor);
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetMonitorRefreshRate", 1);
  }
  return result;
}

int cmd_getmonitorwidth(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto monitor = get_param_int(argc, params, 0, 0);
    auto fnResult = GetMonitorWidth(monitor);
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetMonitorWidth", 1);
  }
  return result;
}

int cmd_getmouseposition(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto position = GetMousePosition();
    map_init(retval);
    v_setint(map_add_var(retval, "x", 0), position.x);
    v_setint(map_add_var(retval, "y", 0), position.y);
  } else {
    error(retval, "GetMousePosition", 0);
  }
  return result;
}

int cmd_getmouseray(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto mousePosition = get_param_str(argc, params, 0, NULL);
    // auto camera = get_param_str(argc, params, 1, NULL);
    // auto fnResult = GetMouseRay(mousePosition, camera);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetMouseRay", 2);
  }
  return result;
}

int cmd_getmousewheelmove(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    v_setreal(retval, GetMouseWheelMove());
  } else {
    error(retval, "GetMouseWheelMove", 0);
  }
  return result;
}

int cmd_getmousex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    v_setint(retval, GetMouseX());
  } else {
    error(retval, "GetMouseX", 0);
  }
  return result;
}

int cmd_getmousey(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    v_setint(retval, GetMouseY());
  } else {
    error(retval, "GetMouseY", 0);
  }
  return result;
}

int cmd_getmusictimelength(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    auto fnResult = GetMusicTimeLength(music);
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetMusicTimeLength", 1);
  }
  return result;
}

int cmd_getmusictimeplayed(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    auto fnResult = GetMusicTimePlayed(music);
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetMusicTimePlayed", 1);
  }
  return result;
}

int cmd_getnextcodepoint(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto text = get_param_str(argc, params, 0, NULL);
    // auto bytesProcessed = get_param_str(argc, params, 1, NULL);
    // auto fnResult = GetNextCodepoint(text, bytesProcessed);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetNextCodepoint", 2);
  }
  return result;
}

int cmd_getpixelcolor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto srcPtr = get_param_str(argc, params, 0, NULL);
    // auto format = get_param_str(argc, params, 1, NULL);
    // auto fnResult = GetPixelColor(srcPtr, format);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetPixelColor", 2);
  }
  return result;
}

int cmd_getpixeldatasize(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto width = get_param_str(argc, params, 0, NULL);
    // auto height = get_param_str(argc, params, 1, NULL);
    // auto format = get_param_str(argc, params, 2, NULL);
    // auto fnResult = GetPixelDataSize(width, height, format);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetPixelDataSize", 3);
  }
  return result;
}

int cmd_getprevdirectorypath(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto dirPath = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetPrevDirectoryPath(dirPath);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetPrevDirectoryPath", 1);
  }
  return result;
}

int cmd_getrandomvalue(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto min = get_param_str(argc, params, 0, NULL);
    // auto max = get_param_str(argc, params, 1, NULL);
    // auto fnResult = GetRandomValue(min, max);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetRandomValue", 2);
  }
  return result;
}

int cmd_getscreendata(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetScreenData();
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetScreenData", 0);
  }
  return result;
}

int cmd_getscreenheight(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetScreenHeight();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetScreenHeight", 0);
  }
  return result;
}

int cmd_getscreentoworld2d(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto position = get_param_str(argc, params, 0, NULL);
    // auto camera = get_param_str(argc, params, 1, NULL);
    // auto fnResult = GetScreenToWorld2D(position, camera);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetScreenToWorld2D", 2);
  }
  return result;
}

int cmd_getscreenwidth(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetScreenWidth();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetScreenWidth", 0);
  }
  return result;
}

int cmd_getshaderdefault(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetShaderDefault();
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetShaderDefault", 0);
  }
  return result;
}

int cmd_getshaderlocation(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto shader = get_param_str(argc, params, 0, NULL);
    // auto uniformName = get_param_str(argc, params, 1, NULL);
    // auto fnResult = GetShaderLocation(shader, uniformName);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetShaderLocation", 2);
  }
  return result;
}

int cmd_getshaderlocationattrib(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto shader = get_param_str(argc, params, 0, NULL);
    // auto attribName = get_param_str(argc, params, 1, NULL);
    // auto fnResult = GetShaderLocationAttrib(shader, attribName);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetShaderLocationAttrib", 2);
  }
  return result;
}

int cmd_getshapestexture(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetShapesTexture();
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetShapesTexture", 0);
  }
  return result;
}

int cmd_getshapestexturerec(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetShapesTextureRec();
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetShapesTextureRec", 0);
  }
  return result;
}

int cmd_getsoundsplaying(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetSoundsPlaying();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetSoundsPlaying", 0);
  }
  return result;
}

int cmd_gettexturedata(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    int id = get_texture_id(argc, params, 0);
    if (id != -1) {
      Image image = GetTextureData(_textureMap.at(id));
      id = ++_nextId;
      _imageMap[id] = image;
      create_rectangle(retval, image.width, image.height, id);
    } else {
      error_texture(retval);
      result = 0;
    }
  } else {
    error(retval, "GetTextureData", 1);
  }
  return result;
}

int cmd_gettexturedefault(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetTextureDefault();
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetTextureDefault", 0);
  }
  return result;
}

int cmd_gettime(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetTime();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetTime", 0);
  }
  return result;
}

int cmd_gettouchpointscount(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetTouchPointsCount();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetTouchPointsCount", 0);
  }
  return result;
}

int cmd_gettouchposition(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto index = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetTouchPosition(index);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetTouchPosition", 1);
  }
  return result;
}

int cmd_gettouchx(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetTouchX();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetTouchX", 0);
  }
  return result;
}

int cmd_gettouchy(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetTouchY();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GetTouchY", 0);
  }
  return result;
}

int cmd_getwavedata(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto wave = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetWaveData(wave);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetWaveData", 1);
  }
  return result;
}

int cmd_getwindowposition(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetWindowPosition();
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetWindowPosition", 0);
  }
  return result;
}

int cmd_getwindowscaledpi(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetWindowScaleDPI();
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetWindowScaleDPI", 0);
  }
  return result;
}

int cmd_getworkingdirectory(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetWorkingDirectory();
    v_setstr(retval, fnResult);
  } else {
    error(retval, "GetWorkingDirectory", 0);
  }
  return result;
}

int cmd_getworldtoscreen(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto position = get_param_str(argc, params, 0, NULL);
    // auto camera = get_param_str(argc, params, 1, NULL);
    // auto fnResult = GetWorldToScreen(position, camera);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetWorldToScreen", 2);
  }
  return result;
}

int cmd_getworldtoscreen2d(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto position = get_param_str(argc, params, 0, NULL);
    // auto camera = get_param_str(argc, params, 1, NULL);
    // auto fnResult = GetWorldToScreen2D(position, camera);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetWorldToScreen2D", 2);
  }
  return result;
}

int cmd_getworldtoscreenex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto position = get_param_str(argc, params, 0, NULL);
    // auto camera = get_param_str(argc, params, 1, NULL);
    // auto width = get_param_str(argc, params, 2, NULL);
    // auto height = get_param_str(argc, params, 3, NULL);
    // auto fnResult = GetWorldToScreenEx(position, camera, width, height);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GetWorldToScreenEx", 4);
  }
  return result;
}

int cmd_imagecopy(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // auto fnResult = ImageCopy(image);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "ImageCopy", 1);
  }
  return result;
}

int cmd_imagefromimage(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // auto rec = get_param_str(argc, params, 1, NULL);
    // auto fnResult = ImageFromImage(image, rec);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "ImageFromImage", 2);
  }
  return result;
}

int cmd_imagetext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto text = get_param_str(argc, params, 0, NULL);
    // auto fontSize = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_color(argc, params, 2, NULL);
    // auto fnResult = ImageText(text, fontSize, color);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "ImageText", 3);
  }
  return result;
}

int cmd_imagetextex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto font = get_param_str(argc, params, 0, NULL);
    // auto text = get_param_str(argc, params, 1, NULL);
    // auto fontSize = get_param_str(argc, params, 2, NULL);
    // auto spacing = get_param_str(argc, params, 3, NULL);
    // auto tint = get_param_str(argc, params, 4, NULL);
    // auto fnResult = ImageTextEx(font, text, fontSize, spacing, tint);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "ImageTextEx", 5);
  }
  return result;
}

int cmd_initaudiostream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto sampleRate = get_param_str(argc, params, 0, NULL);
    // auto sampleSize = get_param_str(argc, params, 1, NULL);
    // auto channels = get_param_str(argc, params, 2, NULL);
    // auto fnResult = InitAudioStream(sampleRate, sampleSize, channels);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "InitAudioStream", 3);
  }
  return result;
}

int cmd_isaudiodeviceready(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsAudioDeviceReady();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsAudioDeviceReady", 0);
  }
  return result;
}

int cmd_isaudiostreamplaying(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    //auto stream = get_param_int(argc, params, 0, 0);
    //auto fnResult = IsAudioStreamPlaying(stream);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "IsAudioStreamPlaying", 1);
  }
  return result;
}

int cmd_isaudiostreamprocessed(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    //auto stream = get_param_str(argc, params, 0, NULL);
    //auto fnResult = IsAudioStreamProcessed(stream);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "IsAudioStreamProcessed", 1);
  }
  return result;
}

int cmd_iscursorhidden(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsCursorHidden();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsCursorHidden", 0);
  }
  return result;
}

int cmd_iscursoronscreen(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsCursorOnScreen();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsCursorOnScreen", 0);
  }
  return result;
}

int cmd_isfiledropped(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsFileDropped();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsFileDropped", 0);
  }
  return result;
}

int cmd_isfileextension(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto fileName = get_param_str(argc, params, 0, NULL);
    auto ext = get_param_str(argc, params, 1, NULL);
    auto fnResult = IsFileExtension(fileName, ext);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsFileExtension", 2);
  }
  return result;
}

int cmd_isgamepadavailable(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto gamepad = get_param_int(argc, params, 0, 0);
    auto fnResult = IsGamepadAvailable(gamepad);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsGamepadAvailable", 1);
  }
  return result;
}

int cmd_isgamepadbuttondown(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto gamepad = get_param_int(argc, params, 0, 0);
    auto button = get_param_int(argc, params, 1, 0);
    auto fnResult = IsGamepadButtonDown(gamepad, button);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsGamepadButtonDown", 2);
  }
  return result;
}

int cmd_isgamepadbuttonpressed(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto gamepad = get_param_int(argc, params, 0, 0);
    auto button = get_param_int(argc, params, 1, 0);
    auto fnResult = IsGamepadButtonPressed(gamepad, button);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsGamepadButtonPressed", 2);
  }
  return result;
}

int cmd_isgamepadbuttonreleased(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto gamepad = get_param_int(argc, params, 0, 0);
    auto button = get_param_int(argc, params, 1, 0);
    auto fnResult = IsGamepadButtonReleased(gamepad, button);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsGamepadButtonReleased", 2);
  }
  return result;
}

int cmd_isgamepadbuttonup(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto gamepad = get_param_int(argc, params, 0, 0);
    auto button = get_param_int(argc, params, 1, 0);
    auto fnResult = IsGamepadButtonUp(gamepad, button);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsGamepadButtonUp", 2);
  }
  return result;
}

int cmd_isgamepadname(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto gamepad = get_param_int(argc, params, 0, 0);
    auto name = get_param_str(argc, params, 1, NULL);
    auto fnResult = IsGamepadName(gamepad, name);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsGamepadName", 2);
  }
  return result;
}

int cmd_isgesturedetected(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto gesture = get_param_int(argc, params, 0, 0);
    auto fnResult = IsGestureDetected(gesture);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsGestureDetected", 1);
  }
  return result;
}

int cmd_iskeydown(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto key = get_param_int(argc, params, 0, 0);
    auto fnResult = IsKeyDown(key);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsKeyDown", 1);
  }
  return result;
}

int cmd_iskeypressed(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto key = get_param_int(argc, params, 0, 0);
    auto fnResult = IsKeyPressed(key);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsKeyPressed", 1);
  }
  return result;
}

int cmd_iskeyreleased(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto key = get_param_int(argc, params, 0, 0);
    auto fnResult = IsKeyReleased(key);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsKeyReleased", 1);
  }
  return result;
}

int cmd_iskeyup(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto key = get_param_int(argc, params, 0, 0);
    auto fnResult = IsKeyUp(key);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsKeyUp", 1);
  }
  return result;
}

int cmd_ismodelanimationvalid(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    //auto model = get_param_str(argc, params, 0, NULL);
    //auto anim = get_param_str(argc, params, 1, NULL);
    //auto fnResult = IsModelAnimationValid(model, anim);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "IsModelAnimationValid", 2);
  }
  return result;
}

int cmd_ismousebuttondown(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto button = get_param_int(argc, params, 0, 0);
    auto fnResult = IsMouseButtonDown(button);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsMouseButtonDown", 1);
  }
  return result;
}

int cmd_ismousebuttonpressed(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto button = get_param_int(argc, params, 0, 0);
    auto fnResult = IsMouseButtonPressed(button);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsMouseButtonPressed", 1);
  }
  return result;
}

int cmd_ismousebuttonreleased(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto button = get_param_int(argc, params, 0, 0);
    auto fnResult = IsMouseButtonReleased(button);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsMouseButtonReleased", 1);
  }
  return result;
}

int cmd_ismousebuttonup(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto button = get_param_int(argc, params, 0, 0);
    auto fnResult = IsMouseButtonUp(button);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsMouseButtonUp", 1);
  }
  return result;
}

int cmd_ismusicplaying(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    auto fnResult = IsMusicPlaying(music);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsMusicPlaying", 1);
  }
  return result;
}

int cmd_issoundplaying(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    auto fnResult = IsSoundPlaying(sound);
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsSoundPlaying", 1);
  }
  return result;
}

int cmd_isvrsimulatorready(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsVrSimulatorReady();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsVrSimulatorReady", 0);
  }
  return result;
}

int cmd_iswindowfocused(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsWindowFocused();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsWindowFocused", 0);
  }
  return result;
}

int cmd_iswindowfullscreen(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsWindowFullscreen();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsWindowFullscreen", 0);
  }
  return result;
}

int cmd_iswindowhidden(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsWindowHidden();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsWindowHidden", 0);
  }
  return result;
}

int cmd_iswindowmaximized(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsWindowMaximized();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsWindowMaximized", 0);
  }
  return result;
}

int cmd_iswindowminimized(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsWindowMinimized();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsWindowMinimized", 0);
  }
  return result;
}

int cmd_iswindowready(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsWindowReady();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsWindowReady", 0);
  }
  return result;
}

int cmd_iswindowresized(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsWindowResized();
    v_setint(retval, fnResult);
  } else {
    error(retval, "IsWindowResized", 0);
  }
  return result;
}

int cmd_loadfiledata(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto fileName = get_param_str(argc, params, 0, NULL);
    // auto bytesRead = get_param_str(argc, params, 1, NULL);
    // auto fnResult = LoadFileData(fileName, bytesRead);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "LoadFileData", 2);
  }
  return result;
}

int cmd_loadfiletext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto fileName = get_param_str(argc, params, 0, NULL);
    // auto fnResult = LoadFileText(fileName);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "LoadFileText", 1);
  }
  return result;
}

int cmd_loadfont(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto fileName = get_param_str(argc, params, 0, NULL);
    // auto fnResult = LoadFont(fileName);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "LoadFont", 1);
  }
  return result;
}

int cmd_loadfontdata(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto fileData = get_param_str(argc, params, 0, NULL);
    // auto dataSize = get_param_str(argc, params, 1, NULL);
    // auto fontSize = get_param_str(argc, params, 2, NULL);
    // auto fontChars = get_param_str(argc, params, 3, NULL);
    // auto charsCount = get_param_str(argc, params, 4, NULL);
    // auto type = get_param_str(argc, params, 5, NULL);
    // auto fnResult = LoadFontData(fileData, dataSize, fontSize, fontChars, charsCount, type);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "LoadFontData", 6);
  }
  return result;
}

int cmd_loadfontex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto fileName = get_param_str(argc, params, 0, NULL);
    // auto fontSize = get_param_str(argc, params, 1, NULL);
    // auto fontChars = get_param_str(argc, params, 2, NULL);
    // auto charsCount = get_param_str(argc, params, 3, NULL);
    // auto fnResult = LoadFontEx(fileName, fontSize, fontChars, charsCount);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "LoadFontEx", 4);
  }
  return result;
}

int cmd_loadfontfromimage(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // auto key = get_param_str(argc, params, 1, NULL);
    // auto firstChar = get_param_str(argc, params, 2, NULL);
    // auto fnResult = LoadFontFromImage(image, key, firstChar);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "LoadFontFromImage", 3);
  }
  return result;
}

int cmd_loadfontfrommemory(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto fileType = get_param_str(argc, params, 0, NULL);
    // auto fileData = get_param_str(argc, params, 1, NULL);
    // auto dataSize = get_param_str(argc, params, 2, NULL);
    // auto fontSize = get_param_str(argc, params, 3, NULL);
    // auto fontChars = get_param_str(argc, params, 4, NULL);
    // auto charsCount = get_param_str(argc, params, 5, NULL);
    // auto fnResult = LoadFontFromMemory(fileType, fileData, dataSize, fontSize, fontChars, charsCount);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "LoadFontFromMemory", 6);
  }
  return result;
}

int cmd_loadimage(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto fileName = get_param_str(argc, params, 0, NULL);
    auto image = LoadImage(fileName);
    auto id = ++_nextId;
    _imageMap[id] = image;
    create_rectangle(retval, image.width, image.height, id);
  } else {
    error(retval, "LoadImage", 1);
  }
  return result;
}

int cmd_loadimageanim(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto fileName = get_param_str(argc, params, 0, NULL);
    // auto frames = get_param_str(argc, params, 1, NULL);
    // auto fnResult = LoadImageAnim(fileName, frames);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "LoadImageAnim", 2);
  }
  return result;
}

int cmd_loadimagefrommemory(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto fileType = get_param_str(argc, params, 0, NULL);
    // auto fileData = get_param_str(argc, params, 1, NULL);
    // auto dataSize = get_param_str(argc, params, 2, NULL);
    // auto fnResult = LoadImageFromMemory(fileType, fileData, dataSize);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "LoadImageFromMemory", 3);
  }
  return result;
}

int cmd_loadimageraw(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto fileName = get_param_str(argc, params, 0, NULL);
    // auto width = get_param_str(argc, params, 1, NULL);
    // auto height = get_param_str(argc, params, 2, NULL);
    // auto format = get_param_str(argc, params, 3, NULL);
    // auto headerSize = get_param_str(argc, params, 4, NULL);
    // auto fnResult = LoadImageRaw(fileName, width, height, format, headerSize);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "LoadImageRaw", 5);
  }
  return result;
}

int cmd_loadmaterialdefault(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = LoadMaterialDefault();
    // v_setint(retval, fnResult);
  } else {
    error(retval, "LoadMaterialDefault", 0);
  }
  return result;
}

int cmd_loadmaterials(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto fileName = get_param_str(argc, params, 0, NULL);
    // auto materialCount = get_param_str(argc, params, 1, NULL);
    // auto fnResult = LoadMaterials(fileName, materialCount);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "LoadMaterials", 2);
  }
  return result;
}

int cmd_loadmeshes(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto fileName = get_param_str(argc, params, 0, NULL);
    // auto meshCount = get_param_str(argc, params, 1, NULL);
    // auto fnResult = LoadMeshes(fileName, meshCount);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "LoadMeshes", 2);
  }
  return result;
}

int cmd_loadmodel(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto fileName = get_param_str(argc, params, 0, NULL);
    auto model = LoadModel(fileName);
    auto id = ++_nextId;
    _modelMap[id] = model;

    map_init(retval);
    v_setint(map_add_var(retval, "meshCount", 0), model.meshCount);
    v_setint(map_add_var(retval, "materialCount", 0), model.materialCount);
    v_setint(map_add_var(retval, "boneCount", 0), model.boneCount);
    v_setint(map_add_var(retval, mapID, 0), id);
  } else {
    error(retval, "LoadModel", 1);
  }
  return result;
}

int cmd_loadmodelanimations(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto fileName = get_param_str(argc, params, 0, NULL);
    // auto animsCount = get_param_str(argc, params, 1, NULL);
    // auto fnResult = LoadModelAnimations(fileName, animsCount);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "LoadModelAnimations", 2);
  }
  return result;
}

int cmd_loadmodelfrommesh(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto mesh = get_param_str(argc, params, 0, NULL);
    // auto fnResult = LoadModelFromMesh(mesh);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "LoadModelFromMesh", 1);
  }
  return result;
}

int cmd_loadmusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto fileName = get_param_str(argc, params, 0, NULL);
    int fnResult = ++_nextId;
    _musicMap[fnResult] = LoadMusicStream(fileName);
    v_setint(retval, fnResult);
  } else {
    error(retval, "LoadMusicStream", 1);
  }
  return result;
}

int cmd_loadrendertexture(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
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
    create_rectangle(texture, renderTexture.texture.width, renderTexture.texture.height, textureId);
  } else {
    error(retval, "LoadRenderTexture", 2);
  }
  return result;
}

int cmd_loadshader(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto vsFileName = get_param_str(argc, params, 0, NULL);
    auto fsFileName = get_param_str(argc, params, 1, NULL);
    Shader shader;
    if (vsFileName[0] == '0' && vsFileName[1] == '\0') {
      shader = LoadShader(0, fsFileName);
    } else {
      shader = LoadShader(vsFileName, fsFileName);
    }
    map_init(retval);
    v_setint(map_add_var(retval, "locs", 0), (var_int_t)shader.locs);
    v_setint(map_add_var(retval, mapID, 0), shader.id);
  } else {
    error(retval, "LoadShader", 2);
  }
  return result;
}

int cmd_loadshadercode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto vsCode = get_param_str(argc, params, 0, NULL);
    // auto fsCode = get_param_str(argc, params, 1, NULL);
    // auto fnResult = LoadShaderCode(vsCode, fsCode);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "LoadShaderCode", 2);
  }
  return result;
}

int cmd_loadsound(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto fileName = get_param_str(argc, params, 0, NULL);
    int fnResult = ++_nextId;
    _soundMap[fnResult] = LoadSound(fileName);
    v_setint(retval, fnResult);
  } else {
    error(retval, "LoadSound", 1);
  }
  return result;
}

int cmd_loadsoundfromwave(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto wave = get_param_str(argc, params, 0, NULL);
    // auto fnResult = LoadSoundFromWave(wave);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "LoadSoundFromWave", 1);
  }
  return result;
}

int cmd_loadstoragevalue(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto position = get_param_str(argc, params, 0, NULL);
    // auto fnResult = LoadStorageValue(position);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "LoadStorageValue", 1);
  }
  return result;
}

int cmd_loadtexture(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto fileName = get_param_str(argc, params, 0, NULL);
    Texture2D texture = LoadTexture(fileName);
    int id = ++_nextId;
    _textureMap[id] = texture;
    create_rectangle(retval, texture.width, texture.height, id);
  } else {
    error(retval, "LoadTexture", 1);
  }
  return result;
}

int cmd_loadtexturecubemap(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // auto layoutType = get_param_str(argc, params, 1, NULL);
    // auto fnResult = LoadTextureCubemap(image, layoutType);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "LoadTextureCubemap", 2);
  }
  return result;
}

int cmd_loadtexturefromimage(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    int id = get_image_id(argc, params, 0);
    if (id != -1) {
      auto texture = LoadTextureFromImage(_imageMap.at(id));
      id = ++_nextId;
      _textureMap[id] = texture;
      create_rectangle(retval, texture.width, texture.height, id);
    } else {
      error_image(retval);
      result = 0;
    }
  } else {
    error(retval, "LoadTextureFromImage", 1);
  }
  return result;
}

int cmd_loadwave(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto fileName = get_param_str(argc, params, 0, NULL);
    // auto fnResult = LoadWave(fileName);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "LoadWave", 1);
  }
  return result;
}

int cmd_loadwavefrommemory(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto fileType = get_param_str(argc, params, 0, NULL);
    // auto fileData = get_param_str(argc, params, 1, NULL);
    // auto dataSize = get_param_str(argc, params, 2, NULL);
    // auto fnResult = LoadWaveFromMemory(fileType, fileData, dataSize);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "LoadWaveFromMemory", 3);
  }
  return result;
}

int cmd_measuretext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto text = get_param_str(argc, params, 0, NULL);
    auto fontSize = get_param_int(argc, params, 1, 0);
    auto fnResult = MeasureText(text, fontSize);
    v_setint(retval, fnResult);
  } else {
    error(retval, "MeasureText", 2);
  }
  return result;
}

int cmd_measuretextex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto font = get_param_str(argc, params, 0, NULL);
    // auto text = get_param_str(argc, params, 1, NULL);
    // auto fontSize = get_param_str(argc, params, 2, NULL);
    // auto spacing = get_param_str(argc, params, 3, NULL);
    // auto fnResult = MeasureTextEx(font, text, fontSize, spacing);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "MeasureTextEx", 4);
  }
  return result;
}

int cmd_meshboundingbox(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto mesh = get_param_str(argc, params, 0, NULL);
    // auto fnResult = MeshBoundingBox(mesh);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "MeshBoundingBox", 1);
  }
  return result;
}

int cmd_textcopy(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto src = get_param_str(argc, params, 1, NULL);
    // auto fnResult = TextCopy(dst, src);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "TextCopy", 2);
  }
  return result;
}

int cmd_textfindindex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto text = get_param_str(argc, params, 0, NULL);
    // auto find = get_param_str(argc, params, 1, NULL);
    // auto fnResult = TextFindIndex(text, find);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "TextFindIndex", 2);
  }
  return result;
}

int cmd_textformat(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto text = get_param_str(argc, params, 0, NULL);
    // auto fnResult = TextFormat(text);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "TextFormat", 1);
  }
  return result;
}

int cmd_textinsert(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto text = get_param_str(argc, params, 0, NULL);
    // auto insert = get_param_str(argc, params, 1, NULL);
    // auto position = get_param_str(argc, params, 2, NULL);
    // auto fnResult = TextInsert(text, insert, position);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "TextInsert", 3);
  }
  return result;
}

int cmd_textisequal(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto text1 = get_param_str(argc, params, 0, NULL);
    // auto text2 = get_param_str(argc, params, 1, NULL);
    // auto fnResult = TextIsEqual(text1, text2);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "TextIsEqual", 2);
  }
  return result;
}

int cmd_textjoin(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto textList = get_param_str(argc, params, 0, NULL);
    // auto count = get_param_str(argc, params, 1, NULL);
    // auto delimiter = get_param_str(argc, params, 2, NULL);
    // auto fnResult = TextJoin(textList, count, delimiter);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "TextJoin", 3);
  }
  return result;
}

int cmd_textlength(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto text = get_param_str(argc, params, 0, NULL);
    // auto fnResult = TextLength(text);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "TextLength", 1);
  }
  return result;
}

int cmd_textreplace(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto text = get_param_str(argc, params, 0, NULL);
    // auto replace = get_param_str(argc, params, 1, NULL);
    // auto by = get_param_str(argc, params, 2, NULL);
    // auto fnResult = TextReplace(text, replace, by);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "TextReplace", 3);
  }
  return result;
}

int cmd_textsplit(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto text = get_param_str(argc, params, 0, NULL);
    // auto delimiter = get_param_str(argc, params, 1, NULL);
    // auto count = get_param_str(argc, params, 2, NULL);
    // auto fnResult = TextSplit(text, delimiter, count);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "TextSplit", 3);
  }
  return result;
}

int cmd_textsubtext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto text = get_param_str(argc, params, 0, NULL);
    // auto position = get_param_str(argc, params, 1, NULL);
    // auto length = get_param_str(argc, params, 2, NULL);
    // auto fnResult = TextSubtext(text, position, length);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "TextSubtext", 3);
  }
  return result;
}

int cmd_texttointeger(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto text = get_param_str(argc, params, 0, NULL);
    // auto fnResult = TextToInteger(text);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "TextToInteger", 1);
  }
  return result;
}

int cmd_texttolower(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto text = get_param_str(argc, params, 0, NULL);
    // auto fnResult = TextToLower(text);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "TextToLower", 1);
  }
  return result;
}

int cmd_texttopascal(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto text = get_param_str(argc, params, 0, NULL);
    // auto fnResult = TextToPascal(text);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "TextToPascal", 1);
  }
  return result;
}

int cmd_texttoupper(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto text = get_param_str(argc, params, 0, NULL);
    // auto fnResult = TextToUpper(text);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "TextToUpper", 1);
  }
  return result;
}

int cmd_texttoutf8(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto codepoints = get_param_str(argc, params, 0, NULL);
    // auto length = get_param_str(argc, params, 1, NULL);
    // auto fnResult = TextToUtf8(codepoints, length);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "TextToUtf8", 2);
  }
  return result;
}

int cmd_wavecopy(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto wave = get_param_str(argc, params, 0, NULL);
    // auto fnResult = WaveCopy(wave);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "WaveCopy", 1);
  }
  return result;
}

int cmd_windowshouldclose(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = WindowShouldClose();
    v_setint(retval, fnResult);
  } else {
    error(retval, "WindowShouldClose", 0);
  }
  return result;
}

int cmd_beginblendmode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto mode = get_param_int(argc, params, 0, 0);
    BeginBlendMode(mode);
  } else {
    error(retval, "BeginBlendMode", 1);
  }
  return result;
}

int cmd_begindrawing(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    BeginDrawing();
  } else {
    error(retval, "BeginDrawing", 0);
  }
  return result;
}

int cmd_beginmode2d(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto camera = get_param_str(argc, params, 0, NULL);
    // BeginMode2D(camera);
  } else {
    error(retval, "BeginMode2D", 1);
  }
  return result;
}

int cmd_beginmode3d(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    BeginMode3D(get_camera_3d(argc, params, 0));
  } else {
    error(retval, "BeginMode3D", 1);
  }
  return result;
}

int cmd_beginscissormode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    auto x = get_param_int(argc, params, 0, 0);
    auto y = get_param_int(argc, params, 1, 0);
    auto width = get_param_int(argc, params, 2, 0);
    auto height = get_param_int(argc, params, 3, 0);
    BeginScissorMode(x, y, width, height);
  } else {
    error(retval, "BeginScissorMode", 4);
  }
  return result;
}

int cmd_beginshadermode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    BeginShaderMode(get_param_shader(argc, params, 0));
  } else {
    error(retval, "BeginShaderMode", 1);
  }
  return result;
}

int cmd_begintexturemode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    int id = get_render_texture_id(argc, params, 0);
    if (id != -1) {
      BeginTextureMode(_renderMap.at(id));
    } else {
      error_texture(retval);
      result = 0;
    }
  } else {
    error(retval, "BeginTextureMode", 1);
  }
  return result;
}

int cmd_beginvrdrawing(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    BeginVrDrawing();
  } else {
    error(retval, "BeginVrDrawing", 0);
  }
  return result;
}

int cmd_clearbackground(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto color = get_param_color(argc, params, 0);
    ClearBackground(color);
  } else {
    error(retval, "ClearBackground", 1);
  }
  return result;
}

int cmd_cleardirectoryfiles(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    ClearDirectoryFiles();
  } else {
    error(retval, "ClearDirectoryFiles", 0);
  }
  return result;
}

int cmd_cleardroppedfiles(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    ClearDroppedFiles();
  } else {
    error(retval, "ClearDroppedFiles", 0);
  }
  return result;
}

int cmd_closeaudiodevice(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    CloseAudioDevice();
  } else {
    error(retval, "CloseAudioDevice", 0);
  }
  return result;
}

int cmd_closeaudiostream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto stream = get_param_str(argc, params, 0, NULL);
    // CloseAudioStream(stream);
  } else {
    error(retval, "CloseAudioStream", 1);
  }
  return result;
}

int cmd_closevrsimulator(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    CloseVrSimulator();
  } else {
    error(retval, "CloseVrSimulator", 0);
  }
  return result;
}

int cmd_closewindow(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    CloseWindow();
  } else {
    error(retval, "CloseWindow", 0);
  }
  return result;
}

int cmd_decoratewindow(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    DecorateWindow();
  } else {
    error(retval, "DecorateWindow", 0);
  }
  return result;
}

int cmd_disablecursor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    DisableCursor();
  } else {
    error(retval, "DisableCursor", 0);
  }
  return result;
}

int cmd_drawbillboard(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto camera = get_param_str(argc, params, 0, NULL);
    // auto texture = get_param_str(argc, params, 1, NULL);
    // auto center = get_param_str(argc, params, 2, NULL);
    // auto size = get_param_str(argc, params, 3, NULL);
    // auto tint = get_param_str(argc, params, 4, NULL);
    // DrawBillboard(camera, texture, center, size, tint);
  } else {
    error(retval, "DrawBillboard", 5);
  }
  return result;
}

int cmd_drawbillboardrec(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto camera = get_param_str(argc, params, 0, NULL);
    // auto texture = get_param_str(argc, params, 1, NULL);
    // auto sourceRec = get_param_str(argc, params, 2, NULL);
    // auto center = get_param_str(argc, params, 3, NULL);
    // auto size = get_param_str(argc, params, 4, NULL);
    // auto tint = get_param_str(argc, params, 5, NULL);
    // DrawBillboardRec(camera, texture, sourceRec, center, size, tint);
  } else {
    error(retval, "DrawBillboardRec", 6);
  }
  return result;
}

int cmd_drawboundingbox(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto box = get_param_str(argc, params, 0, NULL);
    // auto color = get_param_color(argc, params, 1, NULL);
    // DrawBoundingBox(box, color);
  } else {
    error(retval, "DrawBoundingBox", 2);
  }
  return result;
}

int cmd_drawcircle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    auto centerX = get_param_int(argc, params, 0, 0);
    auto centerY = get_param_int(argc, params, 1, 0);
    auto radius = get_param_num(argc, params, 2, 0);
    auto color = get_param_color(argc, params, 3);
    DrawCircle(centerX, centerY, radius, color);
  } else {
    error(retval, "DrawCircle", 4);
  }
  return result;
}

int cmd_drawcircle3d(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto center = get_param_str(argc, params, 0, NULL);
    // auto radius = get_param_num(argc, params, 1, NULL);
    // auto rotationAxis = get_param_str(argc, params, 2, NULL);
    // auto rotationAngle = get_param_str(argc, params, 3, NULL);
    // auto color = get_param_color(argc, params, 4, NULL);
    // DrawCircle3D(center, radius, rotationAxis, rotationAngle, color);
  } else {
    error(retval, "DrawCircle3D", 5);
  }
  return result;
}

int cmd_drawcirclegradient(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto centerX = get_param_str(argc, params, 0, NULL);
    // auto centerY = get_param_str(argc, params, 1, NULL);
    // auto radius = get_param_num(argc, params, 2, NULL);
    // auto color1 = get_param_str(argc, params, 3, NULL);
    // auto color2 = get_param_str(argc, params, 4, NULL);
    // DrawCircleGradient(centerX, centerY, radius, color1, color2);
  } else {
    error(retval, "DrawCircleGradient", 5);
  }
  return result;
}

int cmd_drawcirclelines(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    auto centerX = get_param_int(argc, params, 0, 0);
    auto centerY = get_param_int(argc, params, 1, 0);
    auto radius = get_param_num(argc, params, 2, 0);
    auto color = get_param_color(argc, params, 3);
    DrawCircleLines(centerX, centerY, radius, color);
  } else {
    error(retval, "DrawCircleLines", 4);
  }
  return result;
}

int cmd_drawcirclesector(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    auto center = get_param_vec2(argc, params, 0);
    auto radius = get_param_num(argc, params, 1, 0);
    auto startAngle = get_param_int(argc, params, 2, 0);
    auto endAngle = get_param_int(argc, params, 3, 0);
    auto segments = get_param_int(argc, params, 4, 0);
    auto color = get_param_color(argc, params, 5);
    DrawCircleSector(center, radius, startAngle, endAngle, segments, color);
  } else {
    error(retval, "DrawCircleSector", 6);
  }
  return result;
}

int cmd_drawcirclesectorlines(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    auto center = get_param_vec2(argc, params, 0);
    auto radius = get_param_num(argc, params, 1, 0);
    auto startAngle = get_param_int(argc, params, 2, 0);
    auto endAngle = get_param_int(argc, params, 3, 0);
    auto segments = get_param_int(argc, params, 4, 0);
    auto color = get_param_color(argc, params, 5);
    DrawCircleSectorLines(center, radius, startAngle, endAngle, segments, color);
  } else {
    error(retval, "DrawCircleSectorLines", 6);
  }
  return result;
}

int cmd_drawcirclev(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    auto center = get_param_vec2(argc, params, 0);
    auto radius = get_param_num(argc, params, 1, 0);
    auto color = get_param_color(argc, params, 2);
    DrawCircleV(center, radius, color);
  } else {
    error(retval, "DrawCircleV", 3);
  }
  return result;
}

int cmd_drawcube(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    auto position = get_param_vec3(argc, params, 0);
    auto width = get_param_num(argc, params, 1, 0);
    auto height = get_param_num(argc, params, 2, 0);
    auto length = get_param_num(argc, params, 3, 0);
    auto color = get_param_color(argc, params, 4);
    DrawCube(position, width, height, length, color);
  } else {
    error(retval, "DrawCube", 5);
  }
  return result;
}

int cmd_drawcubetexture(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto texture = get_param_str(argc, params, 0, NULL);
    // auto position = get_param_str(argc, params, 1, NULL);
    // auto width = get_param_str(argc, params, 2, NULL);
    // auto height = get_param_str(argc, params, 3, NULL);
    // auto length = get_param_str(argc, params, 4, NULL);
    // auto color = get_param_color(argc, params, 5, NULL);
    // DrawCubeTexture(texture, position, width, height, length, color);
  } else {
    error(retval, "DrawCubeTexture", 6);
  }
  return result;
}

int cmd_drawcubev(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto position = get_param_str(argc, params, 0, NULL);
    // auto size = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_color(argc, params, 2, NULL);
    // DrawCubeV(position, size, color);
  } else {
    error(retval, "DrawCubeV", 3);
  }
  return result;
}

int cmd_drawcubewires(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto position = get_param_str(argc, params, 0, NULL);
    // auto width = get_param_str(argc, params, 1, NULL);
    // auto height = get_param_str(argc, params, 2, NULL);
    // auto length = get_param_str(argc, params, 3, NULL);
    // auto color = get_param_color(argc, params, 4, NULL);
    // DrawCubeWires(position, width, height, length, color);
  } else {
    error(retval, "DrawCubeWires", 5);
  }
  return result;
}

int cmd_drawcubewiresv(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto position = get_param_str(argc, params, 0, NULL);
    // auto size = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_color(argc, params, 2, NULL);
    // DrawCubeWiresV(position, size, color);
  } else {
    error(retval, "DrawCubeWiresV", 3);
  }
  return result;
}

int cmd_drawcylinder(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto position = get_param_str(argc, params, 0, NULL);
    // auto radiusTop = get_param_str(argc, params, 1, NULL);
    // auto radiusBottom = get_param_str(argc, params, 2, NULL);
    // auto height = get_param_str(argc, params, 3, NULL);
    // auto slices = get_param_str(argc, params, 4, NULL);
    // auto color = get_param_color(argc, params, 5, NULL);
    // DrawCylinder(position, radiusTop, radiusBottom, height, slices, color);
  } else {
    error(retval, "DrawCylinder", 6);
  }
  return result;
}

int cmd_drawcylinderwires(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto position = get_param_str(argc, params, 0, NULL);
    // auto radiusTop = get_param_str(argc, params, 1, NULL);
    // auto radiusBottom = get_param_str(argc, params, 2, NULL);
    // auto height = get_param_str(argc, params, 3, NULL);
    // auto slices = get_param_str(argc, params, 4, NULL);
    // auto color = get_param_color(argc, params, 5, NULL);
    // DrawCylinderWires(position, radiusTop, radiusBottom, height, slices, color);
  } else {
    error(retval, "DrawCylinderWires", 6);
  }
  return result;
}

int cmd_drawellipse(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto centerX = get_param_str(argc, params, 0, NULL);
    // auto centerY = get_param_str(argc, params, 1, NULL);
    // auto radiusH = get_param_str(argc, params, 2, NULL);
    // auto radiusV = get_param_str(argc, params, 3, NULL);
    // auto color = get_param_color(argc, params, 4, NULL);
    // DrawEllipse(centerX, centerY, radiusH, radiusV, color);
  } else {
    error(retval, "DrawEllipse", 5);
  }
  return result;
}

int cmd_drawellipselines(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto centerX = get_param_str(argc, params, 0, NULL);
    // auto centerY = get_param_str(argc, params, 1, NULL);
    // auto radiusH = get_param_str(argc, params, 2, NULL);
    // auto radiusV = get_param_str(argc, params, 3, NULL);
    // auto color = get_param_color(argc, params, 4, NULL);
    // DrawEllipseLines(centerX, centerY, radiusH, radiusV, color);
  } else {
    error(retval, "DrawEllipseLines", 5);
  }
  return result;
}

int cmd_drawfps(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto posX = get_param_int(argc, params, 0, 0);
    auto posY = get_param_int(argc, params, 1, 0);
    DrawFPS(posX, posY);
  } else {
    error(retval, "DrawFPS", 2);
  }
  return result;
}

int cmd_drawgizmo(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    DrawGizmo(get_param_vec3(argc, params, 0));
  } else {
    error(retval, "DrawGizmo", 1);
  }
  return result;
}

int cmd_drawgrid(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto slices = get_param_int(argc, params, 0, 0);
    auto spacing = get_param_num(argc, params, 1, 0);
    DrawGrid(slices, spacing);
  } else {
    error(retval, "DrawGrid", 2);
  }
  return result;
}

int cmd_drawline(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    auto startPosX = get_param_int(argc, params, 0, 0);
    auto startPosY = get_param_int(argc, params, 1, 0);
    auto endPosX = get_param_int(argc, params, 2, 0);
    auto endPosY = get_param_int(argc, params, 3, 0);
    auto color = get_param_color(argc, params, 4);
    DrawLine(startPosX, startPosY, endPosX, endPosY, color);
  } else {
    error(retval, "DrawLine", 5);
  }
  return result;
}

int cmd_drawline3d(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto startPos = get_param_str(argc, params, 0, NULL);
    // auto endPos = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_color(argc, params, 2, NULL);
    // DrawLine3D(startPos, endPos, color);
  } else {
    error(retval, "DrawLine3D", 3);
  }
  return result;
}

int cmd_drawlinebezier(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto startPos = get_param_str(argc, params, 0, NULL);
    // auto endPos = get_param_str(argc, params, 1, NULL);
    // auto thick = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_color(argc, params, 3, NULL);
    // DrawLineBezier(startPos, endPos, thick, color);
  } else {
    error(retval, "DrawLineBezier", 4);
  }
  return result;
}

int cmd_drawlineex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto startPos = get_param_str(argc, params, 0, NULL);
    // auto endPos = get_param_str(argc, params, 1, NULL);
    // auto thick = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_color(argc, params, 3, NULL);
    // DrawLineEx(startPos, endPos, thick, color);
  } else {
    error(retval, "DrawLineEx", 4);
  }
  return result;
}

int cmd_drawlinestrip(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto points = get_param_str(argc, params, 0, NULL);
    // auto numPoints = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_color(argc, params, 2, NULL);
    // DrawLineStrip(points, numPoints, color);
  } else {
    error(retval, "DrawLineStrip", 3);
  }
  return result;
}

int cmd_drawlinev(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto startPos = get_param_str(argc, params, 0, NULL);
    // auto endPos = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_color(argc, params, 2, NULL);
    // DrawLineV(startPos, endPos, color);
  } else {
    error(retval, "DrawLineV", 3);
  }
  return result;
}

int cmd_drawmodel(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    int id = get_model_id(argc, params, 0);
    if (id != -1) {
      auto position = get_param_vec3(argc, params, 1);
      auto scale = get_param_num(argc, params, 2, 0);
      auto tint = get_param_color(argc, params, 3);
      DrawModel(_modelMap.at(id), position, scale, tint);
    } else {
      error_model(retval);
      result = 0;
    }
  } else {
    error(retval, "DrawModel", 4);
  }
  return result;
}

int cmd_drawmodelex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto model = get_param_str(argc, params, 0, NULL);
    // auto position = get_param_str(argc, params, 1, NULL);
    // auto rotationAxis = get_param_str(argc, params, 2, NULL);
    // auto rotationAngle = get_param_str(argc, params, 3, NULL);
    // auto scale = get_param_str(argc, params, 4, NULL);
    // auto tint = get_param_str(argc, params, 5, NULL);
    // DrawModelEx(model, position, rotationAxis, rotationAngle, scale, tint);
  } else {
    error(retval, "DrawModelEx", 6);
  }
  return result;
}

int cmd_drawmodelwires(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto model = get_param_str(argc, params, 0, NULL);
    // auto position = get_param_str(argc, params, 1, NULL);
    // auto scale = get_param_str(argc, params, 2, NULL);
    // auto tint = get_param_str(argc, params, 3, NULL);
    // DrawModelWires(model, position, scale, tint);
  } else {
    error(retval, "DrawModelWires", 4);
  }
  return result;
}

int cmd_drawmodelwiresex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto model = get_param_str(argc, params, 0, NULL);
    // auto position = get_param_str(argc, params, 1, NULL);
    // auto rotationAxis = get_param_str(argc, params, 2, NULL);
    // auto rotationAngle = get_param_str(argc, params, 3, NULL);
    // auto scale = get_param_str(argc, params, 4, NULL);
    // auto tint = get_param_str(argc, params, 5, NULL);
    // DrawModelWiresEx(model, position, rotationAxis, rotationAngle, scale, tint);
  } else {
    error(retval, "DrawModelWiresEx", 6);
  }
  return result;
}

int cmd_drawpixel(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto posX = get_param_str(argc, params, 0, NULL);
    // auto posY = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_color(argc, params, 2, NULL);
    // DrawPixel(posX, posY, color);
  } else {
    error(retval, "DrawPixel", 3);
  }
  return result;
}

int cmd_drawpixelv(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto position = get_param_str(argc, params, 0, NULL);
    // auto color = get_param_color(argc, params, 1, NULL);
    // DrawPixelV(position, color);
  } else {
    error(retval, "DrawPixelV", 2);
  }
  return result;
}

int cmd_drawplane(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto centerPos = get_param_str(argc, params, 0, NULL);
    // auto size = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_color(argc, params, 2, NULL);
    // DrawPlane(centerPos, size, color);
  } else {
    error(retval, "DrawPlane", 3);
  }
  return result;
}

int cmd_drawpoint3d(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto position = get_param_str(argc, params, 0, NULL);
    // auto color = get_param_color(argc, params, 1, NULL);
    // DrawPoint3D(position, color);
  } else {
    error(retval, "DrawPoint3D", 2);
  }
  return result;
}

int cmd_drawpoly(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto center = get_param_str(argc, params, 0, NULL);
    // auto sides = get_param_str(argc, params, 1, NULL);
    // auto radius = get_param_num(argc, params, 2, NULL);
    // auto rotation = get_param_str(argc, params, 3, NULL);
    // auto color = get_param_color(argc, params, 4, NULL);
    // DrawPoly(center, sides, radius, rotation, color);
  } else {
    error(retval, "DrawPoly", 5);
  }
  return result;
}

int cmd_drawpolylines(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto center = get_param_str(argc, params, 0, NULL);
    // auto sides = get_param_str(argc, params, 1, NULL);
    // auto radius = get_param_num(argc, params, 2, NULL);
    // auto rotation = get_param_str(argc, params, 3, NULL);
    // auto color = get_param_color(argc, params, 4, NULL);
    // DrawPolyLines(center, sides, radius, rotation, color);
  } else {
    error(retval, "DrawPolyLines", 5);
  }
  return result;
}

int cmd_drawray(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto ray = get_param_str(argc, params, 0, NULL);
    // auto color = get_param_color(argc, params, 1, NULL);
    // DrawRay(ray, color);
  } else {
    error(retval, "DrawRay", 2);
  }
  return result;
}

int cmd_drawrectangle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    auto posX = get_param_int(argc, params, 0, 0);
    auto posY = get_param_int(argc, params, 1, 0);
    auto width = get_param_int(argc, params, 2, 0);
    auto height = get_param_int(argc, params, 3, 0);
    auto color = get_param_color(argc, params, 4);
    DrawRectangle(posX, posY, width, height, color);
  } else {
    error(retval, "DrawRectangle", 5);
  }
  return result;
}

int cmd_drawrectanglegradientex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto rec = get_param_str(argc, params, 0, NULL);
    // auto col1 = get_param_str(argc, params, 1, NULL);
    // auto col2 = get_param_str(argc, params, 2, NULL);
    // auto col3 = get_param_str(argc, params, 3, NULL);
    // auto col4 = get_param_str(argc, params, 4, NULL);
    // DrawRectangleGradientEx(rec, col1, col2, col3, col4);
  } else {
    error(retval, "DrawRectangleGradientEx", 5);
  }
  return result;
}

int cmd_drawrectanglegradienth(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto posX = get_param_str(argc, params, 0, NULL);
    // auto posY = get_param_str(argc, params, 1, NULL);
    // auto width = get_param_str(argc, params, 2, NULL);
    // auto height = get_param_str(argc, params, 3, NULL);
    // auto color1 = get_param_str(argc, params, 4, NULL);
    // auto color2 = get_param_str(argc, params, 5, NULL);
    // DrawRectangleGradientH(posX, posY, width, height, color1, color2);
  } else {
    error(retval, "DrawRectangleGradientH", 6);
  }
  return result;
}

int cmd_drawrectanglegradientv(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto posX = get_param_str(argc, params, 0, NULL);
    // auto posY = get_param_str(argc, params, 1, NULL);
    // auto width = get_param_str(argc, params, 2, NULL);
    // auto height = get_param_str(argc, params, 3, NULL);
    // auto color1 = get_param_str(argc, params, 4, NULL);
    // auto color2 = get_param_str(argc, params, 5, NULL);
    // DrawRectangleGradientV(posX, posY, width, height, color1, color2);
  } else {
    error(retval, "DrawRectangleGradientV", 6);
  }
  return result;
}

int cmd_drawrectanglelines(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    auto posX = get_param_int(argc, params, 0, 0);
    auto posY = get_param_int(argc, params, 1, 0);
    auto width = get_param_int(argc, params, 2, 0);
    auto height = get_param_int(argc, params, 3, 0);
    auto color = get_param_color(argc, params, 4);
    DrawRectangleLines(posX, posY, width, height, color);
  } else {
    error(retval, "DrawRectangleLines", 5);
  }
  return result;
}

int cmd_drawrectanglelinesex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    auto rec = get_param_rect(argc, params, 0);
    auto lineThick = get_param_int(argc, params, 1, 0);
    auto color = get_param_color(argc, params, 2);
    DrawRectangleLinesEx(rec, lineThick, color);
  } else {
    error(retval, "DrawRectangleLinesEx", 3);
  }
  return result;
}

int cmd_drawrectanglepro(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto rec = get_param_str(argc, params, 0, NULL);
    // auto origin = get_param_str(argc, params, 1, NULL);
    // auto rotation = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_color(argc, params, 3, NULL);
    // DrawRectanglePro(rec, origin, rotation, color);
  } else {
    error(retval, "DrawRectanglePro", 4);
  }
  return result;
}

int cmd_drawrectanglerec(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto rec = get_param_rect(argc, params, 0);
    auto color = get_param_color(argc, params, 1);
    DrawRectangleRec(rec, color);
  } else {
    error(retval, "DrawRectangleRec", 2);
  }
  return result;
}

int cmd_drawrectanglerounded(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto rec = get_param_str(argc, params, 0, NULL);
    // auto roundness = get_param_str(argc, params, 1, NULL);
    // auto segments = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_color(argc, params, 3, NULL);
    // DrawRectangleRounded(rec, roundness, segments, color);
  } else {
    error(retval, "DrawRectangleRounded", 4);
  }
  return result;
}

int cmd_drawrectangleroundedlines(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto rec = get_param_str(argc, params, 0, NULL);
    // auto roundness = get_param_str(argc, params, 1, NULL);
    // auto segments = get_param_str(argc, params, 2, NULL);
    // auto lineThick = get_param_str(argc, params, 3, NULL);
    // auto color = get_param_color(argc, params, 4, NULL);
    // DrawRectangleRoundedLines(rec, roundness, segments, lineThick, color);
  } else {
    error(retval, "DrawRectangleRoundedLines", 5);
  }
  return result;
}

int cmd_drawrectanglev(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto position = get_param_str(argc, params, 0, NULL);
    // auto size = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_color(argc, params, 2, NULL);
    // DrawRectangleV(position, size, color);
  } else {
    error(retval, "DrawRectangleV", 3);
  }
  return result;
}

int cmd_drawring(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    auto center = get_param_vec2(argc, params, 0);
    auto innerRadius = get_param_num(argc, params, 1, 0);
    auto outerRadius = get_param_num(argc, params, 2, 0);
    auto startAngle = get_param_int(argc, params, 3, 0);
    auto endAngle = get_param_int(argc, params, 4, 0);
    auto segments = get_param_int(argc, params, 5, 0);
    auto color = get_param_color(argc, params, 6);
    DrawRing(center, innerRadius, outerRadius, startAngle, endAngle, segments, color);
  } else {
    error(retval, "DrawRing", 7);
  }
  return result;
}

int cmd_drawringlines(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    auto center = get_param_vec2(argc, params, 0);
    auto innerRadius = get_param_num(argc, params, 1, 0);
    auto outerRadius = get_param_num(argc, params, 2, 0);
    auto startAngle = get_param_int(argc, params, 3, 0);
    auto endAngle = get_param_int(argc, params, 4, 0);
    auto segments = get_param_int(argc, params, 5, 0);
    auto color = get_param_color(argc, params, 6);
    DrawRingLines(center, innerRadius, outerRadius, startAngle, endAngle, segments, color);
  } else {
    error(retval, "DrawRingLines", 7);
  }
  return result;
}

int cmd_drawsphere(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    auto centerPos = get_param_vec3(argc, params, 0);
    auto radius = get_param_num(argc, params, 1, 0);
    auto color = get_param_color(argc, params, 2);
    DrawSphere(centerPos, radius, color);
  } else {
    error(retval, "DrawSphere", 3);
  }
  return result;
}

int cmd_drawsphereex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto centerPos = get_param_str(argc, params, 0, NULL);
    // auto radius = get_param_num(argc, params, 1, NULL);
    // auto rings = get_param_str(argc, params, 2, NULL);
    // auto slices = get_param_str(argc, params, 3, NULL);
    // auto color = get_param_color(argc, params, 4, NULL);
    // DrawSphereEx(centerPos, radius, rings, slices, color);
  } else {
    error(retval, "DrawSphereEx", 5);
  }
  return result;
}

int cmd_drawspherewires(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto centerPos = get_param_str(argc, params, 0, NULL);
    // auto radius = get_param_num(argc, params, 1, NULL);
    // auto rings = get_param_str(argc, params, 2, NULL);
    // auto slices = get_param_str(argc, params, 3, NULL);
    // auto color = get_param_color(argc, params, 4, NULL);
    // DrawSphereWires(centerPos, radius, rings, slices, color);
  } else {
    error(retval, "DrawSphereWires", 5);
  }
  return result;
}

int cmd_drawtext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    auto text = get_param_str(argc, params, 0, NULL);
    auto posX = get_param_int(argc, params, 1, 0);
    auto posY = get_param_int(argc, params, 2, 0);
    auto fontSize = get_param_int(argc, params, 3, 0);
    auto color = get_param_color(argc, params, 4);
    DrawText(text, posX, posY, fontSize, color);
  } else {
    error(retval, "DrawText", 5);
  }
  return result;
}

int cmd_drawtextcodepoint(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto font = get_param_str(argc, params, 0, NULL);
    // auto codepoint = get_param_str(argc, params, 1, NULL);
    // auto position = get_param_str(argc, params, 2, NULL);
    // auto scale = get_param_str(argc, params, 3, NULL);
    // auto tint = get_param_str(argc, params, 4, NULL);
    // DrawTextCodepoint(font, codepoint, position, scale, tint);
  } else {
    error(retval, "DrawTextCodepoint", 5);
  }
  return result;
}

int cmd_drawtextex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto font = get_param_str(argc, params, 0, NULL);
    // auto text = get_param_str(argc, params, 1, NULL);
    // auto position = get_param_str(argc, params, 2, NULL);
    // auto fontSize = get_param_str(argc, params, 3, NULL);
    // auto spacing = get_param_str(argc, params, 4, NULL);
    // auto tint = get_param_str(argc, params, 5, NULL);
    // DrawTextEx(font, text, position, fontSize, spacing, tint);
  } else {
    error(retval, "DrawTextEx", 6);
  }
  return result;
}

int cmd_drawtextrec(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    // auto font = get_param_str(argc, params, 0, NULL);
    // auto text = get_param_str(argc, params, 1, NULL);
    // auto rec = get_param_str(argc, params, 2, NULL);
    // auto fontSize = get_param_str(argc, params, 3, NULL);
    // auto spacing = get_param_str(argc, params, 4, NULL);
    // auto wordWrap = get_param_str(argc, params, 5, NULL);
    // auto tint = get_param_str(argc, params, 6, NULL);
    // DrawTextRec(font, text, rec, fontSize, spacing, wordWrap, tint);
  } else {
    error(retval, "DrawTextRec", 7);
  }
  return result;
}

int cmd_drawtextrecex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 11);
  if (result) {
    // auto font = get_param_str(argc, params, 0, NULL);
    // auto text = get_param_str(argc, params, 1, NULL);
    // auto rec = get_param_str(argc, params, 2, NULL);
    // auto fontSize = get_param_str(argc, params, 3, NULL);
    // auto spacing = get_param_str(argc, params, 4, NULL);
    // auto wordWrap = get_param_str(argc, params, 5, NULL);
    // auto tint = get_param_str(argc, params, 6, NULL);
    // auto selectStart = get_param_str(argc, params, 7, NULL);
    // auto selectLength = get_param_str(argc, params, 8, NULL);
    // auto selectTint = get_param_str(argc, params, 9, NULL);
    // auto selectBackTint = get_param_str(argc, params, 10, NULL);
    // DrawTextRecEx(font, text, rec, fontSize, spacing, wordWrap, tint, selectStart, selectLength, selectTint, selectBackTint);
  } else {
    error(retval, "DrawTextRecEx", 11);
  }
  return result;
}

int cmd_drawtexture(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    int id = get_texture_id(argc, params, 0);
    if (id != -1) {
      auto posX = get_param_int(argc, params, 1, 0);
      auto posY = get_param_int(argc, params, 2, 0);
      auto tint = get_param_color(argc, params, 3);
      DrawTexture(_textureMap.at(id), posX, posY, tint);
    } else {
      error_texture(retval);
      result = 0;
    }
  } else {
    error(retval, "DrawTexture", 4);
  }
  return result;
}

int cmd_drawtextureex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    int id = get_texture_id(argc, params, 0);
    if (id != -1) {
      auto position = get_param_vec2(argc, params, 1);
      auto rotation = get_param_num(argc, params, 2, 0);
      auto scale = get_param_num(argc, params, 3, 0);
      auto tint = get_param_color(argc, params, 4);
      DrawTextureEx(_textureMap.at(id), position, rotation, scale, tint);
    } else {
      error_texture(retval);
      result = 0;
    }
  } else {
    error(retval, "DrawTextureEx", 5);
  }
  return result;
}

int cmd_drawtexturenpatch(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto texture = get_param_str(argc, params, 0, NULL);
    // auto nPatchInfo = get_param_str(argc, params, 1, NULL);
    // auto destRec = get_param_str(argc, params, 2, NULL);
    // auto origin = get_param_str(argc, params, 3, NULL);
    // auto rotation = get_param_str(argc, params, 4, NULL);
    // auto tint = get_param_str(argc, params, 5, NULL);
    // DrawTextureNPatch(texture, nPatchInfo, destRec, origin, rotation, tint);
  } else {
    error(retval, "DrawTextureNPatch", 6);
  }
  return result;
}

int cmd_drawtexturepro(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    int id = get_texture_id(argc, params, 0);
    if (id != -1) {
      auto sourceRec = get_param_rect(argc, params, 1);
      auto destRec = get_param_rect(argc, params, 2);
      auto origin = get_param_vec2(argc, params, 3);
      auto rotation = get_param_num(argc, params, 4, 0);
      auto tint = get_param_color(argc, params, 5);
      DrawTexturePro(_textureMap.at(id), sourceRec, destRec, origin, rotation, tint);
    } else {
      error_texture(retval);
      result = 0;
    }
  } else {
    error(retval, "DrawTexturePro", 6);
  }
  return result;
}

int cmd_drawtexturequad(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto texture = get_param_str(argc, params, 0, NULL);
    // auto tiling = get_param_str(argc, params, 1, NULL);
    // auto offset = get_param_str(argc, params, 2, NULL);
    // auto quad = get_param_str(argc, params, 3, NULL);
    // auto tint = get_param_str(argc, params, 4, NULL);
    // DrawTextureQuad(texture, tiling, offset, quad, tint);
  } else {
    error(retval, "DrawTextureQuad", 5);
  }
  return result;
}

int cmd_drawtexturerec(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result && is_param_map(argc, params, 0)) {
    int id = map_get_int(params[0].var_p, mapID, -1);
    if (id != -1 && _textureMap.find(id) != _textureMap.end()) {
      auto sourceRec = get_param_rect(argc, params, 1);
      auto position = get_param_vec2(argc, params, 2);
      auto tint = get_param_color(argc, params, 3);
      DrawTextureRec(_textureMap.at(id), sourceRec, position, tint);
    } else {
      error_texture(retval);
      result = 0;
    }
  } else {
    error(retval, "DrawTextureRec", 4);
  }
  return result;
}

int cmd_drawtexturetiled(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    // auto texture = get_param_str(argc, params, 0, NULL);
    // auto sourceRec = get_param_str(argc, params, 1, NULL);
    // auto destRec = get_param_str(argc, params, 2, NULL);
    // auto origin = get_param_str(argc, params, 3, NULL);
    // auto rotation = get_param_str(argc, params, 4, NULL);
    // auto scale = get_param_str(argc, params, 5, NULL);
    // auto tint = get_param_str(argc, params, 6, NULL);
    // DrawTextureTiled(texture, sourceRec, destRec, origin, rotation, scale, tint);
  } else {
    error(retval, "DrawTextureTiled", 7);
  }
  return result;
}

int cmd_drawtexturev(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto texture = get_param_str(argc, params, 0, NULL);
    // auto position = get_param_str(argc, params, 1, NULL);
    // auto tint = get_param_str(argc, params, 2, NULL);
    // DrawTextureV(texture, position, tint);
  } else {
    error(retval, "DrawTextureV", 3);
  }
  return result;
}

int cmd_drawtriangle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto v1 = get_param_str(argc, params, 0, NULL);
    // auto v2 = get_param_str(argc, params, 1, NULL);
    // auto v3 = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_color(argc, params, 3, NULL);
    // DrawTriangle(v1, v2, v3, color);
  } else {
    error(retval, "DrawTriangle", 4);
  }
  return result;
}

int cmd_drawtriangle3d(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto v1 = get_param_str(argc, params, 0, NULL);
    // auto v2 = get_param_str(argc, params, 1, NULL);
    // auto v3 = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_color(argc, params, 3, NULL);
    // DrawTriangle3D(v1, v2, v3, color);
  } else {
    error(retval, "DrawTriangle3D", 4);
  }
  return result;
}

int cmd_drawtrianglefan(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto points = get_param_str(argc, params, 0, NULL);
    // auto numPoints = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_color(argc, params, 2, NULL);
    // DrawTriangleFan(points, numPoints, color);
  } else {
    error(retval, "DrawTriangleFan", 3);
  }
  return result;
}

int cmd_drawtrianglelines(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto v1 = get_param_str(argc, params, 0, NULL);
    // auto v2 = get_param_str(argc, params, 1, NULL);
    // auto v3 = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_color(argc, params, 3, NULL);
    // DrawTriangleLines(v1, v2, v3, color);
  } else {
    error(retval, "DrawTriangleLines", 4);
  }
  return result;
}

int cmd_drawtrianglestrip(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto points = get_param_str(argc, params, 0, NULL);
    // auto pointsCount = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_color(argc, params, 2, NULL);
    // DrawTriangleStrip(points, pointsCount, color);
  } else {
    error(retval, "DrawTriangleStrip", 3);
  }
  return result;
}

int cmd_drawtrianglestrip3d(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto points = get_param_str(argc, params, 0, NULL);
    // auto pointsCount = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_color(argc, params, 2, NULL);
    // DrawTriangleStrip3D(points, pointsCount, color);
  } else {
    error(retval, "DrawTriangleStrip3D", 3);
  }
  return result;
}

int cmd_enablecursor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EnableCursor();
  } else {
    error(retval, "EnableCursor", 0);
  }
  return result;
}

int cmd_endblendmode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndBlendMode();
  } else {
    error(retval, "EndBlendMode", 0);
  }
  return result;
}

int cmd_enddrawing(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndDrawing();
  } else {
    error(retval, "EndDrawing", 0);
  }
  return result;
}

int cmd_endmode2d(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndMode2D();
  } else {
    error(retval, "EndMode2D", 0);
  }
  return result;
}

int cmd_endmode3d(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndMode3D();
  } else {
    error(retval, "EndMode3D", 0);
  }
  return result;
}

int cmd_endscissormode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndScissorMode();
  } else {
    error(retval, "EndScissorMode", 0);
  }
  return result;
}

int cmd_endshadermode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndShaderMode();
  } else {
    error(retval, "EndShaderMode", 0);
  }
  return result;
}

int cmd_endtexturemode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndTextureMode();
  } else {
    error(retval, "EndTextureMode", 0);
  }
  return result;
}

int cmd_endvrdrawing(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndVrDrawing();
  } else {
    error(retval, "EndVrDrawing", 0);
  }
  return result;
}

int cmd_exportimage(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    int id = get_image_id(argc, params, 0);
    if (id != -1) {
      auto fileName = get_param_str(argc, params, 1, NULL);
      ExportImage(_imageMap.at(id), fileName);
    } else {
      error_image(retval);
      result = 0;
    }
  } else {
    error(retval, "ExportImage", 2);
  }
  return result;
}

int cmd_exportimageascode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // auto fileName = get_param_str(argc, params, 1, NULL);
    // ExportImageAsCode(image, fileName);
  } else {
    error(retval, "ExportImageAsCode", 2);
  }
  return result;
}

int cmd_exportmesh(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto mesh = get_param_str(argc, params, 0, NULL);
    // auto fileName = get_param_str(argc, params, 1, NULL);
    // ExportMesh(mesh, fileName);
  } else {
    error(retval, "ExportMesh", 2);
  }
  return result;
}

int cmd_exportwave(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto wave = get_param_str(argc, params, 0, NULL);
    // auto fileName = get_param_str(argc, params, 1, NULL);
    // ExportWave(wave, fileName);
  } else {
    error(retval, "ExportWave", 2);
  }
  return result;
}

int cmd_exportwaveascode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto wave = get_param_str(argc, params, 0, NULL);
    // auto fileName = get_param_str(argc, params, 1, NULL);
    // ExportWaveAsCode(wave, fileName);
  } else {
    error(retval, "ExportWaveAsCode", 2);
  }
  return result;
}

int cmd_gentexturemipmaps(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto texture = get_param_str(argc, params, 0, NULL);
    // GenTextureMipmaps(texture);
  } else {
    error(retval, "GenTextureMipmaps", 1);
  }
  return result;
}

int cmd_getwindowhandle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    GetWindowHandle();
  } else {
    error(retval, "GetWindowHandle", 0);
  }
  return result;
}

int cmd_hidecursor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    HideCursor();
  } else {
    error(retval, "HideCursor", 0);
  }
  return result;
}

int cmd_hidewindow(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    HideWindow();
  } else {
    error(retval, "HideWindow", 0);
  }
  return result;
}

int cmd_imagealphaclear(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // auto color = get_param_color(argc, params, 1, NULL);
    // auto threshold = get_param_str(argc, params, 2, NULL);
    // ImageAlphaClear(image, color, threshold);
  } else {
    error(retval, "ImageAlphaClear", 3);
  }
  return result;
}

int cmd_imagealphacrop(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // auto threshold = get_param_str(argc, params, 1, NULL);
    // ImageAlphaCrop(image, threshold);
  } else {
    error(retval, "ImageAlphaCrop", 2);
  }
  return result;
}

int cmd_imagealphamask(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // auto alphaMask = get_param_str(argc, params, 1, NULL);
    // ImageAlphaMask(image, alphaMask);
  } else {
    error(retval, "ImageAlphaMask", 2);
  }
  return result;
}

int cmd_imagealphapremultiply(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // ImageAlphaPremultiply(&image);
  } else {
    error(retval, "ImageAlphaPremultiply", 1);
  }
  return result;
}

int cmd_imageclearbackground(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto color = get_param_color(argc, params, 1, NULL);
    // ImageClearBackground(dst, color);
  } else {
    error(retval, "ImageClearBackground", 2);
  }
  return result;
}

int cmd_imagecolorbrightness(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    int id = get_image_id(argc, params, 0);
    if (id != -1) {
      auto brightness = get_param_int(argc, params, 1, 0);
      ImageColorBrightness(&_imageMap.at(id), brightness);
    } else {
      error_image(retval);
      result = 0;
    }
  } else {
    error(retval, "ImageColorBrightness", 2);
  }
  return result;
}

int cmd_imagecolorcontrast(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    int id = get_image_id(argc, params, 0);
    if (id != -1) {
      auto contrast = get_param_int(argc, params, 1, 0);
      ImageColorContrast(&_imageMap.at(id), contrast);
    } else {
      error_image(retval);
      result = 0;
    }
  } else {
    error(retval, "ImageColorContrast", 2);
  }
  return result;
}

int cmd_imagecolorgrayscale(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    int id = get_image_id(argc, params, 0);
    if (id != -1) {
      ImageColorGrayscale(&_imageMap.at(id));
    } else {
      error_image(retval);
      result = 0;
    }
  } else {
    error(retval, "ImageColorGrayscale", 1);
  }
  return result;
}

int cmd_imagecolorinvert(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    int id = get_image_id(argc, params, 0);
    if (id != -1) {
      ImageColorInvert(&_imageMap.at(id));
    } else {
      error_image(retval);
      result = 0;
    }
  } else {
    error(retval, "ImageColorInvert", 1);
  }
  return result;
}

int cmd_imagecolorreplace(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // auto color = get_param_color(argc, params, 1, NULL);
    // auto replace = get_param_str(argc, params, 2, NULL);
    // ImageColorReplace(image, color, replace);
  } else {
    error(retval, "ImageColorReplace", 3);
  }
  return result;
}

int cmd_imagecolortint(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    int id = get_image_id(argc, params, 0);
    if (id != -1) {
      auto color = get_param_color(argc, params, 1);
      ImageColorTint(&_imageMap.at(id), color);
    } else {
      error_image(retval);
      result = 0;
    }
  } else {
    error(retval, "ImageColorTint", 2);
  }
  return result;
}

int cmd_imagecrop(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // auto crop = get_param_str(argc, params, 1, NULL);
    // ImageCrop(&image, crop);
  } else {
    error(retval, "ImageCrop", 2);
  }
  return result;
}

int cmd_imagedither(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // auto rBpp = get_param_str(argc, params, 1, NULL);
    // auto gBpp = get_param_str(argc, params, 2, NULL);
    // auto bBpp = get_param_str(argc, params, 3, NULL);
    // auto aBpp = get_param_str(argc, params, 4, NULL);
    // ImageDither(image, rBpp, gBpp, bBpp, aBpp);
  } else {
    error(retval, "ImageDither", 5);
  }
  return result;
}

int cmd_imagedraw(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto src = get_param_str(argc, params, 1, NULL);
    // auto srcRec = get_param_str(argc, params, 2, NULL);
    // auto dstRec = get_param_str(argc, params, 3, NULL);
    // auto tint = get_param_str(argc, params, 4, NULL);
    // ImageDraw(dst, src, srcRec, dstRec, tint);
  } else {
    error(retval, "ImageDraw", 5);
  }
  return result;
}

int cmd_imagedrawcircle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    int id = get_image_id(argc, params, 0);
    if (id != -1) {
      auto centerX = get_param_int(argc, params, 1, 0);
      auto centerY = get_param_int(argc, params, 2, 0);
      auto radius = get_param_int(argc, params, 3, 0);
      auto color = get_param_color(argc, params, 4);
      ImageDrawCircle(&_imageMap.at(id), centerX, centerY, radius, color);
    } else {
      error_image(retval);
      result = 0;
    }
  } else {
    error(retval, "ImageDrawCircle", 5);
  }
  return result;
}

int cmd_imagedrawcirclev(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto center = get_param_str(argc, params, 1, NULL);
    // auto radius = get_param_num(argc, params, 2, NULL);
    // auto color = get_param_color(argc, params, 3, NULL);
    // ImageDrawCircleV(dst, center, radius, color);
  } else {
    error(retval, "ImageDrawCircleV", 4);
  }
  return result;
}

int cmd_imagedrawline(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto startPosX = get_param_str(argc, params, 1, NULL);
    // auto startPosY = get_param_str(argc, params, 2, NULL);
    // auto endPosX = get_param_str(argc, params, 3, NULL);
    // auto endPosY = get_param_str(argc, params, 4, NULL);
    // auto color = get_param_color(argc, params, 5, NULL);
    // ImageDrawLine(dst, startPosX, startPosY, endPosX, endPosY, color);
  } else {
    error(retval, "ImageDrawLine", 6);
  }
  return result;
}

int cmd_imagedrawlinev(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto start = get_param_str(argc, params, 1, NULL);
    // auto end = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_color(argc, params, 3, NULL);
    // ImageDrawLineV(dst, start, end, color);
  } else {
    error(retval, "ImageDrawLineV", 4);
  }
  return result;
}

int cmd_imagedrawpixel(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto posX = get_param_str(argc, params, 1, NULL);
    // auto posY = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_color(argc, params, 3, NULL);
    // ImageDrawPixel(dst, posX, posY, color);
  } else {
    error(retval, "ImageDrawPixel", 4);
  }
  return result;
}

int cmd_imagedrawpixelv(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto position = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_color(argc, params, 2, NULL);
    // ImageDrawPixelV(dst, position, color);
  } else {
    error(retval, "ImageDrawPixelV", 3);
  }
  return result;
}

int cmd_imagedrawrectangle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto posX = get_param_str(argc, params, 1, NULL);
    // auto posY = get_param_str(argc, params, 2, NULL);
    // auto width = get_param_str(argc, params, 3, NULL);
    // auto height = get_param_str(argc, params, 4, NULL);
    // auto color = get_param_color(argc, params, 5, NULL);
    // ImageDrawRectangle(dst, posX, posY, width, height, color);
  } else {
    error(retval, "ImageDrawRectangle", 6);
  }
  return result;
}

int cmd_imagedrawrectanglelines(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto rec = get_param_str(argc, params, 1, NULL);
    // auto thick = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_color(argc, params, 3, NULL);
    // ImageDrawRectangleLines(dst, rec, thick, color);
  } else {
    error(retval, "ImageDrawRectangleLines", 4);
  }
  return result;
}

int cmd_imagedrawrectanglerec(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    int id = get_image_id(argc, params, 0);
    if (id != -1) {
      auto rec = get_param_rect(argc, params, 1);
      auto color = get_param_color(argc, params, 2);
      ImageDrawRectangleRec(&_imageMap.at(id), rec, color);
    } else {
      error_image(retval);
      result = 0;
    }
  } else {
    error(retval, "ImageDrawRectangleRec", 3);
  }
  return result;
}

int cmd_imagedrawrectanglev(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto position = get_param_str(argc, params, 1, NULL);
    // auto size = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_color(argc, params, 3, NULL);
    // ImageDrawRectangleV(dst, position, size, color);
  } else {
    error(retval, "ImageDrawRectangleV", 4);
  }
  return result;
}

int cmd_imagedrawtext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto text = get_param_str(argc, params, 1, NULL);
    // auto posX = get_param_str(argc, params, 2, NULL);
    // auto posY = get_param_str(argc, params, 3, NULL);
    // auto fontSize = get_param_str(argc, params, 4, NULL);
    // auto color = get_param_color(argc, params, 5);
    // ImageDrawText(dst, text, posX, posY, fontSize, color);
  } else {
    error(retval, "ImageDrawText", 6);
  }
  return result;
}

int cmd_imagedrawtextex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto font = get_param_str(argc, params, 1, NULL);
    // auto text = get_param_str(argc, params, 2, NULL);
    // auto position = get_param_str(argc, params, 3, NULL);
    // auto fontSize = get_param_str(argc, params, 4, NULL);
    // auto spacing = get_param_str(argc, params, 5, NULL);
    // auto tint = get_param_str(argc, params, 6, NULL);
    // ImageDrawTextEx(dst, font, text, position, fontSize, spacing, tint);
  } else {
    error(retval, "ImageDrawTextEx", 7);
  }
  return result;
}

int cmd_imagefliphorizontal(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    int id = get_image_id(argc, params, 0);
    if (id != -1) {
      ImageFlipHorizontal(&_imageMap.at(id));
    } else {
      error_image(retval);
      result = 0;
    }
  } else {
    error(retval, "ImageFlipHorizontal", 1);
  }
  return result;
}

int cmd_imageflipvertical(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    int id = get_image_id(argc, params, 0);
    if (id != -1) {
      ImageFlipVertical(&_imageMap.at(id));
    } else {
      error_image(retval);
      result = 0;
    }
  } else {
    error(retval, "ImageFlipVertical", 1);
  }
  return result;
}

int cmd_imageformat(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    int id = get_image_id(argc, params, 0);
    if (id != -1) {
      auto newFormat = get_param_int(argc, params, 1, 0);
      ImageFormat(&_imageMap.at(id), newFormat);
    } else {
      error_image(retval);
      result = 0;
    }
  } else {
    error(retval, "ImageFormat", 2);
  }
  return result;
}

int cmd_imagemipmaps(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // ImageMipmaps(image);
  } else {
    error(retval, "ImageMipmaps", 1);
  }
  return result;
}

int cmd_imageresize(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // auto newWidth = get_param_str(argc, params, 1, NULL);
    // auto newHeight = get_param_str(argc, params, 2, NULL);
    // ImageResize(image, newWidth, newHeight);
  } else {
    error(retval, "ImageResize", 3);
  }
  return result;
}

int cmd_imageresizecanvas(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // auto newWidth = get_param_str(argc, params, 1, NULL);
    // auto newHeight = get_param_str(argc, params, 2, NULL);
    // auto offsetX = get_param_str(argc, params, 3, NULL);
    // auto offsetY = get_param_str(argc, params, 4, NULL);
    // auto fill = get_param_str(argc, params, 5, NULL);
    // ImageResizeCanvas(image, newWidth, newHeight, offsetX, offsetY, fill);
  } else {
    error(retval, "ImageResizeCanvas", 6);
  }
  return result;
}

int cmd_imageresizenn(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // auto newWidth = get_param_str(argc, params, 1, NULL);
    // auto newHeight = get_param_str(argc, params, 2, NULL);
    // ImageResizeNN(image, newWidth, newHeight);
  } else {
    error(retval, "ImageResizeNN", 3);
  }
  return result;
}

int cmd_imagerotateccw(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // ImageRotateCCW(image);
  } else {
    error(retval, "ImageRotateCCW", 1);
  }
  return result;
}

int cmd_imagerotatecw(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // ImageRotateCW(image);
  } else {
    error(retval, "ImageRotateCW", 1);
  }
  return result;
}

int cmd_imagetopot(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // auto fill = get_param_str(argc, params, 1, NULL);
    // ImageToPOT(image, fill);
  } else {
    error(retval, "ImageToPOT", 2);
  }
  return result;
}

int cmd_initaudiodevice(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    InitAudioDevice();
  } else {
    error(retval, "InitAudioDevice", 0);
  }
  return result;
}

int cmd_initvrsimulator(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    InitVrSimulator();
  } else {
    error(retval, "InitVrSimulator", 0);
  }
  return result;
}

int cmd_initwindow(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    auto width = get_param_int(argc, params, 0, 640);
    auto height = get_param_int(argc, params, 1, 480);
    auto title = get_param_str(argc, params, 2, NULL);
    InitWindow(width, height, title);
  } else {
    error(retval, "InitWindow", 3);
  }
  return result;
}

int cmd_maximizewindow(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    MaximizeWindow();
  } else {
    error(retval, "MaximizeWindow", 0);
  }
  return result;
}

int cmd_meshbinormals(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto mesh = get_param_str(argc, params, 0, NULL);
    // MeshBinormals(mesh);
  } else {
    error(retval, "MeshBinormals", 1);
  }
  return result;
}

int cmd_meshnormalssmooth(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto mesh = get_param_str(argc, params, 0, NULL);
    // MeshNormalsSmooth(mesh);
  } else {
    error(retval, "MeshNormalsSmooth", 1);
  }
  return result;
}

int cmd_meshtangents(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto mesh = get_param_str(argc, params, 0, NULL);
    // MeshTangents(mesh);
  } else {
    error(retval, "MeshTangents", 1);
  }
  return result;
}

int cmd_openurl(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto url = get_param_str(argc, params, 0, NULL);
    // OpenURL(url);
  } else {
    error(retval, "OpenURL", 1);
  }
  return result;
}

int cmd_pauseaudiostream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto stream = get_param_str(argc, params, 0, NULL);
    // PauseAudioStream(stream);
  } else {
    error(retval, "PauseAudioStream", 1);
  }
  return result;
}

int cmd_pausemusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    PauseMusicStream(music);
  } else {
    error(retval, "PauseMusicStream", 1);
  }
  return result;
}

int cmd_pausesound(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    PauseSound(sound);
  } else {
    error(retval, "PauseSound", 1);
  }
  return result;
}

int cmd_playaudiostream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto stream = get_param_str(argc, params, 0, NULL);
    // PlayAudioStream(stream);
  } else {
    error(retval, "PlayAudioStream", 1);
  }
  return result;
}

int cmd_playmusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    PlayMusicStream(music);
  } else {
    error(retval, "PlayMusicStream", 1);
  }
  return result;
}

int cmd_playsound(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    PlaySound(sound);
  } else {
    error(retval, "PlaySound", 1);
  }
  return result;
}

int cmd_playsoundmulti(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    PlaySoundMulti(sound);
  } else {
    error(retval, "PlaySoundMulti", 1);
  }
  return result;
}

int cmd_restorewindow(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    RestoreWindow();
  } else {
    error(retval, "RestoreWindow", 0);
  }
  return result;
}

int cmd_resumeaudiostream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto stream = get_param_str(argc, params, 0, NULL);
    // ResumeAudioStream(stream);
  } else {
    error(retval, "ResumeAudioStream", 1);
  }
  return result;
}

int cmd_resumemusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    ResumeMusicStream(music);
  } else {
    error(retval, "ResumeMusicStream", 1);
  }
  return result;
}

int cmd_resumesound(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    ResumeSound(sound);
  } else {
    error(retval, "ResumeSound", 1);
  }
  return result;
}

int cmd_savefiledata(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto fileName = get_param_str(argc, params, 0, NULL);
    // auto data = get_param_str(argc, params, 1, NULL);
    // auto bytesToWrite = get_param_str(argc, params, 2, NULL);
    // SaveFileData(fileName, data, bytesToWrite);
  } else {
    error(retval, "SaveFileData", 3);
  }
  return result;
}

int cmd_savefiletext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto fileName = get_param_str(argc, params, 0, NULL);
    // auto text = get_param_str(argc, params, 1, NULL);
    // SaveFileText(fileName, text);
  } else {
    error(retval, "SaveFileText", 2);
  }
  return result;
}

int cmd_savestoragevalue(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto position = get_param_str(argc, params, 0, NULL);
    // auto value = get_param_str(argc, params, 1, NULL);
    // SaveStorageValue(position, value);
  } else {
    error(retval, "SaveStorageValue", 2);
  }
  return result;
}

int cmd_setaudiostreambuffersizedefault(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto size = get_param_str(argc, params, 0, NULL);
    // SetAudioStreamBufferSizeDefault(size);
  } else {
    error(retval, "SetAudioStreamBufferSizeDefault", 1);
  }
  return result;
}

int cmd_setaudiostreampitch(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto stream = get_param_str(argc, params, 0, NULL);
    // auto pitch = get_param_str(argc, params, 1, NULL);
    // SetAudioStreamPitch(stream, pitch);
  } else {
    error(retval, "SetAudioStreamPitch", 2);
  }
  return result;
}

int cmd_setaudiostreamvolume(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto stream = get_param_str(argc, params, 0, NULL);
    // auto volume = get_param_str(argc, params, 1, NULL);
    // SetAudioStreamVolume(stream, volume);
  } else {
    error(retval, "SetAudioStreamVolume", 2);
  }
  return result;
}

int cmd_setcameraaltcontrol(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto altKey = get_param_str(argc, params, 0, NULL);
    // SetCameraAltControl(altKey);
  } else {
    error(retval, "SetCameraAltControl", 1);
  }
  return result;
}

int cmd_setcameramode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto mode = get_param_int(argc, params, 1, 0);
    SetCameraMode(get_camera_3d(argc, params, 0), mode);
  } else {
    error(retval, "SetCameraMode", 2);
  }
  return result;
}

int cmd_setcameramovecontrols(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto frontKey = get_param_str(argc, params, 0, NULL);
    // auto backKey = get_param_str(argc, params, 1, NULL);
    // auto rightKey = get_param_str(argc, params, 2, NULL);
    // auto leftKey = get_param_str(argc, params, 3, NULL);
    // auto upKey = get_param_str(argc, params, 4, NULL);
    // auto downKey = get_param_str(argc, params, 5, NULL);
    // SetCameraMoveControls(frontKey, backKey, rightKey, leftKey, upKey, downKey);
  } else {
    error(retval, "SetCameraMoveControls", 6);
  }
  return result;
}

int cmd_setcamerapancontrol(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto panKey = get_param_str(argc, params, 0, NULL);
    // SetCameraPanControl(panKey);
  } else {
    error(retval, "SetCameraPanControl", 1);
  }
  return result;
}

int cmd_setcamerasmoothzoomcontrol(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto szKey = get_param_str(argc, params, 0, NULL);
    // SetCameraSmoothZoomControl(szKey);
  } else {
    error(retval, "SetCameraSmoothZoomControl", 1);
  }
  return result;
}

int cmd_setclipboardtext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto text = get_param_str(argc, params, 0, NULL);
    SetClipboardText(text);
  } else {
    error(retval, "SetClipboardText", 1);
  }
  return result;
}

int cmd_setconfigflags(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto flags = get_param_int(argc, params, 0, 0);
    SetConfigFlags(flags);
  } else {
    error(retval, "SetConfigFlags", 1);
  }
  return result;
}

int cmd_setexitkey(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto key = get_param_int(argc, params, 0, 0);
    SetExitKey(key);
  } else {
    error(retval, "SetExitKey", 1);
  }
  return result;
}

int cmd_setgesturesenabled(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto gestureFlags = get_param_str(argc, params, 0, NULL);
    // SetGesturesEnabled(gestureFlags);
  } else {
    error(retval, "SetGesturesEnabled", 1);
  }
  return result;
}

int cmd_setmastervolume(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto volume = get_param_num(argc, params, 0, 0);
    SetMasterVolume(volume);
  } else {
    error(retval, "SetMasterVolume", 1);
  }
  return result;
}

int cmd_setmaterialtexture(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto material = get_param_str(argc, params, 0, NULL);
    // auto mapType = get_param_str(argc, params, 1, NULL);
    // auto texture = get_param_str(argc, params, 2, NULL);
    // SetMaterialTexture(material, mapType, texture);
  } else {
    error(retval, "SetMaterialTexture", 3);
  }
  return result;
}

int cmd_setmatrixmodelview(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto view = get_param_str(argc, params, 0, NULL);
    // SetMatrixModelview(view);
  } else {
    error(retval, "SetMatrixModelview", 1);
  }
  return result;
}

int cmd_setmatrixprojection(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto proj = get_param_str(argc, params, 0, NULL);
    // SetMatrixProjection(proj);
  } else {
    error(retval, "SetMatrixProjection", 1);
  }
  return result;
}

int cmd_setmodeldiffusetexture(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    int modelId = get_model_id(argc, params, 0);
    int textureId = get_texture_id(argc, params, 1);
    if (modelId != -1 && textureId != -1) {
      _modelMap.at(modelId).materials[0].maps[MAP_DIFFUSE].texture = _textureMap.at(textureId);
    } else {
      error_model(retval);
      result = 0;
    }
  } else {
    error(retval, "SetModelDiffuseTexture", 2);
  }
  return result;
}

int cmd_setmodelmeshmaterial(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto model = get_param_str(argc, params, 0, NULL);
    // auto meshId = get_param_str(argc, params, 1, NULL);
    // auto materialId = get_param_str(argc, params, 2, NULL);
    // SetModelMeshMaterial(model, meshId, materialId);
  } else {
    error(retval, "SetModelMeshMaterial", 3);
  }
  return result;
}

int cmd_setmouseoffset(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto offsetX = get_param_int(argc, params, 0, 0);
    auto offsetY = get_param_int(argc, params, 1, 0);
    SetMouseOffset(offsetX, offsetY);
  } else {
    error(retval, "SetMouseOffset", 2);
  }
  return result;
}

int cmd_setmouseposition(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto x = get_param_int(argc, params, 0, 0);
    auto y = get_param_int(argc, params, 1, 0);
    SetMousePosition(x, y);
  } else {
    error(retval, "SetMousePosition", 2);
  }
  return result;
}

int cmd_setmousescale(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto scaleX = get_param_num(argc, params, 0, 0);
    auto scaleY = get_param_num(argc, params, 1, 0);
    SetMouseScale(scaleX, scaleY);
  } else {
    error(retval, "SetMouseScale", 2);
  }
  return result;
}

int cmd_setmusicpitch(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result && _musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    auto pitch = get_param_int(argc, params, 1, 0);
    SetMusicPitch(music, pitch);
  } else {
    error(retval, "SetMusicPitch", 2);
  }
  return result;
}

int cmd_setmusicvolume(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result && _musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    auto volume = get_param_int(argc, params, 1, 0);
    SetMusicVolume(music, volume);
  } else {
    error(retval, "SetMusicVolume", 2);
  }
  return result;
}

int cmd_setpixelcolor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto dstPtr = get_param_str(argc, params, 0, NULL);
    // auto color = get_param_color(argc, params, 1);
    // auto format = get_param_str(argc, params, 2, NULL);
    // SetPixelColor(dstPtr, color, format);
  } else {
    error(retval, "SetPixelColor", 3);
  }
  return result;
}

int cmd_setshadervalue(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto shader = get_param_str(argc, params, 0, NULL);
    // auto uniformLoc = get_param_str(argc, params, 1, NULL);
    // auto value = get_param_str(argc, params, 2, NULL);
    // auto uniformType = get_param_str(argc, params, 3, NULL);
    // SetShaderValue(shader, uniformLoc, value, uniformType);
  } else {
    error(retval, "SetShaderValue", 4);
  }
  return result;
}

int cmd_setshadervaluematrix(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto shader = get_param_str(argc, params, 0, NULL);
    // auto uniformLoc = get_param_str(argc, params, 1, NULL);
    // auto mat = get_param_str(argc, params, 2, NULL);
    // SetShaderValueMatrix(shader, uniformLoc, mat);
  } else {
    error(retval, "SetShaderValueMatrix", 3);
  }
  return result;
}

int cmd_setshadervaluetexture(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto shader = get_param_str(argc, params, 0, NULL);
    // auto uniformLoc = get_param_str(argc, params, 1, NULL);
    // auto texture = get_param_str(argc, params, 2, NULL);
    // SetShaderValueTexture(shader, uniformLoc, texture);
  } else {
    error(retval, "SetShaderValueTexture", 3);
  }
  return result;
}

int cmd_setshadervaluev(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto shader = get_param_str(argc, params, 0, NULL);
    // auto uniformLoc = get_param_str(argc, params, 1, NULL);
    // auto value = get_param_str(argc, params, 2, NULL);
    // auto uniformType = get_param_str(argc, params, 3, NULL);
    // auto count = get_param_str(argc, params, 4, NULL);
    // SetShaderValueV(shader, uniformLoc, value, uniformType, count);
  } else {
    error(retval, "SetShaderValueV", 5);
  }
  return result;
}

int cmd_setshapestexture(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto texture = get_param_str(argc, params, 0, NULL);
    // auto source = get_param_str(argc, params, 1, NULL);
    // SetShapesTexture(texture, source);
  } else {
    error(retval, "SetShapesTexture", 2);
  }
  return result;
}

int cmd_setsoundpitch(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result && _soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    auto pitch = get_param_int(argc, params, 1, 0);
    SetSoundPitch(sound, pitch);
  } else {
    error(retval, "SetSoundPitch", 2);
  }
  return result;
}

int cmd_setsoundvolume(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result && _soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    auto volume = get_param_int(argc, params, 1, 0);
    SetSoundVolume(sound, volume);
  } else {
    error(retval, "SetSoundVolume", 2);
  }
  return result;
}

int cmd_settargetfps(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto fps = get_param_int(argc, params, 0, 50);
    SetTargetFPS(fps);
  } else {
    error(retval, "SetTargetFPS", 1);
  }
  return result;
}

int cmd_settexturefilter(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto texture = get_param_str(argc, params, 0, NULL);
    // auto filterMode = get_param_str(argc, params, 1, NULL);
    // SetTextureFilter(texture, filterMode);
  } else {
    error(retval, "SetTextureFilter", 2);
  }
  return result;
}

int cmd_settexturewrap(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto texture = get_param_str(argc, params, 0, NULL);
    // auto wrapMode = get_param_str(argc, params, 1, NULL);
    // SetTextureWrap(texture, wrapMode);
  } else {
    error(retval, "SetTextureWrap", 2);
  }
  return result;
}

int cmd_settracelogcallback(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto callback = get_param_str(argc, params, 0, NULL);
    // SetTraceLogCallback(callback);
  } else {
    error(retval, "SetTraceLogCallback", 1);
  }
  return result;
}

int cmd_settracelogexit(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto logType = get_param_int(argc, params, 0, 0);
    SetTraceLogExit(logType);
  } else {
    error(retval, "SetTraceLogExit", 1);
  }
  return result;
}

int cmd_settraceloglevel(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto logType = get_param_int(argc, params, 0, 0);
    SetTraceLogLevel(logType);
  } else {
    error(retval, "SetTraceLogLevel", 1);
  }
  return result;
}

int cmd_setvrconfiguration(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto info = get_param_str(argc, params, 0, NULL);
    // auto distortion = get_param_str(argc, params, 1, NULL);
    // SetVrConfiguration(info, distortion);
  } else {
    error(retval, "SetVrConfiguration", 2);
  }
  return result;
}

int cmd_setwindowicon(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // SetWindowIcon(image);
  } else {
    error(retval, "SetWindowIcon", 1);
  }
  return result;
}

int cmd_setwindowminsize(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto width = get_param_int(argc, params, 0, 0);
    auto height = get_param_int(argc, params, 1, 0);
    SetWindowMinSize(width, height);
  } else {
    error(retval, "SetWindowMinSize", 2);
  }
  return result;
}

int cmd_setwindowmonitor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto monitor = get_param_int(argc, params, 0, 0);
    SetWindowMonitor(monitor);
  } else {
    error(retval, "SetWindowMonitor", 1);
  }
  return result;
}

int cmd_setwindowposition(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto x = get_param_int(argc, params, 0, 0);
    auto y = get_param_int(argc, params, 1, 0);
    SetWindowPosition(x, y);
  } else {
    error(retval, "SetWindowPosition", 2);
  }
  return result;
}

int cmd_setwindowsize(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto width = get_param_int(argc, params, 0, 640);
    auto height = get_param_int(argc, params, 1, 480);
    SetWindowSize(width, height);
  } else {
    error(retval, "SetWindowSize", 2);
  }
  return result;
}

int cmd_setwindowtitle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto title = get_param_str(argc, params, 0, NULL);
    SetWindowTitle(title);
  } else {
    error(retval, "SetWindowTitle", 1);
  }
  return result;
}

int cmd_showcursor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    ShowCursor();
  } else {
    error(retval, "ShowCursor", 0);
  }
  return result;
}

int cmd_stopaudiostream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto stream = get_param_str(argc, params, 0, NULL);
    // StopAudioStream(stream);
  } else {
    error(retval, "StopAudioStream", 1);
  }
  return result;
}

int cmd_stopmusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    StopMusicStream(music);
  } else {
    error(retval, "StopMusicStream", 1);
  }
  return result;
}

int cmd_stopsound(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    StopSound(sound);
  } else {
    error(retval, "StopSound", 1);
  }
  return result;
}

int cmd_stopsoundmulti(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    StopSoundMulti();
  } else {
    error(retval, "StopSoundMulti", 0);
  }
  return result;
}

int cmd_takescreenshot(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto fileName = get_param_str(argc, params, 0, NULL);
    // TakeScreenshot(fileName);
  } else {
    error(retval, "TakeScreenshot", 1);
  }
  return result;
}

int cmd_textappend(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto text = get_param_str(argc, params, 0, NULL);
    // auto append = get_param_str(argc, params, 1, NULL);
    // auto position = get_param_str(argc, params, 2, NULL);
    // TextAppend(text, append, position);
  } else {
    error(retval, "TextAppend", 3);
  }
  return result;
}

int cmd_togglefullscreen(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    ToggleFullscreen();
  } else {
    error(retval, "ToggleFullscreen", 0);
  }
  return result;
}

int cmd_togglevrmode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    ToggleVrMode();
  } else {
    error(retval, "ToggleVrMode", 0);
  }
  return result;
}

int cmd_tracelog(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto logType = get_param_str(argc, params, 0, NULL);
    // auto text = get_param_str(argc, params, 1, NULL);
    // TraceLog(logType, text);
  } else {
    error(retval, "TraceLog", 2);
  }
  return result;
}

int cmd_undecoratewindow(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    UndecorateWindow();
  } else {
    error(retval, "UndecorateWindow", 0);
  }
  return result;
}

int cmd_unhidewindow(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    UnhideWindow();
  } else {
    error(retval, "UnhideWindow", 0);
  }
  return result;
}

int cmd_unloadfont(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto font = get_param_str(argc, params, 0, NULL);
    // UnloadFont(font);
  } else {
    error(retval, "UnloadFont", 1);
  }
  return result;
}

int cmd_unloadimage(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // UnloadImage(image);
  } else {
    error(retval, "UnloadImage", 1);
  }
  return result;
}

int cmd_unloadmaterial(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto material = get_param_str(argc, params, 0, NULL);
    // UnloadMaterial(material);
  } else {
    error(retval, "UnloadMaterial", 1);
  }
  return result;
}

int cmd_unloadmesh(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto mesh = get_param_str(argc, params, 0, NULL);
    // UnloadMesh(mesh);
  } else {
    error(retval, "UnloadMesh", 1);
  }
  return result;
}

int cmd_unloadmodel(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    int id = get_model_id(argc, params, 0);
    if (id != -1) {
      UnloadModel(_modelMap.at(id));
      _modelMap.erase(id);
    } else {
      error_model(retval);
      result = 0;
    }
  } else {
    error(retval, "UnloadModel", 1);
  }
  return result;
}

int cmd_unloadmodelanimation(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto anim = get_param_str(argc, params, 0, NULL);
    // UnloadModelAnimation(anim);
  } else {
    error(retval, "UnloadModelAnimation", 1);
  }
  return result;
}

int cmd_unloadmusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto id = get_param_int(argc, params, 0, 0);
    auto music = _musicMap.at(id);
    UnloadMusicStream(music);
    _musicMap.erase(id);
  } else {
    error(retval, "UnloadMusicStream", 1);
  }
  return result;
}

int cmd_unloadrendertexture(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    int id = get_render_texture_id(argc, params, 0);
    if (id != -1) {
      UnloadRenderTexture(_renderMap.at(id));
      _renderMap.erase(id);
    } else {
      error_texture(retval);
      result = 0;
    }
  } else {
    error(retval, "UnloadRenderTexture", 1);
  }
  return result;
}

int cmd_unloadshader(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    UnloadShader(get_param_shader(argc, params, 0));
  } else {
    error(retval, "UnloadShader", 1);
  }
  return result;
}

int cmd_unloadsound(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto id = get_param_int(argc, params, 0, 0);
    auto sound = _soundMap.at(id);
    UnloadSound(sound);
    _soundMap.erase(id);
  } else {
    error(retval, "UnloadSound", 1);
  }
  return result;
}

int cmd_unloadtexture(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && is_param_map(argc, params, 0)) {
    int id = map_get_int(params[0].var_p, mapID, -1);
    if (id != -1 && _textureMap.find(id) != _textureMap.end()) {
      UnloadTexture(_textureMap.at(id));
      _textureMap.erase(id);
    } else {
      error_texture(retval);
      result = 0;
    }
  } else {
    error(retval, "UnloadTexture", 1);
  }
  return result;
}

int cmd_unloadwave(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto wave = get_param_str(argc, params, 0, NULL);
    // UnloadWave(wave);
  } else {
    error(retval, "UnloadWave", 1);
  }
  return result;
}

int cmd_updateaudiostream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto stream = get_param_str(argc, params, 0, NULL);
    // auto data = get_param_str(argc, params, 1, NULL);
    // auto samplesCount = get_param_str(argc, params, 2, NULL);
    // UpdateAudioStream(stream, data, samplesCount);
  } else {
    error(retval, "UpdateAudioStream", 3);
  }
  return result;
}

int cmd_updatecamera(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto camera = get_camera_3d(argc, params, 0);
    UpdateCamera(&camera);
    var_p_t array = params[0].var_p;
    if (is_array(array, 5)) {
      var_p_t v_position = v_elem(array, 0);
      if (is_array(v_position, 3)) {
        v_setreal(v_elem(v_position, 0), camera.position.x);
        v_setreal(v_elem(v_position, 1), camera.position.y);
        v_setreal(v_elem(v_position, 2), camera.position.z);
      }
      var_p_t v_target = v_elem(array, 1);
      if (is_array(v_target, 3)) {
        v_setreal(v_elem(v_target, 0), camera.target.x);
        v_setreal(v_elem(v_target, 1), camera.target.y);
        v_setreal(v_elem(v_target, 2), camera.target.z);
      }
      var_p_t v_up = v_elem(array, 2);
      if (is_array(v_up, 3)) {
        v_setreal(v_elem(v_up, 0), camera.up.x);
        v_setreal(v_elem(v_up, 1), camera.up.y);
        v_setreal(v_elem(v_up, 2), camera.up.z);
      }
    }
  } else {
    error(retval, "UpdateCamera", 1);
  }
  return result;
}

int cmd_updatemodelanimation(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto model = get_param_str(argc, params, 0, NULL);
    // auto anim = get_param_str(argc, params, 1, NULL);
    // auto frame = get_param_str(argc, params, 2, NULL);
    // UpdateModelAnimation(model, anim, frame);
  } else {
    error(retval, "UpdateModelAnimation", 3);
  }
  return result;
}

int cmd_updatemusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    UpdateMusicStream(music);
  } else {
    error(retval, "UpdateMusicStream", 1);
  }
  return result;
}

int cmd_updatesound(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result && _soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    //auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    //auto data = get_param_int(argc, params, 1, 0);
    //auto samplesCount = get_param_int(argc, params, 2, 0);
    //UpdateSound(sound, data, samplesCount);
  } else {
    error(retval, "UpdateSound", 3);
  }
  return result;
}

int cmd_updatetexture(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    int id = get_texture_id(argc, params, 0);
    if (id != -1 && params[1].var_p->type == V_INT) {
      Color *pixels = (Color *)params[1].var_p->v.i;
      UpdateTexture(_textureMap.at(id), pixels);

      // cleanup for rl.GetImageData(img)
      free(pixels);
    } else {
      error_texture(retval);
      result = 0;
    }
  } else {
    error(retval, "UpdateTexture", 2);
  }
  return result;
}

int cmd_updatetexturerec(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto texture = get_param_str(argc, params, 0, NULL);
    // auto rec = get_param_str(argc, params, 1, NULL);
    // auto pixels = get_param_str(argc, params, 2, NULL);
    // UpdateTextureRec(texture, rec, pixels);
  } else {
    error(retval, "UpdateTextureRec", 3);
  }
  return result;
}

int cmd_updatevrtracking(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto camera = get_param_str(argc, params, 0, NULL);
    // UpdateVrTracking(camera);
  } else {
    error(retval, "UpdateVrTracking", 1);
  }
  return result;
}

int cmd_wavecrop(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto wave = get_param_str(argc, params, 0, NULL);
    // auto initSample = get_param_str(argc, params, 1, NULL);
    // auto finalSample = get_param_str(argc, params, 2, NULL);
    // WaveCrop(wave, initSample, finalSample);
  } else {
    error(retval, "WaveCrop", 3);
  }
  return result;
}

int cmd_waveformat(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto wave = get_param_str(argc, params, 0, NULL);
    // auto sampleRate = get_param_str(argc, params, 1, NULL);
    // auto sampleSize = get_param_str(argc, params, 2, NULL);
    // auto channels = get_param_str(argc, params, 3, NULL);
    // WaveFormat(wave, sampleRate, sampleSize, channels);
  } else {
    error(retval, "WaveFormat", 4);
  }
  return result;
}

int cmd_guibutton(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto bounds = get_param_rect(argc, params, 0);
    auto text = get_param_str(argc, params, 1, 0);
    auto fnResult = GuiButton(bounds, text);
    v_setint(retval, fnResult);
  } else {
    error(retval, "GuiButton", 2);
  }
  return result;
}

int cmd_guicheckbox(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    auto bounds = get_param_rect(argc, params, 0);
    auto text = get_param_str(argc, params, 1, 0);
    auto checked = get_param_int(argc, params, 2, 0);
    auto fnResult = GuiCheckBox(bounds, text, checked);
    v_setint(retval, fnResult);
  } else {
    error(retval, "GuiCheckBox", 3);
  }
  return result;
}

int cmd_guicolorbaralpha(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto bounds = get_param_rect(argc, params, 0);
    auto alpha = get_param_num(argc, params, 1, 0);
    auto fnResult = GuiColorBarAlpha(bounds, alpha);
    v_setreal(retval, fnResult);
  } else {
    error(retval, "GuiColorBarAlpha", 2);
  }
  return result;
}

int cmd_guicolorbarhue(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto bounds = get_param_rect(argc, params, 0);
    auto value = get_param_int(argc, params, 1, 0);
    auto fnResult = GuiColorBarHue(bounds, value);
    v_setreal(retval, fnResult);
  } else {
    error(retval, "GuiColorBarHue", 2);
  }
  return result;
}

int cmd_guicolorpanel(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto bounds = get_param_rect(argc, params, 0);
    auto color = get_param_color(argc, params, 1);
    auto fnResult = GuiColorPanel(bounds, color);
    v_setint(retval, get_color_int(fnResult));
  } else {
    error(retval, "GuiColorPanel", 2);
  }
  return result;
}

int cmd_guicolorpicker(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto bounds = get_param_rect(argc, params, 0);
    auto color = get_param_color(argc, params, 1);
    auto fnResult = GuiColorPicker(bounds, color);
    v_setint(retval, get_color_int(fnResult));
  } else {
    error(retval, "GuiColorPicker", 2);
  }
  return result;
}

int cmd_guicombobox(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    auto bounds = get_param_rect(argc, params, 0);
    auto text = get_param_str(argc, params, 1, 0);
    auto active = get_param_int(argc, params, 2, 0);
    auto fnResult = GuiComboBox(bounds, text, active);
    v_setint(retval, fnResult);
  } else {
    error(retval, "GuiComboBox", 3);
  }
  return result;
}

int cmd_guidropdownbox(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto bounds = get_param_rect(argc, params, 0);
    // auto text = get_param_str(argc, params, 1, 0);
    // auto active = get_param_int(argc, params, 2, 0);
    // auto bool = get_param_int(argc, params, 3, 0);
    // auto editMode = get_param_int(argc, params, 4, 0);
    // auto fnResult = GuiDropdownBox(bounds, text, active, bool, editMode);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GuiDropdownBox", 5);
  }
  return result;
}

int cmd_guigetfont(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    //auto fnResult = GuiGetFont();
    //v_setint(retval, fnResult);
  } else {
    error(retval, "GuiGetFont", 0);
  }
  return result;
}

int cmd_guigetstate(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GuiGetState();
    v_setint(retval, fnResult);
  } else {
    error(retval, "GuiGetState", 0);
  }
  return result;
}

int cmd_guigetstyle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto control = get_param_int(argc, params, 0, 0);
    auto property = get_param_int(argc, params, 1, 0);
    auto fnResult = GuiGetStyle(control, property);
    v_setint(retval, fnResult);
  } else {
    error(retval, "GuiGetStyle",2);
  }
  return result;
}

int cmd_guigrid(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    //auto bounds = get_param_rect(argc, params, 0);
    //auto spacing = get_param_int(argc, params, 1, 0);
    //auto subdivs = get_param_int(argc, params, 2, 0);
    //auto fnResult = GuiGrid(bounds, spacing, subdivs);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "GuiGrid", 3);
  }
  return result;
}

int cmd_guiimagebutton(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto bounds = get_param_rect(argc, params, 0);
    // auto text = get_param_str(argc, params, 1, 0);
    // auto texture = get_param_int(argc, params, 2, 0);
    // auto fnResult = GuiImageButton(bounds, text, texture);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GuiImageButton", 3);
  }
  return result;
}

int cmd_guiimagebuttonex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto bounds = get_param_rect(argc, params, 0);
    // auto text = get_param_str(argc, params, 1, 0);
    // auto texture = get_param_int(argc, params, 2, 0);
    // auto texSource = get_param_int(argc, params, 3, 0);
    // auto fnResult = GuiImageButtonEx(bounds, text, texture, texSource);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GuiImageButtonEx", 4);
  }
  return result;
}

int cmd_guilabelbutton(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto bounds = get_param_rect(argc, params, 0);
    auto text = get_param_str(argc, params, 1, 0);
    auto fnResult = GuiLabelButton(bounds, text);
    v_setint(retval, fnResult);
  } else {
    error(retval, "GuiLabelButton", 2);
  }
  return result;
}

int cmd_guilistview(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto bounds = get_param_rect(argc, params, 0);
    // auto text = get_param_str(argc, params, 1, 0);
    // auto scrollIndex = get_param_int(argc, params, 2, 0);
    // auto active = get_param_int(argc, params, 3, 0);
    // auto fnResult = GuiListView(bounds, text, scrollIndex, active);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GuiListView", 4);
  }
  return result;
}

int cmd_guilistviewex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto bounds = get_param_rect(argc, params, 0);
    // auto text = get_param_str(argc, params, 1, 0);
    // auto count = get_param_int(argc, params, 2, 0);
    // auto focus = get_param_int(argc, params, 3, 0);
    // auto scrollIndex = get_param_int(argc, params, 4, 0);
    // auto active = get_param_int(argc, params, 5, 0);
    // auto fnResult = GuiListViewEx(bounds, text, count, focus, scrollIndex, active);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GuiListViewEx", 6);
  }
  return result;
}

int cmd_guimessagebox(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto bounds = get_param_rect(argc, params, 0);
    // auto title = get_param_int(argc, params, 1, 0);
    // auto message = get_param_int(argc, params, 2, 0);
    // auto buttons = get_param_int(argc, params, 3, 0);
    // auto fnResult = GuiMessageBox(bounds, title, message, buttons);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GuiMessageBox", 5);
  }
  return result;
}

int cmd_guiprogressbar(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto bounds = get_param_rect(argc, params, 0);
    // auto textLeft = get_param_int(argc, params, 1, 0);
    // auto textRight = get_param_int(argc, params, 2, 0);
    // auto value = get_param_num(argc, params, 3, 0);
    // auto minValue = get_param_num(argc, params, 4, 0);
    // auto maxValue = get_param_num(argc, params, 5, 0);
    // auto fnResult = GuiProgressBar(bounds, textLeft, textRight, value, minValue, maxValue);
    // v_setreal(retval, fnResult);
  } else {
    error(retval, "GuiProgressBar", 6);
  }
  return result;
}

int cmd_guiscrollbar(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto bounds = get_param_rect(argc, params, 0);
    // auto value = get_param_num(argc, params, 1, 0);
    // auto minValue = get_param_num(argc, params, 2, 0);
    // auto maxValue = get_param_num(argc, params, 3, 0);
    // auto fnResult = GuiScrollBar(bounds, value, minValue, maxValue);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GuiScrollBar", 4);
  }
  return result;
}

int cmd_guiscrollpanel(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    //auto bounds = get_param_rect(argc, params, 0);
    //auto content = get_param_int(argc, params, 1, 0);
    //auto scroll = get_param_int(argc, params, 2, 0);
    //auto fnResult = GuiScrollPanel(bounds, content, scroll);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "GuiScrollPanel", 3);
  }
  return result;
}

int cmd_guislider(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    auto bounds = get_param_rect(argc, params, 0);
    auto textLeft = get_param_str(argc, params, 1, 0);
    auto textRight = get_param_str(argc, params, 2, 0);
    auto value = get_param_num(argc, params, 3, 0);
    auto minValue = get_param_num(argc, params, 4, 0);
    auto maxValue = get_param_num(argc, params, 5, 0);
    auto fnResult = GuiSlider(bounds, textLeft, textRight, value, minValue, maxValue);
    v_setreal(retval, fnResult);
  } else {
    error(retval, "GuiSlider", 6);
  }
  return result;
}

int cmd_guisliderbar(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    auto bounds = get_param_rect(argc, params, 0);
    auto textLeft = get_param_str(argc, params, 1, 0);
    auto textRight = get_param_str(argc, params, 2, 0);
    auto value = get_param_num(argc, params, 3, 0);
    auto minValue = get_param_num(argc, params, 4, 0);
    auto maxValue = get_param_num(argc, params, 5, 0);
    auto fnResult = GuiSliderBar(bounds, textLeft, textRight, value, minValue, maxValue);
    v_setreal(retval, fnResult);
  } else {
    error(retval, "GuiSliderBar", 6);
  }
  return result;
}

int cmd_guispinner(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    // auto bounds = get_param_rect(argc, params, 0);
    // auto text = get_param_str(argc, params, 1, 0);
    // auto value = get_param_num(argc, params, 2, 0);
    // auto minValue = get_param_num(argc, params, 3, 0);
    // auto maxValue = get_param_num(argc, params, 4, 0);
    // auto bool = get_param_int(argc, params, 5, 0);
    // auto editMode = get_param_int(argc, params, 6, 0);
    // auto fnResult = GuiSpinner(bounds, text, value, minValue, maxValue, bool, editMode);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GuiSpinner", 7);
  }
  return result;
}

int cmd_guitextbox(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto bounds = get_param_rect(argc, params, 0);
    // auto text = get_param_str(argc, params, 1, 0);
    // auto textSize = get_param_int(argc, params, 2, 0);
    // auto bool = get_param_int(argc, params, 3, 0);
    // auto editMode = get_param_int(argc, params, 4, 0);
    // auto fnResult = GuiTextBox(bounds, text, textSize, bool, editMode);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GuiTextBox", 5);
  }
  return result;
}

int cmd_guitextboxmulti(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto bounds = get_param_rect(argc, params, 0);
    // auto text = get_param_str(argc, params, 1, 0);
    // auto textSize = get_param_int(argc, params, 2, 0);
    // auto bool = get_param_int(argc, params, 3, 0);
    // auto editMode = get_param_int(argc, params, 4, 0);
    // auto fnResult = GuiTextBoxMulti(bounds, text, textSize, bool, editMode);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GuiTextBoxMulti", 5);
  }
  return result;
}

int cmd_guitextinputbox(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto bounds = get_param_rect(argc, params, 0);
    // auto title = get_param_int(argc, params, 1, 0);
    // auto message = get_param_int(argc, params, 2, 0);
    // auto buttons = get_param_int(argc, params, 3, 0);
    // auto text = get_param_str(argc, params, 4, 0);
    // auto fnResult = GuiTextInputBox(bounds, title, message, buttons, text);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GuiTextInputBox", 5);
  }
  return result;
}

int cmd_guitoggle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto bounds = get_param_rect(argc, params, 0);
    // auto text = get_param_str(argc, params, 1, 0);
    // auto bool = get_param_int(argc, params, 2, 0);
    // auto active = get_param_int(argc, params, 3, 0);
    // auto fnResult = GuiToggle(bounds, text, bool, active);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GuiToggle", 4);
  }
  return result;
}

int cmd_guitogglegroup(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    auto bounds = get_param_rect(argc, params, 0);
    auto text = get_param_str(argc, params, 1, 0);
    auto active = get_param_int(argc, params, 2, 0);
    auto fnResult = GuiToggleGroup(bounds, text, active);
    v_setint(retval, fnResult);
  } else {
    error(retval, "GuiToggleGroup", 3);
  }
  return result;
}

int cmd_guivaluebox(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    // auto bounds = get_param_rect(argc, params, 0);
    // auto text = get_param_str(argc, params, 1, 0);
    // auto value = get_param_num(argc, params, 2, 0);
    // auto minValue = get_param_num(argc, params, 3, 0);
    // auto maxValue = get_param_num(argc, params, 4, 0);
    // auto bool = get_param_int(argc, params, 5, 0);
    // auto editMode = get_param_int(argc, params, 6, 0);
    // auto fnResult = GuiValueBox(bounds, text, value, minValue, maxValue, bool, editMode);
    // v_setint(retval, fnResult);
  } else {
    error(retval, "GuiValueBox", 7);
  }
  return result;
}

int cmd_guiwindowbox(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    //auto bounds = get_param_rect(argc, params, 0);
    //auto title = get_param_int(argc, params, 1, 0);
    //auto fnResult = GuiWindowBox(bounds, title);
    //v_setint(retval, fnResult);
  } else {
    error(retval, "GuiWindowBox", 2);
  }
  return result;
}

int cmd_guicleartooltip(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    GuiClearTooltip();
  } else {
    error(retval, "GuiClearTooltip", 0);
  }
  return result;
}

int cmd_guidisable(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    GuiDisable();
  } else {
    error(retval, "GuiDisable", 0);
  }
  return result;
}

int cmd_guidisabletooltip(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    GuiDisableTooltip();
  } else {
    error(retval, "GuiDisableTooltip", 0);
  }
  return result;
}

int cmd_guidummyrec(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto bounds = get_param_rect(argc, params, 0);
    auto text = get_param_str(argc, params, 1, 0);
    GuiDummyRec(bounds, text);
  } else {
    error(retval, "GuiDummyRec", 2);
  }
  return result;
}

int cmd_guienable(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    GuiEnable();
  } else {
    error(retval, "GuiEnable", 0);
  }
  return result;
}

int cmd_guienabletooltip(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    GuiEnableTooltip();
  } else {
    error(retval, "GuiEnableTooltip", 0);
  }
  return result;
}

int cmd_guifade(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto alpha = get_param_num(argc, params, 0, 0);
    GuiFade(alpha);
  } else {
    error(retval, "GuiFade", 0);
  }
  return result;
}

int cmd_guigroupbox(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto bounds = get_param_rect(argc, params, 0);
    auto text = get_param_str(argc, params, 1, 0);
    GuiGroupBox(bounds, text);
  } else {
    error(retval, "GuiGroupBox", 2);
  }
  return result;
}

int cmd_guilabel(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto bounds = get_param_rect(argc, params, 0);
    auto text = get_param_str(argc, params, 1, 0);
    GuiLabel(bounds, text);
  } else {
    error(retval, "GuiLabel", 2);
  }
  return result;
}

int cmd_guiline(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto bounds = get_param_rect(argc, params, 0);
    auto text = get_param_str(argc, params, 1, 0);
    GuiLine(bounds, text);
  } else {
    error(retval, "GuiLine", 2);
  }
  return result;
}

int cmd_guiloadstyle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto fileName = get_param_str(argc, params, 0, 0);
    GuiLoadStyle(fileName);
  } else {
    error(retval, "GuiLoadStyle", 2);
  }
  return result;
}

int cmd_guiloadstyledefault(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    GuiLoadStyleDefault();
  } else {
    error(retval, "GuiLoadStyleDefault", 2);
  }
  return result;
}

int cmd_guilock(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    GuiLock();
  } else {
    error(retval, "GuiLock", 0);
  }
  return result;
}

int cmd_guipanel(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto bounds = get_param_rect(argc, params, 0);
    GuiPanel(bounds);
  } else {
    error(retval, "GuiPanel", 1);
  }
  return result;
}

int cmd_guisetfont(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    //auto font = get_param_int(argc, params, 0, 0);
    //GuiSetFont(font);
  } else {
    error(retval, "GuiSetFont", 1);
  }
  return result;
}

int cmd_guisetstate(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto state = get_param_int(argc, params, 0, 0);
    GuiSetState(state);
  } else {
    error(retval, "GuiSetState", 1);
  }
  return result;
}

int cmd_guisetstyle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    auto control = get_param_int(argc, params, 0, 0);
    auto property = get_param_int(argc, params, 1, 0);
    auto value = get_param_int(argc, params, 2, 0);
    GuiSetStyle(control, property, value);
  } else {
    error(retval, "GuiSetStyle", 3);
  }
  return result;
}

int cmd_guisettooltip(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto tooltip = get_param_str(argc, params, 0, 0);
    GuiSetTooltip(tooltip);
  } else {
    error(retval, "GuiSetTooltip", 1);
  }
  return result;
}

int cmd_guistatusbar(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto bounds = get_param_rect(argc, params, 0);
    auto text = get_param_str(argc, params, 1, 0);
    GuiStatusBar(bounds, text);
  } else {
    error(retval, "GuiStatusBar", 2);
  }
  return result;
}

int cmd_guiunlock(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    GuiUnlock();
  } else {
    error(retval, "GuiUnlock", 0);
  }
  return result;
}

API lib_func[] = {
  {"CHANGEDIRECTORY", cmd_changedirectory},
  {"CHECKCOLLISIONBOXES", cmd_checkcollisionboxes},
  {"CHECKCOLLISIONBOXSPHERE", cmd_checkcollisionboxsphere},
  {"CHECKCOLLISIONCIRCLEREC", cmd_checkcollisioncirclerec},
  {"CHECKCOLLISIONCIRCLES", cmd_checkcollisioncircles},
  {"CHECKCOLLISIONPOINTCIRCLE", cmd_checkcollisionpointcircle},
  {"CHECKCOLLISIONPOINTREC", cmd_checkcollisionpointrec},
  {"CHECKCOLLISIONPOINTTRIANGLE", cmd_checkcollisionpointtriangle},
  // {"CHECKCOLLISIONRAYBOX", cmd_checkcollisionraybox},
  // {"CHECKCOLLISIONRAYSPHERE", cmd_checkcollisionraysphere},
  // {"CHECKCOLLISIONRAYSPHEREEX", cmd_checkcollisionraysphereex},
  {"CHECKCOLLISIONRECS", cmd_checkcollisionrecs},
  {"CHECKCOLLISIONSPHERES", cmd_checkcollisionspheres},
  // {"CODEPOINTTOUTF8", cmd_codepointtoutf8},
  // {"COLORALPHA", cmd_coloralpha},
  // {"COLORALPHABLEND", cmd_coloralphablend},
  // {"COLORFROMHSV", cmd_colorfromhsv},
  // {"COLORFROMNORMALIZED", cmd_colorfromnormalized},
  // {"COLORNORMALIZE", cmd_colornormalize},
  // {"COLORTOHSV", cmd_colortohsv},
  // {"COLORTOINT", cmd_colortoint},
  // {"COMPRESSDATA", cmd_compressdata},
  // {"DECOMPRESSDATA", cmd_decompressdata},
  {"DIRECTORYEXISTS", cmd_directoryexists},
  {"FADE", cmd_fade},
  // {"FILEEXISTS", cmd_fileexists},
  {"GENIMAGECELLULAR", cmd_genimagecellular},
  {"GENIMAGECHECKED", cmd_genimagechecked},
  {"GENIMAGECOLOR", cmd_genimagecolor},
  // {"GENIMAGEFONTATLAS", cmd_genimagefontatlas},
  {"GENIMAGEGRADIENTH", cmd_genimagegradienth},
  {"GENIMAGEGRADIENTRADIAL", cmd_genimagegradientradial},
  {"GENIMAGEGRADIENTV", cmd_genimagegradientv},
  {"GENIMAGEPERLINNOISE", cmd_genimageperlinnoise},
  {"GENIMAGEWHITENOISE", cmd_genimagewhitenoise},
  // {"GENMESHCUBE", cmd_genmeshcube},
  // {"GENMESHCUBICMAP", cmd_genmeshcubicmap},
  // {"GENMESHCYLINDER", cmd_genmeshcylinder},
  // {"GENMESHHEIGHTMAP", cmd_genmeshheightmap},
  // {"GENMESHHEMISPHERE", cmd_genmeshhemisphere},
  // {"GENMESHKNOT", cmd_genmeshknot},
  // {"GENMESHPLANE", cmd_genmeshplane},
  // {"GENMESHPOLY", cmd_genmeshpoly},
  // {"GENMESHSPHERE", cmd_genmeshsphere},
  // {"GENMESHTORUS", cmd_genmeshtorus},
  // {"GENTEXTUREBRDF", cmd_gentexturebrdf},
  // {"GENTEXTURECUBEMAP", cmd_gentexturecubemap},
  // {"GENTEXTUREIRRADIANCE", cmd_gentextureirradiance},
  // {"GENTEXTUREPREFILTER", cmd_gentextureprefilter},
  // {"GETCAMERAMATRIX", cmd_getcameramatrix},
  // {"GETCAMERAMATRIX2D", cmd_getcameramatrix2d},
  {"GETCLIPBOARDTEXT", cmd_getclipboardtext},
  // {"GETCODEPOINTS", cmd_getcodepoints},
  // {"GETCODEPOINTSCOUNT", cmd_getcodepointscount},
  // {"GETCOLLISIONRAYGROUND", cmd_getcollisionrayground},
  // {"GETCOLLISIONRAYMODEL", cmd_getcollisionraymodel},
  // {"GETCOLLISIONRAYTRIANGLE", cmd_getcollisionraytriangle},
  {"GETCOLLISIONREC", cmd_getcollisionrec},
  {"GETCOLOR", cmd_getcolor},
  // {"GETDIRECTORYFILES", cmd_getdirectoryfiles},
  // {"GETDIRECTORYPATH", cmd_getdirectorypath},
  // {"GETDROPPEDFILES", cmd_getdroppedfiles},
  // {"GETFILEEXTENSION", cmd_getfileextension},
  // {"GETFILEMODTIME", cmd_getfilemodtime},
  // {"GETFILENAME", cmd_getfilename},
  // {"GETFILENAMEWITHOUTEXT", cmd_getfilenamewithoutext},
  // {"GETFONTDEFAULT", cmd_getfontdefault},
  {"GETFPS", cmd_getfps},
  {"GETFRAMETIME", cmd_getframetime},
  // {"GETGAMEPADAXISCOUNT", cmd_getgamepadaxiscount},
  // {"GETGAMEPADAXISMOVEMENT", cmd_getgamepadaxismovement},
  {"GETGAMEPADBUTTONPRESSED", cmd_getgamepadbuttonpressed},
  // {"GETGAMEPADNAME", cmd_getgamepadname},
  {"GETGESTUREDETECTED", cmd_getgesturedetected},
  {"GETGESTUREDRAGANGLE", cmd_getgesturedragangle},
  // {"GETGESTUREDRAGVECTOR", cmd_getgesturedragvector},
  {"GETGESTUREHOLDDURATION", cmd_getgestureholdduration},
  {"GETGESTUREPINCHANGLE", cmd_getgesturepinchangle},
  // {"GETGESTUREPINCHVECTOR", cmd_getgesturepinchvector},
  // {"GETGLYPHINDEX", cmd_getglyphindex},
  // {"GETIMAGEALPHABORDER", cmd_getimagealphaborder},
  {"GETIMAGEDATA", cmd_getimagedata},
  // {"GETIMAGEDATANORMALIZED", cmd_getimagedatanormalized},
  // {"GETIMAGEPALETTE", cmd_getimagepalette},
  {"GETKEYPRESSED", cmd_getkeypressed},
  // {"GETMATRIXMODELVIEW", cmd_getmatrixmodelview},
  // {"GETMATRIXPROJECTION", cmd_getmatrixprojection},
  {"GETMONITORCOUNT", cmd_getmonitorcount},
  {"GETMONITORHEIGHT", cmd_getmonitorheight},
  {"GETMONITORNAME", cmd_getmonitorname},
  {"GETMONITORPHYSICALHEIGHT", cmd_getmonitorphysicalheight},
  {"GETMONITORPHYSICALWIDTH", cmd_getmonitorphysicalwidth},
  {"GETMONITORREFRESHRATE", cmd_getmonitorrefreshrate},
  {"GETMONITORWIDTH", cmd_getmonitorwidth},
  {"GETMOUSEPOSITION", cmd_getmouseposition},
  // {"GETMOUSERAY", cmd_getmouseray},
  {"GETMOUSEWHEELMOVE", cmd_getmousewheelmove},
  {"GETMOUSEX", cmd_getmousex},
  {"GETMOUSEY", cmd_getmousey},
  {"GETMUSICTIMELENGTH", cmd_getmusictimelength},
  {"GETMUSICTIMEPLAYED", cmd_getmusictimeplayed},
  // {"GETNEXTCODEPOINT", cmd_getnextcodepoint},
  // {"GETPIXELCOLOR", cmd_getpixelcolor},
  // {"GETPIXELDATASIZE", cmd_getpixeldatasize},
  // {"GETPREVDIRECTORYPATH", cmd_getprevdirectorypath},
  // {"GETRANDOMVALUE", cmd_getrandomvalue},
  // {"GETSCREENDATA", cmd_getscreendata},
  {"GETSCREENHEIGHT", cmd_getscreenheight},
  // {"GETSCREENTOWORLD2D", cmd_getscreentoworld2d},
  {"GETSCREENWIDTH", cmd_getscreenwidth},
  // {"GETSHADERDEFAULT", cmd_getshaderdefault},
  // {"GETSHADERLOCATION", cmd_getshaderlocation},
  // {"GETSHADERLOCATIONATTRIB", cmd_getshaderlocationattrib},
  // {"GETSHAPESTEXTURE", cmd_getshapestexture},
  // {"GETSHAPESTEXTUREREC", cmd_getshapestexturerec},
  {"GETSOUNDSPLAYING", cmd_getsoundsplaying},
  {"GETTEXTUREDATA", cmd_gettexturedata},
  // {"GETTEXTUREDEFAULT", cmd_gettexturedefault},
  {"GETTIME", cmd_gettime},
  {"GETTOUCHPOINTSCOUNT", cmd_gettouchpointscount},
  // {"GETTOUCHPOSITION", cmd_gettouchposition},
  {"GETTOUCHX", cmd_gettouchx},
  {"GETTOUCHY", cmd_gettouchy},
  // {"GETWAVEDATA", cmd_getwavedata},
  // {"GETWINDOWPOSITION", cmd_getwindowposition},
  // {"GETWINDOWSCALEDPI", cmd_getwindowscaledpi},
  {"GETWORKINGDIRECTORY", cmd_getworkingdirectory},
  // {"GETWORLDTOSCREEN", cmd_getworldtoscreen},
  // {"GETWORLDTOSCREEN2D", cmd_getworldtoscreen2d},
  // {"GETWORLDTOSCREENEX", cmd_getworldtoscreenex},
  // {"IMAGECOPY", cmd_imagecopy},
  // {"IMAGEFROMIMAGE", cmd_imagefromimage},
  // {"IMAGETEXT", cmd_imagetext},
  // {"IMAGETEXTEX", cmd_imagetextex},
  // {"INITAUDIOSTREAM", cmd_initaudiostream},
  {"ISAUDIODEVICEREADY", cmd_isaudiodeviceready},
  //{"ISAUDIOSTREAMPLAYING", cmd_isaudiostreamplaying},
  //{"ISAUDIOSTREAMPROCESSED", cmd_isaudiostreamprocessed},
  {"ISCURSORHIDDEN", cmd_iscursorhidden},
  {"ISCURSORONSCREEN", cmd_iscursoronscreen},
  {"ISFILEDROPPED", cmd_isfiledropped},
  {"ISFILEEXTENSION", cmd_isfileextension},
  {"ISGAMEPADAVAILABLE", cmd_isgamepadavailable},
  {"ISGAMEPADBUTTONDOWN", cmd_isgamepadbuttondown},
  {"ISGAMEPADBUTTONPRESSED", cmd_isgamepadbuttonpressed},
  {"ISGAMEPADBUTTONRELEASED", cmd_isgamepadbuttonreleased},
  {"ISGAMEPADBUTTONUP", cmd_isgamepadbuttonup},
  {"ISGAMEPADNAME", cmd_isgamepadname},
  {"ISGESTUREDETECTED", cmd_isgesturedetected},
  {"ISKEYDOWN", cmd_iskeydown},
  {"ISKEYPRESSED", cmd_iskeypressed},
  {"ISKEYRELEASED", cmd_iskeyreleased},
  {"ISKEYUP", cmd_iskeyup},
  //{"ISMODELANIMATIONVALID", cmd_ismodelanimationvalid},
  {"ISMOUSEBUTTONDOWN", cmd_ismousebuttondown},
  {"ISMOUSEBUTTONPRESSED", cmd_ismousebuttonpressed},
  {"ISMOUSEBUTTONRELEASED", cmd_ismousebuttonreleased},
  {"ISMOUSEBUTTONUP", cmd_ismousebuttonup},
  {"ISMUSICPLAYING", cmd_ismusicplaying},
  {"ISSOUNDPLAYING", cmd_issoundplaying},
  {"ISVRSIMULATORREADY", cmd_isvrsimulatorready},
  {"ISWINDOWFOCUSED", cmd_iswindowfocused},
  {"ISWINDOWFULLSCREEN", cmd_iswindowfullscreen},
  {"ISWINDOWHIDDEN", cmd_iswindowhidden},
  {"ISWINDOWMAXIMIZED", cmd_iswindowmaximized},
  {"ISWINDOWMINIMIZED", cmd_iswindowminimized},
  {"ISWINDOWREADY", cmd_iswindowready},
  {"ISWINDOWRESIZED", cmd_iswindowresized},
  // {"LOADFILEDATA", cmd_loadfiledata},
  // {"LOADFILETEXT", cmd_loadfiletext},
  // {"LOADFONT", cmd_loadfont},
  // {"LOADFONTDATA", cmd_loadfontdata},
  // {"LOADFONTEX", cmd_loadfontex},
  // {"LOADFONTFROMIMAGE", cmd_loadfontfromimage},
  // {"LOADFONTFROMMEMORY", cmd_loadfontfrommemory},
  {"LOADIMAGE", cmd_loadimage},
  // {"LOADIMAGEANIM", cmd_loadimageanim},
  // {"LOADIMAGEFROMMEMORY", cmd_loadimagefrommemory},
  // {"LOADIMAGERAW", cmd_loadimageraw},
  // {"LOADMATERIALDEFAULT", cmd_loadmaterialdefault},
  // {"LOADMATERIALS", cmd_loadmaterials},
  // {"LOADMESHES", cmd_loadmeshes},
  {"LOADMODEL", cmd_loadmodel},
  // {"LOADMODELANIMATIONS", cmd_loadmodelanimations},
  // {"LOADMODELFROMMESH", cmd_loadmodelfrommesh},
  {"LOADMUSICSTREAM", cmd_loadmusicstream},
  {"LOADRENDERTEXTURE", cmd_loadrendertexture},
  {"LOADSHADER", cmd_loadshader},
  // {"LOADSHADERCODE", cmd_loadshadercode},
  {"LOADSOUND", cmd_loadsound},
  {"LOADSOUNDFROMWAVE", cmd_loadsoundfromwave},
  // {"LOADSTORAGEVALUE", cmd_loadstoragevalue},
  {"LOADTEXTURE", cmd_loadtexture},
  // {"LOADTEXTURECUBEMAP", cmd_loadtexturecubemap},
  {"LOADTEXTUREFROMIMAGE", cmd_loadtexturefromimage},
  // {"LOADWAVE", cmd_loadwave},
  // {"LOADWAVEFROMMEMORY", cmd_loadwavefrommemory},
  {"MEASURETEXT", cmd_measuretext},
  // {"MEASURETEXTEX", cmd_measuretextex},
  // {"MESHBOUNDINGBOX", cmd_meshboundingbox},
  // {"TEXTCOPY", cmd_textcopy},
  // {"TEXTFINDINDEX", cmd_textfindindex},
  // {"TEXTFORMAT", cmd_textformat},
  // {"TEXTINSERT", cmd_textinsert},
  // {"TEXTISEQUAL", cmd_textisequal},
  // {"TEXTJOIN", cmd_textjoin},
  // {"TEXTLENGTH", cmd_textlength},
  // {"TEXTREPLACE", cmd_textreplace},
  // {"TEXTSPLIT", cmd_textsplit},
  // {"TEXTSUBTEXT", cmd_textsubtext},
  // {"TEXTTOINTEGER", cmd_texttointeger},
  // {"TEXTTOLOWER", cmd_texttolower},
  // {"TEXTTOPASCAL", cmd_texttopascal},
  // {"TEXTTOUPPER", cmd_texttoupper},
  // {"TEXTTOUTF8", cmd_texttoutf8},
  // {"WAVECOPY", cmd_wavecopy},
  {"WINDOWSHOULDCLOSE", cmd_windowshouldclose},
  {"GUIBUTTON", cmd_guibutton},
  {"GUICHECKBOX", cmd_guicheckbox},
  {"GUICOLORBARALPHA", cmd_guicolorbaralpha},
  {"GUICOLORBARHUE", cmd_guicolorbarhue},
  {"GUICOLORPANEL", cmd_guicolorpanel},
  {"GUICOLORPICKER", cmd_guicolorpicker},
  {"GUICOMBOBOX", cmd_guicombobox},
  //  {"GUIDROPDOWNBOX", cmd_guidropdownbox},
  //  {"GUIGETFONT", cmd_guigetfont},
  {"GUIGETSTATE", cmd_guigetstate},
  {"GUIGETSTYLE", cmd_guigetstyle},
  //  {"GUIGRID", cmd_guigrid},
  // {"GUIIMAGEBUTTON", cmd_guiimagebutton},
  //  {"GUIIMAGEBUTTONEX", cmd_guiimagebuttonex},
  {"GUILABELBUTTON", cmd_guilabelbutton},
  //  {"GUILISTVIEW", cmd_guilistview},
  //  {"GUILISTVIEWEX", cmd_guilistviewex},
  //  {"GUIMESSAGEBOX", cmd_guimessagebox},
  //  {"GUIPROGRESSBAR", cmd_guiprogressbar},
  //  {"GUISCROLLBAR", cmd_guiscrollbar},
  // {"GUISCROLLPANEL", cmd_guiscrollpanel},
  {"GUISLIDER", cmd_guislider},
  {"GUISLIDERBAR", cmd_guisliderbar},
  //  {"GUISPINNER", cmd_guispinner},
  //  {"GUITEXTBOX", cmd_guitextbox},
  //  {"GUITEXTBOXMULTI", cmd_guitextboxmulti},
  //  {"GUITEXTINPUTBOX", cmd_guitextinputbox},
  //  {"GUITOGGLE", cmd_guitoggle},
  {"GUITOGGLEGROUP", cmd_guitogglegroup},
  //  {"GUIVALUEBOX", cmd_guivaluebox},
  //{"GUIWINDOWBOX", cmd_guiwindowbox},
};

API lib_proc[] = {
  {"BEGINBLENDMODE", cmd_beginblendmode},
  {"BEGINDRAWING", cmd_begindrawing},
  // {"BEGINMODE2D", cmd_beginmode2d},
  {"BEGINMODE3D", cmd_beginmode3d},
  {"BEGINSCISSORMODE", cmd_beginscissormode},
  {"BEGINSHADERMODE", cmd_beginshadermode},
  {"BEGINTEXTUREMODE", cmd_begintexturemode},
  {"BEGINVRDRAWING", cmd_beginvrdrawing},
  {"CLEARBACKGROUND", cmd_clearbackground},
  {"CLEARDIRECTORYFILES", cmd_cleardirectoryfiles},
  {"CLEARDROPPEDFILES", cmd_cleardroppedfiles},
  {"CLOSEAUDIODEVICE", cmd_closeaudiodevice},
  // {"CLOSEAUDIOSTREAM", cmd_closeaudiostream},
  {"CLOSEVRSIMULATOR", cmd_closevrsimulator},
  {"CLOSEWINDOW", cmd_closewindow},
  {"DECORATEWINDOW", cmd_decoratewindow},
  {"DISABLECURSOR", cmd_disablecursor},
  // {"DRAWBILLBOARD", cmd_drawbillboard},
  // {"DRAWBILLBOARDREC", cmd_drawbillboardrec},
  // {"DRAWBOUNDINGBOX", cmd_drawboundingbox},
  {"DRAWCIRCLE", cmd_drawcircle},
  // {"DRAWCIRCLE3D", cmd_drawcircle3d},
  // {"DRAWCIRCLEGRADIENT", cmd_drawcirclegradient},
  {"DRAWCIRCLELINES", cmd_drawcirclelines},
  {"DRAWCIRCLESECTOR", cmd_drawcirclesector},
  {"DRAWCIRCLESECTORLINES", cmd_drawcirclesectorlines},
  {"DRAWCIRCLEV", cmd_drawcirclev},
  {"DRAWCUBE", cmd_drawcube},
  // {"DRAWCUBETEXTURE", cmd_drawcubetexture},
  // {"DRAWCUBEV", cmd_drawcubev},
  // {"DRAWCUBEWIRES", cmd_drawcubewires},
  // {"DRAWCUBEWIRESV", cmd_drawcubewiresv},
  // {"DRAWCYLINDER", cmd_drawcylinder},
  // {"DRAWCYLINDERWIRES", cmd_drawcylinderwires},
  // {"DRAWELLIPSE", cmd_drawellipse},
  // {"DRAWELLIPSELINES", cmd_drawellipselines},
  {"DRAWFPS", cmd_drawfps},
  {"DRAWGIZMO", cmd_drawgizmo},
  {"DRAWGRID", cmd_drawgrid},
  {"DRAWLINE", cmd_drawline},
  // {"DRAWLINE3D", cmd_drawline3d},
  // {"DRAWLINEBEZIER", cmd_drawlinebezier},
  // {"DRAWLINEEX", cmd_drawlineex},
  // {"DRAWLINESTRIP", cmd_drawlinestrip},
  // {"DRAWLINEV", cmd_drawlinev},
  {"DRAWMODEL", cmd_drawmodel},
  // {"DRAWMODELEX", cmd_drawmodelex},
  // {"DRAWMODELWIRES", cmd_drawmodelwires},
  // {"DRAWMODELWIRESEX", cmd_drawmodelwiresex},
  // {"DRAWPIXEL", cmd_drawpixel},
  // {"DRAWPIXELV", cmd_drawpixelv},
  // {"DRAWPLANE", cmd_drawplane},
  // {"DRAWPOINT3D", cmd_drawpoint3d},
  // {"DRAWPOLY", cmd_drawpoly},
  // {"DRAWPOLYLINES", cmd_drawpolylines},
  // {"DRAWRAY", cmd_drawray},
  {"DRAWRECTANGLE", cmd_drawrectangle},
  // {"DRAWRECTANGLEGRADIENTEX", cmd_drawrectanglegradientex},
  // {"DRAWRECTANGLEGRADIENTH", cmd_drawrectanglegradienth},
  // {"DRAWRECTANGLEGRADIENTV", cmd_drawrectanglegradientv},
  {"DRAWRECTANGLELINES", cmd_drawrectanglelines},
  {"DRAWRECTANGLELINESEX", cmd_drawrectanglelinesex},
  // {"DRAWRECTANGLEPRO", cmd_drawrectanglepro},
  {"DRAWRECTANGLEREC", cmd_drawrectanglerec},
  // {"DRAWRECTANGLEROUNDED", cmd_drawrectanglerounded},
  // {"DRAWRECTANGLEROUNDEDLINES", cmd_drawrectangleroundedlines},
  // {"DRAWRECTANGLEV", cmd_drawrectanglev},
  {"DRAWRING", cmd_drawring},
  {"DRAWRINGLINES", cmd_drawringlines},
  {"DRAWSPHERE", cmd_drawsphere},
  // {"DRAWSPHEREEX", cmd_drawsphereex},
  // {"DRAWSPHEREWIRES", cmd_drawspherewires},
  {"DRAWTEXT", cmd_drawtext},
  // {"DRAWTEXTCODEPOINT", cmd_drawtextcodepoint},
  // {"DRAWTEXTEX", cmd_drawtextex},
  // {"DRAWTEXTREC", cmd_drawtextrec},
  // {"DRAWTEXTRECEX", cmd_drawtextrecex},
  {"DRAWTEXTURE", cmd_drawtexture},
  {"DRAWTEXTUREEX", cmd_drawtextureex},
  // {"DRAWTEXTURENPATCH", cmd_drawtexturenpatch},
  {"DRAWTEXTUREPRO", cmd_drawtexturepro},
  // {"DRAWTEXTUREQUAD", cmd_drawtexturequad},
  {"DRAWTEXTUREREC", cmd_drawtexturerec},
  // {"DRAWTEXTURETILED", cmd_drawtexturetiled},
  // {"DRAWTEXTUREV", cmd_drawtexturev},
  // {"DRAWTRIANGLE", cmd_drawtriangle},
  // {"DRAWTRIANGLE3D", cmd_drawtriangle3d},
  // {"DRAWTRIANGLEFAN", cmd_drawtrianglefan},
  // {"DRAWTRIANGLELINES", cmd_drawtrianglelines},
  // {"DRAWTRIANGLESTRIP", cmd_drawtrianglestrip},
  // {"DRAWTRIANGLESTRIP3D", cmd_drawtrianglestrip3d},
  {"ENABLECURSOR", cmd_enablecursor},
  {"ENDBLENDMODE", cmd_endblendmode},
  {"ENDDRAWING", cmd_enddrawing},
  {"ENDMODE2D", cmd_endmode2d},
  {"ENDMODE3D", cmd_endmode3d},
  {"ENDSCISSORMODE", cmd_endscissormode},
  {"ENDSHADERMODE", cmd_endshadermode},
  {"ENDTEXTUREMODE", cmd_endtexturemode},
  {"ENDVRDRAWING", cmd_endvrdrawing},
  {"EXPORTIMAGE", cmd_exportimage},
  // {"EXPORTIMAGEASCODE", cmd_exportimageascode},
  // {"EXPORTMESH", cmd_exportmesh},
  // {"EXPORTWAVE", cmd_exportwave},
  // {"EXPORTWAVEASCODE", cmd_exportwaveascode},
  // {"GENTEXTUREMIPMAPS", cmd_gentexturemipmaps},
  {"GETWINDOWHANDLE", cmd_getwindowhandle},
  {"HIDECURSOR", cmd_hidecursor},
  {"HIDEWINDOW", cmd_hidewindow},
  // {"IMAGEALPHACLEAR", cmd_imagealphaclear},
  // {"IMAGEALPHACROP", cmd_imagealphacrop},
  // {"IMAGEALPHAMASK", cmd_imagealphamask},
  // {"IMAGEALPHAPREMULTIPLY", cmd_imagealphapremultiply},
  // {"IMAGECLEARBACKGROUND", cmd_imageclearbackground},
  {"IMAGECOLORBRIGHTNESS", cmd_imagecolorbrightness},
  {"IMAGECOLORCONTRAST", cmd_imagecolorcontrast},
  {"IMAGECOLORGRAYSCALE", cmd_imagecolorgrayscale},
  {"IMAGECOLORINVERT", cmd_imagecolorinvert},
  // {"IMAGECOLORREPLACE", cmd_imagecolorreplace},
  {"IMAGECOLORTINT", cmd_imagecolortint},
  // {"IMAGECROP", cmd_imagecrop},
  // {"IMAGEDITHER", cmd_imagedither},
  // {"IMAGEDRAW", cmd_imagedraw},
  {"IMAGEDRAWCIRCLE", cmd_imagedrawcircle},
  // {"IMAGEDRAWCIRCLEV", cmd_imagedrawcirclev},
  // {"IMAGEDRAWLINE", cmd_imagedrawline},
  // {"IMAGEDRAWLINEV", cmd_imagedrawlinev},
  // {"IMAGEDRAWPIXEL", cmd_imagedrawpixel},
  // {"IMAGEDRAWPIXELV", cmd_imagedrawpixelv},
  // {"IMAGEDRAWRECTANGLE", cmd_imagedrawrectangle},
  // {"IMAGEDRAWRECTANGLELINES", cmd_imagedrawrectanglelines},
  {"IMAGEDRAWRECTANGLEREC", cmd_imagedrawrectanglerec},
  // {"IMAGEDRAWRECTANGLEV", cmd_imagedrawrectanglev},
  // {"IMAGEDRAWTEXT", cmd_imagedrawtext},
  // {"IMAGEDRAWTEXTEX", cmd_imagedrawtextex},
  {"IMAGEFLIPHORIZONTAL", cmd_imagefliphorizontal},
  {"IMAGEFLIPVERTICAL", cmd_imageflipvertical},
  {"IMAGEFORMAT", cmd_imageformat},
  // {"IMAGEMIPMAPS", cmd_imagemipmaps},
  // {"IMAGERESIZE", cmd_imageresize},
  // {"IMAGERESIZECANVAS", cmd_imageresizecanvas},
  // {"IMAGERESIZENN", cmd_imageresizenn},
  // {"IMAGEROTATECCW", cmd_imagerotateccw},
  // {"IMAGEROTATECW", cmd_imagerotatecw},
  // {"IMAGETOPOT", cmd_imagetopot},
  {"INITAUDIODEVICE", cmd_initaudiodevice},
  {"INITVRSIMULATOR", cmd_initvrsimulator},
  {"INITWINDOW", cmd_initwindow},
  {"MAXIMIZEWINDOW", cmd_maximizewindow},
  // {"MESHBINORMALS", cmd_meshbinormals},
  // {"MESHNORMALSSMOOTH", cmd_meshnormalssmooth},
  // {"MESHTANGENTS", cmd_meshtangents},
  // {"OPENURL", cmd_openurl},
  // {"PAUSEAUDIOSTREAM", cmd_pauseaudiostream},
  {"PAUSEMUSICSTREAM", cmd_pausemusicstream},
  {"PAUSESOUND", cmd_pausesound},
  // {"PLAYAUDIOSTREAM", cmd_playaudiostream},
  {"PLAYMUSICSTREAM", cmd_playmusicstream},
  {"PLAYSOUND", cmd_playsound},
  {"PLAYSOUNDMULTI", cmd_playsoundmulti},
  {"RESTOREWINDOW", cmd_restorewindow},
  // {"RESUMEAUDIOSTREAM", cmd_resumeaudiostream},
  {"RESUMEMUSICSTREAM", cmd_resumemusicstream},
  {"RESUMESOUND", cmd_resumesound},
  // {"SAVEFILEDATA", cmd_savefiledata},
  // {"SAVEFILETEXT", cmd_savefiletext},
  // {"SAVESTORAGEVALUE", cmd_savestoragevalue},
  // {"SETAUDIOSTREAMBUFFERSIZEDEFAULT", cmd_setaudiostreambuffersizedefault},
  // {"SETAUDIOSTREAMPITCH", cmd_setaudiostreampitch},
  // {"SETAUDIOSTREAMVOLUME", cmd_setaudiostreamvolume},
  // {"SETCAMERAALTCONTROL", cmd_setcameraaltcontrol},
  {"SETCAMERAMODE", cmd_setcameramode},
  // {"SETCAMERAMOVECONTROLS", cmd_setcameramovecontrols},
  // {"SETCAMERAPANCONTROL", cmd_setcamerapancontrol},
  // {"SETCAMERASMOOTHZOOMCONTROL", cmd_setcamerasmoothzoomcontrol},
  {"SETCLIPBOARDTEXT", cmd_setclipboardtext},
  {"SETCONFIGFLAGS", cmd_setconfigflags},
  {"SETEXITKEY", cmd_setexitkey},
  // {"SETGESTURESENABLED", cmd_setgesturesenabled},
  {"SETMASTERVOLUME", cmd_setmastervolume},
  // {"SETMATERIALTEXTURE", cmd_setmaterialtexture},
  // {"SETMATRIXMODELVIEW", cmd_setmatrixmodelview},
  // {"SETMATRIXPROJECTION", cmd_setmatrixprojection},
  {"SETMODELDIFFUSETEXTURE", cmd_setmodeldiffusetexture},
  // {"SETMODELMESHMATERIAL", cmd_setmodelmeshmaterial},
  {"SETMOUSEOFFSET", cmd_setmouseoffset},
  {"SETMOUSEPOSITION", cmd_setmouseposition},
  {"SETMOUSESCALE", cmd_setmousescale},
  {"SETMUSICPITCH", cmd_setmusicpitch},
  {"SETMUSICVOLUME", cmd_setmusicvolume},
  // {"SETPIXELCOLOR", cmd_setpixelcolor},
  // {"SETSHADERVALUE", cmd_setshadervalue},
  // {"SETSHADERVALUEMATRIX", cmd_setshadervaluematrix},
  // {"SETSHADERVALUETEXTURE", cmd_setshadervaluetexture},
  // {"SETSHADERVALUEV", cmd_setshadervaluev},
  // {"SETSHAPESTEXTURE", cmd_setshapestexture},
  {"SETSOUNDPITCH", cmd_setsoundpitch},
  {"SETSOUNDVOLUME", cmd_setsoundvolume},
  {"SETTARGETFPS", cmd_settargetfps},
  // {"SETTEXTUREFILTER", cmd_settexturefilter},
  // {"SETTEXTUREWRAP", cmd_settexturewrap},
  // {"SETTRACELOGCALLBACK", cmd_settracelogcallback},
  {"SETTRACELOGEXIT", cmd_settracelogexit},
  {"SETTRACELOGLEVEL", cmd_settraceloglevel},
  // {"SETVRCONFIGURATION", cmd_setvrconfiguration},
  // {"SETWINDOWICON", cmd_setwindowicon},
  {"SETWINDOWMINSIZE", cmd_setwindowminsize},
  {"SETWINDOWMONITOR", cmd_setwindowmonitor},
  {"SETWINDOWPOSITION", cmd_setwindowposition},
  {"SETWINDOWSIZE", cmd_setwindowsize},
  {"SETWINDOWTITLE", cmd_setwindowtitle},
  {"SHOWCURSOR", cmd_showcursor},
  // {"STOPAUDIOSTREAM", cmd_stopaudiostream},
  {"STOPMUSICSTREAM", cmd_stopmusicstream},
  // {"STOPSOUND", cmd_stopsound},
  {"STOPSOUNDMULTI", cmd_stopsoundmulti},
  // {"TAKESCREENSHOT", cmd_takescreenshot},
  // {"TEXTAPPEND", cmd_textappend},
  {"TOGGLEFULLSCREEN", cmd_togglefullscreen},
  {"TOGGLEVRMODE", cmd_togglevrmode},
  // {"TRACELOG", cmd_tracelog},
  {"UNDECORATEWINDOW", cmd_undecoratewindow},
  {"UNHIDEWINDOW", cmd_unhidewindow},
  // {"UNLOADFONT", cmd_unloadfont},
  {"UNLOADIMAGE", cmd_unloadimage},
  // {"UNLOADMATERIAL", cmd_unloadmaterial},
  // {"UNLOADMESH", cmd_unloadmesh},
  {"UNLOADMODEL", cmd_unloadmodel},
  // {"UNLOADMODELANIMATION", cmd_unloadmodelanimation},
  {"UNLOADMUSICSTREAM", cmd_unloadmusicstream},
  {"UNLOADRENDERTEXTURE", cmd_unloadrendertexture},
  {"UNLOADSHADER", cmd_unloadshader},
  {"UNLOADSOUND", cmd_unloadsound},
  {"UNLOADTEXTURE", cmd_unloadtexture},
  // {"UNLOADWAVE", cmd_unloadwave},
  // {"UPDATEAUDIOSTREAM", cmd_updateaudiostream},
  {"UPDATECAMERA", cmd_updatecamera},
  // {"UPDATEMODELANIMATION", cmd_updatemodelanimation},
  {"UPDATEMUSICSTREAM", cmd_updatemusicstream},
  // {"UPDATESOUND", cmd_updatesound},
  {"UPDATETEXTURE", cmd_updatetexture},
  // {"UPDATETEXTUREREC", cmd_updatetexturerec},
  // {"UPDATEVRTRACKING", cmd_updatevrtracking},
  // {"WAVECROP", cmd_wavecrop},
  // {"WAVEFORMAT", cmd_waveformat},
  {"GUICLEARTOOLTIP", cmd_guicleartooltip},
  {"GUIDISABLE", cmd_guidisable},
  {"GUIDISABLETOOLTIP", cmd_guidisabletooltip},
  {"GUIDUMMYREC", cmd_guidummyrec},
  {"GUIENABLE", cmd_guienable},
  {"GUIENABLETOOLTIP", cmd_guienabletooltip},
  {"GUIFADE", cmd_guifade},
  {"GUIGROUPBOX", cmd_guigroupbox},
  {"GUILABEL", cmd_guilabel},
  {"GUILINE", cmd_guiline},
  {"GUILOADSTYLE", cmd_guiloadstyle},
  {"GUILOADSTYLEDEFAULT", cmd_guiloadstyledefault},
  {"GUILOCK", cmd_guilock},
  {"GUIPANEL", cmd_guipanel},
  {"GUISETFONT", cmd_guisetfont},
  {"GUISETSTATE", cmd_guisetstate},
  {"GUISETSTYLE", cmd_guisetstyle},
  {"GUISETTOOLTIP", cmd_guisettooltip},
  {"GUISTATUSBAR", cmd_guistatusbar},
  {"GUIUNLOCK", cmd_guiunlock},
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
  if (index >= 0 && index < sblib_proc_count()) {
    result = lib_proc[index].command(argc, params, retval);
  } else {
    char message[50];
    sprintf(message, "Invalid proc index [%d]", index);
    v_setstr(retval, message);
    result = 0;
  }
  return result;
}

int sblib_func_exec(int index, int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (index >= 0 && index < sblib_func_count()) {
    result = lib_func[index].command(argc, params, retval);
  } else {
    char message[50];
    sprintf(message, "Invalid func index [%d]", index);
    v_setstr(retval, message);
    result = 0;
  }
  return result;
}

int sblib_init(void) {
  return 1;
}

void sblib_close(void) {
  _imageMap.clear();
  _modelMap.clear();
  _musicMap.clear();
  _renderMap.clear();
  _soundMap.clear();
  _textureMap.clear();
}
