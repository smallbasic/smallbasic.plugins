//
// Change working directory, return true on success
//
static int cmd_changedirectory(int argc, slib_par_t *params, var_t *retval) {
  auto dir = get_param_str(argc, params, 0, 0);
  auto fnResult = ChangeDirectory(dir);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check collision between two bounding boxes
//
static int cmd_checkcollisionboxes(int argc, slib_par_t *params, var_t *retval) {
  auto box1 = get_param_bounding_box(argc, params, 0);
  auto box2 = get_param_bounding_box(argc, params, 1);
  auto fnResult = CheckCollisionBoxes(box1, box2);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check collision between box and sphere
//
static int cmd_checkcollisionboxsphere(int argc, slib_par_t *params, var_t *retval) {
  auto box = get_param_bounding_box(argc, params, 0);
  auto center = get_param_vec3(argc, params, 1);
  auto radius = get_param_num(argc, params, 2, 0);
  auto fnResult = CheckCollisionBoxSphere(box, center, radius);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check collision between circle and rectangle
//
static int cmd_checkcollisioncirclerec(int argc, slib_par_t *params, var_t *retval) {
  auto center = get_param_vec2(argc, params, 0);
  auto radius = get_param_num(argc, params, 1, 0);
  auto rec = get_param_rect(argc, params, 2);
  auto fnResult = CheckCollisionCircleRec(center, radius, rec);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check collision between two circles
//
static int cmd_checkcollisioncircles(int argc, slib_par_t *params, var_t *retval) {
  auto center1 = get_param_vec2(argc, params, 0);
  auto radius1 = get_param_num(argc, params, 1, 0);
  auto center2 = get_param_vec2(argc, params, 2);
  auto radius2 = get_param_num(argc, params, 3, 0);
  auto fnResult = CheckCollisionCircles(center1, radius1, center2, radius2);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check the collision between two lines defined by two points each, returns collision point by reference
//
static int cmd_checkcollisionlines(int argc, slib_par_t *params, var_t *retval) {
  auto startPos1 = get_param_vec2(argc, params, 0);
  auto endPos1 = get_param_vec2(argc, params, 1);
  auto startPos2 = get_param_vec2(argc, params, 2);
  auto endPos2 = get_param_vec2(argc, params, 3);
  auto collisionPoint = (Vector2 *)get_param_vec2_array(argc, params, 4);
  auto fnResult = CheckCollisionLines(startPos1, endPos1, startPos2, endPos2, collisionPoint);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if point is inside circle
//
static int cmd_checkcollisionpointcircle(int argc, slib_par_t *params, var_t *retval) {
  auto point = get_param_vec2(argc, params, 0);
  auto center = get_param_vec2(argc, params, 1);
  auto radius = get_param_num(argc, params, 2, 0);
  auto fnResult = CheckCollisionPointCircle(point, center, radius);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if point belongs to line created between two points [p1] and [p2] with defined margin in pixels [threshold]
//
static int cmd_checkcollisionpointline(int argc, slib_par_t *params, var_t *retval) {
  auto point = get_param_vec2(argc, params, 0);
  auto p1 = get_param_vec2(argc, params, 1);
  auto p2 = get_param_vec2(argc, params, 2);
  auto threshold = get_param_int(argc, params, 3, 0);
  auto fnResult = CheckCollisionPointLine(point, p1, p2, threshold);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if point is within a polygon described by array of vertices
//
static int cmd_checkcollisionpointpoly(int argc, slib_par_t *params, var_t *retval) {
  auto point = get_param_vec2(argc, params, 0);
  auto points = (Vector2 *)get_param_vec2_array(argc, params, 1);
  auto pointCount = get_param_int(argc, params, 2, 0);
  auto fnResult = CheckCollisionPointPoly(point, points, pointCount);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if point is inside rectangle
//
static int cmd_checkcollisionpointrec(int argc, slib_par_t *params, var_t *retval) {
  auto point = get_param_vec2(argc, params, 0);
  auto rec = get_param_rect(argc, params, 1);
  auto fnResult = CheckCollisionPointRec(point, rec);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if point is inside a triangle
//
static int cmd_checkcollisionpointtriangle(int argc, slib_par_t *params, var_t *retval) {
  auto point = get_param_vec2(argc, params, 0);
  auto p1 = get_param_vec2(argc, params, 1);
  auto p2 = get_param_vec2(argc, params, 2);
  auto p3 = get_param_vec2(argc, params, 3);
  auto fnResult = CheckCollisionPointTriangle(point, p1, p2, p3);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check collision between two rectangles
//
static int cmd_checkcollisionrecs(int argc, slib_par_t *params, var_t *retval) {
  auto rec1 = get_param_rect(argc, params, 0);
  auto rec2 = get_param_rect(argc, params, 1);
  auto fnResult = CheckCollisionRecs(rec1, rec2);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check collision between two spheres
//
static int cmd_checkcollisionspheres(int argc, slib_par_t *params, var_t *retval) {
  auto center1 = get_param_vec3(argc, params, 0);
  auto radius1 = get_param_num(argc, params, 1, 0);
  auto center2 = get_param_vec3(argc, params, 2);
  auto radius2 = get_param_num(argc, params, 3, 0);
  auto fnResult = CheckCollisionSpheres(center1, radius1, center2, radius2);
  v_setint(retval, fnResult);
  return 1;
}

//
// Encode one codepoint into UTF-8 byte array (array length returned as parameter)
//
static int cmd_codepointtoutf8(int argc, slib_par_t *params, var_t *retval) {
  auto codepoint = get_param_int(argc, params, 0, 0);
  auto utf8Size = 0;
  auto fnResult = (const char *)CodepointToUTF8(codepoint, &utf8Size);
  v_setstrn(retval, fnResult, utf8Size);
  MemFree((void *)fnResult);
  return 1;
}

//
// Get color with alpha applied, alpha goes from 0.0f to 1.0f
//
static int cmd_coloralpha(int argc, slib_par_t *params, var_t *retval) {
  auto color = get_param_color(argc, params, 0);
  auto alpha = get_param_num(argc, params, 1, 0);
  auto fnResult = ColorAlpha(color, alpha);
  v_setcolor(retval, fnResult);
  return 1;
}

//
// Get src alpha-blended into dst color with tint
//
static int cmd_coloralphablend(int argc, slib_par_t *params, var_t *retval) {
  auto dst = get_param_color(argc, params, 0);
  auto src = get_param_color(argc, params, 1);
  auto tint = get_param_color(argc, params, 2);
  auto fnResult = ColorAlphaBlend(dst, src, tint);
  v_setcolor(retval, fnResult);
  return 1;
}

//
// Get color with brightness correction, brightness factor goes from -1.0f to 1.0f
//
static int cmd_colorbrightness(int argc, slib_par_t *params, var_t *retval) {
  auto color = get_param_color(argc, params, 0);
  auto factor = get_param_num(argc, params, 1, 0);
  auto fnResult = ColorBrightness(color, factor);
  v_setcolor(retval, fnResult);
  return 1;
}

//
// Get color with contrast correction, contrast values between -1.0f and 1.0f
//
static int cmd_colorcontrast(int argc, slib_par_t *params, var_t *retval) {
  auto color = get_param_color(argc, params, 0);
  auto contrast = get_param_num(argc, params, 1, 0);
  auto fnResult = ColorContrast(color, contrast);
  v_setcolor(retval, fnResult);
  return 1;
}

//
// Get a Color from HSV values, hue [0..360], saturation/value [0..1]
//
static int cmd_colorfromhsv(int argc, slib_par_t *params, var_t *retval) {
  auto hue = get_param_num(argc, params, 0, 0);
  auto saturation = get_param_num(argc, params, 1, 0);
  auto value = get_param_num(argc, params, 2, 0);
  auto fnResult = ColorFromHSV(hue, saturation, value);
  v_setcolor(retval, fnResult);
  return 1;
}

//
// Get Color from normalized values [0..1]
//
static int cmd_colorfromnormalized(int argc, slib_par_t *params, var_t *retval) {
  auto normalized = get_param_vec4(argc, params, 0);
  auto fnResult = ColorFromNormalized(normalized);
  v_setcolor(retval, fnResult);
  return 1;
}

//
// Check if two colors are equal
//
static int cmd_colorisequal(int argc, slib_par_t *params, var_t *retval) {
  auto col1 = get_param_color(argc, params, 0);
  auto col2 = get_param_color(argc, params, 1);
  auto fnResult = ColorIsEqual(col1, col2);
  v_setint(retval, fnResult);
  return 1;
}

//
// Get Color normalized as float [0..1]
//
static int cmd_colornormalize(int argc, slib_par_t *params, var_t *retval) {
  auto color = get_param_color(argc, params, 0);
  auto fnResult = ColorNormalize(color);
  v_setvec4(retval, fnResult);
  return 1;
}

//
// Get color multiplied with another color
//
static int cmd_colortint(int argc, slib_par_t *params, var_t *retval) {
  auto color = get_param_color(argc, params, 0);
  auto tint = get_param_color(argc, params, 1);
  auto fnResult = ColorTint(color, tint);
  v_setcolor(retval, fnResult);
  return 1;
}

//
// Get HSV values for a Color, hue [0..360], saturation/value [0..1]
//
static int cmd_colortohsv(int argc, slib_par_t *params, var_t *retval) {
  auto color = get_param_color(argc, params, 0);
  auto fnResult = ColorToHSV(color);
  v_setvec3(retval, fnResult);
  return 1;
}

//
// Get hexadecimal value for a Color (0xRRGGBBAA)
//
static int cmd_colortoint(int argc, slib_par_t *params, var_t *retval) {
  auto color = get_param_color(argc, params, 0);
  auto fnResult = ColorToInt(color);
  v_setint(retval, fnResult);
  return 1;
}

//
// Compress data (DEFLATE algorithm), memory must be MemFree()
//
static int cmd_compressdata(int argc, slib_par_t *params, var_t *retval) {
  auto data = (const unsigned char *)get_param_str(argc, params, 0, 0);
  auto dataSize = get_param_int(argc, params, 1, 0);
  auto compDataSize = 0;
  auto fnResult = (const char *)CompressData(data, dataSize, &compDataSize);
  v_setstrn(retval, fnResult, compDataSize);
  MemFree((void *)fnResult);
  return 1;
}

//
// Decode Base64 string data, memory must be MemFree()
//
static int cmd_decodedatabase64(int argc, slib_par_t *params, var_t *retval) {
  auto data = (const unsigned char *)get_param_str(argc, params, 0, 0);
  auto outputSize = 0;
  auto fnResult = (const char *)DecodeDataBase64(data, &outputSize);
  v_setstrn(retval, fnResult, outputSize);
  MemFree((void *)fnResult);
  return 1;
}

//
// Decompress data (DEFLATE algorithm), memory must be MemFree()
//
static int cmd_decompressdata(int argc, slib_par_t *params, var_t *retval) {
  auto compData = (const unsigned char *)get_param_str(argc, params, 0, 0);
  auto compDataSize = get_param_int(argc, params, 1, 0);
  auto dataSize = 0;
  auto fnResult = (const char *)DecompressData(compData, compDataSize, &dataSize);
  v_setstrn(retval, fnResult, dataSize);
  MemFree((void *)fnResult);
  return 1;
}

//
// Check if a directory path exists
//
static int cmd_directoryexists(int argc, slib_par_t *params, var_t *retval) {
  auto dirPath = get_param_str(argc, params, 0, 0);
  auto fnResult = DirectoryExists(dirPath);
  v_setint(retval, fnResult);
  return 1;
}

//
// Encode data to Base64 string, memory must be MemFree()
//
static int cmd_encodedatabase64(int argc, slib_par_t *params, var_t *retval) {
  auto data = (const unsigned char *)get_param_str(argc, params, 0, 0);
  auto dataSize = get_param_int(argc, params, 1, 0);
  auto outputSize = 0;
  auto fnResult = (const char *)EncodeDataBase64(data, dataSize, &outputSize);
  v_setstrn(retval, fnResult, outputSize);
  MemFree((void *)fnResult);
  return 1;
}

//
// Export automation events list as text file
//
static int cmd_exportautomationeventlist(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int list_id = get_automationeventlist_id(argc, params, 0, retval);
  if (list_id != -1) {
    auto fileName = get_param_str(argc, params, 1, 0);
    auto fnResult = ExportAutomationEventList(_automationEventListMap.at(list_id), fileName);
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Export data to code (.h), returns true on success
//
static int cmd_exportdataascode(int argc, slib_par_t *params, var_t *retval) {
  auto data = (const unsigned char *)get_param_str(argc, params, 0, 0);
  auto dataSize = get_param_int(argc, params, 1, 0);
  auto fileName = get_param_str(argc, params, 2, 0);
  auto fnResult = ExportDataAsCode(data, dataSize, fileName);
  v_setint(retval, fnResult);
  return 1;
}

//
// Export font as code file, returns true on success
//
static int cmd_exportfontascode(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int font_id = get_font_id(argc, params, 0, retval);
  if (font_id != -1) {
    auto fileName = get_param_str(argc, params, 1, 0);
    auto fnResult = ExportFontAsCode(_fontMap.at(font_id), fileName);
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Export image data to file, returns true on success
//
static int cmd_exportimage(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto fileName = get_param_str(argc, params, 1, 0);
    auto fnResult = ExportImage(_imageMap.at(image_id), fileName);
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Export image as code file defining an array of bytes, returns true on success
//
static int cmd_exportimageascode(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto fileName = get_param_str(argc, params, 1, 0);
    auto fnResult = ExportImageAsCode(_imageMap.at(image_id), fileName);
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Export image to memory buffer
//
static int cmd_exportimagetomemory(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto fileType = get_param_str(argc, params, 1, 0);
    auto fileSize = 0;
    auto fnResult = (const char *)ExportImageToMemory(_imageMap.at(image_id), fileType, &fileSize);
  v_setstrn(retval, fnResult, fileSize);
  MemFree((void *)fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Export mesh data to file, returns true on success
//
static int cmd_exportmesh(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int mesh_id = get_mesh_id(argc, params, 0, retval);
  if (mesh_id != -1) {
    auto fileName = get_param_str(argc, params, 1, 0);
    auto fnResult = ExportMesh(_meshMap.at(mesh_id), fileName);
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Export mesh as code file (.h) defining multiple arrays of vertex attributes
//
static int cmd_exportmeshascode(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int mesh_id = get_mesh_id(argc, params, 0, retval);
  if (mesh_id != -1) {
    auto fileName = get_param_str(argc, params, 1, 0);
    auto fnResult = ExportMeshAsCode(_meshMap.at(mesh_id), fileName);
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Export wave data to file, returns true on success
//
static int cmd_exportwave(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int wave_id = get_wave_id(argc, params, 0, retval);
  if (wave_id != -1) {
    auto fileName = get_param_str(argc, params, 1, 0);
    auto fnResult = ExportWave(_waveMap.at(wave_id), fileName);
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Export wave sample data to code (.h), returns true on success
//
static int cmd_exportwaveascode(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int wave_id = get_wave_id(argc, params, 0, retval);
  if (wave_id != -1) {
    auto fileName = get_param_str(argc, params, 1, 0);
    auto fnResult = ExportWaveAsCode(_waveMap.at(wave_id), fileName);
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Get color with alpha applied, alpha goes from 0.0f to 1.0f
//
static int cmd_fade(int argc, slib_par_t *params, var_t *retval) {
  auto color = get_param_color(argc, params, 0);
  auto alpha = get_param_num(argc, params, 1, 0);
  auto fnResult = Fade(color, alpha);
  v_setcolor(retval, fnResult);
  return 1;
}

//
// Check if file exists
//
static int cmd_fileexists(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  auto fnResult = FileExists(fileName);
  v_setint(retval, fnResult);
  return 1;
}

//
// Generate image: cellular algorithm, bigger tileSize means bigger cells
//
static int cmd_genimagecellular(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  auto tileSize = get_param_int(argc, params, 2, 0);
  auto fnResult = GenImageCellular(width, height, tileSize);
  v_setimage(retval, fnResult);
  return 1;
}

//
// Generate image: checked
//
static int cmd_genimagechecked(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  auto checksX = get_param_int(argc, params, 2, 0);
  auto checksY = get_param_int(argc, params, 3, 0);
  auto col1 = get_param_color(argc, params, 4);
  auto col2 = get_param_color(argc, params, 5);
  auto fnResult = GenImageChecked(width, height, checksX, checksY, col1, col2);
  v_setimage(retval, fnResult);
  return 1;
}

//
// Generate image: plain color
//
static int cmd_genimagecolor(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  auto color = get_param_color(argc, params, 2);
  auto fnResult = GenImageColor(width, height, color);
  v_setimage(retval, fnResult);
  return 1;
}

//
// Generate image: linear gradient, direction in degrees [0..360], 0=Vertical gradient
//
static int cmd_genimagegradientlinear(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  auto direction = get_param_int(argc, params, 2, 0);
  auto start = get_param_color(argc, params, 3);
  auto end = get_param_color(argc, params, 4);
  auto fnResult = GenImageGradientLinear(width, height, direction, start, end);
  v_setimage(retval, fnResult);
  return 1;
}

//
// Generate image: radial gradient
//
static int cmd_genimagegradientradial(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  auto density = get_param_num(argc, params, 2, 0);
  auto inner = get_param_color(argc, params, 3);
  auto outer = get_param_color(argc, params, 4);
  auto fnResult = GenImageGradientRadial(width, height, density, inner, outer);
  v_setimage(retval, fnResult);
  return 1;
}

//
// Generate image: square gradient
//
static int cmd_genimagegradientsquare(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  auto density = get_param_num(argc, params, 2, 0);
  auto inner = get_param_color(argc, params, 3);
  auto outer = get_param_color(argc, params, 4);
  auto fnResult = GenImageGradientSquare(width, height, density, inner, outer);
  v_setimage(retval, fnResult);
  return 1;
}

//
// Generate image: perlin noise
//
static int cmd_genimageperlinnoise(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  auto offsetX = get_param_int(argc, params, 2, 0);
  auto offsetY = get_param_int(argc, params, 3, 0);
  auto scale = get_param_num(argc, params, 4, 0);
  auto fnResult = GenImagePerlinNoise(width, height, offsetX, offsetY, scale);
  v_setimage(retval, fnResult);
  return 1;
}

//
// Generate image: grayscale image from text data
//
static int cmd_genimagetext(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  auto text = get_param_str(argc, params, 2, 0);
  auto fnResult = GenImageText(width, height, text);
  v_setimage(retval, fnResult);
  return 1;
}

//
// Generate image: white noise
//
static int cmd_genimagewhitenoise(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  auto factor = get_param_num(argc, params, 2, 0);
  auto fnResult = GenImageWhiteNoise(width, height, factor);
  v_setimage(retval, fnResult);
  return 1;
}

//
// Generate cone/pyramid mesh
//
static int cmd_genmeshcone(int argc, slib_par_t *params, var_t *retval) {
  auto radius = get_param_num(argc, params, 0, 0);
  auto height = get_param_num(argc, params, 1, 0);
  auto slices = get_param_int(argc, params, 2, 0);
  auto fnResult = GenMeshCone(radius, height, slices);
  v_setmesh(retval, fnResult);
  return 1;
}

//
// Generate cuboid mesh
//
static int cmd_genmeshcube(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_num(argc, params, 0, 0);
  auto height = get_param_num(argc, params, 1, 0);
  auto length = get_param_num(argc, params, 2, 0);
  auto fnResult = GenMeshCube(width, height, length);
  v_setmesh(retval, fnResult);
  return 1;
}

//
// Generate cubes-based map mesh from image data
//
static int cmd_genmeshcubicmap(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int cubicmap_id = get_image_id(argc, params, 0, retval);
  if (cubicmap_id != -1) {
    auto cubeSize = get_param_vec3(argc, params, 1);
    auto fnResult = GenMeshCubicmap(_imageMap.at(cubicmap_id), cubeSize);
    v_setmesh(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Generate cylinder mesh
//
static int cmd_genmeshcylinder(int argc, slib_par_t *params, var_t *retval) {
  auto radius = get_param_num(argc, params, 0, 0);
  auto height = get_param_num(argc, params, 1, 0);
  auto slices = get_param_int(argc, params, 2, 0);
  auto fnResult = GenMeshCylinder(radius, height, slices);
  v_setmesh(retval, fnResult);
  return 1;
}

//
// Generate heightmap mesh from image data
//
static int cmd_genmeshheightmap(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int heightmap_id = get_image_id(argc, params, 0, retval);
  if (heightmap_id != -1) {
    auto size = get_param_vec3(argc, params, 1);
    auto fnResult = GenMeshHeightmap(_imageMap.at(heightmap_id), size);
    v_setmesh(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Generate half-sphere mesh (no bottom cap)
//
static int cmd_genmeshhemisphere(int argc, slib_par_t *params, var_t *retval) {
  auto radius = get_param_num(argc, params, 0, 0);
  auto rings = get_param_int(argc, params, 1, 0);
  auto slices = get_param_int(argc, params, 2, 0);
  auto fnResult = GenMeshHemiSphere(radius, rings, slices);
  v_setmesh(retval, fnResult);
  return 1;
}

//
// Generate trefoil knot mesh
//
static int cmd_genmeshknot(int argc, slib_par_t *params, var_t *retval) {
  auto radius = get_param_num(argc, params, 0, 0);
  auto size = get_param_num(argc, params, 1, 0);
  auto radSeg = get_param_int(argc, params, 2, 0);
  auto sides = get_param_int(argc, params, 3, 0);
  auto fnResult = GenMeshKnot(radius, size, radSeg, sides);
  v_setmesh(retval, fnResult);
  return 1;
}

//
// Generate plane mesh (with subdivisions)
//
static int cmd_genmeshplane(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_num(argc, params, 0, 0);
  auto length = get_param_num(argc, params, 1, 0);
  auto resX = get_param_int(argc, params, 2, 0);
  auto resZ = get_param_int(argc, params, 3, 0);
  auto fnResult = GenMeshPlane(width, length, resX, resZ);
  v_setmesh(retval, fnResult);
  return 1;
}

//
// Generate polygonal mesh
//
static int cmd_genmeshpoly(int argc, slib_par_t *params, var_t *retval) {
  auto sides = get_param_int(argc, params, 0, 0);
  auto radius = get_param_num(argc, params, 1, 0);
  auto fnResult = GenMeshPoly(sides, radius);
  v_setmesh(retval, fnResult);
  return 1;
}

//
// Generate sphere mesh (standard sphere)
//
static int cmd_genmeshsphere(int argc, slib_par_t *params, var_t *retval) {
  auto radius = get_param_num(argc, params, 0, 0);
  auto rings = get_param_int(argc, params, 1, 0);
  auto slices = get_param_int(argc, params, 2, 0);
  auto fnResult = GenMeshSphere(radius, rings, slices);
  v_setmesh(retval, fnResult);
  return 1;
}

//
// Generate torus mesh
//
static int cmd_genmeshtorus(int argc, slib_par_t *params, var_t *retval) {
  auto radius = get_param_num(argc, params, 0, 0);
  auto size = get_param_num(argc, params, 1, 0);
  auto radSeg = get_param_int(argc, params, 2, 0);
  auto sides = get_param_int(argc, params, 3, 0);
  auto fnResult = GenMeshTorus(radius, size, radSeg, sides);
  v_setmesh(retval, fnResult);
  return 1;
}

//
// Get the directory of the running application (uses static string)
//
static int cmd_getapplicationdirectory(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = (const char *)GetApplicationDirectory();
  v_setstr(retval, fnResult);
  return 1;
}

//
// Get camera transform matrix (view matrix)
//
static int cmd_getcameramatrix(int argc, slib_par_t *params, var_t *retval) {
  auto camera = get_camera_3d(argc, params, 0);
  auto fnResult = GetCameraMatrix(camera);
  v_setmatrix(retval, fnResult);
  return 1;
}

//
// Get camera 2d transform matrix
//
static int cmd_getcameramatrix2d(int argc, slib_par_t *params, var_t *retval) {
  auto camera = get_camera_2d(argc, params, 0);
  auto fnResult = GetCameraMatrix2D(camera);
  v_setmatrix(retval, fnResult);
  return 1;
}

//
// Get char pressed (unicode), call it multiple times for chars queued, returns 0 when the queue is empty
//
static int cmd_getcharpressed(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetCharPressed();
  v_setint(retval, fnResult);
  return 1;
}

//
// Get clipboard text content
//
static int cmd_getclipboardtext(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = (const char *)GetClipboardText();
  v_setstr(retval, fnResult);
  return 1;
}

//
// Get next codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure
//
static int cmd_getcodepoint(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, 0);
  auto codepointSize = (int *)0;
  auto fnResult = GetCodepoint(text, codepointSize);
  v_setint(retval, fnResult);
  return 1;
}

//
// Get total number of codepoints in a UTF-8 encoded string
//
static int cmd_getcodepointcount(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, 0);
  auto fnResult = GetCodepointCount(text);
  v_setint(retval, fnResult);
  return 1;
}

//
// Get next codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure
//
static int cmd_getcodepointnext(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, 0);
  auto codepointSize = (int *)0;
  auto fnResult = GetCodepointNext(text, codepointSize);
  v_setint(retval, fnResult);
  return 1;
}

//
// Get previous codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure
//
static int cmd_getcodepointprevious(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, 0);
  auto codepointSize = (int *)0;
  auto fnResult = GetCodepointPrevious(text, codepointSize);
  v_setint(retval, fnResult);
  return 1;
}

//
// Get collision rectangle for two rectangles collision
//
static int cmd_getcollisionrec(int argc, slib_par_t *params, var_t *retval) {
  auto rec1 = get_param_rect(argc, params, 0);
  auto rec2 = get_param_rect(argc, params, 1);
  auto fnResult = GetCollisionRec(rec1, rec2);
  v_setrect(retval, fnResult);
  return 1;
}

//
// Get Color structure from hexadecimal value
//
static int cmd_getcolor(int argc, slib_par_t *params, var_t *retval) {
  auto hexValue = get_param_int(argc, params, 0, 0);
  auto fnResult = GetColor(hexValue);
  v_setcolor(retval, fnResult);
  return 1;
}

//
// Get current connected monitor
//
static int cmd_getcurrentmonitor(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetCurrentMonitor();
  v_setint(retval, fnResult);
  return 1;
}

//
// Get full path for a given fileName with path (uses static string)
//
static int cmd_getdirectorypath(int argc, slib_par_t *params, var_t *retval) {
  auto filePath = get_param_str(argc, params, 0, 0);
  auto fnResult = (const char *)GetDirectoryPath(filePath);
  v_setstr(retval, fnResult);
  return 1;
}

//
// Get pointer to extension for a filename string (includes dot: '.png')
//
static int cmd_getfileextension(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  auto fnResult = (const char *)GetFileExtension(fileName);
  v_setstr(retval, fnResult);
  return 1;
}

//
// Get file length in bytes (NOTE: GetFileSize() conflicts with windows.h)
//
static int cmd_getfilelength(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  auto fnResult = GetFileLength(fileName);
  v_setint(retval, fnResult);
  return 1;
}

//
// Get file modification time (last write time)
//
static int cmd_getfilemodtime(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  auto fnResult = GetFileModTime(fileName);
  v_setint(retval, fnResult);
  return 1;
}

//
// Get pointer to filename for a path string
//
static int cmd_getfilename(int argc, slib_par_t *params, var_t *retval) {
  auto filePath = get_param_str(argc, params, 0, 0);
  auto fnResult = (const char *)GetFileName(filePath);
  v_setstr(retval, fnResult);
  return 1;
}

//
// Get filename string without extension (uses static string)
//
static int cmd_getfilenamewithoutext(int argc, slib_par_t *params, var_t *retval) {
  auto filePath = get_param_str(argc, params, 0, 0);
  auto fnResult = (const char *)GetFileNameWithoutExt(filePath);
  v_setstr(retval, fnResult);
  return 1;
}

//
// Get the default Font
//
static int cmd_getfontdefault(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetFontDefault();
  v_setfont(retval, fnResult);
  return 1;
}

//
// Get current FPS
//
static int cmd_getfps(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetFPS();
  v_setint(retval, fnResult);
  return 1;
}

//
// Get time in seconds for last frame drawn (delta time)
//
static int cmd_getframetime(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetFrameTime();
  v_setreal(retval, fnResult);
  return 1;
}

//
// Get gamepad axis count for a gamepad
//
static int cmd_getgamepadaxiscount(int argc, slib_par_t *params, var_t *retval) {
  auto gamepad = get_param_int(argc, params, 0, 0);
  auto fnResult = GetGamepadAxisCount(gamepad);
  v_setint(retval, fnResult);
  return 1;
}

//
// Get axis movement value for a gamepad axis
//
static int cmd_getgamepadaxismovement(int argc, slib_par_t *params, var_t *retval) {
  auto gamepad = get_param_int(argc, params, 0, 0);
  auto axis = get_param_int(argc, params, 1, 0);
  auto fnResult = GetGamepadAxisMovement(gamepad, axis);
  v_setreal(retval, fnResult);
  return 1;
}

//
// Get the last gamepad button pressed
//
static int cmd_getgamepadbuttonpressed(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetGamepadButtonPressed();
  v_setint(retval, fnResult);
  return 1;
}

//
// Get gamepad internal name id
//
static int cmd_getgamepadname(int argc, slib_par_t *params, var_t *retval) {
  auto gamepad = get_param_int(argc, params, 0, 0);
  auto fnResult = (const char *)GetGamepadName(gamepad);
  v_setstr(retval, fnResult);
  return 1;
}

//
// Get latest detected gesture
//
static int cmd_getgesturedetected(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetGestureDetected();
  v_setint(retval, fnResult);
  return 1;
}

//
// Get gesture drag angle
//
static int cmd_getgesturedragangle(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetGestureDragAngle();
  v_setreal(retval, fnResult);
  return 1;
}

//
// Get gesture drag vector
//
static int cmd_getgesturedragvector(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetGestureDragVector();
  v_setvec2(retval, fnResult);
  return 1;
}

//
// Get gesture hold time in milliseconds
//
static int cmd_getgestureholdduration(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetGestureHoldDuration();
  v_setreal(retval, fnResult);
  return 1;
}

//
// Get gesture pinch angle
//
static int cmd_getgesturepinchangle(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetGesturePinchAngle();
  v_setreal(retval, fnResult);
  return 1;
}

//
// Get gesture pinch delta
//
static int cmd_getgesturepinchvector(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetGesturePinchVector();
  v_setvec2(retval, fnResult);
  return 1;
}

//
// Get glyph rectangle in font atlas for a codepoint (unicode character), fallback to '?' if not found
//
static int cmd_getglyphatlasrec(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int font_id = get_font_id(argc, params, 0, retval);
  if (font_id != -1) {
    auto codepoint = get_param_int(argc, params, 1, 0);
    auto fnResult = GetGlyphAtlasRec(_fontMap.at(font_id), codepoint);
    v_setrect(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Get glyph index position in font for a codepoint (unicode character), fallback to '?' if not found
//
static int cmd_getglyphindex(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int font_id = get_font_id(argc, params, 0, retval);
  if (font_id != -1) {
    auto codepoint = get_param_int(argc, params, 1, 0);
    auto fnResult = GetGlyphIndex(_fontMap.at(font_id), codepoint);
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Get image alpha border rectangle
//
static int cmd_getimagealphaborder(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto threshold = get_param_num(argc, params, 1, 0);
    auto fnResult = GetImageAlphaBorder(_imageMap.at(image_id), threshold);
    v_setrect(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Get image pixel color at (x, y) position
//
static int cmd_getimagecolor(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto x = get_param_int(argc, params, 1, 0);
    auto y = get_param_int(argc, params, 2, 0);
    auto fnResult = GetImageColor(_imageMap.at(image_id), x, y);
    v_setcolor(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Get key pressed (keycode), call it multiple times for keys queued, returns 0 when the queue is empty
//
static int cmd_getkeypressed(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetKeyPressed();
  v_setint(retval, fnResult);
  return 1;
}

//
// Get master volume (listener)
//
static int cmd_getmastervolume(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetMasterVolume();
  v_setreal(retval, fnResult);
  return 1;
}

//
// Compute model bounding box limits (considers all meshes)
//
static int cmd_getmodelboundingbox(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int model_id = get_model_id(argc, params, 0, retval);
  if (model_id != -1) {
    auto fnResult = GetModelBoundingBox(_modelMap.at(model_id));
    v_setboundingbox(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Get number of connected monitors
//
static int cmd_getmonitorcount(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetMonitorCount();
  v_setint(retval, fnResult);
  return 1;
}

//
// Get specified monitor height (current video mode used by monitor)
//
static int cmd_getmonitorheight(int argc, slib_par_t *params, var_t *retval) {
  auto monitor = get_param_int(argc, params, 0, 0);
  auto fnResult = GetMonitorHeight(monitor);
  v_setint(retval, fnResult);
  return 1;
}

//
// Get the human-readable, UTF-8 encoded name of the specified monitor
//
static int cmd_getmonitorname(int argc, slib_par_t *params, var_t *retval) {
  auto monitor = get_param_int(argc, params, 0, 0);
  auto fnResult = (const char *)GetMonitorName(monitor);
  v_setstr(retval, fnResult);
  return 1;
}

//
// Get specified monitor physical height in millimetres
//
static int cmd_getmonitorphysicalheight(int argc, slib_par_t *params, var_t *retval) {
  auto monitor = get_param_int(argc, params, 0, 0);
  auto fnResult = GetMonitorPhysicalHeight(monitor);
  v_setint(retval, fnResult);
  return 1;
}

//
// Get specified monitor physical width in millimetres
//
static int cmd_getmonitorphysicalwidth(int argc, slib_par_t *params, var_t *retval) {
  auto monitor = get_param_int(argc, params, 0, 0);
  auto fnResult = GetMonitorPhysicalWidth(monitor);
  v_setint(retval, fnResult);
  return 1;
}

//
// Get specified monitor position
//
static int cmd_getmonitorposition(int argc, slib_par_t *params, var_t *retval) {
  auto monitor = get_param_int(argc, params, 0, 0);
  auto fnResult = GetMonitorPosition(monitor);
  v_setvec2(retval, fnResult);
  return 1;
}

//
// Get specified monitor refresh rate
//
static int cmd_getmonitorrefreshrate(int argc, slib_par_t *params, var_t *retval) {
  auto monitor = get_param_int(argc, params, 0, 0);
  auto fnResult = GetMonitorRefreshRate(monitor);
  v_setint(retval, fnResult);
  return 1;
}

//
// Get specified monitor width (current video mode used by monitor)
//
static int cmd_getmonitorwidth(int argc, slib_par_t *params, var_t *retval) {
  auto monitor = get_param_int(argc, params, 0, 0);
  auto fnResult = GetMonitorWidth(monitor);
  v_setint(retval, fnResult);
  return 1;
}

//
// Get mouse delta between frames
//
static int cmd_getmousedelta(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetMouseDelta();
  v_setvec2(retval, fnResult);
  return 1;
}

//
// Get mouse position XY
//
static int cmd_getmouseposition(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetMousePosition();
  v_setvec2(retval, fnResult);
  return 1;
}

//
// Get mouse wheel movement for X or Y, whichever is larger
//
static int cmd_getmousewheelmove(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetMouseWheelMove();
  v_setreal(retval, fnResult);
  return 1;
}

//
// Get mouse wheel movement for both X and Y
//
static int cmd_getmousewheelmovev(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetMouseWheelMoveV();
  v_setvec2(retval, fnResult);
  return 1;
}

//
// Get mouse position X
//
static int cmd_getmousex(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetMouseX();
  v_setint(retval, fnResult);
  return 1;
}

//
// Get mouse position Y
//
static int cmd_getmousey(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetMouseY();
  v_setint(retval, fnResult);
  return 1;
}

//
// Get music time length (in seconds)
//
static int cmd_getmusictimelength(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int music_id = get_music_id(argc, params, 0, retval);
  if (music_id != -1) {
    auto fnResult = GetMusicTimeLength(_musicMap.at(music_id));
    v_setreal(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Get current music time played (in seconds)
//
static int cmd_getmusictimeplayed(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int music_id = get_music_id(argc, params, 0, retval);
  if (music_id != -1) {
    auto fnResult = GetMusicTimePlayed(_musicMap.at(music_id));
    v_setreal(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Get Color from a source pixel pointer of certain format
//
static int cmd_getpixelcolor(int argc, slib_par_t *params, var_t *retval) {
  auto srcPtr = (void *)get_param_int_t(argc, params, 0, 0);
  auto format = get_param_int(argc, params, 1, 0);
  auto fnResult = GetPixelColor(srcPtr, format);
  v_setcolor(retval, fnResult);
  return 1;
}

//
// Get pixel data size in bytes for certain format
//
static int cmd_getpixeldatasize(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  auto format = get_param_int(argc, params, 2, 0);
  auto fnResult = GetPixelDataSize(width, height, format);
  v_setint(retval, fnResult);
  return 1;
}

//
// Get previous directory path for a given path (uses static string)
//
static int cmd_getprevdirectorypath(int argc, slib_par_t *params, var_t *retval) {
  auto dirPath = get_param_str(argc, params, 0, 0);
  auto fnResult = (const char *)GetPrevDirectoryPath(dirPath);
  v_setstr(retval, fnResult);
  return 1;
}

//
// Get a random value between min and max (both included)
//
static int cmd_getrandomvalue(int argc, slib_par_t *params, var_t *retval) {
  auto min = get_param_int(argc, params, 0, 0);
  auto max = get_param_int(argc, params, 1, 0);
  auto fnResult = GetRandomValue(min, max);
  v_setint(retval, fnResult);
  return 1;
}

//
// Get collision info between ray and box
//
static int cmd_getraycollisionbox(int argc, slib_par_t *params, var_t *retval) {
  auto ray = get_param_ray(argc, params, 0);
  auto box = get_param_bounding_box(argc, params, 1);
  auto fnResult = GetRayCollisionBox(ray, box);
  v_setraycollision(retval, fnResult);
  return 1;
}

//
// Get collision info between ray and mesh
//
static int cmd_getraycollisionmesh(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int mesh_id = get_mesh_id(argc, params, 1, retval);
  int transform_id = get_matrix_id(argc, params, 2, retval);
  if (mesh_id != -1 && transform_id != -1) {
    auto ray = get_param_ray(argc, params, 0);
    auto fnResult = GetRayCollisionMesh(ray, _meshMap.at(mesh_id), _matrixMap.at(transform_id));
    v_setraycollision(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Get collision info between ray and quad
//
static int cmd_getraycollisionquad(int argc, slib_par_t *params, var_t *retval) {
  auto ray = get_param_ray(argc, params, 0);
  auto p1 = get_param_vec3(argc, params, 1);
  auto p2 = get_param_vec3(argc, params, 2);
  auto p3 = get_param_vec3(argc, params, 3);
  auto p4 = get_param_vec3(argc, params, 4);
  auto fnResult = GetRayCollisionQuad(ray, p1, p2, p3, p4);
  v_setraycollision(retval, fnResult);
  return 1;
}

//
// Get collision info between ray and sphere
//
static int cmd_getraycollisionsphere(int argc, slib_par_t *params, var_t *retval) {
  auto ray = get_param_ray(argc, params, 0);
  auto center = get_param_vec3(argc, params, 1);
  auto radius = get_param_num(argc, params, 2, 0);
  auto fnResult = GetRayCollisionSphere(ray, center, radius);
  v_setraycollision(retval, fnResult);
  return 1;
}

//
// Get collision info between ray and triangle
//
static int cmd_getraycollisiontriangle(int argc, slib_par_t *params, var_t *retval) {
  auto ray = get_param_ray(argc, params, 0);
  auto p1 = get_param_vec3(argc, params, 1);
  auto p2 = get_param_vec3(argc, params, 2);
  auto p3 = get_param_vec3(argc, params, 3);
  auto fnResult = GetRayCollisionTriangle(ray, p1, p2, p3);
  v_setraycollision(retval, fnResult);
  return 1;
}

//
// Get current render height (it considers HiDPI)
//
static int cmd_getrenderheight(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetRenderHeight();
  v_setint(retval, fnResult);
  return 1;
}

//
// Get current render width (it considers HiDPI)
//
static int cmd_getrenderwidth(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetRenderWidth();
  v_setint(retval, fnResult);
  return 1;
}

//
// Get current screen height
//
static int cmd_getscreenheight(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetScreenHeight();
  v_setint(retval, fnResult);
  return 1;
}

//
// Get the world space position for a 2d camera screen space position
//
static int cmd_getscreentoworld2d(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec2(argc, params, 0);
  auto camera = get_camera_2d(argc, params, 1);
  auto fnResult = GetScreenToWorld2D(position, camera);
  v_setvec2(retval, fnResult);
  return 1;
}

//
// Get a ray trace from screen position (i.e mouse)
//
static int cmd_getscreentoworldray(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec2(argc, params, 0);
  auto camera = get_camera_3d(argc, params, 1);
  auto fnResult = GetScreenToWorldRay(position, camera);
  v_setray(retval, fnResult);
  return 1;
}

//
// Get a ray trace from screen position (i.e mouse) in a viewport
//
static int cmd_getscreentoworldrayex(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec2(argc, params, 0);
  auto camera = get_camera_3d(argc, params, 1);
  auto width = get_param_int(argc, params, 2, 0);
  auto height = get_param_int(argc, params, 3, 0);
  auto fnResult = GetScreenToWorldRayEx(position, camera, width, height);
  v_setray(retval, fnResult);
  return 1;
}

//
// Get current screen width
//
static int cmd_getscreenwidth(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetScreenWidth();
  v_setint(retval, fnResult);
  return 1;
}

//
// Get shader uniform location
//
static int cmd_getshaderlocation(int argc, slib_par_t *params, var_t *retval) {
  auto shader = get_param_shader(argc, params, 0);
  auto uniformName = get_param_str(argc, params, 1, 0);
  auto fnResult = GetShaderLocation(shader, uniformName);
  v_setint(retval, fnResult);
  return 1;
}

//
// Get shader attribute location
//
static int cmd_getshaderlocationattrib(int argc, slib_par_t *params, var_t *retval) {
  auto shader = get_param_shader(argc, params, 0);
  auto attribName = get_param_str(argc, params, 1, 0);
  auto fnResult = GetShaderLocationAttrib(shader, attribName);
  v_setint(retval, fnResult);
  return 1;
}

//
// Get texture that is used for shapes drawing
//
static int cmd_getshapestexture(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetShapesTexture();
  v_settexture2d(retval, fnResult);
  return 1;
}

//
// Get texture source rectangle that is used for shapes drawing
//
static int cmd_getshapestexturerectangle(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetShapesTextureRectangle();
  v_setrect(retval, fnResult);
  return 1;
}

//
// Get (evaluate) spline point: B-Spline
//
static int cmd_getsplinepointbasis(int argc, slib_par_t *params, var_t *retval) {
  auto p1 = get_param_vec2(argc, params, 0);
  auto p2 = get_param_vec2(argc, params, 1);
  auto p3 = get_param_vec2(argc, params, 2);
  auto p4 = get_param_vec2(argc, params, 3);
  auto t = get_param_num(argc, params, 4, 0);
  auto fnResult = GetSplinePointBasis(p1, p2, p3, p4, t);
  v_setvec2(retval, fnResult);
  return 1;
}

//
// Get (evaluate) spline point: Cubic Bezier
//
static int cmd_getsplinepointbeziercubic(int argc, slib_par_t *params, var_t *retval) {
  auto p1 = get_param_vec2(argc, params, 0);
  auto c2 = get_param_vec2(argc, params, 1);
  auto c3 = get_param_vec2(argc, params, 2);
  auto p4 = get_param_vec2(argc, params, 3);
  auto t = get_param_num(argc, params, 4, 0);
  auto fnResult = GetSplinePointBezierCubic(p1, c2, c3, p4, t);
  v_setvec2(retval, fnResult);
  return 1;
}

//
// Get (evaluate) spline point: Quadratic Bezier
//
static int cmd_getsplinepointbezierquad(int argc, slib_par_t *params, var_t *retval) {
  auto p1 = get_param_vec2(argc, params, 0);
  auto c2 = get_param_vec2(argc, params, 1);
  auto p3 = get_param_vec2(argc, params, 2);
  auto t = get_param_num(argc, params, 3, 0);
  auto fnResult = GetSplinePointBezierQuad(p1, c2, p3, t);
  v_setvec2(retval, fnResult);
  return 1;
}

//
// Get (evaluate) spline point: Catmull-Rom
//
static int cmd_getsplinepointcatmullrom(int argc, slib_par_t *params, var_t *retval) {
  auto p1 = get_param_vec2(argc, params, 0);
  auto p2 = get_param_vec2(argc, params, 1);
  auto p3 = get_param_vec2(argc, params, 2);
  auto p4 = get_param_vec2(argc, params, 3);
  auto t = get_param_num(argc, params, 4, 0);
  auto fnResult = GetSplinePointCatmullRom(p1, p2, p3, p4, t);
  v_setvec2(retval, fnResult);
  return 1;
}

//
// Get (evaluate) spline point: Linear
//
static int cmd_getsplinepointlinear(int argc, slib_par_t *params, var_t *retval) {
  auto startPos = get_param_vec2(argc, params, 0);
  auto endPos = get_param_vec2(argc, params, 1);
  auto t = get_param_num(argc, params, 2, 0);
  auto fnResult = GetSplinePointLinear(startPos, endPos, t);
  v_setvec2(retval, fnResult);
  return 1;
}

//
// Get elapsed time in seconds since InitWindow()
//
static int cmd_gettime(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetTime();
  v_setreal(retval, fnResult);
  return 1;
}

//
// Get number of touch points
//
static int cmd_gettouchpointcount(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetTouchPointCount();
  v_setint(retval, fnResult);
  return 1;
}

//
// Get touch point identifier for given index
//
static int cmd_gettouchpointid(int argc, slib_par_t *params, var_t *retval) {
  auto index = get_param_int(argc, params, 0, 0);
  auto fnResult = GetTouchPointId(index);
  v_setint(retval, fnResult);
  return 1;
}

//
// Get touch position XY for a touch point index (relative to screen size)
//
static int cmd_gettouchposition(int argc, slib_par_t *params, var_t *retval) {
  auto index = get_param_int(argc, params, 0, 0);
  auto fnResult = GetTouchPosition(index);
  v_setvec2(retval, fnResult);
  return 1;
}

//
// Get touch position X for touch point 0 (relative to screen size)
//
static int cmd_gettouchx(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetTouchX();
  v_setint(retval, fnResult);
  return 1;
}

//
// Get touch position Y for touch point 0 (relative to screen size)
//
static int cmd_gettouchy(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetTouchY();
  v_setint(retval, fnResult);
  return 1;
}

//
// Get native window handle
//
static int cmd_getwindowhandle(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = (var_int_t)GetWindowHandle();
  v_setint(retval, fnResult);
  return 1;
}

//
// Get window position XY on monitor
//
static int cmd_getwindowposition(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetWindowPosition();
  v_setvec2(retval, fnResult);
  return 1;
}

//
// Get window scale DPI factor
//
static int cmd_getwindowscaledpi(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = GetWindowScaleDPI();
  v_setvec2(retval, fnResult);
  return 1;
}

//
// Get current working directory (uses static string)
//
static int cmd_getworkingdirectory(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = (const char *)GetWorkingDirectory();
  v_setstr(retval, fnResult);
  return 1;
}

//
// Get the screen space position for a 3d world space position
//
static int cmd_getworldtoscreen(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec3(argc, params, 0);
  auto camera = get_camera_3d(argc, params, 1);
  auto fnResult = GetWorldToScreen(position, camera);
  v_setvec2(retval, fnResult);
  return 1;
}

//
// Get the screen space position for a 2d camera world space position
//
static int cmd_getworldtoscreen2d(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec2(argc, params, 0);
  auto camera = get_camera_2d(argc, params, 1);
  auto fnResult = GetWorldToScreen2D(position, camera);
  v_setvec2(retval, fnResult);
  return 1;
}

//
// Get size position for a 3d world space position
//
static int cmd_getworldtoscreenex(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec3(argc, params, 0);
  auto camera = get_camera_3d(argc, params, 1);
  auto width = get_param_int(argc, params, 2, 0);
  auto height = get_param_int(argc, params, 3, 0);
  auto fnResult = GetWorldToScreenEx(position, camera, width, height);
  v_setvec2(retval, fnResult);
  return 1;
}

//
// Create an image duplicate (useful for transformations)
//
static int cmd_imagecopy(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto fnResult = ImageCopy(_imageMap.at(image_id));
    v_setimage(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Create an image from another image piece
//
static int cmd_imagefromimage(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto rec = get_param_rect(argc, params, 1);
    auto fnResult = ImageFromImage(_imageMap.at(image_id), rec);
    v_setimage(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Create an image from text (default font)
//
static int cmd_imagetext(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, 0);
  auto fontSize = get_param_int(argc, params, 1, 0);
  auto color = get_param_color(argc, params, 2);
  auto fnResult = ImageText(text, fontSize, color);
  v_setimage(retval, fnResult);
  return 1;
}

//
// Create an image from text (custom sprite font)
//
static int cmd_imagetextex(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int font_id = get_font_id(argc, params, 0, retval);
  if (font_id != -1) {
    auto text = get_param_str(argc, params, 1, 0);
    auto fontSize = get_param_num(argc, params, 2, 0);
    auto spacing = get_param_num(argc, params, 3, 0);
    auto tint = get_param_color(argc, params, 4);
    auto fnResult = ImageTextEx(_fontMap.at(font_id), text, fontSize, spacing, tint);
    v_setimage(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Check if audio device has been initialized successfully
//
static int cmd_isaudiodeviceready(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsAudioDeviceReady();
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if audio stream is playing
//
static int cmd_isaudiostreamplaying(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int stream_id = get_audiostream_id(argc, params, 0, retval);
  if (stream_id != -1) {
    auto fnResult = IsAudioStreamPlaying(_audioStream.at(stream_id));
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Check if any audio stream buffers requires refill
//
static int cmd_isaudiostreamprocessed(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int stream_id = get_audiostream_id(argc, params, 0, retval);
  if (stream_id != -1) {
    auto fnResult = IsAudioStreamProcessed(_audioStream.at(stream_id));
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Checks if an audio stream is ready
//
static int cmd_isaudiostreamready(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int stream_id = get_audiostream_id(argc, params, 0, retval);
  if (stream_id != -1) {
    auto fnResult = IsAudioStreamReady(_audioStream.at(stream_id));
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Check if cursor is not visible
//
static int cmd_iscursorhidden(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsCursorHidden();
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if cursor is on the screen
//
static int cmd_iscursoronscreen(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsCursorOnScreen();
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if a file has been dropped into window
//
static int cmd_isfiledropped(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsFileDropped();
  v_setint(retval, fnResult);
  return 1;
}

//
// Check file extension (including point: .png, .wav)
//
static int cmd_isfileextension(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  auto ext = get_param_str(argc, params, 1, 0);
  auto fnResult = IsFileExtension(fileName, ext);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if a font is ready
//
static int cmd_isfontready(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int font_id = get_font_id(argc, params, 0, retval);
  if (font_id != -1) {
    auto fnResult = IsFontReady(_fontMap.at(font_id));
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Check if a gamepad is available
//
static int cmd_isgamepadavailable(int argc, slib_par_t *params, var_t *retval) {
  auto gamepad = get_param_int(argc, params, 0, 0);
  auto fnResult = IsGamepadAvailable(gamepad);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if a gamepad button is being pressed
//
static int cmd_isgamepadbuttondown(int argc, slib_par_t *params, var_t *retval) {
  auto gamepad = get_param_int(argc, params, 0, 0);
  auto button = get_param_int(argc, params, 1, 0);
  auto fnResult = IsGamepadButtonDown(gamepad, button);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if a gamepad button has been pressed once
//
static int cmd_isgamepadbuttonpressed(int argc, slib_par_t *params, var_t *retval) {
  auto gamepad = get_param_int(argc, params, 0, 0);
  auto button = get_param_int(argc, params, 1, 0);
  auto fnResult = IsGamepadButtonPressed(gamepad, button);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if a gamepad button has been released once
//
static int cmd_isgamepadbuttonreleased(int argc, slib_par_t *params, var_t *retval) {
  auto gamepad = get_param_int(argc, params, 0, 0);
  auto button = get_param_int(argc, params, 1, 0);
  auto fnResult = IsGamepadButtonReleased(gamepad, button);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if a gamepad button is NOT being pressed
//
static int cmd_isgamepadbuttonup(int argc, slib_par_t *params, var_t *retval) {
  auto gamepad = get_param_int(argc, params, 0, 0);
  auto button = get_param_int(argc, params, 1, 0);
  auto fnResult = IsGamepadButtonUp(gamepad, button);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if a gesture have been detected
//
static int cmd_isgesturedetected(int argc, slib_par_t *params, var_t *retval) {
  auto gesture = get_param_int(argc, params, 0, 0);
  auto fnResult = IsGestureDetected(gesture);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if an image is ready
//
static int cmd_isimageready(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto fnResult = IsImageReady(_imageMap.at(image_id));
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Check if a key is being pressed
//
static int cmd_iskeydown(int argc, slib_par_t *params, var_t *retval) {
  auto key = get_param_int(argc, params, 0, 0);
  auto fnResult = IsKeyDown(key);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if a key has been pressed once
//
static int cmd_iskeypressed(int argc, slib_par_t *params, var_t *retval) {
  auto key = get_param_int(argc, params, 0, 0);
  auto fnResult = IsKeyPressed(key);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if a key has been pressed again (Only PLATFORM_DESKTOP)
//
static int cmd_iskeypressedrepeat(int argc, slib_par_t *params, var_t *retval) {
  auto key = get_param_int(argc, params, 0, 0);
  auto fnResult = IsKeyPressedRepeat(key);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if a key has been released once
//
static int cmd_iskeyreleased(int argc, slib_par_t *params, var_t *retval) {
  auto key = get_param_int(argc, params, 0, 0);
  auto fnResult = IsKeyReleased(key);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if a key is NOT being pressed
//
static int cmd_iskeyup(int argc, slib_par_t *params, var_t *retval) {
  auto key = get_param_int(argc, params, 0, 0);
  auto fnResult = IsKeyUp(key);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check model animation skeleton match
//
static int cmd_ismodelanimationvalid(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int model_id = get_model_id(argc, params, 0, retval);
  int anim_id = get_model_animation_id(argc, params, 1, retval);
  if (model_id != -1 && anim_id != -1) {
    auto fnResult = IsModelAnimationValid(_modelMap.at(model_id), _modelAnimationMap.at(anim_id));
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Check if a model is ready
//
static int cmd_ismodelready(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int model_id = get_model_id(argc, params, 0, retval);
  if (model_id != -1) {
    auto fnResult = IsModelReady(_modelMap.at(model_id));
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Check if a mouse button is being pressed
//
static int cmd_ismousebuttondown(int argc, slib_par_t *params, var_t *retval) {
  auto button = get_param_int(argc, params, 0, 0);
  auto fnResult = IsMouseButtonDown(button);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if a mouse button has been pressed once
//
static int cmd_ismousebuttonpressed(int argc, slib_par_t *params, var_t *retval) {
  auto button = get_param_int(argc, params, 0, 0);
  auto fnResult = IsMouseButtonPressed(button);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if a mouse button has been released once
//
static int cmd_ismousebuttonreleased(int argc, slib_par_t *params, var_t *retval) {
  auto button = get_param_int(argc, params, 0, 0);
  auto fnResult = IsMouseButtonReleased(button);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if a mouse button is NOT being pressed
//
static int cmd_ismousebuttonup(int argc, slib_par_t *params, var_t *retval) {
  auto button = get_param_int(argc, params, 0, 0);
  auto fnResult = IsMouseButtonUp(button);
  v_setint(retval, fnResult);
  return 1;
}

//
// Checks if a music stream is ready
//
static int cmd_ismusicready(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int music_id = get_music_id(argc, params, 0, retval);
  if (music_id != -1) {
    auto fnResult = IsMusicReady(_musicMap.at(music_id));
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Check if music is playing
//
static int cmd_ismusicstreamplaying(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int music_id = get_music_id(argc, params, 0, retval);
  if (music_id != -1) {
    auto fnResult = IsMusicStreamPlaying(_musicMap.at(music_id));
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Check if a given path is a file or a directory
//
static int cmd_ispathfile(int argc, slib_par_t *params, var_t *retval) {
  auto path = get_param_str(argc, params, 0, 0);
  auto fnResult = IsPathFile(path);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if a render texture is ready
//
static int cmd_isrendertextureready(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int target_id = get_render_texture_id(argc, params, 0, retval);
  if (target_id != -1) {
    auto fnResult = IsRenderTextureReady(_renderMap.at(target_id));
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Check if a shader is ready
//
static int cmd_isshaderready(int argc, slib_par_t *params, var_t *retval) {
  auto shader = get_param_shader(argc, params, 0);
  auto fnResult = IsShaderReady(shader);
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if a sound is currently playing
//
static int cmd_issoundplaying(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int sound_id = get_sound_id(argc, params, 0, retval);
  if (sound_id != -1) {
    auto fnResult = IsSoundPlaying(_soundMap.at(sound_id));
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Checks if a sound is ready
//
static int cmd_issoundready(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int sound_id = get_sound_id(argc, params, 0, retval);
  if (sound_id != -1) {
    auto fnResult = IsSoundReady(_soundMap.at(sound_id));
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Check if a texture is ready
//
static int cmd_istextureready(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int texture_id = get_texture_id(argc, params, 0, retval);
  if (texture_id != -1) {
    auto fnResult = IsTextureReady(_textureMap.at(texture_id));
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Checks if wave data is ready
//
static int cmd_iswaveready(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int wave_id = get_wave_id(argc, params, 0, retval);
  if (wave_id != -1) {
    auto fnResult = IsWaveReady(_waveMap.at(wave_id));
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Check if window is currently focused (only PLATFORM_DESKTOP)
//
static int cmd_iswindowfocused(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsWindowFocused();
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if window is currently fullscreen
//
static int cmd_iswindowfullscreen(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsWindowFullscreen();
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if window is currently hidden (only PLATFORM_DESKTOP)
//
static int cmd_iswindowhidden(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsWindowHidden();
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if window is currently maximized (only PLATFORM_DESKTOP)
//
static int cmd_iswindowmaximized(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsWindowMaximized();
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if window is currently minimized (only PLATFORM_DESKTOP)
//
static int cmd_iswindowminimized(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsWindowMinimized();
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if window has been initialized successfully
//
static int cmd_iswindowready(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsWindowReady();
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if window has been resized last frame
//
static int cmd_iswindowresized(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = IsWindowResized();
  v_setint(retval, fnResult);
  return 1;
}

//
// Check if one specific window flag is enabled
//
static int cmd_iswindowstate(int argc, slib_par_t *params, var_t *retval) {
  auto flag = get_param_int(argc, params, 0, 0);
  auto fnResult = IsWindowState(flag);
  v_setint(retval, fnResult);
  return 1;
}

//
// Load audio stream (to stream raw audio pcm data)
//
static int cmd_loadaudiostream(int argc, slib_par_t *params, var_t *retval) {
  auto sampleRate = get_param_int(argc, params, 0, 0);
  auto sampleSize = get_param_int(argc, params, 1, 0);
  auto channels = get_param_int(argc, params, 2, 0);
  auto fnResult = LoadAudioStream(sampleRate, sampleSize, channels);
  v_setaudiostream(retval, fnResult);
  return 1;
}

//
// Load automation events list from file, NULL for empty list, capacity = MAX_AUTOMATION_EVENTS
//
static int cmd_loadautomationeventlist(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  auto fnResult = LoadAutomationEventList(fileName);
  v_setautomationeventlist(retval, fnResult);
  return 1;
}

//
// Load all codepoints from a UTF-8 text string, codepoints count returned by parameter
//
static int cmd_loadcodepoints(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, 0);
  auto count = (int *)0;
  auto fnResult = (var_int_t)LoadCodepoints(text, count);
  v_setint(retval, fnResult);
  return 1;
}

//
// Load directory filepaths
//
static int cmd_loaddirectoryfiles(int argc, slib_par_t *params, var_t *retval) {
  auto dirPath = get_param_str(argc, params, 0, 0);
  auto fnResult = LoadDirectoryFiles(dirPath);
  v_setfilepathlist(retval, fnResult);
  return 1;
}

//
// Load directory filepaths with extension filtering and recursive directory scan
//
static int cmd_loaddirectoryfilesex(int argc, slib_par_t *params, var_t *retval) {
  auto basePath = get_param_str(argc, params, 0, 0);
  auto filter = get_param_str(argc, params, 1, 0);
  auto scanSubdirs = get_param_int(argc, params, 2, 0);
  auto fnResult = LoadDirectoryFilesEx(basePath, filter, scanSubdirs);
  v_setfilepathlist(retval, fnResult);
  return 1;
}

//
// Load dropped filepaths
//
static int cmd_loaddroppedfiles(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = LoadDroppedFiles();
  v_setfilepathlist(retval, fnResult);
  UnloadDroppedFiles(fnResult);
  return 1;
}

//
// Load file data as byte array (read)
//
static int cmd_loadfiledata(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  auto dataSize = 0;
  auto fnResult = (const char *)LoadFileData(fileName, &dataSize);
  v_setstrn(retval, fnResult, dataSize);
  MemFree((void *)fnResult);
  return 1;
}

//
// Load text data from file (read), returns a '\\0' terminated string
//
static int cmd_loadfiletext(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  auto fnResult = (const char *)LoadFileText(fileName);
  v_setstr(retval, fnResult);
  return 1;
}

//
// Load font from file into GPU memory (VRAM)
//
static int cmd_loadfont(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  auto fnResult = LoadFont(fileName);
  v_setfont(retval, fnResult);
  return 1;
}

//
// Load font from file with extended parameters, use NULL for codepoints and 0 for codepointCount to load the default character setFont
//
static int cmd_loadfontex(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  auto fontSize = get_param_int(argc, params, 1, 0);
  auto codepoints = (int *)0;
  auto codepointCount = get_param_int(argc, params, 2, 0);
  auto fnResult = LoadFontEx(fileName, fontSize, codepoints, codepointCount);
  v_setfont(retval, fnResult);
  return 1;
}

//
// Load font from Image (XNA style)
//
static int cmd_loadfontfromimage(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto key = get_param_color(argc, params, 1);
    auto firstChar = get_param_int(argc, params, 2, 0);
    auto fnResult = LoadFontFromImage(_imageMap.at(image_id), key, firstChar);
    v_setfont(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Load font from memory buffer, fileType refers to extension: i.e. '.ttf'
//
static int cmd_loadfontfrommemory(int argc, slib_par_t *params, var_t *retval) {
  auto fileType = get_param_str(argc, params, 0, 0);
  auto fileData = (const unsigned char *)get_param_str(argc, params, 1, 0);
  auto dataSize = get_param_int(argc, params, 2, 0);
  auto fontSize = get_param_int(argc, params, 3, 0);
  auto codepoints = (int *)0;
  auto codepointCount = get_param_int(argc, params, 4, 0);
  auto fnResult = LoadFontFromMemory(fileType, fileData, dataSize, fontSize, codepoints, codepointCount);
  v_setfont(retval, fnResult);
  return 1;
}

//
// Load image from file into CPU memory (RAM)
//
static int cmd_loadimage(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  auto fnResult = LoadImage(fileName);
  v_setimage(retval, fnResult);
  return 1;
}

//
// Load image sequence from file (frames appended to image.data)
//
static int cmd_loadimageanim(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  auto frames = (int *)0;
  auto fnResult = LoadImageAnim(fileName, frames);
  v_setimage(retval, fnResult);
  return 1;
}

//
// Load image sequence from memory buffer
//
static int cmd_loadimageanimfrommemory(int argc, slib_par_t *params, var_t *retval) {
  auto fileType = get_param_str(argc, params, 0, 0);
  auto fileData = (const unsigned char *)get_param_str(argc, params, 1, 0);
  auto dataSize = get_param_int(argc, params, 2, 0);
  auto frames = (int *)0;
  auto fnResult = LoadImageAnimFromMemory(fileType, fileData, dataSize, frames);
  v_setimage(retval, fnResult);
  return 1;
}

//
// Load color data from image as a Color array (RGBA - 32bit)
//
static int cmd_loadimagecolors(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto fnResult = (var_int_t)LoadImageColors(_imageMap.at(image_id));
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Load image from memory buffer, fileType refers to extension: i.e. '.png'
//
static int cmd_loadimagefrommemory(int argc, slib_par_t *params, var_t *retval) {
  auto fileType = get_param_str(argc, params, 0, 0);
  auto fileData = (const unsigned char *)get_param_str(argc, params, 1, 0);
  auto dataSize = get_param_int(argc, params, 2, 0);
  auto fnResult = LoadImageFromMemory(fileType, fileData, dataSize);
  v_setimage(retval, fnResult);
  return 1;
}

//
// Load image from screen buffer and (screenshot)
//
static int cmd_loadimagefromscreen(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = LoadImageFromScreen();
  v_setimage(retval, fnResult);
  return 1;
}

//
// Load image from GPU texture data
//
static int cmd_loadimagefromtexture(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int texture_id = get_texture_id(argc, params, 0, retval);
  if (texture_id != -1) {
    auto fnResult = LoadImageFromTexture(_textureMap.at(texture_id));
    v_setimage(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Load colors palette from image as a Color array (RGBA - 32bit)
//
static int cmd_loadimagepalette(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto maxPaletteSize = get_param_int(argc, params, 1, 0);
    auto colorCount = (int *)0;
    auto fnResult = (var_int_t)LoadImagePalette(_imageMap.at(image_id), maxPaletteSize, colorCount);
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Load image from RAW file data
//
static int cmd_loadimageraw(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  auto width = get_param_int(argc, params, 1, 0);
  auto height = get_param_int(argc, params, 2, 0);
  auto format = get_param_int(argc, params, 3, 0);
  auto headerSize = get_param_int(argc, params, 4, 0);
  auto fnResult = LoadImageRaw(fileName, width, height, format, headerSize);
  v_setimage(retval, fnResult);
  return 1;
}

//
// Load image from SVG file data or string with specified size
//
static int cmd_loadimagesvg(int argc, slib_par_t *params, var_t *retval) {
  auto fileNameOrString = get_param_str(argc, params, 0, 0);
  auto width = get_param_int(argc, params, 1, 0);
  auto height = get_param_int(argc, params, 2, 0);
  auto fnResult = LoadImageSvg(fileNameOrString, width, height);
  v_setimage(retval, fnResult);
  return 1;
}

//
// Load model from files (meshes and materials)
//
static int cmd_loadmodel(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  auto fnResult = LoadModel(fileName);
  v_setmodel(retval, fnResult);
  return 1;
}

//
// Load model from generated mesh (default material)
//
static int cmd_loadmodelfrommesh(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int mesh_id = get_mesh_id(argc, params, 0, retval);
  if (mesh_id != -1) {
    auto fnResult = LoadModelFromMesh(_meshMap.at(mesh_id));
    v_setmodel(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Load music stream from file
//
static int cmd_loadmusicstream(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  auto fnResult = LoadMusicStream(fileName);
  v_setmusic(retval, fnResult);
  return 1;
}

//
// Load music stream from data
//
static int cmd_loadmusicstreamfrommemory(int argc, slib_par_t *params, var_t *retval) {
  auto fileType = get_param_str(argc, params, 0, 0);
  auto data = (const unsigned char *)get_param_str(argc, params, 1, 0);
  auto dataSize = get_param_int(argc, params, 2, 0);
  auto fnResult = LoadMusicStreamFromMemory(fileType, data, dataSize);
  v_setmusic(retval, fnResult);
  return 1;
}

//
// Load random values sequence, no values repeated
//
static int cmd_loadrandomsequence(int argc, slib_par_t *params, var_t *retval) {
  auto count = get_param_int(argc, params, 0, 0);
  auto min = get_param_int(argc, params, 1, 0);
  auto max = get_param_int(argc, params, 2, 0);
  auto fnResult = (var_int_t)LoadRandomSequence(count, min, max);
  v_setint(retval, fnResult);
  return 1;
}

//
// Load shader from code strings and bind default locations
//
static int cmd_loadshaderfrommemory(int argc, slib_par_t *params, var_t *retval) {
  auto vsCode = get_param_str(argc, params, 0, 0);
  auto fsCode = get_param_str(argc, params, 1, 0);
  auto fnResult = LoadShaderFromMemory(vsCode, fsCode);
  v_setshader(retval, fnResult);
  return 1;
}

//
// Load sound from file
//
static int cmd_loadsound(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  auto fnResult = LoadSound(fileName);
  v_setsound(retval, fnResult);
  return 1;
}

//
// Create a new sound that shares the same sample data as the source sound, does not own the sound data
//
static int cmd_loadsoundalias(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int source_id = get_sound_id(argc, params, 0, retval);
  if (source_id != -1) {
    auto fnResult = LoadSoundAlias(_soundMap.at(source_id));
    v_setsound(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Load sound from wave data
//
static int cmd_loadsoundfromwave(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int wave_id = get_wave_id(argc, params, 0, retval);
  if (wave_id != -1) {
    auto fnResult = LoadSoundFromWave(_waveMap.at(wave_id));
    v_setsound(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Load texture from file into GPU memory (VRAM)
//
static int cmd_loadtexture(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  auto fnResult = LoadTexture(fileName);
  v_settexture2d(retval, fnResult);
  return 1;
}

//
// Load cubemap from image, multiple image cubemap layouts supported
//
static int cmd_loadtexturecubemap(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto layout = get_param_int(argc, params, 1, 0);
    auto fnResult = LoadTextureCubemap(_imageMap.at(image_id), layout);
    v_settexture2d(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Load texture from image data
//
static int cmd_loadtexturefromimage(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto fnResult = LoadTextureFromImage(_imageMap.at(image_id));
    v_settexture2d(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Load UTF-8 text encoded from codepoints array
//
static int cmd_loadutf8(int argc, slib_par_t *params, var_t *retval) {
  auto codepoints = (const int *)get_param_int_t(argc, params, 0, 0);
  auto length = get_param_int(argc, params, 1, 0);
  auto fnResult = (const char *)LoadUTF8(codepoints, length);
  v_setstr(retval, fnResult);
  return 1;
}

//
// Load wave data from file
//
static int cmd_loadwave(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  auto fnResult = LoadWave(fileName);
  v_setwave(retval, fnResult);
  return 1;
}

//
// Load wave from memory buffer, fileType refers to extension: i.e. '.wav'
//
static int cmd_loadwavefrommemory(int argc, slib_par_t *params, var_t *retval) {
  auto fileType = get_param_str(argc, params, 0, 0);
  auto fileData = (const unsigned char *)get_param_str(argc, params, 1, 0);
  auto dataSize = get_param_int(argc, params, 2, 0);
  auto fnResult = LoadWaveFromMemory(fileType, fileData, dataSize);
  v_setwave(retval, fnResult);
  return 1;
}

//
// Load samples data from wave as a 32bit float data array
//
static int cmd_loadwavesamples(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int wave_id = get_wave_id(argc, params, 0, retval);
  if (wave_id != -1) {
    auto fnResult = (var_int_t)LoadWaveSamples(_waveMap.at(wave_id));
    v_setint(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Measure string width for default font
//
static int cmd_measuretext(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, 0);
  auto fontSize = get_param_int(argc, params, 1, 0);
  auto fnResult = MeasureText(text, fontSize);
  v_setint(retval, fnResult);
  return 1;
}

//
// Measure string size for Font
//
static int cmd_measuretextex(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int font_id = get_font_id(argc, params, 0, retval);
  if (font_id != -1) {
    auto text = get_param_str(argc, params, 1, 0);
    auto fontSize = get_param_num(argc, params, 2, 0);
    auto spacing = get_param_num(argc, params, 3, 0);
    auto fnResult = MeasureTextEx(_fontMap.at(font_id), text, fontSize, spacing);
    v_setvec2(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Internal memory allocator
//
static int cmd_memalloc(int argc, slib_par_t *params, var_t *retval) {
  auto size = get_param_int(argc, params, 0, 0);
  auto fnResult = (var_int_t)MemAlloc(size);
  v_setint(retval, fnResult);
  return 1;
}

//
// Internal memory reallocator
//
static int cmd_memrealloc(int argc, slib_par_t *params, var_t *retval) {
  auto ptr = (void *)get_param_int_t(argc, params, 0, 0);
  auto size = get_param_int(argc, params, 1, 0);
  auto fnResult = (var_int_t)MemRealloc(ptr, size);
  v_setint(retval, fnResult);
  return 1;
}

//
// Save data to file from byte array (write), returns true on success
//
static int cmd_savefiledata(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  auto data = (void *)get_param_int_t(argc, params, 1, 0);
  auto dataSize = get_param_int(argc, params, 2, 0);
  auto fnResult = SaveFileData(fileName, data, dataSize);
  v_setint(retval, fnResult);
  return 1;
}

//
// Save text data to file (write), string must be '\\0' terminated, returns true on success
//
static int cmd_savefiletext(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  auto text = (char *)get_param_str(argc, params, 1, 0);
  auto fnResult = SaveFileText(fileName, text);
  v_setint(retval, fnResult);
  return 1;
}

//
// Set internal gamepad mappings (SDL_GameControllerDB)
//
static int cmd_setgamepadmappings(int argc, slib_par_t *params, var_t *retval) {
  auto mappings = get_param_str(argc, params, 0, 0);
  auto fnResult = SetGamepadMappings(mappings);
  v_setint(retval, fnResult);
  return 1;
}

//
// Copy one string to another, returns bytes copied
//
static int cmd_textcopy(int argc, slib_par_t *params, var_t *retval) {
  auto dst = (char *)get_param_str(argc, params, 0, 0);
  auto src = get_param_str(argc, params, 1, 0);
  auto fnResult = TextCopy(dst, src);
  v_setint(retval, fnResult);
  return 1;
}

//
// Find first text occurrence within a string
//
static int cmd_textfindindex(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, 0);
  auto find = get_param_str(argc, params, 1, 0);
  auto fnResult = TextFindIndex(text, find);
  v_setint(retval, fnResult);
  return 1;
}

//
// Insert text in a position (WARNING: memory must be freed!)
//
static int cmd_textinsert(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, 0);
  auto insert = get_param_str(argc, params, 1, 0);
  auto position = get_param_int(argc, params, 2, 0);
  auto fnResult = (const char *)TextInsert(text, insert, position);
  v_setstr(retval, fnResult);
  return 1;
}

//
// Check if two text string are equal
//
static int cmd_textisequal(int argc, slib_par_t *params, var_t *retval) {
  auto text1 = get_param_str(argc, params, 0, 0);
  auto text2 = get_param_str(argc, params, 1, 0);
  auto fnResult = TextIsEqual(text1, text2);
  v_setint(retval, fnResult);
  return 1;
}

//
// Get text length, checks for '\\0' ending
//
static int cmd_textlength(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, 0);
  auto fnResult = TextLength(text);
  v_setint(retval, fnResult);
  return 1;
}

//
// Replace text string (WARNING: memory must be freed!)
//
static int cmd_textreplace(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, 0);
  auto replace = get_param_str(argc, params, 1, 0);
  auto by = get_param_str(argc, params, 2, 0);
  auto fnResult = (const char *)TextReplace(text, replace, by);
  v_setstr(retval, fnResult);
  return 1;
}

//
// Get a piece of a text string
//
static int cmd_textsubtext(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, 0);
  auto position = get_param_int(argc, params, 1, 0);
  auto length = get_param_int(argc, params, 2, 0);
  auto fnResult = (const char *)TextSubtext(text, position, length);
  v_setstr(retval, fnResult);
  return 1;
}

//
// Get float value from text (negative values not supported)
//
static int cmd_texttofloat(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, 0);
  auto fnResult = TextToFloat(text);
  v_setreal(retval, fnResult);
  return 1;
}

//
// Get integer value from text (negative values not supported)
//
static int cmd_texttointeger(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, 0);
  auto fnResult = TextToInteger(text);
  v_setint(retval, fnResult);
  return 1;
}

//
// Get lower case version of provided string
//
static int cmd_texttolower(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, 0);
  auto fnResult = (const char *)TextToLower(text);
  v_setstr(retval, fnResult);
  return 1;
}

//
// Get Pascal case notation version of provided string
//
static int cmd_texttopascal(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, 0);
  auto fnResult = (const char *)TextToPascal(text);
  v_setstr(retval, fnResult);
  return 1;
}

//
// Get upper case version of provided string
//
static int cmd_texttoupper(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, 0);
  auto fnResult = (const char *)TextToUpper(text);
  v_setstr(retval, fnResult);
  return 1;
}

//
// Copy a wave to a new wave
//
static int cmd_wavecopy(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int wave_id = get_wave_id(argc, params, 0, retval);
  if (wave_id != -1) {
    auto fnResult = WaveCopy(_waveMap.at(wave_id));
    v_setwave(retval, fnResult);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Check if application should close (KEY_ESCAPE pressed or windows close icon clicked)
//
static int cmd_windowshouldclose(int argc, slib_par_t *params, var_t *retval) {
  auto fnResult = WindowShouldClose();
  v_setint(retval, fnResult);
  return 1;
}

