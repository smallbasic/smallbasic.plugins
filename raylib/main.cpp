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
#include <cstring>
#include <map>

#include "var.h"
#include "var_map.h"
#include "module.h"
#include "param.h"

std::map<int, Image> _imageMap;
std::map<int, Model> _modelMap;
std::map<int, Music> _musicMap;
std::map<int, RenderTexture2D> _renderMap;
std::map<int, Sound> _soundMap;
std::map<int, Texture2D> _textureMap;
int _nextId = 1;
const char *mapID = "_ID";

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
  if (is_param_array(argc, params, n)) {
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
  if (is_param_array(argc, params, n)) {
    result.x = get_array_elem_num(params[n].var_p, 0);
    result.y = get_array_elem_num(params[n].var_p, 1);
    result.z = get_array_elem_num(params[n].var_p, 2);
  } else {
    result.x = 0;
    result.y = 0;
  }
  return result;
}

Vector3 get_array_elem_vector(var_p_t array, int index) {
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
    result.position = get_array_elem_vector(array, 0);
    result.target = get_array_elem_vector(array, 1);
    result.up = get_array_elem_vector(array, 2);
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

int cmd_changedirectory(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
     auto dir = get_param_str(argc, params, 0, NULL);
     auto fnResult = ChangeDirectory(dir);
     v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: ChangeDirectory");
  }
  return result;
}

int cmd_checkcollisionboxes(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto box1 = get_param_str(argc, params, 0, NULL);
    // auto box2 = get_param_str(argc, params, 1, NULL);
    // auto fnResult = CheckCollisionBoxes(box1, box2);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: CheckCollisionBoxes");
  }
  return result;
}

int cmd_checkcollisionboxsphere(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto box = get_param_str(argc, params, 0, NULL);
    // auto center = get_param_str(argc, params, 1, NULL);
    // auto radius = get_param_str(argc, params, 2, NULL);
    // auto fnResult = CheckCollisionBoxSphere(box, center, radius);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: CheckCollisionBoxSphere");
  }
  return result;
}

int cmd_checkcollisioncirclerec(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto center = get_param_str(argc, params, 0, NULL);
    // auto radius = get_param_str(argc, params, 1, NULL);
    // auto rec = get_param_str(argc, params, 2, NULL);
    // auto fnResult = CheckCollisionCircleRec(center, radius, rec);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: CheckCollisionCircleRec");
  }
  return result;
}

int cmd_checkcollisioncircles(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto center1 = get_param_str(argc, params, 0, NULL);
    // auto radius1 = get_param_str(argc, params, 1, NULL);
    // auto center2 = get_param_str(argc, params, 2, NULL);
    // auto radius2 = get_param_str(argc, params, 3, NULL);
    // auto fnResult = CheckCollisionCircles(center1, radius1, center2, radius2);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: CheckCollisionCircles");
  }
  return result;
}

int cmd_checkcollisionpointcircle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto point = get_param_str(argc, params, 0, NULL);
    // auto center = get_param_str(argc, params, 1, NULL);
    // auto radius = get_param_str(argc, params, 2, NULL);
    // auto fnResult = CheckCollisionPointCircle(point, center, radius);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: CheckCollisionPointCircle");
  }
  return result;
}

int cmd_checkcollisionpointrec(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto point = get_param_str(argc, params, 0, NULL);
    // auto rec = get_param_str(argc, params, 1, NULL);
    // auto fnResult = CheckCollisionPointRec(point, rec);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: CheckCollisionPointRec");
  }
  return result;
}

int cmd_checkcollisionpointtriangle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto point = get_param_str(argc, params, 0, NULL);
    // auto p1 = get_param_str(argc, params, 1, NULL);
    // auto p2 = get_param_str(argc, params, 2, NULL);
    // auto p3 = get_param_str(argc, params, 3, NULL);
    // auto fnResult = CheckCollisionPointTriangle(point, p1, p2, p3);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: CheckCollisionPointTriangle");
  }
  return result;
}

int cmd_checkcollisionraybox(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto ray = get_param_str(argc, params, 0, NULL);
    // auto box = get_param_str(argc, params, 1, NULL);
    // auto fnResult = CheckCollisionRayBox(ray, box);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: CheckCollisionRayBox");
  }
  return result;
}

int cmd_checkcollisionraysphere(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto ray = get_param_str(argc, params, 0, NULL);
    // auto center = get_param_str(argc, params, 1, NULL);
    // auto radius = get_param_str(argc, params, 2, NULL);
    // auto fnResult = CheckCollisionRaySphere(ray, center, radius);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: CheckCollisionRaySphere");
  }
  return result;
}

int cmd_checkcollisionraysphereex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto ray = get_param_str(argc, params, 0, NULL);
    // auto center = get_param_str(argc, params, 1, NULL);
    // auto radius = get_param_str(argc, params, 2, NULL);
    // auto collisionPoint = get_param_str(argc, params, 3, NULL);
    // auto fnResult = CheckCollisionRaySphereEx(ray, center, radius, collisionPoint);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: CheckCollisionRaySphereEx");
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
    v_setstr(retval, "Invalid input: CheckCollisionRecs");
  }
  return result;
}

int cmd_checkcollisionspheres(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto centerA = get_param_str(argc, params, 0, NULL);
    // auto radiusA = get_param_str(argc, params, 1, NULL);
    // auto centerB = get_param_str(argc, params, 2, NULL);
    // auto radiusB = get_param_str(argc, params, 3, NULL);
    // auto fnResult = CheckCollisionSpheres(centerA, radiusA, centerB, radiusB);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: CheckCollisionSpheres");
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
    v_setstr(retval, "Invalid input: CodepointToUtf8");
  }
  return result;
}

int cmd_coloralpha(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto color = get_param_str(argc, params, 0, NULL);
    // auto alpha = get_param_str(argc, params, 1, NULL);
    // auto fnResult = ColorAlpha(color, alpha);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: ColorAlpha");
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
    v_setstr(retval, "Invalid input: ColorAlphaBlend");
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
    v_setstr(retval, "Invalid input: ColorFromHSV");
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
    v_setstr(retval, "Invalid input: ColorFromNormalized");
  }
  return result;
}

int cmd_colornormalize(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto color = get_param_str(argc, params, 0, NULL);
    // auto fnResult = ColorNormalize(color);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: ColorNormalize");
  }
  return result;
}

int cmd_colortohsv(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto color = get_param_str(argc, params, 0, NULL);
    // auto fnResult = ColorToHSV(color);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: ColorToHSV");
  }
  return result;
}

int cmd_colortoint(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto color = get_param_str(argc, params, 0, NULL);
    // auto fnResult = ColorToInt(color);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: ColorToInt");
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
    v_setstr(retval, "Invalid input: CompressData");
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
    v_setstr(retval, "Invalid input: DecompressData");
  }
  return result;
}

int cmd_directoryexists(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto dirPath = get_param_str(argc, params, 0, NULL);
    // auto fnResult = DirectoryExists(dirPath);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: DirectoryExists");
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
    v_setstr(retval, "Invalid input: Fade");
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
    v_setstr(retval, "Invalid input: FileExists");
  }
  return result;
}

int cmd_genimagecellular(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto width = get_param_str(argc, params, 0, NULL);
    // auto height = get_param_str(argc, params, 1, NULL);
    // auto tileSize = get_param_str(argc, params, 2, NULL);
    // auto fnResult = GenImageCellular(width, height, tileSize);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GenImageCellular");
  }
  return result;
}

int cmd_genimagechecked(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto width = get_param_str(argc, params, 0, NULL);
    // auto height = get_param_str(argc, params, 1, NULL);
    // auto checksX = get_param_str(argc, params, 2, NULL);
    // auto checksY = get_param_str(argc, params, 3, NULL);
    // auto col1 = get_param_str(argc, params, 4, NULL);
    // auto col2 = get_param_str(argc, params, 5, NULL);
    // auto fnResult = GenImageChecked(width, height, checksX, checksY, col1, col2);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GenImageChecked");
  }
  return result;
}

int cmd_genimagecolor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto width = get_param_str(argc, params, 0, NULL);
    // auto height = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_str(argc, params, 2, NULL);
    // auto fnResult = GenImageColor(width, height, color);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GenImageColor");
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
    v_setstr(retval, "Invalid input: GenImageFontAtlas");
  }
  return result;
}

int cmd_genimagegradienth(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto width = get_param_str(argc, params, 0, NULL);
    // auto height = get_param_str(argc, params, 1, NULL);
    // auto left = get_param_str(argc, params, 2, NULL);
    // auto right = get_param_str(argc, params, 3, NULL);
    // auto fnResult = GenImageGradientH(width, height, left, right);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GenImageGradientH");
  }
  return result;
}

int cmd_genimagegradientradial(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto width = get_param_str(argc, params, 0, NULL);
    // auto height = get_param_str(argc, params, 1, NULL);
    // auto density = get_param_str(argc, params, 2, NULL);
    // auto inner = get_param_str(argc, params, 3, NULL);
    // auto outer = get_param_str(argc, params, 4, NULL);
    // auto fnResult = GenImageGradientRadial(width, height, density, inner, outer);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GenImageGradientRadial");
  }
  return result;
}

int cmd_genimagegradientv(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto width = get_param_str(argc, params, 0, NULL);
    // auto height = get_param_str(argc, params, 1, NULL);
    // auto top = get_param_str(argc, params, 2, NULL);
    // auto bottom = get_param_str(argc, params, 3, NULL);
    // auto fnResult = GenImageGradientV(width, height, top, bottom);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GenImageGradientV");
  }
  return result;
}

int cmd_genimageperlinnoise(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto width = get_param_str(argc, params, 0, NULL);
    // auto height = get_param_str(argc, params, 1, NULL);
    // auto offsetX = get_param_str(argc, params, 2, NULL);
    // auto offsetY = get_param_str(argc, params, 3, NULL);
    // auto scale = get_param_str(argc, params, 4, NULL);
    // auto fnResult = GenImagePerlinNoise(width, height, offsetX, offsetY, scale);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GenImagePerlinNoise");
  }
  return result;
}

int cmd_genimagewhitenoise(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto width = get_param_str(argc, params, 0, NULL);
    // auto height = get_param_str(argc, params, 1, NULL);
    // auto factor = get_param_str(argc, params, 2, NULL);
    // auto fnResult = GenImageWhiteNoise(width, height, factor);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GenImageWhiteNoise");
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
    v_setstr(retval, "Invalid input: GenMeshCube");
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
    v_setstr(retval, "Invalid input: GenMeshCubicmap");
  }
  return result;
}

int cmd_genmeshcylinder(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto radius = get_param_str(argc, params, 0, NULL);
    // auto height = get_param_str(argc, params, 1, NULL);
    // auto slices = get_param_str(argc, params, 2, NULL);
    // auto fnResult = GenMeshCylinder(radius, height, slices);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GenMeshCylinder");
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
    v_setstr(retval, "Invalid input: GenMeshHeightmap");
  }
  return result;
}

int cmd_genmeshhemisphere(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto radius = get_param_str(argc, params, 0, NULL);
    // auto rings = get_param_str(argc, params, 1, NULL);
    // auto slices = get_param_str(argc, params, 2, NULL);
    // auto fnResult = GenMeshHemiSphere(radius, rings, slices);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GenMeshHemiSphere");
  }
  return result;
}

int cmd_genmeshknot(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto radius = get_param_str(argc, params, 0, NULL);
    // auto size = get_param_str(argc, params, 1, NULL);
    // auto radSeg = get_param_str(argc, params, 2, NULL);
    // auto sides = get_param_str(argc, params, 3, NULL);
    // auto fnResult = GenMeshKnot(radius, size, radSeg, sides);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GenMeshKnot");
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
    v_setstr(retval, "Invalid input: GenMeshPlane");
  }
  return result;
}

int cmd_genmeshpoly(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto sides = get_param_str(argc, params, 0, NULL);
    // auto radius = get_param_str(argc, params, 1, NULL);
    // auto fnResult = GenMeshPoly(sides, radius);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GenMeshPoly");
  }
  return result;
}

int cmd_genmeshsphere(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto radius = get_param_str(argc, params, 0, NULL);
    // auto rings = get_param_str(argc, params, 1, NULL);
    // auto slices = get_param_str(argc, params, 2, NULL);
    // auto fnResult = GenMeshSphere(radius, rings, slices);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GenMeshSphere");
  }
  return result;
}

int cmd_genmeshtorus(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto radius = get_param_str(argc, params, 0, NULL);
    // auto size = get_param_str(argc, params, 1, NULL);
    // auto radSeg = get_param_str(argc, params, 2, NULL);
    // auto sides = get_param_str(argc, params, 3, NULL);
    // auto fnResult = GenMeshTorus(radius, size, radSeg, sides);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GenMeshTorus");
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
    v_setstr(retval, "Invalid input: GenTextureBRDF");
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
    v_setstr(retval, "Invalid input: GenTextureCubemap");
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
    v_setstr(retval, "Invalid input: GenTextureIrradiance");
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
    v_setstr(retval, "Invalid input: GenTexturePrefilter");
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
    v_setstr(retval, "Invalid input: GetCameraMatrix");
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
    v_setstr(retval, "Invalid input: GetCameraMatrix2D");
  }
  return result;
}

int cmd_getclipboardtext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetClipboardText();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetClipboardText");
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
    v_setstr(retval, "Invalid input: GetCodepoints");
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
    v_setstr(retval, "Invalid input: GetCodepointsCount");
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
    v_setstr(retval, "Invalid input: GetCollisionRayGround");
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
    v_setstr(retval, "Invalid input: GetCollisionRayModel");
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
    v_setstr(retval, "Invalid input: GetCollisionRayTriangle");
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
    v_setstr(retval, "Invalid input: GetCollisionRec");
  }
  return result;
}

int cmd_getcolor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto hexValue = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetColor(hexValue);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetColor");
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
    v_setstr(retval, "Invalid input: GetDirectoryFiles");
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
    v_setstr(retval, "Invalid input: GetDirectoryPath");
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
    v_setstr(retval, "Invalid input: GetDroppedFiles");
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
    v_setstr(retval, "Invalid input: GetFileExtension");
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
    v_setstr(retval, "Invalid input: GetFileModTime");
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
    v_setstr(retval, "Invalid input: GetFileName");
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
    v_setstr(retval, "Invalid input: GetFileNameWithoutExt");
  }
  return result;
}

int cmd_getfontdefault(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetFontDefault();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetFontDefault");
  }
  return result;
}

int cmd_getfps(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetFPS();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetFPS");
  }
  return result;
}

int cmd_getframetime(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetFrameTime();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetFrameTime");
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
    v_setstr(retval, "Invalid input: GetGamepadAxisCount");
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
    v_setstr(retval, "Invalid input: GetGamepadAxisMovement");
  }
  return result;
}

int cmd_getgamepadbuttonpressed(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetGamepadButtonPressed();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetGamepadButtonPressed");
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
    v_setstr(retval, "Invalid input: GetGamepadName");
  }
  return result;
}

int cmd_getgesturedetected(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetGestureDetected();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetGestureDetected");
  }
  return result;
}

int cmd_getgesturedragangle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetGestureDragAngle();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetGestureDragAngle");
  }
  return result;
}

int cmd_getgesturedragvector(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetGestureDragVector();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetGestureDragVector");
  }
  return result;
}

int cmd_getgestureholdduration(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetGestureHoldDuration();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetGestureHoldDuration");
  }
  return result;
}

int cmd_getgesturepinchangle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetGesturePinchAngle();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetGesturePinchAngle");
  }
  return result;
}

int cmd_getgesturepinchvector(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetGesturePinchVector();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetGesturePinchVector");
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
    v_setstr(retval, "Invalid input: GetGlyphIndex");
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
    v_setstr(retval, "Invalid input: GetImageAlphaBorder");
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
      v_setstr(retval, "Invalid input: Image not found");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid input: GetImageData");
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
    v_setstr(retval, "Invalid input: GetImageDataNormalized");
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
    v_setstr(retval, "Invalid input: GetImagePalette");
  }
  return result;
}

int cmd_getkeypressed(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetKeyPressed();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetKeyPressed");
  }
  return result;
}

int cmd_getmatrixmodelview(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetMatrixModelview();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetMatrixModelview");
  }
  return result;
}

int cmd_getmatrixprojection(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetMatrixProjection();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetMatrixProjection");
  }
  return result;
}

int cmd_getmonitorcount(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetMonitorCount();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetMonitorCount");
  }
  return result;
}

int cmd_getmonitorheight(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto monitor = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetMonitorHeight(monitor);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetMonitorHeight");
  }
  return result;
}

int cmd_getmonitorname(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto monitor = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetMonitorName(monitor);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetMonitorName");
  }
  return result;
}

int cmd_getmonitorphysicalheight(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto monitor = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetMonitorPhysicalHeight(monitor);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetMonitorPhysicalHeight");
  }
  return result;
}

int cmd_getmonitorphysicalwidth(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto monitor = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetMonitorPhysicalWidth(monitor);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetMonitorPhysicalWidth");
  }
  return result;
}

int cmd_getmonitorrefreshrate(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto monitor = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetMonitorRefreshRate(monitor);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetMonitorRefreshRate");
  }
  return result;
}

int cmd_getmonitorwidth(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto monitor = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetMonitorWidth(monitor);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetMonitorWidth");
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
    v_setstr(retval, "Invalid input: GetMousePosition");
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
    v_setstr(retval, "Invalid input: GetMouseRay");
  }
  return result;
}

int cmd_getmousewheelmove(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetMouseWheelMove();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetMouseWheelMove");
  }
  return result;
}

int cmd_getmousex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    v_setint(retval, GetMouseX());
  } else {
    v_setstr(retval, "Invalid input: GetMouseX");
  }
  return result;
}

int cmd_getmousey(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    v_setint(retval, GetMouseY());
  } else {
    v_setstr(retval, "Invalid input: GetMouseY");
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
    v_setstr(retval, "Invalid input: GetMusicTimeLength");
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
    v_setstr(retval, "Invalid input: GetMusicTimePlayed");
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
    v_setstr(retval, "Invalid input: GetNextCodepoint");
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
    v_setstr(retval, "Invalid input: GetPixelColor");
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
    v_setstr(retval, "Invalid input: GetPixelDataSize");
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
    v_setstr(retval, "Invalid input: GetPrevDirectoryPath");
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
    v_setstr(retval, "Invalid input: GetRandomValue");
  }
  return result;
}

int cmd_getscreendata(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetScreenData();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetScreenData");
  }
  return result;
}

int cmd_getscreenheight(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetScreenHeight();
    v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetScreenHeight");
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
    v_setstr(retval, "Invalid input: GetScreenToWorld2D");
  }
  return result;
}

int cmd_getscreenwidth(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = GetScreenWidth();
    v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetScreenWidth");
  }
  return result;
}

int cmd_getshaderdefault(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetShaderDefault();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetShaderDefault");
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
    v_setstr(retval, "Invalid input: GetShaderLocation");
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
    v_setstr(retval, "Invalid input: GetShaderLocationAttrib");
  }
  return result;
}

int cmd_getshapestexture(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetShapesTexture();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetShapesTexture");
  }
  return result;
}

int cmd_getshapestexturerec(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetShapesTextureRec();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetShapesTextureRec");
  }
  return result;
}

int cmd_getsoundsplaying(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetSoundsPlaying();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetSoundsPlaying");
  }
  return result;
}

int cmd_gettexturedata(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto texture = get_param_str(argc, params, 0, NULL);
    // auto fnResult = GetTextureData(texture);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetTextureData");
  }
  return result;
}

int cmd_gettexturedefault(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetTextureDefault();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetTextureDefault");
  }
  return result;
}

int cmd_gettime(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetTime();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetTime");
  }
  return result;
}

int cmd_gettouchpointscount(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetTouchPointsCount();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetTouchPointsCount");
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
    v_setstr(retval, "Invalid input: GetTouchPosition");
  }
  return result;
}

int cmd_gettouchx(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetTouchX();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetTouchX");
  }
  return result;
}

int cmd_gettouchy(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetTouchY();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetTouchY");
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
    v_setstr(retval, "Invalid input: GetWaveData");
  }
  return result;
}

int cmd_getwindowposition(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetWindowPosition();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetWindowPosition");
  }
  return result;
}

int cmd_getwindowscaledpi(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetWindowScaleDPI();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetWindowScaleDPI");
  }
  return result;
}

int cmd_getworkingdirectory(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = GetWorkingDirectory();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: GetWorkingDirectory");
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
    v_setstr(retval, "Invalid input: GetWorldToScreen");
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
    v_setstr(retval, "Invalid input: GetWorldToScreen2D");
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
    v_setstr(retval, "Invalid input: GetWorldToScreenEx");
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
    v_setstr(retval, "Invalid input: ImageCopy");
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
    v_setstr(retval, "Invalid input: ImageFromImage");
  }
  return result;
}

int cmd_imagetext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto text = get_param_str(argc, params, 0, NULL);
    // auto fontSize = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_str(argc, params, 2, NULL);
    // auto fnResult = ImageText(text, fontSize, color);
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: ImageText");
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
    v_setstr(retval, "Invalid input: ImageTextEx");
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
    v_setstr(retval, "Invalid input: InitAudioStream");
  }
  return result;
}

int cmd_isaudiodeviceready(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsAudioDeviceReady();
    v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: IsAudioDeviceReady");
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
    v_setstr(retval, "Invalid input: IsAudioStreamPlaying");
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
    v_setstr(retval, "Invalid input: IsAudioStreamProcessed");
  }
  return result;
}

int cmd_iscursorhidden(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsCursorHidden();
    v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: IsCursorHidden");
  }
  return result;
}

int cmd_iscursoronscreen(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsCursorOnScreen();
    v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: IsCursorOnScreen");
  }
  return result;
}

int cmd_isfiledropped(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsFileDropped();
    v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: IsFileDropped");
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
    v_setstr(retval, "Invalid input: IsFileExtension");
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
    v_setstr(retval, "Invalid input: IsGamepadAvailable");
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
    v_setstr(retval, "Invalid input: IsGamepadButtonDown");
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
    v_setstr(retval, "Invalid input: IsGamepadButtonPressed");
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
    v_setstr(retval, "Invalid input: IsGamepadButtonReleased");
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
    v_setstr(retval, "Invalid input: IsGamepadButtonUp");
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
    v_setstr(retval, "Invalid input: IsGamepadName");
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
    v_setstr(retval, "Invalid input: IsGestureDetected");
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
    v_setstr(retval, "Invalid input: IsKeyDown");
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
    v_setstr(retval, "Invalid input: IsKeyPressed");
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
    v_setstr(retval, "Invalid input: IsKeyReleased");
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
    v_setstr(retval, "Invalid input: IsKeyUp");
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
    v_setstr(retval, "Invalid input: IsModelAnimationValid");
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
    v_setstr(retval, "Invalid input: IsMouseButtonDown");
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
    v_setstr(retval, "Invalid input: IsMouseButtonPressed");
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
    v_setstr(retval, "Invalid input: IsMouseButtonReleased");
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
    v_setstr(retval, "Invalid input: IsMouseButtonUp");
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
    v_setstr(retval, "Invalid input: IsMusicPlaying");
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
    v_setstr(retval, "Invalid input: IsSoundPlaying");
  }
  return result;
}

int cmd_isvrsimulatorready(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsVrSimulatorReady();
    v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: IsVrSimulatorReady");
  }
  return result;
}

int cmd_iswindowfocused(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsWindowFocused();
    v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: IsWindowFocused");
  }
  return result;
}

int cmd_iswindowfullscreen(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsWindowFullscreen();
    v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: IsWindowFullscreen");
  }
  return result;
}

int cmd_iswindowhidden(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsWindowHidden();
    v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: IsWindowHidden");
  }
  return result;
}

int cmd_iswindowmaximized(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsWindowMaximized();
    v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: IsWindowMaximized");
  }
  return result;
}

int cmd_iswindowminimized(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsWindowMinimized();
    v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: IsWindowMinimized");
  }
  return result;
}

int cmd_iswindowready(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsWindowReady();
    v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: IsWindowReady");
  }
  return result;
}

int cmd_iswindowresized(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = IsWindowResized();
    v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: IsWindowResized");
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
    v_setstr(retval, "Invalid input: LoadFileData");
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
    v_setstr(retval, "Invalid input: LoadFileText");
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
    v_setstr(retval, "Invalid input: LoadFont");
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
    v_setstr(retval, "Invalid input: LoadFontData");
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
    v_setstr(retval, "Invalid input: LoadFontEx");
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
    v_setstr(retval, "Invalid input: LoadFontFromImage");
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
    v_setstr(retval, "Invalid input: LoadFontFromMemory");
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
    v_setstr(retval, "Invalid input: LoadImage");
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
    v_setstr(retval, "Invalid input: LoadImageAnim");
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
    v_setstr(retval, "Invalid input: LoadImageFromMemory");
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
    v_setstr(retval, "Invalid input: LoadImageRaw");
  }
  return result;
}

int cmd_loadmaterialdefault(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    // auto fnResult = LoadMaterialDefault();
    // v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: LoadMaterialDefault");
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
    v_setstr(retval, "Invalid input: LoadMaterials");
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
    v_setstr(retval, "Invalid input: LoadMeshes");
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
    v_setstr(retval, "Invalid input: LoadModel");
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
    v_setstr(retval, "Invalid input: LoadModelAnimations");
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
    v_setstr(retval, "Invalid input: LoadModelFromMesh");
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
    v_setstr(retval, "Invalid input: LoadMusicStream");
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
    v_setstr(retval, "Invalid input: LoadRenderTexture");
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
    v_setstr(retval, "Invalid input: LoadShader");
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
    v_setstr(retval, "Invalid input: LoadShaderCode");
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
    v_setstr(retval, "Invalid input:L oadSound");
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
    v_setstr(retval, "Invalid input: LoadSoundFromWave");
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
    v_setstr(retval, "Invalid input: LoadStorageValue");
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
    v_setstr(retval, "Invalid input: LoadTexture");
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
    v_setstr(retval, "Invalid input: LoadTextureCubemap");
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
      v_setstr(retval, "Invalid input: Image not found");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid input: LoadTextureFromImage");
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
    v_setstr(retval, "Invalid input: LoadWave");
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
    v_setstr(retval, "Invalid input: LoadWaveFromMemory");
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
    v_setstr(retval, "Invalid input: MeasureText");
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
    v_setstr(retval, "Invalid input: MeasureTextEx");
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
    v_setstr(retval, "Invalid input: MeshBoundingBox");
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
    v_setstr(retval, "Invalid input: TextCopy");
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
    v_setstr(retval, "Invalid input: TextFindIndex");
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
    v_setstr(retval, "Invalid input: TextFormat");
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
    v_setstr(retval, "Invalid input: TextInsert");
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
    v_setstr(retval, "Invalid input: TextIsEqual");
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
    v_setstr(retval, "Invalid input: TextJoin");
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
    v_setstr(retval, "Invalid input: TextLength");
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
    v_setstr(retval, "Invalid input: TextReplace");
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
    v_setstr(retval, "Invalid input: TextSplit");
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
    v_setstr(retval, "Invalid input: TextSubtext");
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
    v_setstr(retval, "Invalid input: TextToInteger");
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
    v_setstr(retval, "Invalid input: TextToLower");
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
    v_setstr(retval, "Invalid input: TextToPascal");
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
    v_setstr(retval, "Invalid input: TextToUpper");
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
    v_setstr(retval, "Invalid input: TextToUtf8");
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
    v_setstr(retval, "Invalid input: WaveCopy");
  }
  return result;
}

int cmd_windowshouldclose(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    auto fnResult = WindowShouldClose();
    v_setint(retval, fnResult);
  } else {
    v_setstr(retval, "Invalid input: WindowShouldClose");
  }
  return result;
}

int cmd_beginblendmode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto mode = get_param_int(argc, params, 0, 0);
    BeginBlendMode(mode);
  } else {
    v_setstr(retval, "Invalid input: BeginBlendMode");
  }
  return result;
}

int cmd_begindrawing(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    BeginDrawing();
  } else {
    v_setstr(retval, "Invalid input: BeginDrawing");
  }
  return result;
}

int cmd_beginmode2d(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto camera = get_param_str(argc, params, 0, NULL);
    // BeginMode2D(camera);
  } else {
    v_setstr(retval, "Invalid input: BeginMode2D");
  }
  return result;
}

int cmd_beginmode3d(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    BeginMode3D(get_camera_3d(argc, params, 0));
  } else {
    v_setstr(retval, "Invalid input: BeginMode3D");
  }
  return result;
}

int cmd_beginscissormode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto x = get_param_str(argc, params, 0, NULL);
    // auto y = get_param_str(argc, params, 1, NULL);
    // auto width = get_param_str(argc, params, 2, NULL);
    // auto height = get_param_str(argc, params, 3, NULL);
    // BeginScissorMode(x, y, width, height);
  } else {
    v_setstr(retval, "Invalid input: BeginScissorMode");
  }
  return result;
}

int cmd_beginshadermode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    BeginShaderMode(get_param_shader(argc, params, 0));
  } else {
    v_setstr(retval, "Invalid input: BeginShaderMode");
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
      v_setstr(retval, "Invalid input: RenderTexture not found");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid input: BeginTextureMode");
  }
  return result;
}

int cmd_beginvrdrawing(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    BeginVrDrawing();
  } else {
    v_setstr(retval, "Invalid input: BeginVrDrawing");
  }
  return result;
}

int cmd_clearbackground(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto color = get_param_color(argc, params, 0);
    ClearBackground(color);
  } else {
    v_setstr(retval, "Invalid input: ClearBackground");
  }
  return result;
}

int cmd_cleardirectoryfiles(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    ClearDirectoryFiles();
  } else {
    v_setstr(retval, "Invalid input: ClearDirectoryFiles");
  }
  return result;
}

int cmd_cleardroppedfiles(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    ClearDroppedFiles();
  } else {
    v_setstr(retval, "Invalid input: ClearDroppedFiles");
  }
  return result;
}

int cmd_closeaudiodevice(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    CloseAudioDevice();
  } else {
    v_setstr(retval, "Invalid input: CloseAudioDevice");
  }
  return result;
}

int cmd_closeaudiostream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto stream = get_param_str(argc, params, 0, NULL);
    // CloseAudioStream(stream);
  } else {
    v_setstr(retval, "Invalid input: CloseAudioStream");
  }
  return result;
}

int cmd_closevrsimulator(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    CloseVrSimulator();
  } else {
    v_setstr(retval, "Invalid input: CloseVrSimulator");
  }
  return result;
}

int cmd_closewindow(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    CloseWindow();
  } else {
    v_setstr(retval, "Invalid input: CloseWindow");
  }
  return result;
}

int cmd_decoratewindow(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    DecorateWindow();
  } else {
    v_setstr(retval, "Invalid input: DecorateWindow");
  }
  return result;
}

int cmd_disablecursor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    DisableCursor();
  } else {
    v_setstr(retval, "Invalid input: DisableCursor");
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
    v_setstr(retval, "Invalid input: DrawBillboard");
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
    v_setstr(retval, "Invalid input: DrawBillboardRec");
  }
  return result;
}

int cmd_drawboundingbox(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto box = get_param_str(argc, params, 0, NULL);
    // auto color = get_param_str(argc, params, 1, NULL);
    // DrawBoundingBox(box, color);
  } else {
    v_setstr(retval, "Invalid input: DrawBoundingBox");
  }
  return result;
}

int cmd_drawcircle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto centerX = get_param_str(argc, params, 0, NULL);
    // auto centerY = get_param_str(argc, params, 1, NULL);
    // auto radius = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_str(argc, params, 3, NULL);
    // DrawCircle(centerX, centerY, radius, color);
  } else {
    v_setstr(retval, "Invalid input: DrawCircle");
  }
  return result;
}

int cmd_drawcircle3d(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto center = get_param_str(argc, params, 0, NULL);
    // auto radius = get_param_str(argc, params, 1, NULL);
    // auto rotationAxis = get_param_str(argc, params, 2, NULL);
    // auto rotationAngle = get_param_str(argc, params, 3, NULL);
    // auto color = get_param_str(argc, params, 4, NULL);
    // DrawCircle3D(center, radius, rotationAxis, rotationAngle, color);
  } else {
    v_setstr(retval, "Invalid input: DrawCircle3D");
  }
  return result;
}

int cmd_drawcirclegradient(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto centerX = get_param_str(argc, params, 0, NULL);
    // auto centerY = get_param_str(argc, params, 1, NULL);
    // auto radius = get_param_str(argc, params, 2, NULL);
    // auto color1 = get_param_str(argc, params, 3, NULL);
    // auto color2 = get_param_str(argc, params, 4, NULL);
    // DrawCircleGradient(centerX, centerY, radius, color1, color2);
  } else {
    v_setstr(retval, "Invalid input: DrawCircleGradient");
  }
  return result;
}

int cmd_drawcirclelines(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto centerX = get_param_str(argc, params, 0, NULL);
    // auto centerY = get_param_str(argc, params, 1, NULL);
    // auto radius = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_str(argc, params, 3, NULL);
    // DrawCircleLines(centerX, centerY, radius, color);
  } else {
    v_setstr(retval, "Invalid input: DrawCircleLines");
  }
  return result;
}

int cmd_drawcirclesector(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto center = get_param_str(argc, params, 0, NULL);
    // auto radius = get_param_str(argc, params, 1, NULL);
    // auto startAngle = get_param_str(argc, params, 2, NULL);
    // auto endAngle = get_param_str(argc, params, 3, NULL);
    // auto segments = get_param_str(argc, params, 4, NULL);
    // auto color = get_param_str(argc, params, 5, NULL);
    // DrawCircleSector(center, radius, startAngle, endAngle, segments, color);
  } else {
    v_setstr(retval, "Invalid input: DrawCircleSector");
  }
  return result;
}

int cmd_drawcirclesectorlines(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 6);
  if (result) {
    // auto center = get_param_str(argc, params, 0, NULL);
    // auto radius = get_param_str(argc, params, 1, NULL);
    // auto startAngle = get_param_str(argc, params, 2, NULL);
    // auto endAngle = get_param_str(argc, params, 3, NULL);
    // auto segments = get_param_str(argc, params, 4, NULL);
    // auto color = get_param_str(argc, params, 5, NULL);
    // DrawCircleSectorLines(center, radius, startAngle, endAngle, segments, color);
  } else {
    v_setstr(retval, "Invalid input: DrawCircleSectorLines");
  }
  return result;
}

int cmd_drawcirclev(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto center = get_param_str(argc, params, 0, NULL);
    // auto radius = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_str(argc, params, 2, NULL);
    // DrawCircleV(center, radius, color);
  } else {
    v_setstr(retval, "Invalid input: DrawCircleV");
  }
  return result;
}

int cmd_drawcube(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto position = get_param_str(argc, params, 0, NULL);
    // auto width = get_param_str(argc, params, 1, NULL);
    // auto height = get_param_str(argc, params, 2, NULL);
    // auto length = get_param_str(argc, params, 3, NULL);
    // auto color = get_param_str(argc, params, 4, NULL);
    // DrawCube(position, width, height, length, color);
  } else {
    v_setstr(retval, "Invalid input: DrawCube");
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
    // auto color = get_param_str(argc, params, 5, NULL);
    // DrawCubeTexture(texture, position, width, height, length, color);
  } else {
    v_setstr(retval, "Invalid input: DrawCubeTexture");
  }
  return result;
}

int cmd_drawcubev(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto position = get_param_str(argc, params, 0, NULL);
    // auto size = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_str(argc, params, 2, NULL);
    // DrawCubeV(position, size, color);
  } else {
    v_setstr(retval, "Invalid input: DrawCubeV");
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
    // auto color = get_param_str(argc, params, 4, NULL);
    // DrawCubeWires(position, width, height, length, color);
  } else {
    v_setstr(retval, "Invalid input: DrawCubeWires");
  }
  return result;
}

int cmd_drawcubewiresv(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto position = get_param_str(argc, params, 0, NULL);
    // auto size = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_str(argc, params, 2, NULL);
    // DrawCubeWiresV(position, size, color);
  } else {
    v_setstr(retval, "Invalid input: DrawCubeWiresV");
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
    // auto color = get_param_str(argc, params, 5, NULL);
    // DrawCylinder(position, radiusTop, radiusBottom, height, slices, color);
  } else {
    v_setstr(retval, "Invalid input: DrawCylinder");
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
    // auto color = get_param_str(argc, params, 5, NULL);
    // DrawCylinderWires(position, radiusTop, radiusBottom, height, slices, color);
  } else {
    v_setstr(retval, "Invalid input: DrawCylinderWires");
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
    // auto color = get_param_str(argc, params, 4, NULL);
    // DrawEllipse(centerX, centerY, radiusH, radiusV, color);
  } else {
    v_setstr(retval, "Invalid input: DrawEllipse");
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
    // auto color = get_param_str(argc, params, 4, NULL);
    // DrawEllipseLines(centerX, centerY, radiusH, radiusV, color);
  } else {
    v_setstr(retval, "Invalid input: DrawEllipseLines");
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
    v_setstr(retval, "Invalid input: DrawFPS");
  }
  return result;
}

int cmd_drawgizmo(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    DrawGizmo(get_param_vec3(argc, params, 0));
  } else {
    v_setstr(retval, "Invalid input: DrawGizmo");
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
    v_setstr(retval, "Invalid input: DrawGrid");
  }
  return result;
}

int cmd_drawline(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto startPosX = get_param_str(argc, params, 0, NULL);
    // auto startPosY = get_param_str(argc, params, 1, NULL);
    // auto endPosX = get_param_str(argc, params, 2, NULL);
    // auto endPosY = get_param_str(argc, params, 3, NULL);
    // auto color = get_param_str(argc, params, 4, NULL);
    // DrawLine(startPosX, startPosY, endPosX, endPosY, color);
  } else {
    v_setstr(retval, "Invalid input: DrawLine");
  }
  return result;
}

int cmd_drawline3d(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto startPos = get_param_str(argc, params, 0, NULL);
    // auto endPos = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_str(argc, params, 2, NULL);
    // DrawLine3D(startPos, endPos, color);
  } else {
    v_setstr(retval, "Invalid input: DrawLine3D");
  }
  return result;
}

int cmd_drawlinebezier(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto startPos = get_param_str(argc, params, 0, NULL);
    // auto endPos = get_param_str(argc, params, 1, NULL);
    // auto thick = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_str(argc, params, 3, NULL);
    // DrawLineBezier(startPos, endPos, thick, color);
  } else {
    v_setstr(retval, "Invalid input: DrawLineBezier");
  }
  return result;
}

int cmd_drawlineex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto startPos = get_param_str(argc, params, 0, NULL);
    // auto endPos = get_param_str(argc, params, 1, NULL);
    // auto thick = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_str(argc, params, 3, NULL);
    // DrawLineEx(startPos, endPos, thick, color);
  } else {
    v_setstr(retval, "Invalid input: DrawLineEx");
  }
  return result;
}

int cmd_drawlinestrip(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto points = get_param_str(argc, params, 0, NULL);
    // auto numPoints = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_str(argc, params, 2, NULL);
    // DrawLineStrip(points, numPoints, color);
  } else {
    v_setstr(retval, "Invalid input: DrawLineStrip");
  }
  return result;
}

int cmd_drawlinev(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto startPos = get_param_str(argc, params, 0, NULL);
    // auto endPos = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_str(argc, params, 2, NULL);
    // DrawLineV(startPos, endPos, color);
  } else {
    v_setstr(retval, "Invalid input: DrawLineV");
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
      v_setstr(retval, "Invalid input: Model not found");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid input: DrawModel");
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
    v_setstr(retval, "Invalid input: DrawModelEx");
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
    v_setstr(retval, "Invalid input: DrawModelWires");
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
    v_setstr(retval, "Invalid input: DrawModelWiresEx");
  }
  return result;
}

int cmd_drawpixel(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto posX = get_param_str(argc, params, 0, NULL);
    // auto posY = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_str(argc, params, 2, NULL);
    // DrawPixel(posX, posY, color);
  } else {
    v_setstr(retval, "Invalid input: DrawPixel");
  }
  return result;
}

int cmd_drawpixelv(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto position = get_param_str(argc, params, 0, NULL);
    // auto color = get_param_str(argc, params, 1, NULL);
    // DrawPixelV(position, color);
  } else {
    v_setstr(retval, "Invalid input: DrawPixelV");
  }
  return result;
}

int cmd_drawplane(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto centerPos = get_param_str(argc, params, 0, NULL);
    // auto size = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_str(argc, params, 2, NULL);
    // DrawPlane(centerPos, size, color);
  } else {
    v_setstr(retval, "Invalid input: DrawPlane");
  }
  return result;
}

int cmd_drawpoint3d(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto position = get_param_str(argc, params, 0, NULL);
    // auto color = get_param_str(argc, params, 1, NULL);
    // DrawPoint3D(position, color);
  } else {
    v_setstr(retval, "Invalid input: DrawPoint3D");
  }
  return result;
}

int cmd_drawpoly(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto center = get_param_str(argc, params, 0, NULL);
    // auto sides = get_param_str(argc, params, 1, NULL);
    // auto radius = get_param_str(argc, params, 2, NULL);
    // auto rotation = get_param_str(argc, params, 3, NULL);
    // auto color = get_param_str(argc, params, 4, NULL);
    // DrawPoly(center, sides, radius, rotation, color);
  } else {
    v_setstr(retval, "Invalid input: DrawPoly");
  }
  return result;
}

int cmd_drawpolylines(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto center = get_param_str(argc, params, 0, NULL);
    // auto sides = get_param_str(argc, params, 1, NULL);
    // auto radius = get_param_str(argc, params, 2, NULL);
    // auto rotation = get_param_str(argc, params, 3, NULL);
    // auto color = get_param_str(argc, params, 4, NULL);
    // DrawPolyLines(center, sides, radius, rotation, color);
  } else {
    v_setstr(retval, "Invalid input: DrawPolyLines");
  }
  return result;
}

int cmd_drawray(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto ray = get_param_str(argc, params, 0, NULL);
    // auto color = get_param_str(argc, params, 1, NULL);
    // DrawRay(ray, color);
  } else {
    v_setstr(retval, "Invalid input: DrawRay");
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
    v_setstr(retval, "Invalid input: DrawRectangle");
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
    v_setstr(retval, "Invalid input: DrawRectangleGradientEx");
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
    v_setstr(retval, "Invalid input: DrawRectangleGradientH");
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
    v_setstr(retval, "Invalid input: DrawRectangleGradientV");
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
    v_setstr(retval, "Invalid input: DrawRectangleLines");
  }
  return result;
}

int cmd_drawrectanglelinesex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto rec = get_param_str(argc, params, 0, NULL);
    // auto lineThick = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_str(argc, params, 2, NULL);
    // DrawRectangleLinesEx(rec, lineThick, color);
  } else {
    v_setstr(retval, "Invalid input: DrawRectangleLinesEx");
  }
  return result;
}

int cmd_drawrectanglepro(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto rec = get_param_str(argc, params, 0, NULL);
    // auto origin = get_param_str(argc, params, 1, NULL);
    // auto rotation = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_str(argc, params, 3, NULL);
    // DrawRectanglePro(rec, origin, rotation, color);
  } else {
    v_setstr(retval, "Invalid input: DrawRectanglePro");
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
    v_setstr(retval, "Invalid input: DrawRectangleRec");
  }
  return result;
}

int cmd_drawrectanglerounded(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto rec = get_param_str(argc, params, 0, NULL);
    // auto roundness = get_param_str(argc, params, 1, NULL);
    // auto segments = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_str(argc, params, 3, NULL);
    // DrawRectangleRounded(rec, roundness, segments, color);
  } else {
    v_setstr(retval, "Invalid input: DrawRectangleRounded");
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
    // auto color = get_param_str(argc, params, 4, NULL);
    // DrawRectangleRoundedLines(rec, roundness, segments, lineThick, color);
  } else {
    v_setstr(retval, "Invalid input: DrawRectangleRoundedLines");
  }
  return result;
}

int cmd_drawrectanglev(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto position = get_param_str(argc, params, 0, NULL);
    // auto size = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_str(argc, params, 2, NULL);
    // DrawRectangleV(position, size, color);
  } else {
    v_setstr(retval, "Invalid input: DrawRectangleV");
  }
  return result;
}

int cmd_drawring(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    // auto center = get_param_str(argc, params, 0, NULL);
    // auto innerRadius = get_param_str(argc, params, 1, NULL);
    // auto outerRadius = get_param_str(argc, params, 2, NULL);
    // auto startAngle = get_param_str(argc, params, 3, NULL);
    // auto endAngle = get_param_str(argc, params, 4, NULL);
    // auto segments = get_param_str(argc, params, 5, NULL);
    // auto color = get_param_str(argc, params, 6, NULL);
    // DrawRing(center, innerRadius, outerRadius, startAngle, endAngle, segments, color);
  } else {
    v_setstr(retval, "Invalid input: DrawRing");
  }
  return result;
}

int cmd_drawringlines(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 7);
  if (result) {
    // auto center = get_param_str(argc, params, 0, NULL);
    // auto innerRadius = get_param_str(argc, params, 1, NULL);
    // auto outerRadius = get_param_str(argc, params, 2, NULL);
    // auto startAngle = get_param_str(argc, params, 3, NULL);
    // auto endAngle = get_param_str(argc, params, 4, NULL);
    // auto segments = get_param_str(argc, params, 5, NULL);
    // auto color = get_param_str(argc, params, 6, NULL);
    // DrawRingLines(center, innerRadius, outerRadius, startAngle, endAngle, segments, color);
  } else {
    v_setstr(retval, "Invalid input: DrawRingLines");
  }
  return result;
}

int cmd_drawsphere(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto centerPos = get_param_str(argc, params, 0, NULL);
    // auto radius = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_str(argc, params, 2, NULL);
    // DrawSphere(centerPos, radius, color);
  } else {
    v_setstr(retval, "Invalid input: DrawSphere");
  }
  return result;
}

int cmd_drawsphereex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto centerPos = get_param_str(argc, params, 0, NULL);
    // auto radius = get_param_str(argc, params, 1, NULL);
    // auto rings = get_param_str(argc, params, 2, NULL);
    // auto slices = get_param_str(argc, params, 3, NULL);
    // auto color = get_param_str(argc, params, 4, NULL);
    // DrawSphereEx(centerPos, radius, rings, slices, color);
  } else {
    v_setstr(retval, "Invalid input: DrawSphereEx");
  }
  return result;
}

int cmd_drawspherewires(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto centerPos = get_param_str(argc, params, 0, NULL);
    // auto radius = get_param_str(argc, params, 1, NULL);
    // auto rings = get_param_str(argc, params, 2, NULL);
    // auto slices = get_param_str(argc, params, 3, NULL);
    // auto color = get_param_str(argc, params, 4, NULL);
    // DrawSphereWires(centerPos, radius, rings, slices, color);
  } else {
    v_setstr(retval, "Invalid input: DrawSphereWires");
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
    v_setstr(retval, "Invalid input: DrawText");
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
    v_setstr(retval, "Invalid input: DrawTextCodepoint");
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
    v_setstr(retval, "Invalid input: DrawTextEx");
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
    v_setstr(retval, "Invalid input: DrawTextRec");
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
    v_setstr(retval, "Invalid input: DrawTextRecEx");
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
      v_setstr(retval, "Invalid input: Texture not found");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid input: DrawTexture");
  }
  return result;
}

int cmd_drawtextureex(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto texture = get_param_str(argc, params, 0, NULL);
    // auto position = get_param_str(argc, params, 1, NULL);
    // auto rotation = get_param_str(argc, params, 2, NULL);
    // auto scale = get_param_str(argc, params, 3, NULL);
    // auto tint = get_param_str(argc, params, 4, NULL);
    // DrawTextureEx(texture, position, rotation, scale, tint);
  } else {
    v_setstr(retval, "Invalid input: DrawTextureEx");
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
    v_setstr(retval, "Invalid input: DrawTextureNPatch");
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
      v_setstr(retval, "Invalid input: Texture not found");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid input: DrawTexturePro");
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
    v_setstr(retval, "Invalid input: DrawTextureQuad");
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
      v_setstr(retval, "Invalid input: Texture not found");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid input: DrawTextureRec");
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
    v_setstr(retval, "Invalid input: DrawTextureTiled");
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
    v_setstr(retval, "Invalid input: DrawTextureV");
  }
  return result;
}

int cmd_drawtriangle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto v1 = get_param_str(argc, params, 0, NULL);
    // auto v2 = get_param_str(argc, params, 1, NULL);
    // auto v3 = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_str(argc, params, 3, NULL);
    // DrawTriangle(v1, v2, v3, color);
  } else {
    v_setstr(retval, "Invalid input: DrawTriangle");
  }
  return result;
}

int cmd_drawtriangle3d(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto v1 = get_param_str(argc, params, 0, NULL);
    // auto v2 = get_param_str(argc, params, 1, NULL);
    // auto v3 = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_str(argc, params, 3, NULL);
    // DrawTriangle3D(v1, v2, v3, color);
  } else {
    v_setstr(retval, "Invalid input: DrawTriangle3D");
  }
  return result;
}

int cmd_drawtrianglefan(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto points = get_param_str(argc, params, 0, NULL);
    // auto numPoints = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_str(argc, params, 2, NULL);
    // DrawTriangleFan(points, numPoints, color);
  } else {
    v_setstr(retval, "Invalid input: DrawTriangleFan");
  }
  return result;
}

int cmd_drawtrianglelines(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto v1 = get_param_str(argc, params, 0, NULL);
    // auto v2 = get_param_str(argc, params, 1, NULL);
    // auto v3 = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_str(argc, params, 3, NULL);
    // DrawTriangleLines(v1, v2, v3, color);
  } else {
    v_setstr(retval, "Invalid input: DrawTriangleLines");
  }
  return result;
}

int cmd_drawtrianglestrip(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto points = get_param_str(argc, params, 0, NULL);
    // auto pointsCount = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_str(argc, params, 2, NULL);
    // DrawTriangleStrip(points, pointsCount, color);
  } else {
    v_setstr(retval, "Invalid input: DrawTriangleStrip");
  }
  return result;
}

int cmd_drawtrianglestrip3d(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto points = get_param_str(argc, params, 0, NULL);
    // auto pointsCount = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_str(argc, params, 2, NULL);
    // DrawTriangleStrip3D(points, pointsCount, color);
  } else {
    v_setstr(retval, "Invalid input: DrawTriangleStrip3D");
  }
  return result;
}

int cmd_enablecursor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EnableCursor();
  } else {
    v_setstr(retval, "Invalid input: EnableCursor");
  }
  return result;
}

int cmd_endblendmode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndBlendMode();
  } else {
    v_setstr(retval, "Invalid input: EndBlendMode");
  }
  return result;
}

int cmd_enddrawing(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndDrawing();
  } else {
    v_setstr(retval, "Invalid input: EndDrawing");
  }
  return result;
}

int cmd_endmode2d(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndMode2D();
  } else {
    v_setstr(retval, "Invalid input: EndMode2D");
  }
  return result;
}

int cmd_endmode3d(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndMode3D();
  } else {
    v_setstr(retval, "Invalid input: EndMode3D");
  }
  return result;
}

int cmd_endscissormode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndScissorMode();
  } else {
    v_setstr(retval, "Invalid input: EndScissorMode");
  }
  return result;
}

int cmd_endshadermode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndShaderMode();
  } else {
    v_setstr(retval, "Invalid input: EndShaderMode");
  }
  return result;
}

int cmd_endtexturemode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndTextureMode();
  } else {
    v_setstr(retval, "Invalid input: EndTextureMode");
  }
  return result;
}

int cmd_endvrdrawing(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    EndVrDrawing();
  } else {
    v_setstr(retval, "Invalid input: EndVrDrawing");
  }
  return result;
}

int cmd_exportimage(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // auto fileName = get_param_str(argc, params, 1, NULL);
    // ExportImage(image, fileName);
  } else {
    v_setstr(retval, "Invalid input: ExportImage");
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
    v_setstr(retval, "Invalid input: ExportImageAsCode");
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
    v_setstr(retval, "Invalid input: ExportMesh");
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
    v_setstr(retval, "Invalid input: ExportWave");
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
    v_setstr(retval, "Invalid input: ExportWaveAsCode");
  }
  return result;
}

int cmd_gentexturemipmaps(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto texture = get_param_str(argc, params, 0, NULL);
    // GenTextureMipmaps(texture);
  } else {
    v_setstr(retval, "Invalid input: GenTextureMipmaps");
  }
  return result;
}

int cmd_getwindowhandle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    GetWindowHandle();
  } else {
    v_setstr(retval, "Invalid input: GetWindowHandle");
  }
  return result;
}

int cmd_hidecursor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    HideCursor();
  } else {
    v_setstr(retval, "Invalid input: HideCursor");
  }
  return result;
}

int cmd_hidewindow(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    HideWindow();
  } else {
    v_setstr(retval, "Invalid input: HideWindow");
  }
  return result;
}

int cmd_imagealphaclear(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // auto color = get_param_str(argc, params, 1, NULL);
    // auto threshold = get_param_str(argc, params, 2, NULL);
    // ImageAlphaClear(image, color, threshold);
  } else {
    v_setstr(retval, "Invalid input: ImageAlphaClear");
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
    v_setstr(retval, "Invalid input: ImageAlphaCrop");
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
    v_setstr(retval, "Invalid input: ImageAlphaMask");
  }
  return result;
}

int cmd_imagealphapremultiply(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // ImageAlphaPremultiply(&image);
  } else {
    v_setstr(retval, "Invalid input: ImageAlphaPremultiply");
  }
  return result;
}

int cmd_imageclearbackground(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto color = get_param_str(argc, params, 1, NULL);
    // ImageClearBackground(dst, color);
  } else {
    v_setstr(retval, "Invalid input: ImageClearBackground");
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
      v_setstr(retval, "Invalid input: Image not found");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid input: ImageColorBrightness");
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
      v_setstr(retval, "Invalid input: Image not found");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid input: ImageColorContrast");
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
      v_setstr(retval, "Invalid input: Image not found");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid input: ImageColorGrayscale");
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
      v_setstr(retval, "Invalid input: Image not found");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid input: ImageColorInvert");
  }
  return result;
}

int cmd_imagecolorreplace(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // auto color = get_param_str(argc, params, 1, NULL);
    // auto replace = get_param_str(argc, params, 2, NULL);
    // ImageColorReplace(image, color, replace);
  } else {
    v_setstr(retval, "Invalid input: ImageColorReplace");
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
      v_setstr(retval, "Invalid input: Image not found");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid input: ImageColorTint");
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
    v_setstr(retval, "Invalid input: ImageCrop");
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
    v_setstr(retval, "Invalid input: ImageDither");
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
    v_setstr(retval, "Invalid input: ImageDraw");
  }
  return result;
}

int cmd_imagedrawcircle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 5);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto centerX = get_param_str(argc, params, 1, NULL);
    // auto centerY = get_param_str(argc, params, 2, NULL);
    // auto radius = get_param_str(argc, params, 3, NULL);
    // auto color = get_param_str(argc, params, 4, NULL);
    // ImageDrawCircle(dst, centerX, centerY, radius, color);
  } else {
    v_setstr(retval, "Invalid input: ImageDrawCircle");
  }
  return result;
}

int cmd_imagedrawcirclev(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto center = get_param_str(argc, params, 1, NULL);
    // auto radius = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_str(argc, params, 3, NULL);
    // ImageDrawCircleV(dst, center, radius, color);
  } else {
    v_setstr(retval, "Invalid input: ImageDrawCircleV");
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
    // auto color = get_param_str(argc, params, 5, NULL);
    // ImageDrawLine(dst, startPosX, startPosY, endPosX, endPosY, color);
  } else {
    v_setstr(retval, "Invalid input: ImageDrawLine");
  }
  return result;
}

int cmd_imagedrawlinev(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto start = get_param_str(argc, params, 1, NULL);
    // auto end = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_str(argc, params, 3, NULL);
    // ImageDrawLineV(dst, start, end, color);
  } else {
    v_setstr(retval, "Invalid input: ImageDrawLineV");
  }
  return result;
}

int cmd_imagedrawpixel(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto posX = get_param_str(argc, params, 1, NULL);
    // auto posY = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_str(argc, params, 3, NULL);
    // ImageDrawPixel(dst, posX, posY, color);
  } else {
    v_setstr(retval, "Invalid input: ImageDrawPixel");
  }
  return result;
}

int cmd_imagedrawpixelv(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto position = get_param_str(argc, params, 1, NULL);
    // auto color = get_param_str(argc, params, 2, NULL);
    // ImageDrawPixelV(dst, position, color);
  } else {
    v_setstr(retval, "Invalid input: ImageDrawPixelV");
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
    // auto color = get_param_str(argc, params, 5, NULL);
    // ImageDrawRectangle(dst, posX, posY, width, height, color);
  } else {
    v_setstr(retval, "Invalid input: ImageDrawRectangle");
  }
  return result;
}

int cmd_imagedrawrectanglelines(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto rec = get_param_str(argc, params, 1, NULL);
    // auto thick = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_str(argc, params, 3, NULL);
    // ImageDrawRectangleLines(dst, rec, thick, color);
  } else {
    v_setstr(retval, "Invalid input: ImageDrawRectangleLines");
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
      v_setstr(retval, "Invalid input: Image not found");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid input: ImageDrawRectangleRec");
  }
  return result;
}

int cmd_imagedrawrectanglev(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 4);
  if (result) {
    // auto dst = get_param_str(argc, params, 0, NULL);
    // auto position = get_param_str(argc, params, 1, NULL);
    // auto size = get_param_str(argc, params, 2, NULL);
    // auto color = get_param_str(argc, params, 3, NULL);
    // ImageDrawRectangleV(dst, position, size, color);
  } else {
    v_setstr(retval, "Invalid input: ImageDrawRectangleV");
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
    // auto color = get_param_str(argc, params, 5, NULL);
    // ImageDrawText(dst, text, posX, posY, fontSize, color);
  } else {
    v_setstr(retval, "Invalid input: ImageDrawText");
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
    v_setstr(retval, "Invalid input: ImageDrawTextEx");
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
      v_setstr(retval, "Invalid input: Image not found");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid input: ImageFlipHorizontal");
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
      v_setstr(retval, "Invalid input: Image not found");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid input: ImageFlipVertical");
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
      v_setstr(retval, "Invalid input: Image not found");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid input: ImageFormat");
  }
  return result;
}

int cmd_imagemipmaps(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // ImageMipmaps(image);
  } else {
    v_setstr(retval, "Invalid input: ImageMipmaps");
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
    v_setstr(retval, "Invalid input: ImageResize");
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
    v_setstr(retval, "Invalid input: ImageResizeCanvas");
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
    v_setstr(retval, "Invalid input: ImageResizeNN");
  }
  return result;
}

int cmd_imagerotateccw(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // ImageRotateCCW(image);
  } else {
    v_setstr(retval, "Invalid input: ImageRotateCCW");
  }
  return result;
}

int cmd_imagerotatecw(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // ImageRotateCW(image);
  } else {
    v_setstr(retval, "Invalid input: ImageRotateCW");
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
    v_setstr(retval, "Invalid input: ImageToPOT");
  }
  return result;
}

int cmd_initaudiodevice(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    InitAudioDevice();
  } else {
    v_setstr(retval, "Invalid input: InitAudioDevice");
  }
  return result;
}

int cmd_initvrsimulator(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    InitVrSimulator();
  } else {
    v_setstr(retval, "Invalid input: InitVrSimulator");
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
    v_setstr(retval, "Invalid input: InitWindow");
  }
  return result;
}

int cmd_maximizewindow(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    MaximizeWindow();
  } else {
    v_setstr(retval, "Invalid input: MaximizeWindow");
  }
  return result;
}

int cmd_meshbinormals(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto mesh = get_param_str(argc, params, 0, NULL);
    // MeshBinormals(mesh);
  } else {
    v_setstr(retval, "Invalid input: MeshBinormals");
  }
  return result;
}

int cmd_meshnormalssmooth(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto mesh = get_param_str(argc, params, 0, NULL);
    // MeshNormalsSmooth(mesh);
  } else {
    v_setstr(retval, "Invalid input: MeshNormalsSmooth");
  }
  return result;
}

int cmd_meshtangents(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto mesh = get_param_str(argc, params, 0, NULL);
    // MeshTangents(mesh);
  } else {
    v_setstr(retval, "Invalid input: MeshTangents");
  }
  return result;
}

int cmd_openurl(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto url = get_param_str(argc, params, 0, NULL);
    // OpenURL(url);
  } else {
    v_setstr(retval, "Invalid input: OpenURL");
  }
  return result;
}

int cmd_pauseaudiostream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto stream = get_param_str(argc, params, 0, NULL);
    // PauseAudioStream(stream);
  } else {
    v_setstr(retval, "Invalid input: PauseAudioStream");
  }
  return result;
}

int cmd_pausemusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    PauseMusicStream(music);
  } else {
    v_setstr(retval, "Invalid input: PauseMusicStream");
  }
  return result;
}

int cmd_pausesound(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    PauseSound(sound);
  } else {
    v_setstr(retval, "Invalid input: PauseSound");
  }
  return result;
}

int cmd_playaudiostream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto stream = get_param_str(argc, params, 0, NULL);
    // PlayAudioStream(stream);
  } else {
    v_setstr(retval, "Invalid input: PlayAudioStream");
  }
  return result;
}

int cmd_playmusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    PlayMusicStream(music);
  } else {
    v_setstr(retval, "Invalid input: PlayMusicStream");
  }
  return result;
}

int cmd_playsound(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    PlaySound(sound);
  } else {
    v_setstr(retval, "Invalid input: PlaySound");
  }
  return result;
}

int cmd_playsoundmulti(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    PlaySoundMulti(sound);
  } else {
    v_setstr(retval, "Invalid input: PlaySoundMulti");
  }
  return result;
}

int cmd_restorewindow(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    RestoreWindow();
  } else {
    v_setstr(retval, "Invalid input: RestoreWindow");
  }
  return result;
}

int cmd_resumeaudiostream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto stream = get_param_str(argc, params, 0, NULL);
    // ResumeAudioStream(stream);
  } else {
    v_setstr(retval, "Invalid input: ResumeAudioStream");
  }
  return result;
}

int cmd_resumemusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    ResumeMusicStream(music);
  } else {
    v_setstr(retval, "Invalid input: ResumeMusicStream");
  }
  return result;
}

int cmd_resumesound(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    ResumeSound(sound);
  } else {
    v_setstr(retval, "Invalid input: ResumeSound");
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
    v_setstr(retval, "Invalid input: SaveFileData");
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
    v_setstr(retval, "Invalid input: SaveFileText");
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
    v_setstr(retval, "Invalid input: SaveStorageValue");
  }
  return result;
}

int cmd_setaudiostreambuffersizedefault(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto size = get_param_str(argc, params, 0, NULL);
    // SetAudioStreamBufferSizeDefault(size);
  } else {
    v_setstr(retval, "Invalid input: SetAudioStreamBufferSizeDefault");
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
    v_setstr(retval, "Invalid input: SetAudioStreamPitch");
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
    v_setstr(retval, "Invalid input: SetAudioStreamVolume");
  }
  return result;
}

int cmd_setcameraaltcontrol(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto altKey = get_param_str(argc, params, 0, NULL);
    // SetCameraAltControl(altKey);
  } else {
    v_setstr(retval, "Invalid input: SetCameraAltControl");
  }
  return result;
}

int cmd_setcameramode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    auto mode = get_param_int(argc, params, 1, 0);
    SetCameraMode(get_camera_3d(argc, params, 0), mode);
  } else {
    v_setstr(retval, "Invalid input: SetCameraMode");
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
    v_setstr(retval, "Invalid input: SetCameraMoveControls");
  }
  return result;
}

int cmd_setcamerapancontrol(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto panKey = get_param_str(argc, params, 0, NULL);
    // SetCameraPanControl(panKey);
  } else {
    v_setstr(retval, "Invalid input: SetCameraPanControl");
  }
  return result;
}

int cmd_setcamerasmoothzoomcontrol(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto szKey = get_param_str(argc, params, 0, NULL);
    // SetCameraSmoothZoomControl(szKey);
  } else {
    v_setstr(retval, "Invalid input: SetCameraSmoothZoomControl");
  }
  return result;
}

int cmd_setclipboardtext(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto text = get_param_str(argc, params, 0, NULL);
    // SetClipboardText(text);
  } else {
    v_setstr(retval, "Invalid input: SetClipboardText");
  }
  return result;
}

int cmd_setconfigflags(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto flags = get_param_int(argc, params, 0, 0);
    SetConfigFlags(flags);
  } else {
    v_setstr(retval, "Invalid input: SetConfigFlags");
  }
  return result;
}

int cmd_setexitkey(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto key = get_param_str(argc, params, 0, NULL);
    // SetExitKey(key);
  } else {
    v_setstr(retval, "Invalid input: SetExitKey");
  }
  return result;
}

int cmd_setgesturesenabled(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto gestureFlags = get_param_str(argc, params, 0, NULL);
    // SetGesturesEnabled(gestureFlags);
  } else {
    v_setstr(retval, "Invalid input: SetGesturesEnabled");
  }
  return result;
}

int cmd_setmastervolume(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto volume = get_param_str(argc, params, 0, NULL);
    // SetMasterVolume(volume);
  } else {
    v_setstr(retval, "Invalid input: SetMasterVolume");
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
    v_setstr(retval, "Invalid input: SetMaterialTexture");
  }
  return result;
}

int cmd_setmatrixmodelview(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto view = get_param_str(argc, params, 0, NULL);
    // SetMatrixModelview(view);
  } else {
    v_setstr(retval, "Invalid input: SetMatrixModelview");
  }
  return result;
}

int cmd_setmatrixprojection(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto proj = get_param_str(argc, params, 0, NULL);
    // SetMatrixProjection(proj);
  } else {
    v_setstr(retval, "Invalid input: SetMatrixProjection");
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
      v_setstr(retval, "Invalid input: Model not found");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid input: SetModelDiffuseTexture");
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
    v_setstr(retval, "Invalid input: SetModelMeshMaterial");
  }
  return result;
}

int cmd_setmouseoffset(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto offsetX = get_param_str(argc, params, 0, NULL);
    // auto offsetY = get_param_str(argc, params, 1, NULL);
    // SetMouseOffset(offsetX, offsetY);
  } else {
    v_setstr(retval, "Invalid input: SetMouseOffset");
  }
  return result;
}

int cmd_setmouseposition(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto x = get_param_str(argc, params, 0, NULL);
    // auto y = get_param_str(argc, params, 1, NULL);
    // SetMousePosition(x, y);
  } else {
    v_setstr(retval, "Invalid input: SetMousePosition");
  }
  return result;
}

int cmd_setmousescale(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto scaleX = get_param_str(argc, params, 0, NULL);
    // auto scaleY = get_param_str(argc, params, 1, NULL);
    // SetMouseScale(scaleX, scaleY);
  } else {
    v_setstr(retval, "Invalid input: SetMouseScale");
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
    v_setstr(retval, "Invalid input: SetMusicPitch");
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
    v_setstr(retval, "Invalid input: SetMusicVolume");
  }
  return result;
}

int cmd_setpixelcolor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 3);
  if (result) {
    // auto dstPtr = get_param_str(argc, params, 0, NULL);
    // auto color = get_param_str(argc, params, 1, NULL);
    // auto format = get_param_str(argc, params, 2, NULL);
    // SetPixelColor(dstPtr, color, format);
  } else {
    v_setstr(retval, "Invalid input: SetPixelColor");
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
    v_setstr(retval, "Invalid input: SetShaderValue");
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
    v_setstr(retval, "Invalid input: SetShaderValueMatrix");
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
    v_setstr(retval, "Invalid input: SetShaderValueTexture");
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
    v_setstr(retval, "Invalid input: SetShaderValueV");
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
    v_setstr(retval, "Invalid input: SetShapesTexture");
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
    v_setstr(retval, "Invalid input: SetSoundPitch");
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
    v_setstr(retval, "Invalid input: SetSoundVolume");
  }
  return result;
}

int cmd_settargetfps(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto fps = get_param_int(argc, params, 0, 50);
    SetTargetFPS(fps);
  } else {
    v_setstr(retval, "Invalid input: SetTargetFPS");
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
    v_setstr(retval, "Invalid input: SetTextureFilter");
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
    v_setstr(retval, "Invalid input: SetTextureWrap");
  }
  return result;
}

int cmd_settracelogcallback(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto callback = get_param_str(argc, params, 0, NULL);
    // SetTraceLogCallback(callback);
  } else {
    v_setstr(retval, "Invalid input: SetTraceLogCallback");
  }
  return result;
}

int cmd_settracelogexit(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto logType = get_param_str(argc, params, 0, NULL);
    // SetTraceLogExit(logType);
  } else {
    v_setstr(retval, "Invalid input: SetTraceLogExit");
  }
  return result;
}

int cmd_settraceloglevel(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto logType = get_param_int(argc, params, 0, 0);
    SetTraceLogLevel(logType);
  } else {
    v_setstr(retval, "Invalid input: SetTraceLogLevel");
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
    v_setstr(retval, "Invalid input: SetVrConfiguration");
  }
  return result;
}

int cmd_setwindowicon(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // SetWindowIcon(image);
  } else {
    v_setstr(retval, "Invalid input: SetWindowIcon");
  }
  return result;
}

int cmd_setwindowminsize(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto width = get_param_str(argc, params, 0, NULL);
    // auto height = get_param_str(argc, params, 1, NULL);
    // SetWindowMinSize(width, height);
  } else {
    v_setstr(retval, "Invalid input: SetWindowMinSize");
  }
  return result;
}

int cmd_setwindowmonitor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto monitor = get_param_str(argc, params, 0, NULL);
    // SetWindowMonitor(monitor);
  } else {
    v_setstr(retval, "Invalid input: SetWindowMonitor");
  }
  return result;
}

int cmd_setwindowposition(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 2);
  if (result) {
    // auto x = get_param_str(argc, params, 0, NULL);
    // auto y = get_param_str(argc, params, 1, NULL);
    // SetWindowPosition(x, y);
  } else {
    v_setstr(retval, "Invalid input: SetWindowPosition");
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
    v_setstr(retval, "Invalid input: SetWindowSize");
  }
  return result;
}

int cmd_setwindowtitle(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    auto title = get_param_str(argc, params, 0, NULL);
    SetWindowTitle(title);
  } else {
    v_setstr(retval, "Invalid input: SetWindowTitle");
  }
  return result;
}

int cmd_showcursor(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    ShowCursor();
  } else {
    v_setstr(retval, "Invalid input: ShowCursor");
  }
  return result;
}

int cmd_stopaudiostream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto stream = get_param_str(argc, params, 0, NULL);
    // StopAudioStream(stream);
  } else {
    v_setstr(retval, "Invalid input: StopAudioStream");
  }
  return result;
}

int cmd_stopmusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    StopMusicStream(music);
  } else {
    v_setstr(retval, "Invalid input: StopMusicStream");
  }
  return result;
}

int cmd_stopsound(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _soundMap.find(get_param_int(argc, params, 0, 0)) != _soundMap.end()) {
    auto sound = _soundMap.at(get_param_int(argc, params, 0, 0));
    StopSound(sound);
  } else {
    v_setstr(retval, "Invalid input: StopSound");
  }
  return result;
}

int cmd_stopsoundmulti(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    StopSoundMulti();
  } else {
    v_setstr(retval, "Invalid input: StopSoundMulti");
  }
  return result;
}

int cmd_takescreenshot(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto fileName = get_param_str(argc, params, 0, NULL);
    // TakeScreenshot(fileName);
  } else {
    v_setstr(retval, "Invalid input: TakeScreenshot");
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
    v_setstr(retval, "Invalid input: TextAppend");
  }
  return result;
}

int cmd_togglefullscreen(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    ToggleFullscreen();
  } else {
    v_setstr(retval, "Invalid input: ToggleFullscreen");
  }
  return result;
}

int cmd_togglevrmode(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    ToggleVrMode();
  } else {
    v_setstr(retval, "Invalid input: ToggleVrMode");
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
    v_setstr(retval, "Invalid input: TraceLog");
  }
  return result;
}

int cmd_undecoratewindow(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    UndecorateWindow();
  } else {
    v_setstr(retval, "Invalid input: UndecorateWindow");
  }
  return result;
}

int cmd_unhidewindow(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 0);
  if (result) {
    UnhideWindow();
  } else {
    v_setstr(retval, "Invalid input: UnhideWindow");
  }
  return result;
}

int cmd_unloadfont(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto font = get_param_str(argc, params, 0, NULL);
    // UnloadFont(font);
  } else {
    v_setstr(retval, "Invalid input: UnloadFont");
  }
  return result;
}

int cmd_unloadimage(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto image = get_param_str(argc, params, 0, NULL);
    // UnloadImage(image);
  } else {
    v_setstr(retval, "Invalid input: UnloadImage");
  }
  return result;
}

int cmd_unloadmaterial(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto material = get_param_str(argc, params, 0, NULL);
    // UnloadMaterial(material);
  } else {
    v_setstr(retval, "Invalid input: UnloadMaterial");
  }
  return result;
}

int cmd_unloadmesh(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto mesh = get_param_str(argc, params, 0, NULL);
    // UnloadMesh(mesh);
  } else {
    v_setstr(retval, "Invalid input: UnloadMesh");
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
      v_setstr(retval, "Invalid input: Model not found");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid input: UnloadModel");
  }
  return result;
}

int cmd_unloadmodelanimation(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto anim = get_param_str(argc, params, 0, NULL);
    // UnloadModelAnimation(anim);
  } else {
    v_setstr(retval, "Invalid input: UnloadModelAnimation");
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
    v_setstr(retval, "Invalid input: UnloadMusicStream");
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
      v_setstr(retval, "Invalid input: RenderTexture not found");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid input: UnloadRenderTexture");
  }
  return result;
}

int cmd_unloadshader(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    UnloadShader(get_param_shader(argc, params, 0));
  } else {
    v_setstr(retval, "Invalid input: UnloadShader");
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
    v_setstr(retval, "Invalid input: UnloadSound");
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
      v_setstr(retval, "Invalid input: Texture not found");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid input: UnloadTexture");
  }
  return result;
}

int cmd_unloadwave(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto wave = get_param_str(argc, params, 0, NULL);
    // UnloadWave(wave);
  } else {
    v_setstr(retval, "Invalid input: UnloadWave");
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
    v_setstr(retval, "Invalid input: UpdateAudioStream");
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
    v_setstr(retval, "Invalid input: UpdateCamera");
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
    v_setstr(retval, "Invalid input: UpdateModelAnimation");
  }
  return result;
}

int cmd_updatemusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result && _musicMap.find(get_param_int(argc, params, 0, 0)) != _musicMap.end()) {
    auto music = _musicMap.at(get_param_int(argc, params, 0, 0));
    UpdateMusicStream(music);
  } else {
    v_setstr(retval, "Invalid input: UpdateMusicStream");
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
    v_setstr(retval, "Invalid input: UpdateSound");
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
      v_setstr(retval, "Invalid input: Texture not found");
      result = 0;
    }
  } else {
    v_setstr(retval, "Invalid input: UpdateTexture");
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
    v_setstr(retval, "Invalid input: UpdateTextureRec");
  }
  return result;
}

int cmd_updatevrtracking(int argc, slib_par_t *params, var_t *retval) {
  int result = (argc == 1);
  if (result) {
    // auto camera = get_param_str(argc, params, 0, NULL);
    // UpdateVrTracking(camera);
  } else {
    v_setstr(retval, "Invalid input: UpdateVrTracking");
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
    v_setstr(retval, "Invalid input: WaveCrop");
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
    v_setstr(retval, "Invalid input: WaveFormat");
  }
  return result;
}

API lib_func[] = {
  {"CHANGEDIRECTORY", cmd_changedirectory},
  // {"CHECKCOLLISIONBOXES", cmd_checkcollisionboxes},
  // {"CHECKCOLLISIONBOXSPHERE", cmd_checkcollisionboxsphere},
  // {"CHECKCOLLISIONCIRCLEREC", cmd_checkcollisioncirclerec},
  // {"CHECKCOLLISIONCIRCLES", cmd_checkcollisioncircles},
  // {"CHECKCOLLISIONPOINTCIRCLE", cmd_checkcollisionpointcircle},
  // {"CHECKCOLLISIONPOINTREC", cmd_checkcollisionpointrec},
  // {"CHECKCOLLISIONPOINTTRIANGLE", cmd_checkcollisionpointtriangle},
  // {"CHECKCOLLISIONRAYBOX", cmd_checkcollisionraybox},
  // {"CHECKCOLLISIONRAYSPHERE", cmd_checkcollisionraysphere},
  // {"CHECKCOLLISIONRAYSPHEREEX", cmd_checkcollisionraysphereex},
  {"CHECKCOLLISIONRECS", cmd_checkcollisionrecs},
  // {"CHECKCOLLISIONSPHERES", cmd_checkcollisionspheres},
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
  // {"DIRECTORYEXISTS", cmd_directoryexists},
  {"FADE", cmd_fade},
  // {"FILEEXISTS", cmd_fileexists},
  // {"GENIMAGECELLULAR", cmd_genimagecellular},
  // {"GENIMAGECHECKED", cmd_genimagechecked},
  // {"GENIMAGECOLOR", cmd_genimagecolor},
  // {"GENIMAGEFONTATLAS", cmd_genimagefontatlas},
  // {"GENIMAGEGRADIENTH", cmd_genimagegradienth},
  // {"GENIMAGEGRADIENTRADIAL", cmd_genimagegradientradial},
  // {"GENIMAGEGRADIENTV", cmd_genimagegradientv},
  // {"GENIMAGEPERLINNOISE", cmd_genimageperlinnoise},
  // {"GENIMAGEWHITENOISE", cmd_genimagewhitenoise},
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
  // {"GETCLIPBOARDTEXT", cmd_getclipboardtext},
  // {"GETCODEPOINTS", cmd_getcodepoints},
  // {"GETCODEPOINTSCOUNT", cmd_getcodepointscount},
  // {"GETCOLLISIONRAYGROUND", cmd_getcollisionrayground},
  // {"GETCOLLISIONRAYMODEL", cmd_getcollisionraymodel},
  // {"GETCOLLISIONRAYTRIANGLE", cmd_getcollisionraytriangle},
  {"GETCOLLISIONREC", cmd_getcollisionrec},
  // {"GETCOLOR", cmd_getcolor},
  // {"GETDIRECTORYFILES", cmd_getdirectoryfiles},
  // {"GETDIRECTORYPATH", cmd_getdirectorypath},
  // {"GETDROPPEDFILES", cmd_getdroppedfiles},
  // {"GETFILEEXTENSION", cmd_getfileextension},
  // {"GETFILEMODTIME", cmd_getfilemodtime},
  // {"GETFILENAME", cmd_getfilename},
  // {"GETFILENAMEWITHOUTEXT", cmd_getfilenamewithoutext},
  // {"GETFONTDEFAULT", cmd_getfontdefault},
  // {"GETFPS", cmd_getfps},
  // {"GETFRAMETIME", cmd_getframetime},
  // {"GETGAMEPADAXISCOUNT", cmd_getgamepadaxiscount},
  // {"GETGAMEPADAXISMOVEMENT", cmd_getgamepadaxismovement},
  // {"GETGAMEPADBUTTONPRESSED", cmd_getgamepadbuttonpressed},
  // {"GETGAMEPADNAME", cmd_getgamepadname},
  // {"GETGESTUREDETECTED", cmd_getgesturedetected},
  // {"GETGESTUREDRAGANGLE", cmd_getgesturedragangle},
  // {"GETGESTUREDRAGVECTOR", cmd_getgesturedragvector},
  // {"GETGESTUREHOLDDURATION", cmd_getgestureholdduration},
  // {"GETGESTUREPINCHANGLE", cmd_getgesturepinchangle},
  // {"GETGESTUREPINCHVECTOR", cmd_getgesturepinchvector},
  // {"GETGLYPHINDEX", cmd_getglyphindex},
  // {"GETIMAGEALPHABORDER", cmd_getimagealphaborder},
  {"GETIMAGEDATA", cmd_getimagedata},
  // {"GETIMAGEDATANORMALIZED", cmd_getimagedatanormalized},
  // {"GETIMAGEPALETTE", cmd_getimagepalette},
  // {"GETKEYPRESSED", cmd_getkeypressed},
  // {"GETMATRIXMODELVIEW", cmd_getmatrixmodelview},
  // {"GETMATRIXPROJECTION", cmd_getmatrixprojection},
  // {"GETMONITORCOUNT", cmd_getmonitorcount},
  // {"GETMONITORHEIGHT", cmd_getmonitorheight},
  // {"GETMONITORNAME", cmd_getmonitorname},
  // {"GETMONITORPHYSICALHEIGHT", cmd_getmonitorphysicalheight},
  // {"GETMONITORPHYSICALWIDTH", cmd_getmonitorphysicalwidth},
  // {"GETMONITORREFRESHRATE", cmd_getmonitorrefreshrate},
  // {"GETMONITORWIDTH", cmd_getmonitorwidth},
  {"GETMOUSEPOSITION", cmd_getmouseposition},
  // {"GETMOUSERAY", cmd_getmouseray},
  // {"GETMOUSEWHEELMOVE", cmd_getmousewheelmove},
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
  // {"GETTEXTUREDATA", cmd_gettexturedata},
  // {"GETTEXTUREDEFAULT", cmd_gettexturedefault},
  // {"GETTIME", cmd_gettime},
  // {"GETTOUCHPOINTSCOUNT", cmd_gettouchpointscount},
  // {"GETTOUCHPOSITION", cmd_gettouchposition},
  // {"GETTOUCHX", cmd_gettouchx},
  // {"GETTOUCHY", cmd_gettouchy},
  // {"GETWAVEDATA", cmd_getwavedata},
  // {"GETWINDOWPOSITION", cmd_getwindowposition},
  // {"GETWINDOWSCALEDPI", cmd_getwindowscaledpi},
  // {"GETWORKINGDIRECTORY", cmd_getworkingdirectory},
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
};

API lib_proc[] = {
  {"BEGINBLENDMODE", cmd_beginblendmode},
  {"BEGINDRAWING", cmd_begindrawing},
  // {"BEGINMODE2D", cmd_beginmode2d},
  {"BEGINMODE3D", cmd_beginmode3d},
  // {"BEGINSCISSORMODE", cmd_beginscissormode},
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
  // {"DRAWCIRCLE", cmd_drawcircle},
  // {"DRAWCIRCLE3D", cmd_drawcircle3d},
  // {"DRAWCIRCLEGRADIENT", cmd_drawcirclegradient},
  // {"DRAWCIRCLELINES", cmd_drawcirclelines},
  // {"DRAWCIRCLESECTOR", cmd_drawcirclesector},
  // {"DRAWCIRCLESECTORLINES", cmd_drawcirclesectorlines},
  // {"DRAWCIRCLEV", cmd_drawcirclev},
  // {"DRAWCUBE", cmd_drawcube},
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
  // {"DRAWLINE", cmd_drawline},
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
  // {"DRAWRECTANGLELINESEX", cmd_drawrectanglelinesex},
  // {"DRAWRECTANGLEPRO", cmd_drawrectanglepro},
  {"DRAWRECTANGLEREC", cmd_drawrectanglerec},
  // {"DRAWRECTANGLEROUNDED", cmd_drawrectanglerounded},
  // {"DRAWRECTANGLEROUNDEDLINES", cmd_drawrectangleroundedlines},
  // {"DRAWRECTANGLEV", cmd_drawrectanglev},
  // {"DRAWRING", cmd_drawring},
  // {"DRAWRINGLINES", cmd_drawringlines},
  // {"DRAWSPHERE", cmd_drawsphere},
  // {"DRAWSPHEREEX", cmd_drawsphereex},
  // {"DRAWSPHEREWIRES", cmd_drawspherewires},
  {"DRAWTEXT", cmd_drawtext},
  // {"DRAWTEXTCODEPOINT", cmd_drawtextcodepoint},
  // {"DRAWTEXTEX", cmd_drawtextex},
  // {"DRAWTEXTREC", cmd_drawtextrec},
  // {"DRAWTEXTRECEX", cmd_drawtextrecex},
  {"DRAWTEXTURE", cmd_drawtexture},
  // {"DRAWTEXTUREEX", cmd_drawtextureex},
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
  // {"EXPORTIMAGE", cmd_exportimage},
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
  // {"IMAGEDRAWCIRCLE", cmd_imagedrawcircle},
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
  // {"SETCLIPBOARDTEXT", cmd_setclipboardtext},
  {"SETCONFIGFLAGS", cmd_setconfigflags},
  // {"SETEXITKEY", cmd_setexitkey},
  // {"SETGESTURESENABLED", cmd_setgesturesenabled},
  // {"SETMASTERVOLUME", cmd_setmastervolume},
  // {"SETMATERIALTEXTURE", cmd_setmaterialtexture},
  // {"SETMATRIXMODELVIEW", cmd_setmatrixmodelview},
  // {"SETMATRIXPROJECTION", cmd_setmatrixprojection},
  {"SETMODELDIFFUSETEXTURE", cmd_setmodeldiffusetexture},
  // {"SETMODELMESHMATERIAL", cmd_setmodelmeshmaterial},
  // {"SETMOUSEOFFSET", cmd_setmouseoffset},
  // {"SETMOUSEPOSITION", cmd_setmouseposition},
  // {"SETMOUSESCALE", cmd_setmousescale},
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
  // {"SETTRACELOGEXIT", cmd_settracelogexit},
  {"SETTRACELOGLEVEL", cmd_settraceloglevel},
  // {"SETVRCONFIGURATION", cmd_setvrconfiguration},
  // {"SETWINDOWICON", cmd_setwindowicon},
  // {"SETWINDOWMINSIZE", cmd_setwindowminsize},
  // {"SETWINDOWMONITOR", cmd_setwindowmonitor},
  // {"SETWINDOWPOSITION", cmd_setwindowposition},
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
