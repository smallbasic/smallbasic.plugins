//
// Begin blending mode (alpha, additive, multiplied, subtract, custom)
//
static int cmd_beginblendmode(int argc, slib_par_t *params, var_t *retval) {
  auto mode = get_param_int(argc, params, 0, 0);
  BeginBlendMode(mode);
  return 1;
}

//
// Setup canvas (framebuffer) to start drawing
//
static int cmd_begindrawing(int argc, slib_par_t *params, var_t *retval) {
  BeginDrawing();
  return 1;
}

//
// Begin 2D mode with custom camera (2D)
//
static int cmd_beginmode2d(int argc, slib_par_t *params, var_t *retval) {
  auto camera = get_camera_2d(argc, params, 0);
  BeginMode2D(camera);
  return 1;
}

//
// Begin 3D mode with custom camera (3D)
//
static int cmd_beginmode3d(int argc, slib_par_t *params, var_t *retval) {
  auto camera = get_camera_3d(argc, params, 0);
  BeginMode3D(camera);
  return 1;
}

//
// Begin scissor mode (define screen area for following drawing)
//
static int cmd_beginscissormode(int argc, slib_par_t *params, var_t *retval) {
  auto x = get_param_int(argc, params, 0, 0);
  auto y = get_param_int(argc, params, 1, 0);
  auto width = get_param_int(argc, params, 2, 0);
  auto height = get_param_int(argc, params, 3, 0);
  BeginScissorMode(x, y, width, height);
  return 1;
}

//
// Begin custom shader drawing
//
static int cmd_beginshadermode(int argc, slib_par_t *params, var_t *retval) {
  auto shader = get_param_shader(argc, params, 0);
  BeginShaderMode(shader);
  return 1;
}

//
// Begin drawing to render texture
//
static int cmd_begintexturemode(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int target_id = get_render_texture_id(argc, params, 0, retval);
  if (target_id != -1) {
    BeginTextureMode(_renderMap.at(target_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Set background color (framebuffer clear color)
//
static int cmd_clearbackground(int argc, slib_par_t *params, var_t *retval) {
  auto color = get_param_color(argc, params, 0);
  ClearBackground(color);
  return 1;
}

//
// Clear window configuration state flags
//
static int cmd_clearwindowstate(int argc, slib_par_t *params, var_t *retval) {
  auto flags = get_param_int(argc, params, 0, 0);
  ClearWindowState(flags);
  return 1;
}

//
// Close the audio device and context
//
static int cmd_closeaudiodevice(int argc, slib_par_t *params, var_t *retval) {
  CloseAudioDevice();
  return 1;
}

//
// Close window and unload OpenGL context
//
static int cmd_closewindow(int argc, slib_par_t *params, var_t *retval) {
  CloseWindow();
  return 1;
}

//
// Disables cursor (lock cursor)
//
static int cmd_disablecursor(int argc, slib_par_t *params, var_t *retval) {
  DisableCursor();
  return 1;
}

//
// Disable waiting for events on EndDrawing(), automatic events polling
//
static int cmd_disableeventwaiting(int argc, slib_par_t *params, var_t *retval) {
  DisableEventWaiting();
  return 1;
}

//
// Draw a billboard texture
//
static int cmd_drawbillboard(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int texture_id = get_texture_id(argc, params, 1, retval);
  if (texture_id != -1) {
    auto camera = get_camera_3d(argc, params, 0);
    auto position = get_param_vec3(argc, params, 2);
    auto scale = get_param_num(argc, params, 3, 0);
    auto tint = get_param_color(argc, params, 4);
    DrawBillboard(camera, _textureMap.at(texture_id), position, scale, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw a billboard texture defined by source and rotation
//
static int cmd_drawbillboardpro(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int texture_id = get_texture_id(argc, params, 1, retval);
  if (texture_id != -1) {
    auto camera = get_camera_3d(argc, params, 0);
    auto source = get_param_rect(argc, params, 2);
    auto position = get_param_vec3(argc, params, 3);
    auto up = get_param_vec3(argc, params, 4);
    auto size = get_param_vec2(argc, params, 5);
    auto origin = get_param_vec2(argc, params, 6);
    auto rotation = get_param_num(argc, params, 7, 0);
    auto tint = get_param_color(argc, params, 8);
    DrawBillboardPro(camera, _textureMap.at(texture_id), source, position, up, size, origin, rotation, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw a billboard texture defined by source
//
static int cmd_drawbillboardrec(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int texture_id = get_texture_id(argc, params, 1, retval);
  if (texture_id != -1) {
    auto camera = get_camera_3d(argc, params, 0);
    auto source = get_param_rect(argc, params, 2);
    auto position = get_param_vec3(argc, params, 3);
    auto size = get_param_vec2(argc, params, 4);
    auto tint = get_param_color(argc, params, 5);
    DrawBillboardRec(camera, _textureMap.at(texture_id), source, position, size, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw bounding box (wires)
//
static int cmd_drawboundingbox(int argc, slib_par_t *params, var_t *retval) {
  auto box = get_param_bounding_box(argc, params, 0);
  auto color = get_param_color(argc, params, 1);
  DrawBoundingBox(box, color);
  return 1;
}

//
// Draw a capsule with the center of its sphere caps at startPos and endPos
//
static int cmd_drawcapsule(int argc, slib_par_t *params, var_t *retval) {
  auto startPos = get_param_vec3(argc, params, 0);
  auto endPos = get_param_vec3(argc, params, 1);
  auto radius = get_param_num(argc, params, 2, 0);
  auto slices = get_param_int(argc, params, 3, 0);
  auto rings = get_param_int(argc, params, 4, 0);
  auto color = get_param_color(argc, params, 5);
  DrawCapsule(startPos, endPos, radius, slices, rings, color);
  return 1;
}

//
// Draw capsule wireframe with the center of its sphere caps at startPos and endPos
//
static int cmd_drawcapsulewires(int argc, slib_par_t *params, var_t *retval) {
  auto startPos = get_param_vec3(argc, params, 0);
  auto endPos = get_param_vec3(argc, params, 1);
  auto radius = get_param_num(argc, params, 2, 0);
  auto slices = get_param_int(argc, params, 3, 0);
  auto rings = get_param_int(argc, params, 4, 0);
  auto color = get_param_color(argc, params, 5);
  DrawCapsuleWires(startPos, endPos, radius, slices, rings, color);
  return 1;
}

//
// Draw a color-filled circle
//
static int cmd_drawcircle(int argc, slib_par_t *params, var_t *retval) {
  auto centerX = get_param_int(argc, params, 0, 0);
  auto centerY = get_param_int(argc, params, 1, 0);
  auto radius = get_param_num(argc, params, 2, 0);
  auto color = get_param_color(argc, params, 3);
  DrawCircle(centerX, centerY, radius, color);
  return 1;
}

//
// Draw a circle in 3D world space
//
static int cmd_drawcircle3d(int argc, slib_par_t *params, var_t *retval) {
  auto center = get_param_vec3(argc, params, 0);
  auto radius = get_param_num(argc, params, 1, 0);
  auto rotationAxis = get_param_vec3(argc, params, 2);
  auto rotationAngle = get_param_num(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawCircle3D(center, radius, rotationAxis, rotationAngle, color);
  return 1;
}

//
// Draw a gradient-filled circle
//
static int cmd_drawcirclegradient(int argc, slib_par_t *params, var_t *retval) {
  auto centerX = get_param_int(argc, params, 0, 0);
  auto centerY = get_param_int(argc, params, 1, 0);
  auto radius = get_param_num(argc, params, 2, 0);
  auto inner = get_param_color(argc, params, 3);
  auto outer = get_param_color(argc, params, 4);
  DrawCircleGradient(centerX, centerY, radius, inner, outer);
  return 1;
}

//
// Draw circle outline
//
static int cmd_drawcirclelines(int argc, slib_par_t *params, var_t *retval) {
  auto centerX = get_param_int(argc, params, 0, 0);
  auto centerY = get_param_int(argc, params, 1, 0);
  auto radius = get_param_num(argc, params, 2, 0);
  auto color = get_param_color(argc, params, 3);
  DrawCircleLines(centerX, centerY, radius, color);
  return 1;
}

//
// Draw circle outline (Vector version)
//
static int cmd_drawcirclelinesv(int argc, slib_par_t *params, var_t *retval) {
  auto center = get_param_vec2(argc, params, 0);
  auto radius = get_param_num(argc, params, 1, 0);
  auto color = get_param_color(argc, params, 2);
  DrawCircleLinesV(center, radius, color);
  return 1;
}

//
// Draw a piece of a circle
//
static int cmd_drawcirclesector(int argc, slib_par_t *params, var_t *retval) {
  auto center = get_param_vec2(argc, params, 0);
  auto radius = get_param_num(argc, params, 1, 0);
  auto startAngle = get_param_num(argc, params, 2, 0);
  auto endAngle = get_param_num(argc, params, 3, 0);
  auto segments = get_param_int(argc, params, 4, 0);
  auto color = get_param_color(argc, params, 5);
  DrawCircleSector(center, radius, startAngle, endAngle, segments, color);
  return 1;
}

//
// Draw circle sector outline
//
static int cmd_drawcirclesectorlines(int argc, slib_par_t *params, var_t *retval) {
  auto center = get_param_vec2(argc, params, 0);
  auto radius = get_param_num(argc, params, 1, 0);
  auto startAngle = get_param_num(argc, params, 2, 0);
  auto endAngle = get_param_num(argc, params, 3, 0);
  auto segments = get_param_int(argc, params, 4, 0);
  auto color = get_param_color(argc, params, 5);
  DrawCircleSectorLines(center, radius, startAngle, endAngle, segments, color);
  return 1;
}

//
// Draw a color-filled circle (Vector version)
//
static int cmd_drawcirclev(int argc, slib_par_t *params, var_t *retval) {
  auto center = get_param_vec2(argc, params, 0);
  auto radius = get_param_num(argc, params, 1, 0);
  auto color = get_param_color(argc, params, 2);
  DrawCircleV(center, radius, color);
  return 1;
}

//
// Draw cube
//
static int cmd_drawcube(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec3(argc, params, 0);
  auto width = get_param_num(argc, params, 1, 0);
  auto height = get_param_num(argc, params, 2, 0);
  auto length = get_param_num(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawCube(position, width, height, length, color);
  return 1;
}

//
// Draw cube (Vector version)
//
static int cmd_drawcubev(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec3(argc, params, 0);
  auto size = get_param_vec3(argc, params, 1);
  auto color = get_param_color(argc, params, 2);
  DrawCubeV(position, size, color);
  return 1;
}

//
// Draw cube wires
//
static int cmd_drawcubewires(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec3(argc, params, 0);
  auto width = get_param_num(argc, params, 1, 0);
  auto height = get_param_num(argc, params, 2, 0);
  auto length = get_param_num(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawCubeWires(position, width, height, length, color);
  return 1;
}

//
// Draw cube wires (Vector version)
//
static int cmd_drawcubewiresv(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec3(argc, params, 0);
  auto size = get_param_vec3(argc, params, 1);
  auto color = get_param_color(argc, params, 2);
  DrawCubeWiresV(position, size, color);
  return 1;
}

//
// Draw a cylinder/cone
//
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

//
// Draw a cylinder with base at startPos and top at endPos
//
static int cmd_drawcylinderex(int argc, slib_par_t *params, var_t *retval) {
  auto startPos = get_param_vec3(argc, params, 0);
  auto endPos = get_param_vec3(argc, params, 1);
  auto startRadius = get_param_num(argc, params, 2, 0);
  auto endRadius = get_param_num(argc, params, 3, 0);
  auto sides = get_param_int(argc, params, 4, 0);
  auto color = get_param_color(argc, params, 5);
  DrawCylinderEx(startPos, endPos, startRadius, endRadius, sides, color);
  return 1;
}

//
// Draw a cylinder/cone wires
//
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

//
// Draw a cylinder wires with base at startPos and top at endPos
//
static int cmd_drawcylinderwiresex(int argc, slib_par_t *params, var_t *retval) {
  auto startPos = get_param_vec3(argc, params, 0);
  auto endPos = get_param_vec3(argc, params, 1);
  auto startRadius = get_param_num(argc, params, 2, 0);
  auto endRadius = get_param_num(argc, params, 3, 0);
  auto sides = get_param_int(argc, params, 4, 0);
  auto color = get_param_color(argc, params, 5);
  DrawCylinderWiresEx(startPos, endPos, startRadius, endRadius, sides, color);
  return 1;
}

//
// Draw ellipse
//
static int cmd_drawellipse(int argc, slib_par_t *params, var_t *retval) {
  auto centerX = get_param_int(argc, params, 0, 0);
  auto centerY = get_param_int(argc, params, 1, 0);
  auto radiusH = get_param_num(argc, params, 2, 0);
  auto radiusV = get_param_num(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawEllipse(centerX, centerY, radiusH, radiusV, color);
  return 1;
}

//
// Draw ellipse outline
//
static int cmd_drawellipselines(int argc, slib_par_t *params, var_t *retval) {
  auto centerX = get_param_int(argc, params, 0, 0);
  auto centerY = get_param_int(argc, params, 1, 0);
  auto radiusH = get_param_num(argc, params, 2, 0);
  auto radiusV = get_param_num(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawEllipseLines(centerX, centerY, radiusH, radiusV, color);
  return 1;
}

//
// Draw current FPS
//
static int cmd_drawfps(int argc, slib_par_t *params, var_t *retval) {
  auto posX = get_param_int(argc, params, 0, 0);
  auto posY = get_param_int(argc, params, 1, 0);
  DrawFPS(posX, posY);
  return 1;
}

//
// Draw a grid (centered at (0, 0, 0))
//
static int cmd_drawgrid(int argc, slib_par_t *params, var_t *retval) {
  auto slices = get_param_int(argc, params, 0, 0);
  auto spacing = get_param_num(argc, params, 1, 0);
  DrawGrid(slices, spacing);
  return 1;
}

//
// Draw a line
//
static int cmd_drawline(int argc, slib_par_t *params, var_t *retval) {
  auto startPosX = get_param_int(argc, params, 0, 0);
  auto startPosY = get_param_int(argc, params, 1, 0);
  auto endPosX = get_param_int(argc, params, 2, 0);
  auto endPosY = get_param_int(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawLine(startPosX, startPosY, endPosX, endPosY, color);
  return 1;
}

//
// Draw a line in 3D world space
//
static int cmd_drawline3d(int argc, slib_par_t *params, var_t *retval) {
  auto startPos = get_param_vec3(argc, params, 0);
  auto endPos = get_param_vec3(argc, params, 1);
  auto color = get_param_color(argc, params, 2);
  DrawLine3D(startPos, endPos, color);
  return 1;
}

//
// Draw line segment cubic-bezier in-out interpolation
//
static int cmd_drawlinebezier(int argc, slib_par_t *params, var_t *retval) {
  auto startPos = get_param_vec2(argc, params, 0);
  auto endPos = get_param_vec2(argc, params, 1);
  auto thick = get_param_num(argc, params, 2, 0);
  auto color = get_param_color(argc, params, 3);
  DrawLineBezier(startPos, endPos, thick, color);
  return 1;
}

//
// Draw a line (using triangles/quads)
//
static int cmd_drawlineex(int argc, slib_par_t *params, var_t *retval) {
  auto startPos = get_param_vec2(argc, params, 0);
  auto endPos = get_param_vec2(argc, params, 1);
  auto thick = get_param_num(argc, params, 2, 0);
  auto color = get_param_color(argc, params, 3);
  DrawLineEx(startPos, endPos, thick, color);
  return 1;
}

//
// Draw lines sequence (using gl lines)
//
static int cmd_drawlinestrip(int argc, slib_par_t *params, var_t *retval) {
  auto points = (Vector2 *)get_param_vec2_array(argc, params, 0);
  auto pointCount = get_param_int(argc, params, 1, 0);
  auto color = get_param_color(argc, params, 2);
  DrawLineStrip(points, pointCount, color);
  return 1;
}

//
// Draw a line (using gl lines)
//
static int cmd_drawlinev(int argc, slib_par_t *params, var_t *retval) {
  auto startPos = get_param_vec2(argc, params, 0);
  auto endPos = get_param_vec2(argc, params, 1);
  auto color = get_param_color(argc, params, 2);
  DrawLineV(startPos, endPos, color);
  return 1;
}

//
// Draw a model (with texture if set)
//
static int cmd_drawmodel(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int model_id = get_model_id(argc, params, 0, retval);
  if (model_id != -1) {
    auto position = get_param_vec3(argc, params, 1);
    auto scale = get_param_num(argc, params, 2, 0);
    auto tint = get_param_color(argc, params, 3);
    DrawModel(_modelMap.at(model_id), position, scale, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw a model with extended parameters
//
static int cmd_drawmodelex(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int model_id = get_model_id(argc, params, 0, retval);
  if (model_id != -1) {
    auto position = get_param_vec3(argc, params, 1);
    auto rotationAxis = get_param_vec3(argc, params, 2);
    auto rotationAngle = get_param_num(argc, params, 3, 0);
    auto scale = get_param_vec3(argc, params, 4);
    auto tint = get_param_color(argc, params, 5);
    DrawModelEx(_modelMap.at(model_id), position, rotationAxis, rotationAngle, scale, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw a model as points
//
static int cmd_drawmodelpoints(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int model_id = get_model_id(argc, params, 0, retval);
  if (model_id != -1) {
    auto position = get_param_vec3(argc, params, 1);
    auto scale = get_param_num(argc, params, 2, 0);
    auto tint = get_param_color(argc, params, 3);
    DrawModelPoints(_modelMap.at(model_id), position, scale, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw a model as points with extended parameters
//
static int cmd_drawmodelpointsex(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int model_id = get_model_id(argc, params, 0, retval);
  if (model_id != -1) {
    auto position = get_param_vec3(argc, params, 1);
    auto rotationAxis = get_param_vec3(argc, params, 2);
    auto rotationAngle = get_param_num(argc, params, 3, 0);
    auto scale = get_param_vec3(argc, params, 4);
    auto tint = get_param_color(argc, params, 5);
    DrawModelPointsEx(_modelMap.at(model_id), position, rotationAxis, rotationAngle, scale, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw a model wires (with texture if set)
//
static int cmd_drawmodelwires(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int model_id = get_model_id(argc, params, 0, retval);
  if (model_id != -1) {
    auto position = get_param_vec3(argc, params, 1);
    auto scale = get_param_num(argc, params, 2, 0);
    auto tint = get_param_color(argc, params, 3);
    DrawModelWires(_modelMap.at(model_id), position, scale, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw a model wires (with texture if set) with extended parameters
//
static int cmd_drawmodelwiresex(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int model_id = get_model_id(argc, params, 0, retval);
  if (model_id != -1) {
    auto position = get_param_vec3(argc, params, 1);
    auto rotationAxis = get_param_vec3(argc, params, 2);
    auto rotationAngle = get_param_num(argc, params, 3, 0);
    auto scale = get_param_vec3(argc, params, 4);
    auto tint = get_param_color(argc, params, 5);
    DrawModelWiresEx(_modelMap.at(model_id), position, rotationAxis, rotationAngle, scale, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw a pixel using geometry [Can be slow, use with care]
//
static int cmd_drawpixel(int argc, slib_par_t *params, var_t *retval) {
  auto posX = get_param_int(argc, params, 0, 0);
  auto posY = get_param_int(argc, params, 1, 0);
  auto color = get_param_color(argc, params, 2);
  DrawPixel(posX, posY, color);
  return 1;
}

//
// Draw a pixel using geometry (Vector version) [Can be slow, use with care]
//
static int cmd_drawpixelv(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec2(argc, params, 0);
  auto color = get_param_color(argc, params, 1);
  DrawPixelV(position, color);
  return 1;
}

//
// Draw a plane XZ
//
static int cmd_drawplane(int argc, slib_par_t *params, var_t *retval) {
  auto centerPos = get_param_vec3(argc, params, 0);
  auto size = get_param_vec2(argc, params, 1);
  auto color = get_param_color(argc, params, 2);
  DrawPlane(centerPos, size, color);
  return 1;
}

//
// Draw a point in 3D space, actually a small line
//
static int cmd_drawpoint3d(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec3(argc, params, 0);
  auto color = get_param_color(argc, params, 1);
  DrawPoint3D(position, color);
  return 1;
}

//
// Draw a regular polygon (Vector version)
//
static int cmd_drawpoly(int argc, slib_par_t *params, var_t *retval) {
  auto center = get_param_vec2(argc, params, 0);
  auto sides = get_param_int(argc, params, 1, 0);
  auto radius = get_param_num(argc, params, 2, 0);
  auto rotation = get_param_num(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawPoly(center, sides, radius, rotation, color);
  return 1;
}

//
// Draw a polygon outline of n sides
//
static int cmd_drawpolylines(int argc, slib_par_t *params, var_t *retval) {
  auto center = get_param_vec2(argc, params, 0);
  auto sides = get_param_int(argc, params, 1, 0);
  auto radius = get_param_num(argc, params, 2, 0);
  auto rotation = get_param_num(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawPolyLines(center, sides, radius, rotation, color);
  return 1;
}

//
// Draw a polygon outline of n sides with extended parameters
//
static int cmd_drawpolylinesex(int argc, slib_par_t *params, var_t *retval) {
  auto center = get_param_vec2(argc, params, 0);
  auto sides = get_param_int(argc, params, 1, 0);
  auto radius = get_param_num(argc, params, 2, 0);
  auto rotation = get_param_num(argc, params, 3, 0);
  auto lineThick = get_param_num(argc, params, 4, 0);
  auto color = get_param_color(argc, params, 5);
  DrawPolyLinesEx(center, sides, radius, rotation, lineThick, color);
  return 1;
}

//
// Draw a ray line
//
static int cmd_drawray(int argc, slib_par_t *params, var_t *retval) {
  auto ray = get_param_ray(argc, params, 0);
  auto color = get_param_color(argc, params, 1);
  DrawRay(ray, color);
  return 1;
}

//
// Draw a color-filled rectangle
//
static int cmd_drawrectangle(int argc, slib_par_t *params, var_t *retval) {
  auto posX = get_param_int(argc, params, 0, 0);
  auto posY = get_param_int(argc, params, 1, 0);
  auto width = get_param_int(argc, params, 2, 0);
  auto height = get_param_int(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawRectangle(posX, posY, width, height, color);
  return 1;
}

//
// Draw a gradient-filled rectangle with custom vertex colors
//
static int cmd_drawrectanglegradientex(int argc, slib_par_t *params, var_t *retval) {
  auto rec = get_param_rect(argc, params, 0);
  auto topLeft = get_param_color(argc, params, 1);
  auto bottomLeft = get_param_color(argc, params, 2);
  auto topRight = get_param_color(argc, params, 3);
  auto bottomRight = get_param_color(argc, params, 4);
  DrawRectangleGradientEx(rec, topLeft, bottomLeft, topRight, bottomRight);
  return 1;
}

//
// Draw a horizontal-gradient-filled rectangle
//
static int cmd_drawrectanglegradienth(int argc, slib_par_t *params, var_t *retval) {
  auto posX = get_param_int(argc, params, 0, 0);
  auto posY = get_param_int(argc, params, 1, 0);
  auto width = get_param_int(argc, params, 2, 0);
  auto height = get_param_int(argc, params, 3, 0);
  auto left = get_param_color(argc, params, 4);
  auto right = get_param_color(argc, params, 5);
  DrawRectangleGradientH(posX, posY, width, height, left, right);
  return 1;
}

//
// Draw a vertical-gradient-filled rectangle
//
static int cmd_drawrectanglegradientv(int argc, slib_par_t *params, var_t *retval) {
  auto posX = get_param_int(argc, params, 0, 0);
  auto posY = get_param_int(argc, params, 1, 0);
  auto width = get_param_int(argc, params, 2, 0);
  auto height = get_param_int(argc, params, 3, 0);
  auto top = get_param_color(argc, params, 4);
  auto bottom = get_param_color(argc, params, 5);
  DrawRectangleGradientV(posX, posY, width, height, top, bottom);
  return 1;
}

//
// Draw rectangle outline
//
static int cmd_drawrectanglelines(int argc, slib_par_t *params, var_t *retval) {
  auto posX = get_param_int(argc, params, 0, 0);
  auto posY = get_param_int(argc, params, 1, 0);
  auto width = get_param_int(argc, params, 2, 0);
  auto height = get_param_int(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawRectangleLines(posX, posY, width, height, color);
  return 1;
}

//
// Draw rectangle outline with extended parameters
//
static int cmd_drawrectanglelinesex(int argc, slib_par_t *params, var_t *retval) {
  auto rec = get_param_rect(argc, params, 0);
  auto lineThick = get_param_num(argc, params, 1, 0);
  auto color = get_param_color(argc, params, 2);
  DrawRectangleLinesEx(rec, lineThick, color);
  return 1;
}

//
// Draw a color-filled rectangle with pro parameters
//
static int cmd_drawrectanglepro(int argc, slib_par_t *params, var_t *retval) {
  auto rec = get_param_rect(argc, params, 0);
  auto origin = get_param_vec2(argc, params, 1);
  auto rotation = get_param_num(argc, params, 2, 0);
  auto color = get_param_color(argc, params, 3);
  DrawRectanglePro(rec, origin, rotation, color);
  return 1;
}

//
// Draw a color-filled rectangle
//
static int cmd_drawrectanglerec(int argc, slib_par_t *params, var_t *retval) {
  auto rec = get_param_rect(argc, params, 0);
  auto color = get_param_color(argc, params, 1);
  DrawRectangleRec(rec, color);
  return 1;
}

//
// Draw rectangle with rounded edges
//
static int cmd_drawrectanglerounded(int argc, slib_par_t *params, var_t *retval) {
  auto rec = get_param_rect(argc, params, 0);
  auto roundness = get_param_num(argc, params, 1, 0);
  auto segments = get_param_int(argc, params, 2, 0);
  auto color = get_param_color(argc, params, 3);
  DrawRectangleRounded(rec, roundness, segments, color);
  return 1;
}

//
// Draw rectangle lines with rounded edges
//
static int cmd_drawrectangleroundedlines(int argc, slib_par_t *params, var_t *retval) {
  auto rec = get_param_rect(argc, params, 0);
  auto roundness = get_param_num(argc, params, 1, 0);
  auto segments = get_param_int(argc, params, 2, 0);
  auto color = get_param_color(argc, params, 3);
  DrawRectangleRoundedLines(rec, roundness, segments, color);
  return 1;
}

//
// Draw rectangle with rounded edges outline
//
static int cmd_drawrectangleroundedlinesex(int argc, slib_par_t *params, var_t *retval) {
  auto rec = get_param_rect(argc, params, 0);
  auto roundness = get_param_num(argc, params, 1, 0);
  auto segments = get_param_int(argc, params, 2, 0);
  auto lineThick = get_param_num(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawRectangleRoundedLinesEx(rec, roundness, segments, lineThick, color);
  return 1;
}

//
// Draw a color-filled rectangle (Vector version)
//
static int cmd_drawrectanglev(int argc, slib_par_t *params, var_t *retval) {
  auto position = get_param_vec2(argc, params, 0);
  auto size = get_param_vec2(argc, params, 1);
  auto color = get_param_color(argc, params, 2);
  DrawRectangleV(position, size, color);
  return 1;
}

//
// Draw ring
//
static int cmd_drawring(int argc, slib_par_t *params, var_t *retval) {
  auto center = get_param_vec2(argc, params, 0);
  auto innerRadius = get_param_num(argc, params, 1, 0);
  auto outerRadius = get_param_num(argc, params, 2, 0);
  auto startAngle = get_param_num(argc, params, 3, 0);
  auto endAngle = get_param_num(argc, params, 4, 0);
  auto segments = get_param_int(argc, params, 5, 0);
  auto color = get_param_color(argc, params, 6);
  DrawRing(center, innerRadius, outerRadius, startAngle, endAngle, segments, color);
  return 1;
}

//
// Draw ring outline
//
static int cmd_drawringlines(int argc, slib_par_t *params, var_t *retval) {
  auto center = get_param_vec2(argc, params, 0);
  auto innerRadius = get_param_num(argc, params, 1, 0);
  auto outerRadius = get_param_num(argc, params, 2, 0);
  auto startAngle = get_param_num(argc, params, 3, 0);
  auto endAngle = get_param_num(argc, params, 4, 0);
  auto segments = get_param_int(argc, params, 5, 0);
  auto color = get_param_color(argc, params, 6);
  DrawRingLines(center, innerRadius, outerRadius, startAngle, endAngle, segments, color);
  return 1;
}

//
// Draw sphere
//
static int cmd_drawsphere(int argc, slib_par_t *params, var_t *retval) {
  auto centerPos = get_param_vec3(argc, params, 0);
  auto radius = get_param_num(argc, params, 1, 0);
  auto color = get_param_color(argc, params, 2);
  DrawSphere(centerPos, radius, color);
  return 1;
}

//
// Draw sphere with extended parameters
//
static int cmd_drawsphereex(int argc, slib_par_t *params, var_t *retval) {
  auto centerPos = get_param_vec3(argc, params, 0);
  auto radius = get_param_num(argc, params, 1, 0);
  auto rings = get_param_int(argc, params, 2, 0);
  auto slices = get_param_int(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawSphereEx(centerPos, radius, rings, slices, color);
  return 1;
}

//
// Draw sphere wires
//
static int cmd_drawspherewires(int argc, slib_par_t *params, var_t *retval) {
  auto centerPos = get_param_vec3(argc, params, 0);
  auto radius = get_param_num(argc, params, 1, 0);
  auto rings = get_param_int(argc, params, 2, 0);
  auto slices = get_param_int(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawSphereWires(centerPos, radius, rings, slices, color);
  return 1;
}

//
// Draw spline: B-Spline, minimum 4 points
//
static int cmd_drawsplinebasis(int argc, slib_par_t *params, var_t *retval) {
  auto points = (Vector2 *)get_param_vec2_array(argc, params, 0);
  auto pointCount = get_param_int(argc, params, 1, 0);
  auto thick = get_param_num(argc, params, 2, 0);
  auto color = get_param_color(argc, params, 3);
  DrawSplineBasis(points, pointCount, thick, color);
  return 1;
}

//
// Draw spline: Cubic Bezier, minimum 4 points (2 control points): [p1, c2, c3, p4, c5, c6...]
//
static int cmd_drawsplinebeziercubic(int argc, slib_par_t *params, var_t *retval) {
  auto points = (Vector2 *)get_param_vec2_array(argc, params, 0);
  auto pointCount = get_param_int(argc, params, 1, 0);
  auto thick = get_param_num(argc, params, 2, 0);
  auto color = get_param_color(argc, params, 3);
  DrawSplineBezierCubic(points, pointCount, thick, color);
  return 1;
}

//
// Draw spline: Quadratic Bezier, minimum 3 points (1 control point): [p1, c2, p3, c4...]
//
static int cmd_drawsplinebezierquadratic(int argc, slib_par_t *params, var_t *retval) {
  auto points = (Vector2 *)get_param_vec2_array(argc, params, 0);
  auto pointCount = get_param_int(argc, params, 1, 0);
  auto thick = get_param_num(argc, params, 2, 0);
  auto color = get_param_color(argc, params, 3);
  DrawSplineBezierQuadratic(points, pointCount, thick, color);
  return 1;
}

//
// Draw spline: Catmull-Rom, minimum 4 points
//
static int cmd_drawsplinecatmullrom(int argc, slib_par_t *params, var_t *retval) {
  auto points = (Vector2 *)get_param_vec2_array(argc, params, 0);
  auto pointCount = get_param_int(argc, params, 1, 0);
  auto thick = get_param_num(argc, params, 2, 0);
  auto color = get_param_color(argc, params, 3);
  DrawSplineCatmullRom(points, pointCount, thick, color);
  return 1;
}

//
// Draw spline: Linear, minimum 2 points
//
static int cmd_drawsplinelinear(int argc, slib_par_t *params, var_t *retval) {
  auto points = (Vector2 *)get_param_vec2_array(argc, params, 0);
  auto pointCount = get_param_int(argc, params, 1, 0);
  auto thick = get_param_num(argc, params, 2, 0);
  auto color = get_param_color(argc, params, 3);
  DrawSplineLinear(points, pointCount, thick, color);
  return 1;
}

//
// Draw spline segment: B-Spline, 4 points
//
static int cmd_drawsplinesegmentbasis(int argc, slib_par_t *params, var_t *retval) {
  auto p1 = get_param_vec2(argc, params, 0);
  auto p2 = get_param_vec2(argc, params, 1);
  auto p3 = get_param_vec2(argc, params, 2);
  auto p4 = get_param_vec2(argc, params, 3);
  auto thick = get_param_num(argc, params, 4, 0);
  auto color = get_param_color(argc, params, 5);
  DrawSplineSegmentBasis(p1, p2, p3, p4, thick, color);
  return 1;
}

//
// Draw spline segment: Cubic Bezier, 2 points, 2 control points
//
static int cmd_drawsplinesegmentbeziercubic(int argc, slib_par_t *params, var_t *retval) {
  auto p1 = get_param_vec2(argc, params, 0);
  auto c2 = get_param_vec2(argc, params, 1);
  auto c3 = get_param_vec2(argc, params, 2);
  auto p4 = get_param_vec2(argc, params, 3);
  auto thick = get_param_num(argc, params, 4, 0);
  auto color = get_param_color(argc, params, 5);
  DrawSplineSegmentBezierCubic(p1, c2, c3, p4, thick, color);
  return 1;
}

//
// Draw spline segment: Quadratic Bezier, 2 points, 1 control point
//
static int cmd_drawsplinesegmentbezierquadratic(int argc, slib_par_t *params, var_t *retval) {
  auto p1 = get_param_vec2(argc, params, 0);
  auto c2 = get_param_vec2(argc, params, 1);
  auto p3 = get_param_vec2(argc, params, 2);
  auto thick = get_param_num(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawSplineSegmentBezierQuadratic(p1, c2, p3, thick, color);
  return 1;
}

//
// Draw spline segment: Catmull-Rom, 4 points
//
static int cmd_drawsplinesegmentcatmullrom(int argc, slib_par_t *params, var_t *retval) {
  auto p1 = get_param_vec2(argc, params, 0);
  auto p2 = get_param_vec2(argc, params, 1);
  auto p3 = get_param_vec2(argc, params, 2);
  auto p4 = get_param_vec2(argc, params, 3);
  auto thick = get_param_num(argc, params, 4, 0);
  auto color = get_param_color(argc, params, 5);
  DrawSplineSegmentCatmullRom(p1, p2, p3, p4, thick, color);
  return 1;
}

//
// Draw spline segment: Linear, 2 points
//
static int cmd_drawsplinesegmentlinear(int argc, slib_par_t *params, var_t *retval) {
  auto p1 = get_param_vec2(argc, params, 0);
  auto p2 = get_param_vec2(argc, params, 1);
  auto thick = get_param_num(argc, params, 2, 0);
  auto color = get_param_color(argc, params, 3);
  DrawSplineSegmentLinear(p1, p2, thick, color);
  return 1;
}

//
// Draw text (using default font)
//
static int cmd_drawtext(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, 0);
  auto posX = get_param_int(argc, params, 1, 0);
  auto posY = get_param_int(argc, params, 2, 0);
  auto fontSize = get_param_int(argc, params, 3, 0);
  auto color = get_param_color(argc, params, 4);
  DrawText(text, posX, posY, fontSize, color);
  return 1;
}

//
// Draw one character (codepoint)
//
static int cmd_drawtextcodepoint(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int font_id = get_font_id(argc, params, 0, retval);
  if (font_id != -1) {
    auto codepoint = get_param_int(argc, params, 1, 0);
    auto position = get_param_vec2(argc, params, 2);
    auto fontSize = get_param_num(argc, params, 3, 0);
    auto tint = get_param_color(argc, params, 4);
    DrawTextCodepoint(_fontMap.at(font_id), codepoint, position, fontSize, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw multiple character (codepoint)
//
static int cmd_drawtextcodepoints(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int font_id = get_font_id(argc, params, 0, retval);
  if (font_id != -1) {
    auto codepoints = (const int *)get_param_int_t(argc, params, 1, 0);
    auto codepointCount = get_param_int(argc, params, 2, 0);
    auto position = get_param_vec2(argc, params, 3);
    auto fontSize = get_param_num(argc, params, 4, 0);
    auto spacing = get_param_num(argc, params, 5, 0);
    auto tint = get_param_color(argc, params, 6);
    DrawTextCodepoints(_fontMap.at(font_id), codepoints, codepointCount, position, fontSize, spacing, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw text using font and additional parameters
//
static int cmd_drawtextex(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int font_id = get_font_id(argc, params, 0, retval);
  if (font_id != -1) {
    auto text = get_param_str(argc, params, 1, 0);
    auto position = get_param_vec2(argc, params, 2);
    auto fontSize = get_param_num(argc, params, 3, 0);
    auto spacing = get_param_num(argc, params, 4, 0);
    auto tint = get_param_color(argc, params, 5);
    DrawTextEx(_fontMap.at(font_id), text, position, fontSize, spacing, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw text using Font and pro parameters (rotation)
//
static int cmd_drawtextpro(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int font_id = get_font_id(argc, params, 0, retval);
  if (font_id != -1) {
    auto text = get_param_str(argc, params, 1, 0);
    auto position = get_param_vec2(argc, params, 2);
    auto origin = get_param_vec2(argc, params, 3);
    auto rotation = get_param_num(argc, params, 4, 0);
    auto fontSize = get_param_num(argc, params, 5, 0);
    auto spacing = get_param_num(argc, params, 6, 0);
    auto tint = get_param_color(argc, params, 7);
    DrawTextPro(_fontMap.at(font_id), text, position, origin, rotation, fontSize, spacing, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw a Texture2D
//
static int cmd_drawtexture(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int texture_id = get_texture_id(argc, params, 0, retval);
  if (texture_id != -1) {
    auto posX = get_param_int(argc, params, 1, 0);
    auto posY = get_param_int(argc, params, 2, 0);
    auto tint = get_param_color(argc, params, 3);
    DrawTexture(_textureMap.at(texture_id), posX, posY, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw a Texture2D with extended parameters
//
static int cmd_drawtextureex(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int texture_id = get_texture_id(argc, params, 0, retval);
  if (texture_id != -1) {
    auto position = get_param_vec2(argc, params, 1);
    auto rotation = get_param_num(argc, params, 2, 0);
    auto scale = get_param_num(argc, params, 3, 0);
    auto tint = get_param_color(argc, params, 4);
    DrawTextureEx(_textureMap.at(texture_id), position, rotation, scale, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draws a texture (or part of it) that stretches or shrinks nicely
//
static int cmd_drawtexturenpatch(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int texture_id = get_texture_id(argc, params, 0, retval);
  if (texture_id != -1) {
    auto nPatchInfo = get_param_npatch(argc, params, 1);
    auto dest = get_param_rect(argc, params, 2);
    auto origin = get_param_vec2(argc, params, 3);
    auto rotation = get_param_num(argc, params, 4, 0);
    auto tint = get_param_color(argc, params, 5);
    DrawTextureNPatch(_textureMap.at(texture_id), nPatchInfo, dest, origin, rotation, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw a part of a texture defined by a rectangle with 'pro' parameters
//
static int cmd_drawtexturepro(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int texture_id = get_texture_id(argc, params, 0, retval);
  if (texture_id != -1) {
    auto source = get_param_rect(argc, params, 1);
    auto dest = get_param_rect(argc, params, 2);
    auto origin = get_param_vec2(argc, params, 3);
    auto rotation = get_param_num(argc, params, 4, 0);
    auto tint = get_param_color(argc, params, 5);
    DrawTexturePro(_textureMap.at(texture_id), source, dest, origin, rotation, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw a part of a texture defined by a rectangle
//
static int cmd_drawtexturerec(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int texture_id = get_texture_id(argc, params, 0, retval);
  if (texture_id != -1) {
    auto source = get_param_rect(argc, params, 1);
    auto position = get_param_vec2(argc, params, 2);
    auto tint = get_param_color(argc, params, 3);
    DrawTextureRec(_textureMap.at(texture_id), source, position, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw a Texture2D with position defined as Vector2
//
static int cmd_drawtexturev(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int texture_id = get_texture_id(argc, params, 0, retval);
  if (texture_id != -1) {
    auto position = get_param_vec2(argc, params, 1);
    auto tint = get_param_color(argc, params, 2);
    DrawTextureV(_textureMap.at(texture_id), position, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw a color-filled triangle (vertex in counter-clockwise order!)
//
static int cmd_drawtriangle(int argc, slib_par_t *params, var_t *retval) {
  auto v1 = get_param_vec2(argc, params, 0);
  auto v2 = get_param_vec2(argc, params, 1);
  auto v3 = get_param_vec2(argc, params, 2);
  auto color = get_param_color(argc, params, 3);
  DrawTriangle(v1, v2, v3, color);
  return 1;
}

//
// Draw a color-filled triangle (vertex in counter-clockwise order!)
//
static int cmd_drawtriangle3d(int argc, slib_par_t *params, var_t *retval) {
  auto v1 = get_param_vec3(argc, params, 0);
  auto v2 = get_param_vec3(argc, params, 1);
  auto v3 = get_param_vec3(argc, params, 2);
  auto color = get_param_color(argc, params, 3);
  DrawTriangle3D(v1, v2, v3, color);
  return 1;
}

//
// Draw a triangle fan defined by points (first vertex is the center)
//
static int cmd_drawtrianglefan(int argc, slib_par_t *params, var_t *retval) {
  auto points = (Vector2 *)get_param_vec2_array(argc, params, 0);
  auto pointCount = get_param_int(argc, params, 1, 0);
  auto color = get_param_color(argc, params, 2);
  DrawTriangleFan(points, pointCount, color);
  return 1;
}

//
// Draw triangle outline (vertex in counter-clockwise order!)
//
static int cmd_drawtrianglelines(int argc, slib_par_t *params, var_t *retval) {
  auto v1 = get_param_vec2(argc, params, 0);
  auto v2 = get_param_vec2(argc, params, 1);
  auto v3 = get_param_vec2(argc, params, 2);
  auto color = get_param_color(argc, params, 3);
  DrawTriangleLines(v1, v2, v3, color);
  return 1;
}

//
// Draw a triangle strip defined by points
//
static int cmd_drawtrianglestrip(int argc, slib_par_t *params, var_t *retval) {
  auto points = (Vector2 *)get_param_vec2_array(argc, params, 0);
  auto pointCount = get_param_int(argc, params, 1, 0);
  auto color = get_param_color(argc, params, 2);
  DrawTriangleStrip(points, pointCount, color);
  return 1;
}

//
// Draw a triangle strip defined by points
//
static int cmd_drawtrianglestrip3d(int argc, slib_par_t *params, var_t *retval) {
  auto points = (Vector3 *)get_param_vec3_array(argc, params, 0);
  auto pointCount = get_param_int(argc, params, 1, 0);
  auto color = get_param_color(argc, params, 2);
  DrawTriangleStrip3D(points, pointCount, color);
  return 1;
}

//
// Enables cursor (unlock cursor)
//
static int cmd_enablecursor(int argc, slib_par_t *params, var_t *retval) {
  EnableCursor();
  return 1;
}

//
// Enable waiting for events on EndDrawing(), no automatic event polling
//
static int cmd_enableeventwaiting(int argc, slib_par_t *params, var_t *retval) {
  EnableEventWaiting();
  return 1;
}

//
// End blending mode (reset to default: alpha blending)
//
static int cmd_endblendmode(int argc, slib_par_t *params, var_t *retval) {
  EndBlendMode();
  return 1;
}

//
// End canvas drawing and swap buffers (double buffering)
//
static int cmd_enddrawing(int argc, slib_par_t *params, var_t *retval) {
  EndDrawing();
  return 1;
}

//
// Ends 2D mode with custom camera
//
static int cmd_endmode2d(int argc, slib_par_t *params, var_t *retval) {
  EndMode2D();
  return 1;
}

//
// Ends 3D mode and returns to default 2D orthographic mode
//
static int cmd_endmode3d(int argc, slib_par_t *params, var_t *retval) {
  EndMode3D();
  return 1;
}

//
// End scissor mode
//
static int cmd_endscissormode(int argc, slib_par_t *params, var_t *retval) {
  EndScissorMode();
  return 1;
}

//
// End custom shader drawing (use default shader)
//
static int cmd_endshadermode(int argc, slib_par_t *params, var_t *retval) {
  EndShaderMode();
  return 1;
}

//
// Ends drawing to render texture
//
static int cmd_endtexturemode(int argc, slib_par_t *params, var_t *retval) {
  EndTextureMode();
  return 1;
}

//
// End stereo rendering (requires VR simulator)
//
static int cmd_endvrstereomode(int argc, slib_par_t *params, var_t *retval) {
  EndVrStereoMode();
  return 1;
}

//
// Compute mesh tangents
//
static int cmd_genmeshtangents(int argc, slib_par_t *params, var_t *retval) {
  auto mesh = (Mesh *)get_param_int_t(argc, params, 0, 0);
  GenMeshTangents(mesh);
  return 1;
}

//
// Generate GPU mipmaps for a texture
//
static int cmd_gentexturemipmaps(int argc, slib_par_t *params, var_t *retval) {
  auto texture = (Texture2D *)get_param_int_t(argc, params, 0, 0);
  GenTextureMipmaps(texture);
  return 1;
}

//
// Hides cursor
//
static int cmd_hidecursor(int argc, slib_par_t *params, var_t *retval) {
  HideCursor();
  return 1;
}

//
// Clear alpha channel to desired color
//
static int cmd_imagealphaclear(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto color = get_param_color(argc, params, 1);
    auto threshold = get_param_num(argc, params, 2, 0);
    ImageAlphaClear(&_imageMap.at(image_id), color, threshold);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Crop image depending on alpha value
//
static int cmd_imagealphacrop(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto threshold = get_param_num(argc, params, 1, 0);
    ImageAlphaCrop(&_imageMap.at(image_id), threshold);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Apply alpha mask to image
//
static int cmd_imagealphamask(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  int alphamask_id = get_image_id(argc, params, 1, retval);
  if (image_id != -1 && alphamask_id != -1) {
    ImageAlphaMask(&_imageMap.at(image_id), _imageMap.at(alphamask_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Premultiply alpha channel
//
static int cmd_imagealphapremultiply(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    ImageAlphaPremultiply(&_imageMap.at(image_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Apply Gaussian blur using a box blur approximation
//
static int cmd_imageblurgaussian(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto blurSize = get_param_int(argc, params, 1, 0);
    ImageBlurGaussian(&_imageMap.at(image_id), blurSize);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Clear image background with given color
//
static int cmd_imageclearbackground(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  if (dst_id != -1) {
    auto color = get_param_color(argc, params, 1);
    ImageClearBackground(&_imageMap.at(dst_id), color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Modify image color: brightness (-255 to 255)
//
static int cmd_imagecolorbrightness(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto brightness = get_param_int(argc, params, 1, 0);
    ImageColorBrightness(&_imageMap.at(image_id), brightness);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Modify image color: contrast (-100 to 100)
//
static int cmd_imagecolorcontrast(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto contrast = get_param_num(argc, params, 1, 0);
    ImageColorContrast(&_imageMap.at(image_id), contrast);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Modify image color: grayscale
//
static int cmd_imagecolorgrayscale(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    ImageColorGrayscale(&_imageMap.at(image_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Modify image color: invert
//
static int cmd_imagecolorinvert(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    ImageColorInvert(&_imageMap.at(image_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Modify image color: replace color
//
static int cmd_imagecolorreplace(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto color = get_param_color(argc, params, 1);
    auto replace = get_param_color(argc, params, 2);
    ImageColorReplace(&_imageMap.at(image_id), color, replace);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Modify image color: tint
//
static int cmd_imagecolortint(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto color = get_param_color(argc, params, 1);
    ImageColorTint(&_imageMap.at(image_id), color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Crop an image to a defined rectangle
//
static int cmd_imagecrop(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto crop = get_param_rect(argc, params, 1);
    ImageCrop(&_imageMap.at(image_id), crop);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Dither image data to 16bpp or lower (Floyd-Steinberg dithering)
//
static int cmd_imagedither(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto rBpp = get_param_int(argc, params, 1, 0);
    auto gBpp = get_param_int(argc, params, 2, 0);
    auto bBpp = get_param_int(argc, params, 3, 0);
    auto aBpp = get_param_int(argc, params, 4, 0);
    ImageDither(&_imageMap.at(image_id), rBpp, gBpp, bBpp, aBpp);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw a source image within a destination image (tint applied to source)
//
static int cmd_imagedraw(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  int src_id = get_image_id(argc, params, 1, retval);
  if (dst_id != -1 && src_id != -1) {
    auto srcRec = get_param_rect(argc, params, 2);
    auto dstRec = get_param_rect(argc, params, 3);
    auto tint = get_param_color(argc, params, 4);
    ImageDraw(&_imageMap.at(dst_id), _imageMap.at(src_id), srcRec, dstRec, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw a filled circle within an image
//
static int cmd_imagedrawcircle(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  if (dst_id != -1) {
    auto centerX = get_param_int(argc, params, 1, 0);
    auto centerY = get_param_int(argc, params, 2, 0);
    auto radius = get_param_int(argc, params, 3, 0);
    auto color = get_param_color(argc, params, 4);
    ImageDrawCircle(&_imageMap.at(dst_id), centerX, centerY, radius, color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw circle outline within an image
//
static int cmd_imagedrawcirclelines(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  if (dst_id != -1) {
    auto centerX = get_param_int(argc, params, 1, 0);
    auto centerY = get_param_int(argc, params, 2, 0);
    auto radius = get_param_int(argc, params, 3, 0);
    auto color = get_param_color(argc, params, 4);
    ImageDrawCircleLines(&_imageMap.at(dst_id), centerX, centerY, radius, color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw circle outline within an image (Vector version)
//
static int cmd_imagedrawcirclelinesv(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  if (dst_id != -1) {
    auto center = get_param_vec2(argc, params, 1);
    auto radius = get_param_int(argc, params, 2, 0);
    auto color = get_param_color(argc, params, 3);
    ImageDrawCircleLinesV(&_imageMap.at(dst_id), center, radius, color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw a filled circle within an image (Vector version)
//
static int cmd_imagedrawcirclev(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  if (dst_id != -1) {
    auto center = get_param_vec2(argc, params, 1);
    auto radius = get_param_int(argc, params, 2, 0);
    auto color = get_param_color(argc, params, 3);
    ImageDrawCircleV(&_imageMap.at(dst_id), center, radius, color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw line within an image
//
static int cmd_imagedrawline(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  if (dst_id != -1) {
    auto startPosX = get_param_int(argc, params, 1, 0);
    auto startPosY = get_param_int(argc, params, 2, 0);
    auto endPosX = get_param_int(argc, params, 3, 0);
    auto endPosY = get_param_int(argc, params, 4, 0);
    auto color = get_param_color(argc, params, 5);
    ImageDrawLine(&_imageMap.at(dst_id), startPosX, startPosY, endPosX, endPosY, color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw a line defining thickness within an image
//
static int cmd_imagedrawlineex(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  if (dst_id != -1) {
    auto start = get_param_vec2(argc, params, 1);
    auto end = get_param_vec2(argc, params, 2);
    auto thick = get_param_int(argc, params, 3, 0);
    auto color = get_param_color(argc, params, 4);
    ImageDrawLineEx(&_imageMap.at(dst_id), start, end, thick, color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw line within an image (Vector version)
//
static int cmd_imagedrawlinev(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  if (dst_id != -1) {
    auto start = get_param_vec2(argc, params, 1);
    auto end = get_param_vec2(argc, params, 2);
    auto color = get_param_color(argc, params, 3);
    ImageDrawLineV(&_imageMap.at(dst_id), start, end, color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw pixel within an image
//
static int cmd_imagedrawpixel(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  if (dst_id != -1) {
    auto posX = get_param_int(argc, params, 1, 0);
    auto posY = get_param_int(argc, params, 2, 0);
    auto color = get_param_color(argc, params, 3);
    ImageDrawPixel(&_imageMap.at(dst_id), posX, posY, color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw pixel within an image (Vector version)
//
static int cmd_imagedrawpixelv(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  if (dst_id != -1) {
    auto position = get_param_vec2(argc, params, 1);
    auto color = get_param_color(argc, params, 2);
    ImageDrawPixelV(&_imageMap.at(dst_id), position, color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw rectangle within an image
//
static int cmd_imagedrawrectangle(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  if (dst_id != -1) {
    auto posX = get_param_int(argc, params, 1, 0);
    auto posY = get_param_int(argc, params, 2, 0);
    auto width = get_param_int(argc, params, 3, 0);
    auto height = get_param_int(argc, params, 4, 0);
    auto color = get_param_color(argc, params, 5);
    ImageDrawRectangle(&_imageMap.at(dst_id), posX, posY, width, height, color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw rectangle lines within an image
//
static int cmd_imagedrawrectanglelines(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  if (dst_id != -1) {
    auto rec = get_param_rect(argc, params, 1);
    auto thick = get_param_int(argc, params, 2, 0);
    auto color = get_param_color(argc, params, 3);
    ImageDrawRectangleLines(&_imageMap.at(dst_id), rec, thick, color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw rectangle within an image
//
static int cmd_imagedrawrectanglerec(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  if (dst_id != -1) {
    auto rec = get_param_rect(argc, params, 1);
    auto color = get_param_color(argc, params, 2);
    ImageDrawRectangleRec(&_imageMap.at(dst_id), rec, color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw rectangle within an image (Vector version)
//
static int cmd_imagedrawrectanglev(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  if (dst_id != -1) {
    auto position = get_param_vec2(argc, params, 1);
    auto size = get_param_vec2(argc, params, 2);
    auto color = get_param_color(argc, params, 3);
    ImageDrawRectangleV(&_imageMap.at(dst_id), position, size, color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw text (using default font) within an image (destination)
//
static int cmd_imagedrawtext(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  if (dst_id != -1) {
    auto text = get_param_str(argc, params, 1, 0);
    auto posX = get_param_int(argc, params, 2, 0);
    auto posY = get_param_int(argc, params, 3, 0);
    auto fontSize = get_param_int(argc, params, 4, 0);
    auto color = get_param_color(argc, params, 5);
    ImageDrawText(&_imageMap.at(dst_id), text, posX, posY, fontSize, color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw text (custom sprite font) within an image (destination)
//
static int cmd_imagedrawtextex(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  int font_id = get_font_id(argc, params, 1, retval);
  if (dst_id != -1 && font_id != -1) {
    auto text = get_param_str(argc, params, 2, 0);
    auto position = get_param_vec2(argc, params, 3);
    auto fontSize = get_param_num(argc, params, 4, 0);
    auto spacing = get_param_num(argc, params, 5, 0);
    auto tint = get_param_color(argc, params, 6);
    ImageDrawTextEx(&_imageMap.at(dst_id), _fontMap.at(font_id), text, position, fontSize, spacing, tint);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw triangle within an image
//
static int cmd_imagedrawtriangle(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  if (dst_id != -1) {
    auto v1 = get_param_vec2(argc, params, 1);
    auto v2 = get_param_vec2(argc, params, 2);
    auto v3 = get_param_vec2(argc, params, 3);
    auto color = get_param_color(argc, params, 4);
    ImageDrawTriangle(&_imageMap.at(dst_id), v1, v2, v3, color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw triangle with interpolated colors within an image
//
static int cmd_imagedrawtriangleex(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  if (dst_id != -1) {
    auto v1 = get_param_vec2(argc, params, 1);
    auto v2 = get_param_vec2(argc, params, 2);
    auto v3 = get_param_vec2(argc, params, 3);
    auto c1 = get_param_color(argc, params, 4);
    auto c2 = get_param_color(argc, params, 5);
    auto c3 = get_param_color(argc, params, 6);
    ImageDrawTriangleEx(&_imageMap.at(dst_id), v1, v2, v3, c1, c2, c3);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw a triangle fan defined by points within an image (first vertex is the center)
//
static int cmd_imagedrawtrianglefan(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  if (dst_id != -1) {
    auto points = (Vector2 *)get_param_vec2_array(argc, params, 1);
    auto pointCount = get_param_int(argc, params, 2, 0);
    auto color = get_param_color(argc, params, 3);
    ImageDrawTriangleFan(&_imageMap.at(dst_id), points, pointCount, color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw triangle outline within an image
//
static int cmd_imagedrawtrianglelines(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  if (dst_id != -1) {
    auto v1 = get_param_vec2(argc, params, 1);
    auto v2 = get_param_vec2(argc, params, 2);
    auto v3 = get_param_vec2(argc, params, 3);
    auto color = get_param_color(argc, params, 4);
    ImageDrawTriangleLines(&_imageMap.at(dst_id), v1, v2, v3, color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Draw a triangle strip defined by points within an image
//
static int cmd_imagedrawtrianglestrip(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int dst_id = get_image_id(argc, params, 0, retval);
  if (dst_id != -1) {
    auto points = (Vector2 *)get_param_vec2_array(argc, params, 1);
    auto pointCount = get_param_int(argc, params, 2, 0);
    auto color = get_param_color(argc, params, 3);
    ImageDrawTriangleStrip(&_imageMap.at(dst_id), points, pointCount, color);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Flip image horizontally
//
static int cmd_imagefliphorizontal(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    ImageFlipHorizontal(&_imageMap.at(image_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Flip image vertically
//
static int cmd_imageflipvertical(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    ImageFlipVertical(&_imageMap.at(image_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Convert image data to desired format
//
static int cmd_imageformat(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto newFormat = get_param_int(argc, params, 1, 0);
    ImageFormat(&_imageMap.at(image_id), newFormat);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Compute all mipmap levels for a provided image
//
static int cmd_imagemipmaps(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    ImageMipmaps(&_imageMap.at(image_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Resize image (Bicubic scaling algorithm)
//
static int cmd_imageresize(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto newWidth = get_param_int(argc, params, 1, 0);
    auto newHeight = get_param_int(argc, params, 2, 0);
    ImageResize(&_imageMap.at(image_id), newWidth, newHeight);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Resize canvas and fill with color
//
static int cmd_imageresizecanvas(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto newWidth = get_param_int(argc, params, 1, 0);
    auto newHeight = get_param_int(argc, params, 2, 0);
    auto offsetX = get_param_int(argc, params, 3, 0);
    auto offsetY = get_param_int(argc, params, 4, 0);
    auto fill = get_param_color(argc, params, 5);
    ImageResizeCanvas(&_imageMap.at(image_id), newWidth, newHeight, offsetX, offsetY, fill);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Resize image (Nearest-Neighbor scaling algorithm)
//
static int cmd_imageresizenn(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto newWidth = get_param_int(argc, params, 1, 0);
    auto newHeight = get_param_int(argc, params, 2, 0);
    ImageResizeNN(&_imageMap.at(image_id), newWidth, newHeight);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Rotate image by input angle in degrees (-359 to 359)
//
static int cmd_imagerotate(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto degrees = get_param_int(argc, params, 1, 0);
    ImageRotate(&_imageMap.at(image_id), degrees);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Rotate image counter-clockwise 90deg
//
static int cmd_imagerotateccw(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    ImageRotateCCW(&_imageMap.at(image_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Rotate image clockwise 90deg
//
static int cmd_imagerotatecw(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    ImageRotateCW(&_imageMap.at(image_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Convert image to POT (power-of-two)
//
static int cmd_imagetopot(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    auto fill = get_param_color(argc, params, 1);
    ImageToPOT(&_imageMap.at(image_id), fill);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Initialize audio device and context
//
static int cmd_initaudiodevice(int argc, slib_par_t *params, var_t *retval) {
  InitAudioDevice();
  return 1;
}

//
// Initialize window and OpenGL context
//
static int cmd_initwindow(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  auto title = get_param_str(argc, params, 2, 0);
  InitWindow(width, height, title);
  return 1;
}

//
// Set window state: maximized, if resizable
//
static int cmd_maximizewindow(int argc, slib_par_t *params, var_t *retval) {
  MaximizeWindow();
  return 1;
}

//
// Internal memory free
//
static int cmd_memfree(int argc, slib_par_t *params, var_t *retval) {
  auto ptr = (void *)get_param_int_t(argc, params, 0, 0);
  MemFree(ptr);
  return 1;
}

//
// Set window state: minimized, if resizable
//
static int cmd_minimizewindow(int argc, slib_par_t *params, var_t *retval) {
  MinimizeWindow();
  return 1;
}

//
// Open URL with default system browser (if available)
//
static int cmd_openurl(int argc, slib_par_t *params, var_t *retval) {
  auto url = get_param_str(argc, params, 0, 0);
  OpenURL(url);
  return 1;
}

//
// Pause audio stream
//
static int cmd_pauseaudiostream(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int stream_id = get_audiostream_id(argc, params, 0, retval);
  if (stream_id != -1) {
    PauseAudioStream(_audioStream.at(stream_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Pause music playing
//
static int cmd_pausemusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int music_id = get_music_id(argc, params, 0, retval);
  if (music_id != -1) {
    PauseMusicStream(_musicMap.at(music_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Pause a sound
//
static int cmd_pausesound(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int sound_id = get_sound_id(argc, params, 0, retval);
  if (sound_id != -1) {
    PauseSound(_soundMap.at(sound_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Play audio stream
//
static int cmd_playaudiostream(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int stream_id = get_audiostream_id(argc, params, 0, retval);
  if (stream_id != -1) {
    PlayAudioStream(_audioStream.at(stream_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Play a recorded automation event
//
static int cmd_playautomationevent(int argc, slib_par_t *params, var_t *retval) {
  auto event = get_param_automationevent(argc, params, 0);
  PlayAutomationEvent(event);
  return 1;
}

//
// Start music playing
//
static int cmd_playmusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int music_id = get_music_id(argc, params, 0, retval);
  if (music_id != -1) {
    PlayMusicStream(_musicMap.at(music_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Play a sound
//
static int cmd_playsound(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int sound_id = get_sound_id(argc, params, 0, retval);
  if (sound_id != -1) {
    PlaySound(_soundMap.at(sound_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Register all input events
//
static int cmd_pollinputevents(int argc, slib_par_t *params, var_t *retval) {
  PollInputEvents();
  return 1;
}

//
// Set window state: not minimized/maximized
//
static int cmd_restorewindow(int argc, slib_par_t *params, var_t *retval) {
  RestoreWindow();
  return 1;
}

//
// Resume audio stream
//
static int cmd_resumeaudiostream(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int stream_id = get_audiostream_id(argc, params, 0, retval);
  if (stream_id != -1) {
    ResumeAudioStream(_audioStream.at(stream_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Resume playing paused music
//
static int cmd_resumemusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int music_id = get_music_id(argc, params, 0, retval);
  if (music_id != -1) {
    ResumeMusicStream(_musicMap.at(music_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Resume a paused sound
//
static int cmd_resumesound(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int sound_id = get_sound_id(argc, params, 0, retval);
  if (sound_id != -1) {
    ResumeSound(_soundMap.at(sound_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Seek music to a position (in seconds)
//
static int cmd_seekmusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int music_id = get_music_id(argc, params, 0, retval);
  if (music_id != -1) {
    auto position = get_param_num(argc, params, 1, 0);
    SeekMusicStream(_musicMap.at(music_id), position);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Default size for new audio streams
//
static int cmd_setaudiostreambuffersizedefault(int argc, slib_par_t *params, var_t *retval) {
  auto size = get_param_int(argc, params, 0, 0);
  SetAudioStreamBufferSizeDefault(size);
  return 1;
}

//
// Set pan for audio stream (0.5 is centered)
//
static int cmd_setaudiostreampan(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int stream_id = get_audiostream_id(argc, params, 0, retval);
  if (stream_id != -1) {
    auto pan = get_param_num(argc, params, 1, 0);
    SetAudioStreamPan(_audioStream.at(stream_id), pan);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Set pitch for audio stream (1.0 is base level)
//
static int cmd_setaudiostreampitch(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int stream_id = get_audiostream_id(argc, params, 0, retval);
  if (stream_id != -1) {
    auto pitch = get_param_num(argc, params, 1, 0);
    SetAudioStreamPitch(_audioStream.at(stream_id), pitch);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Set volume for audio stream (1.0 is max level)
//
static int cmd_setaudiostreamvolume(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int stream_id = get_audiostream_id(argc, params, 0, retval);
  if (stream_id != -1) {
    auto volume = get_param_num(argc, params, 1, 0);
    SetAudioStreamVolume(_audioStream.at(stream_id), volume);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Set automation event internal base frame to start recording
//
static int cmd_setautomationeventbaseframe(int argc, slib_par_t *params, var_t *retval) {
  auto frame = get_param_int(argc, params, 0, 0);
  SetAutomationEventBaseFrame(frame);
  return 1;
}

//
// Set automation event list to record to
//
static int cmd_setautomationeventlist(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int list_id = get_automationeventlist_id(argc, params, 0, retval);
  if (list_id != -1) {
    SetAutomationEventList(&_automationEventListMap.at(list_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Set clipboard text content
//
static int cmd_setclipboardtext(int argc, slib_par_t *params, var_t *retval) {
  auto text = get_param_str(argc, params, 0, 0);
  SetClipboardText(text);
  return 1;
}

//
// Setup init configuration flags (view FLAGS)
//
static int cmd_setconfigflags(int argc, slib_par_t *params, var_t *retval) {
  auto flags = get_param_int(argc, params, 0, 0);
  SetConfigFlags(flags);
  return 1;
}

//
// Set a custom key to exit program (default is ESC)
//
static int cmd_setexitkey(int argc, slib_par_t *params, var_t *retval) {
  auto key = get_param_int(argc, params, 0, 0);
  SetExitKey(key);
  return 1;
}

//
// Set gamepad vibration for both motors (duration in seconds)
//
static int cmd_setgamepadvibration(int argc, slib_par_t *params, var_t *retval) {
  auto gamepad = get_param_int(argc, params, 0, 0);
  auto leftMotor = get_param_num(argc, params, 1, 0);
  auto rightMotor = get_param_num(argc, params, 2, 0);
  auto duration = get_param_num(argc, params, 3, 0);
  SetGamepadVibration(gamepad, leftMotor, rightMotor, duration);
  return 1;
}

//
// Enable a set of gestures using flags
//
static int cmd_setgesturesenabled(int argc, slib_par_t *params, var_t *retval) {
  auto flags = get_param_int(argc, params, 0, 0);
  SetGesturesEnabled(flags);
  return 1;
}

//
// Set master volume (listener)
//
static int cmd_setmastervolume(int argc, slib_par_t *params, var_t *retval) {
  auto volume = get_param_num(argc, params, 0, 0);
  SetMasterVolume(volume);
  return 1;
}

//
// Set material for a mesh
//
static int cmd_setmodelmeshmaterial(int argc, slib_par_t *params, var_t *retval) {
  auto model = (Model *)get_param_int_t(argc, params, 0, 0);
  auto meshId = get_param_int(argc, params, 1, 0);
  auto materialId = get_param_int(argc, params, 2, 0);
  SetModelMeshMaterial(model, meshId, materialId);
  return 1;
}

//
// Set mouse cursor
//
static int cmd_setmousecursor(int argc, slib_par_t *params, var_t *retval) {
  auto cursor = get_param_int(argc, params, 0, 0);
  SetMouseCursor(cursor);
  return 1;
}

//
// Set mouse offset
//
static int cmd_setmouseoffset(int argc, slib_par_t *params, var_t *retval) {
  auto offsetX = get_param_int(argc, params, 0, 0);
  auto offsetY = get_param_int(argc, params, 1, 0);
  SetMouseOffset(offsetX, offsetY);
  return 1;
}

//
// Set mouse position XY
//
static int cmd_setmouseposition(int argc, slib_par_t *params, var_t *retval) {
  auto x = get_param_int(argc, params, 0, 0);
  auto y = get_param_int(argc, params, 1, 0);
  SetMousePosition(x, y);
  return 1;
}

//
// Set mouse scaling
//
static int cmd_setmousescale(int argc, slib_par_t *params, var_t *retval) {
  auto scaleX = get_param_num(argc, params, 0, 0);
  auto scaleY = get_param_num(argc, params, 1, 0);
  SetMouseScale(scaleX, scaleY);
  return 1;
}

//
// Set pan for a music (0.5 is center)
//
static int cmd_setmusicpan(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int music_id = get_music_id(argc, params, 0, retval);
  if (music_id != -1) {
    auto pan = get_param_num(argc, params, 1, 0);
    SetMusicPan(_musicMap.at(music_id), pan);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Set pitch for a music (1.0 is base level)
//
static int cmd_setmusicpitch(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int music_id = get_music_id(argc, params, 0, retval);
  if (music_id != -1) {
    auto pitch = get_param_num(argc, params, 1, 0);
    SetMusicPitch(_musicMap.at(music_id), pitch);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Set volume for music (1.0 is max level)
//
static int cmd_setmusicvolume(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int music_id = get_music_id(argc, params, 0, retval);
  if (music_id != -1) {
    auto volume = get_param_num(argc, params, 1, 0);
    SetMusicVolume(_musicMap.at(music_id), volume);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Set color formatted into destination pixel pointer
//
static int cmd_setpixelcolor(int argc, slib_par_t *params, var_t *retval) {
  auto dstPtr = (void *)get_param_int_t(argc, params, 0, 0);
  auto color = get_param_color(argc, params, 1);
  auto format = get_param_int(argc, params, 2, 0);
  SetPixelColor(dstPtr, color, format);
  return 1;
}

//
// Set the seed for the random number generator
//
static int cmd_setrandomseed(int argc, slib_par_t *params, var_t *retval) {
  auto seed = get_param_int(argc, params, 0, 0);
  SetRandomSeed(seed);
  return 1;
}

//
// Set shader uniform value (matrix 4x4)
//
static int cmd_setshadervaluematrix(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int mat_id = get_matrix_id(argc, params, 2, retval);
  if (mat_id != -1) {
    auto shader = get_param_shader(argc, params, 0);
    auto locIndex = get_param_int(argc, params, 1, 0);
    SetShaderValueMatrix(shader, locIndex, _matrixMap.at(mat_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Set shader uniform value and bind the texture (sampler2d)
//
static int cmd_setshadervaluetexture(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int texture_id = get_texture_id(argc, params, 2, retval);
  if (texture_id != -1) {
    auto shader = get_param_shader(argc, params, 0);
    auto locIndex = get_param_int(argc, params, 1, 0);
    SetShaderValueTexture(shader, locIndex, _textureMap.at(texture_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Set shader uniform value vector
//
static int cmd_setshadervaluev(int argc, slib_par_t *params, var_t *retval) {
  auto shader = get_param_shader(argc, params, 0);
  auto locIndex = get_param_int(argc, params, 1, 0);
  auto value = (const void *)get_param_int_t(argc, params, 2, 0);
  auto uniformType = get_param_int(argc, params, 3, 0);
  auto count = get_param_int(argc, params, 4, 0);
  SetShaderValueV(shader, locIndex, value, uniformType, count);
  return 1;
}

//
// Set texture and rectangle to be used on shapes drawing
//
static int cmd_setshapestexture(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int texture_id = get_texture_id(argc, params, 0, retval);
  if (texture_id != -1) {
    auto source = get_param_rect(argc, params, 1);
    SetShapesTexture(_textureMap.at(texture_id), source);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Set pan for a sound (0.5 is center)
//
static int cmd_setsoundpan(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int sound_id = get_sound_id(argc, params, 0, retval);
  if (sound_id != -1) {
    auto pan = get_param_num(argc, params, 1, 0);
    SetSoundPan(_soundMap.at(sound_id), pan);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Set pitch for a sound (1.0 is base level)
//
static int cmd_setsoundpitch(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int sound_id = get_sound_id(argc, params, 0, retval);
  if (sound_id != -1) {
    auto pitch = get_param_num(argc, params, 1, 0);
    SetSoundPitch(_soundMap.at(sound_id), pitch);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Set volume for a sound (1.0 is max level)
//
static int cmd_setsoundvolume(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int sound_id = get_sound_id(argc, params, 0, retval);
  if (sound_id != -1) {
    auto volume = get_param_num(argc, params, 1, 0);
    SetSoundVolume(_soundMap.at(sound_id), volume);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Set target FPS (maximum)
//
static int cmd_settargetfps(int argc, slib_par_t *params, var_t *retval) {
  auto fps = get_param_int(argc, params, 0, 0);
  SetTargetFPS(fps);
  return 1;
}

//
// Set vertical line spacing when drawing with line-breaks
//
static int cmd_settextlinespacing(int argc, slib_par_t *params, var_t *retval) {
  auto spacing = get_param_int(argc, params, 0, 0);
  SetTextLineSpacing(spacing);
  return 1;
}

//
// Set texture scaling filter mode
//
static int cmd_settexturefilter(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int texture_id = get_texture_id(argc, params, 0, retval);
  if (texture_id != -1) {
    auto filter = get_param_int(argc, params, 1, 0);
    SetTextureFilter(_textureMap.at(texture_id), filter);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Set texture wrapping mode
//
static int cmd_settexturewrap(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int texture_id = get_texture_id(argc, params, 0, retval);
  if (texture_id != -1) {
    auto wrap = get_param_int(argc, params, 1, 0);
    SetTextureWrap(_textureMap.at(texture_id), wrap);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Set the current threshold (minimum) log level
//
static int cmd_settraceloglevel(int argc, slib_par_t *params, var_t *retval) {
  auto logLevel = get_param_int(argc, params, 0, 0);
  SetTraceLogLevel(logLevel);
  return 1;
}

//
// Set window focused
//
static int cmd_setwindowfocused(int argc, slib_par_t *params, var_t *retval) {
  SetWindowFocused();
  return 1;
}

//
// Set icon for window (single image, RGBA 32bit)
//
static int cmd_setwindowicon(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    SetWindowIcon(_imageMap.at(image_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Set icon for window (multiple images, RGBA 32bit)
//
static int cmd_setwindowicons(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int images_id = get_image_id(argc, params, 0, retval);
  if (images_id != -1) {
    auto count = get_param_int(argc, params, 1, 0);
    SetWindowIcons(&_imageMap.at(images_id), count);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Set window maximum dimensions (for FLAG_WINDOW_RESIZABLE)
//
static int cmd_setwindowmaxsize(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  SetWindowMaxSize(width, height);
  return 1;
}

//
// Set window minimum dimensions (for FLAG_WINDOW_RESIZABLE)
//
static int cmd_setwindowminsize(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  SetWindowMinSize(width, height);
  return 1;
}

//
// Set monitor for the current window
//
static int cmd_setwindowmonitor(int argc, slib_par_t *params, var_t *retval) {
  auto monitor = get_param_int(argc, params, 0, 0);
  SetWindowMonitor(monitor);
  return 1;
}

//
// Set window opacity [0.0f..1.0f]
//
static int cmd_setwindowopacity(int argc, slib_par_t *params, var_t *retval) {
  auto opacity = get_param_num(argc, params, 0, 0);
  SetWindowOpacity(opacity);
  return 1;
}

//
// Set window position on screen
//
static int cmd_setwindowposition(int argc, slib_par_t *params, var_t *retval) {
  auto x = get_param_int(argc, params, 0, 0);
  auto y = get_param_int(argc, params, 1, 0);
  SetWindowPosition(x, y);
  return 1;
}

//
// Set window dimensions
//
static int cmd_setwindowsize(int argc, slib_par_t *params, var_t *retval) {
  auto width = get_param_int(argc, params, 0, 0);
  auto height = get_param_int(argc, params, 1, 0);
  SetWindowSize(width, height);
  return 1;
}

//
// Set window configuration state using flags
//
static int cmd_setwindowstate(int argc, slib_par_t *params, var_t *retval) {
  auto flags = get_param_int(argc, params, 0, 0);
  SetWindowState(flags);
  return 1;
}

//
// Set title for window
//
static int cmd_setwindowtitle(int argc, slib_par_t *params, var_t *retval) {
  auto title = get_param_str(argc, params, 0, 0);
  SetWindowTitle(title);
  return 1;
}

//
// Shows cursor
//
static int cmd_showcursor(int argc, slib_par_t *params, var_t *retval) {
  ShowCursor();
  return 1;
}

//
// Start recording automation events (AutomationEventList must be set)
//
static int cmd_startautomationeventrecording(int argc, slib_par_t *params, var_t *retval) {
  StartAutomationEventRecording();
  return 1;
}

//
// Stop audio stream
//
static int cmd_stopaudiostream(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int stream_id = get_audiostream_id(argc, params, 0, retval);
  if (stream_id != -1) {
    StopAudioStream(_audioStream.at(stream_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Stop recording automation events
//
static int cmd_stopautomationeventrecording(int argc, slib_par_t *params, var_t *retval) {
  StopAutomationEventRecording();
  return 1;
}

//
// Stop music playing
//
static int cmd_stopmusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int music_id = get_music_id(argc, params, 0, retval);
  if (music_id != -1) {
    StopMusicStream(_musicMap.at(music_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Stop playing a sound
//
static int cmd_stopsound(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int sound_id = get_sound_id(argc, params, 0, retval);
  if (sound_id != -1) {
    StopSound(_soundMap.at(sound_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Swap back buffer with front buffer (screen drawing)
//
static int cmd_swapscreenbuffer(int argc, slib_par_t *params, var_t *retval) {
  SwapScreenBuffer();
  return 1;
}

//
// Takes a screenshot of current screen (filename extension defines format)
//
static int cmd_takescreenshot(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, 0);
  TakeScreenshot(fileName);
  return 1;
}

//
// Append text at specific position and move cursor!
//
static int cmd_textappend(int argc, slib_par_t *params, var_t *retval) {
  auto text = (char *)get_param_str(argc, params, 0, 0);
  auto append = get_param_str(argc, params, 1, 0);
  auto position = (int *)0;
  TextAppend(text, append, position);
  return 1;
}

//
// Toggle window state: borderless windowed, resizes window to match monitor resolution
//
static int cmd_toggleborderlesswindowed(int argc, slib_par_t *params, var_t *retval) {
  ToggleBorderlessWindowed();
  return 1;
}

//
// Toggle window state: fullscreen/windowed, resizes monitor to match window resolution
//
static int cmd_togglefullscreen(int argc, slib_par_t *params, var_t *retval) {
  ToggleFullscreen();
  return 1;
}

//
// Unload audio stream and free memory
//
static int cmd_unloadaudiostream(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int stream_id = get_audiostream_id(argc, params, 0, retval);
  if (stream_id != -1) {
    UnloadAudioStream(_audioStream.at(stream_id));
    _audioStream.erase(stream_id);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Unload automation events list from file
//
static int cmd_unloadautomationeventlist(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int list_id = get_automationeventlist_id(argc, params, 0, retval);
  if (list_id != -1) {
    UnloadAutomationEventList(_automationEventListMap.at(list_id));
    _automationEventListMap.erase(list_id);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Unload codepoints data from memory
//
static int cmd_unloadcodepoints(int argc, slib_par_t *params, var_t *retval) {
  auto codepoints = (int *)0;
  UnloadCodepoints(codepoints);
  return 1;
}

//
// Unload filepaths
//
static int cmd_unloaddirectoryfiles(int argc, slib_par_t *params, var_t *retval) {
  auto files = get_param_filepathlist(argc, params, 0);
  UnloadDirectoryFiles(files);
  return 1;
}

//
// Unload file data allocated by LoadFileData()
//
static int cmd_unloadfiledata(int argc, slib_par_t *params, var_t *retval) {
  auto data = (unsigned char *)get_param_str(argc, params, 0, 0);
  UnloadFileData(data);
  return 1;
}

//
// Unload file text data allocated by LoadFileText()
//
static int cmd_unloadfiletext(int argc, slib_par_t *params, var_t *retval) {
  auto text = (char *)get_param_str(argc, params, 0, 0);
  UnloadFileText(text);
  return 1;
}

//
// Unload font from GPU memory (VRAM)
//
static int cmd_unloadfont(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int font_id = get_font_id(argc, params, 0, retval);
  if (font_id != -1) {
    UnloadFont(_fontMap.at(font_id));
    _fontMap.erase(font_id);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Unload image from CPU memory (RAM)
//
static int cmd_unloadimage(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int image_id = get_image_id(argc, params, 0, retval);
  if (image_id != -1) {
    UnloadImage(_imageMap.at(image_id));
    _imageMap.erase(image_id);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Unload color data loaded with LoadImageColors()
//
static int cmd_unloadimagecolors(int argc, slib_par_t *params, var_t *retval) {
  auto colors = (Color *)get_param_int_t(argc, params, 0, 0);
  UnloadImageColors(colors);
  return 1;
}

//
// Unload colors palette loaded with LoadImagePalette()
//
static int cmd_unloadimagepalette(int argc, slib_par_t *params, var_t *retval) {
  auto colors = (Color *)get_param_int_t(argc, params, 0, 0);
  UnloadImagePalette(colors);
  return 1;
}

//
// Unload mesh data from CPU and GPU
//
static int cmd_unloadmesh(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int mesh_id = get_mesh_id(argc, params, 0, retval);
  if (mesh_id != -1) {
    UnloadMesh(_meshMap.at(mesh_id));
    _meshMap.erase(mesh_id);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Unload model (including meshes) from memory (RAM and/or VRAM)
//
static int cmd_unloadmodel(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int model_id = get_model_id(argc, params, 0, retval);
  if (model_id != -1) {
    UnloadModel(_modelMap.at(model_id));
    _modelMap.erase(model_id);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Unload animation data
//
static int cmd_unloadmodelanimation(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int anim_id = get_model_animation_id(argc, params, 0, retval);
  if (anim_id != -1) {
    UnloadModelAnimation(_modelAnimationMap.at(anim_id));
    _modelAnimationMap.erase(anim_id);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Unload animation array data
//
static int cmd_unloadmodelanimations(int argc, slib_par_t *params, var_t *retval) {
  auto animations = (ModelAnimation *)get_param_int_t(argc, params, 0, 0);
  auto animCount = get_param_int(argc, params, 1, 0);
  UnloadModelAnimations(animations, animCount);
  return 1;
}

//
// Unload music stream
//
static int cmd_unloadmusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int music_id = get_music_id(argc, params, 0, retval);
  if (music_id != -1) {
    UnloadMusicStream(_musicMap.at(music_id));
    _musicMap.erase(music_id);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Unload random values sequence
//
static int cmd_unloadrandomsequence(int argc, slib_par_t *params, var_t *retval) {
  auto sequence = (int *)0;
  UnloadRandomSequence(sequence);
  return 1;
}

//
// Unload render texture from GPU memory (VRAM)
//
static int cmd_unloadrendertexture(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int target_id = get_render_texture_id(argc, params, 0, retval);
  if (target_id != -1) {
    UnloadRenderTexture(_renderMap.at(target_id));
    _renderMap.erase(target_id);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Unload shader from GPU memory (VRAM)
//
static int cmd_unloadshader(int argc, slib_par_t *params, var_t *retval) {
  auto shader = get_param_shader(argc, params, 0);
  UnloadShader(shader);
  return 1;
}

//
// Unload sound
//
static int cmd_unloadsound(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int sound_id = get_sound_id(argc, params, 0, retval);
  if (sound_id != -1) {
    UnloadSound(_soundMap.at(sound_id));
    _soundMap.erase(sound_id);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Unload a sound alias (does not deallocate sample data)
//
static int cmd_unloadsoundalias(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int alias_id = get_sound_id(argc, params, 0, retval);
  if (alias_id != -1) {
    UnloadSoundAlias(_soundMap.at(alias_id));
    _soundMap.erase(alias_id);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Unload texture from GPU memory (VRAM)
//
static int cmd_unloadtexture(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int texture_id = get_texture_id(argc, params, 0, retval);
  if (texture_id != -1) {
    UnloadTexture(_textureMap.at(texture_id));
    _textureMap.erase(texture_id);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Unload UTF-8 text encoded from codepoints array
//
static int cmd_unloadutf8(int argc, slib_par_t *params, var_t *retval) {
  auto text = (char *)get_param_str(argc, params, 0, 0);
  UnloadUTF8(text);
  return 1;
}

//
// Unload wave data
//
static int cmd_unloadwave(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int wave_id = get_wave_id(argc, params, 0, retval);
  if (wave_id != -1) {
    UnloadWave(_waveMap.at(wave_id));
    _waveMap.erase(wave_id);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Unload samples data loaded with LoadWaveSamples()
//
static int cmd_unloadwavesamples(int argc, slib_par_t *params, var_t *retval) {
  auto samples = (float *)get_param_int_t(argc, params, 0, 0);
  UnloadWaveSamples(samples);
  return 1;
}

//
// Update audio stream buffers with data
//
static int cmd_updateaudiostream(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int stream_id = get_audiostream_id(argc, params, 0, retval);
  if (stream_id != -1) {
    auto data = (const void *)get_param_int_t(argc, params, 1, 0);
    auto frameCount = get_param_int(argc, params, 2, 0);
    UpdateAudioStream(_audioStream.at(stream_id), data, frameCount);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Update mesh vertex data in GPU for a specific buffer index
//
static int cmd_updatemeshbuffer(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int mesh_id = get_mesh_id(argc, params, 0, retval);
  if (mesh_id != -1) {
    auto index = get_param_int(argc, params, 1, 0);
    auto data = (const void *)get_param_int_t(argc, params, 2, 0);
    auto dataSize = get_param_int(argc, params, 3, 0);
    auto offset = get_param_int(argc, params, 4, 0);
    UpdateMeshBuffer(_meshMap.at(mesh_id), index, data, dataSize, offset);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Update model animation pose (CPU)
//
static int cmd_updatemodelanimation(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int model_id = get_model_id(argc, params, 0, retval);
  int anim_id = get_model_animation_id(argc, params, 1, retval);
  if (model_id != -1 && anim_id != -1) {
    auto frame = get_param_int(argc, params, 2, 0);
    UpdateModelAnimation(_modelMap.at(model_id), _modelAnimationMap.at(anim_id), frame);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Update model animation mesh bone matrices (GPU skinning)
//
static int cmd_updatemodelanimationbones(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int model_id = get_model_id(argc, params, 0, retval);
  int anim_id = get_model_animation_id(argc, params, 1, retval);
  if (model_id != -1 && anim_id != -1) {
    auto frame = get_param_int(argc, params, 2, 0);
    UpdateModelAnimationBones(_modelMap.at(model_id), _modelAnimationMap.at(anim_id), frame);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Updates buffers for music streaming
//
static int cmd_updatemusicstream(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int music_id = get_music_id(argc, params, 0, retval);
  if (music_id != -1) {
    UpdateMusicStream(_musicMap.at(music_id));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Update sound buffer with new data
//
static int cmd_updatesound(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int sound_id = get_sound_id(argc, params, 0, retval);
  if (sound_id != -1) {
    auto data = (const void *)get_param_int_t(argc, params, 1, 0);
    auto sampleCount = get_param_int(argc, params, 2, 0);
    UpdateSound(_soundMap.at(sound_id), data, sampleCount);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Update GPU texture rectangle with new data
//
static int cmd_updatetexturerec(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int texture_id = get_texture_id(argc, params, 0, retval);
  if (texture_id != -1) {
    auto rec = get_param_rect(argc, params, 1);
    auto pixels = (const void *)get_param_int_t(argc, params, 2, 0);
    UpdateTextureRec(_textureMap.at(texture_id), rec, pixels);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

//
// Upload mesh vertex data in GPU and provide VAO/VBO ids
//
static int cmd_uploadmesh(int argc, slib_par_t *params, var_t *retval) {
  auto mesh = (Mesh *)get_param_int_t(argc, params, 0, 0);
  auto dynamic = get_param_int(argc, params, 1, 0);
  UploadMesh(mesh, dynamic);
  return 1;
}

//
// Wait for some time (halt program execution)
//
static int cmd_waittime(int argc, slib_par_t *params, var_t *retval) {
  auto seconds = get_param_num(argc, params, 0, 0);
  WaitTime(seconds);
  return 1;
}

//
// Crop a wave to defined frames range
//
static int cmd_wavecrop(int argc, slib_par_t *params, var_t *retval) {
  auto wave = (Wave *)get_param_int_t(argc, params, 0, 0);
  auto initFrame = get_param_int(argc, params, 1, 0);
  auto finalFrame = get_param_int(argc, params, 2, 0);
  WaveCrop(wave, initFrame, finalFrame);
  return 1;
}

//
// Convert wave data to desired format
//
static int cmd_waveformat(int argc, slib_par_t *params, var_t *retval) {
  auto wave = (Wave *)get_param_int_t(argc, params, 0, 0);
  auto sampleRate = get_param_int(argc, params, 1, 0);
  auto sampleSize = get_param_int(argc, params, 2, 0);
  auto channels = get_param_int(argc, params, 3, 0);
  WaveFormat(wave, sampleRate, sampleSize, channels);
  return 1;
}

