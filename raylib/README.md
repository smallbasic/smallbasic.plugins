*Raylib* 4.1-dev
=======
raylib is a simple and easy-to-use library to enjoy videogames programming.

https://www.raylib.com/

Implemented APIs (553)
----------------

| Name    | Description   |
|---------|---------------|
| sub BeginBlendMode(mode) | Begin blending mode (alpha, additive, multiplied, subtract, custom) |
| sub BeginDrawing() | Setup canvas (framebuffer) to start drawing |
| sub BeginMode2D(camera) | Begin 2D mode with custom camera (2D) |
| sub BeginMode3D(camera) | Begin 3D mode with custom camera (3D) |
| sub BeginScissorMode(x, y, width, height) | Begin scissor mode (define screen area for following drawing) |
| sub BeginShaderMode(shader) | Begin custom shader drawing |
| sub BeginTextureMode(target) | Begin drawing to render texture |
| func ChangeDirectory(dir) | Change working directory, return true on success |
| func CheckCollisionBoxes(box1, box2) | Check collision between two bounding boxes |
| func CheckCollisionBoxSphere(box, center, radius) | Check collision between box and sphere |
| func CheckCollisionCircleRec(center, radius, rec) | Check collision between circle and rectangle |
| func CheckCollisionCircles(center1, radius1, center2, radius2) | Check collision between two circles |
| func CheckCollisionLines(startPos1, endPos1, startPos2, endPos2, collisionPoint) | Check the collision between two lines defined by two points each, returns collision point by reference |
| func CheckCollisionPointCircle(point, center, radius) | Check if point is inside circle |
| func CheckCollisionPointLine(point, p1, p2, threshold) | Check if point belongs to line created between two points [p1] and [p2] with defined margin in pixels [threshold] |
| func CheckCollisionPointRec(point, rec) | Check if point is inside rectangle |
| func CheckCollisionPointTriangle(point, p1, p2, p3) | Check if point is inside a triangle |
| func CheckCollisionRecs(rec1, rec2) | Check collision between two rectangles |
| func CheckCollisionSpheres(center1, radius1, center2, radius2) | Check collision between two spheres |
| sub ClearBackground(color) | Set background color (framebuffer clear color) |
| sub ClearDirectoryFiles() | Clear directory files paths buffers (free memory) |
| sub ClearDroppedFiles() | Clear dropped files paths buffer (free memory) |
| sub ClearWindowState(flags) | Clear window configuration state flags |
| sub CloseAudioDevice() | Close the audio device and context |
| func closePhysics() | n/a |
| sub CloseWindow() | Close window and unload OpenGL context |
| func CodepointToUTF8(codepoint, byteSize) | Encode one codepoint into UTF-8 byte array (array length returned as parameter) |
| func ColorAlpha(color, alpha) | Get color with alpha applied, alpha goes from 0.0f to 1.0f |
| func ColorAlphaBlend(dst, src, tint) | Get src alpha-blended into dst color with tint |
| func ColorFromHSV(hue, saturation, value) | Get a Color from HSV values, hue [0..360], saturation/value [0..1] |
| func ColorFromNormalized(normalized) | Get Color from normalized values [0..1] |
| func ColorNormalize(color) | Get Color normalized as float [0..1] |
| func ColorToHSV(color) | Get HSV values for a Color, hue [0..360], saturation/value [0..1] |
| func ColorToInt(color) | Get hexadecimal value for a Color |
| func CompressData(data, dataSize, compDataSize) | Compress data (DEFLATE algorithm), memory must be MemFree() |
| func createPhysicsbodycircle() | n/a |
| func createPhysicsbodypolygon() | n/a |
| func createPhysicsbodyrectangle() | n/a |
| func DecodeDataBase64(data, outputSize) | Decode Base64 string data, memory must be MemFree() |
| func DecompressData(compData, compDataSize, dataSize) | Decompress data (DEFLATE algorithm), memory must be MemFree() |
| func destroyPhysicsbody() | n/a |
| func DirectoryExists(dirPath) | Check if a directory path exists |
| sub DisableCursor() | Disables cursor (lock cursor) |
| sub DrawBillboard(camera, texture, position, size, tint) | Draw a billboard texture |
| sub DrawBillboardPro(camera, texture, source, position, up, size, origin, rotation, tint) | Draw a billboard texture defined by source and rotation |
| sub DrawBillboardRec(camera, texture, source, position, size, tint) | Draw a billboard texture defined by source |
| sub DrawBoundingBox(box, color) | Draw bounding box (wires) |
| sub DrawCircle(centerX, centerY, radius, color) | Draw a color-filled circle |
| sub DrawCircle3D(center, radius, rotationAxis, rotationAngle, color) | Draw a circle in 3D world space |
| sub DrawCircleGradient(centerX, centerY, radius, color1, color2) | Draw a gradient-filled circle |
| sub DrawCircleLines(centerX, centerY, radius, color) | Draw circle outline |
| sub DrawCircleSector(center, radius, startAngle, endAngle, segments, color) | Draw a piece of a circle |
| sub DrawCircleSectorLines(center, radius, startAngle, endAngle, segments, color) | Draw circle sector outline |
| sub DrawCircleV(center, radius, color) | Draw a color-filled circle (Vector version) |
| sub DrawCube(position, width, height, length, color) | Draw cube |
| sub DrawCubeTexture(texture, position, width, height, length, color) | Draw cube textured |
| sub DrawCubeTextureRec(texture, source, position, width, height, length, color) | Draw cube with a region of a texture |
| sub DrawCubeV(position, size, color) | Draw cube (Vector version) |
| sub DrawCubeWires(position, width, height, length, color) | Draw cube wires |
| sub DrawCubeWiresV(position, size, color) | Draw cube wires (Vector version) |
| sub DrawCylinder(position, radiusTop, radiusBottom, height, slices, color) | Draw a cylinder/cone |
| sub DrawCylinderEx(startPos, endPos, startRadius, endRadius, sides, color) | Draw a cylinder with base at startPos and top at endPos |
| sub DrawCylinderWires(position, radiusTop, radiusBottom, height, slices, color) | Draw a cylinder/cone wires |
| sub DrawCylinderWiresEx(startPos, endPos, startRadius, endRadius, sides, color) | Draw a cylinder wires with base at startPos and top at endPos |
| sub DrawEllipse(centerX, centerY, radiusH, radiusV, color) | Draw ellipse |
| sub DrawEllipseLines(centerX, centerY, radiusH, radiusV, color) | Draw ellipse outline |
| sub DrawFPS(posX, posY) | Draw current FPS |
| sub DrawGrid(slices, spacing) | Draw a grid (centered at (0, 0, 0)) |
| sub DrawLine(startPosX, startPosY, endPosX, endPosY, color) | Draw a line |
| sub DrawLine3D(startPos, endPos, color) | Draw a line in 3D world space |
| sub DrawLineBezier(startPos, endPos, thick, color) | Draw a line using cubic-bezier curves in-out |
| sub DrawLineBezierCubic(startPos, endPos, startControlPos, endControlPos, thick, color) | Draw line using cubic bezier curves with 2 control points |
| sub DrawLineBezierQuad(startPos, endPos, controlPos, thick, color) | Draw line using quadratic bezier curves with a control point |
| sub DrawLineEx(startPos, endPos, thick, color) | Draw a line defining thickness |
| sub DrawLineStrip(points, pointCount, color) | Draw lines sequence |
| sub DrawLineV(startPos, endPos, color) | Draw a line (Vector version) |
| sub DrawModel(model, position, scale, tint) | Draw a model (with texture if set) |
| sub DrawModelEx(model, position, rotationAxis, rotationAngle, scale, tint) | Draw a model with extended parameters |
| sub DrawModelWires(model, position, scale, tint) | Draw a model wires (with texture if set) |
| sub DrawModelWiresEx(model, position, rotationAxis, rotationAngle, scale, tint) | Draw a model wires (with texture if set) with extended parameters |
| sub DrawPixel(posX, posY, color) | Draw a pixel |
| sub DrawPixelV(position, color) | Draw a pixel (Vector version) |
| sub DrawPlane(centerPos, size, color) | Draw a plane XZ |
| sub DrawPoint3D(position, color) | Draw a point in 3D space, actually a small line |
| sub DrawPoly(center, sides, radius, rotation, color) | Draw a regular polygon (Vector version) |
| sub DrawPolyLines(center, sides, radius, rotation, color) | Draw a polygon outline of n sides |
| sub DrawPolyLinesEx(center, sides, radius, rotation, lineThick, color) | Draw a polygon outline of n sides with extended parameters |
| sub DrawRay(ray, color) | Draw a ray line |
| sub DrawRectangle(posX, posY, width, height, color) | Draw a color-filled rectangle |
| sub DrawRectangleGradientEx(rec, col1, col2, col3, col4) | Draw a gradient-filled rectangle with custom vertex colors |
| sub DrawRectangleGradientH(posX, posY, width, height, color1, color2) | Draw a horizontal-gradient-filled rectangle |
| sub DrawRectangleGradientV(posX, posY, width, height, color1, color2) | Draw a vertical-gradient-filled rectangle |
| sub DrawRectangleLines(posX, posY, width, height, color) | Draw rectangle outline |
| sub DrawRectangleLinesEx(rec, lineThick, color) | Draw rectangle outline with extended parameters |
| sub DrawRectanglePro(rec, origin, rotation, color) | Draw a color-filled rectangle with pro parameters |
| sub DrawRectangleRec(rec, color) | Draw a color-filled rectangle |
| sub DrawRectangleRounded(rec, roundness, segments, color) | Draw rectangle with rounded edges |
| sub DrawRectangleRoundedLines(rec, roundness, segments, lineThick, color) | Draw rectangle with rounded edges outline |
| sub DrawRectangleV(position, size, color) | Draw a color-filled rectangle (Vector version) |
| sub DrawRing(center, innerRadius, outerRadius, startAngle, endAngle, segments, color) | Draw ring |
| sub DrawRingLines(center, innerRadius, outerRadius, startAngle, endAngle, segments, color) | Draw ring outline |
| sub DrawSphere(centerPos, radius, color) | Draw sphere |
| sub DrawSphereEx(centerPos, radius, rings, slices, color) | Draw sphere with extended parameters |
| sub DrawSphereWires(centerPos, radius, rings, slices, color) | Draw sphere wires |
| sub DrawText(text, posX, posY, fontSize, color) | Draw text (using default font) |
| sub DrawTextCodepoint(font, codepoint, position, fontSize, tint) | Draw one character (codepoint) |
| sub DrawTextCodepoints(font, codepoints, count, position, fontSize, spacing, tint) | Draw multiple character (codepoint) |
| sub DrawTextEx(font, text, position, fontSize, spacing, tint) | Draw text using font and additional parameters |
| sub DrawTextPro(font, text, position, origin, rotation, fontSize, spacing, tint) | Draw text using Font and pro parameters (rotation) |
| sub DrawTexture(texture, posX, posY, tint) | Draw a Texture2D |
| sub DrawTextureEx(texture, position, rotation, scale, tint) | Draw a Texture2D with extended parameters |
| sub DrawTextureNPatch(texture, nPatchInfo, dest, origin, rotation, tint) | Draws a texture (or part of it) that stretches or shrinks nicely |
| sub DrawTexturePoly(texture, center, points, texcoords, pointCount, tint) | Draw a textured polygon |
| sub DrawTexturePro(texture, source, dest, origin, rotation, tint) | Draw a part of a texture defined by a rectangle with 'pro' parameters |
| sub DrawTextureQuad(texture, tiling, offset, quad, tint) | Draw texture quad with tiling and offset parameters |
| sub DrawTextureRec(texture, source, position, tint) | Draw a part of a texture defined by a rectangle |
| sub DrawTextureTiled(texture, source, dest, origin, rotation, scale, tint) | Draw part of a texture (defined by a rectangle) with rotation and scale tiled into dest. |
| sub DrawTextureV(texture, position, tint) | Draw a Texture2D with position defined as Vector2 |
| sub DrawTriangle(v1, v2, v3, color) | Draw a color-filled triangle (vertex in counter-clockwise order!) |
| sub DrawTriangle3D(v1, v2, v3, color) | Draw a color-filled triangle (vertex in counter-clockwise order!) |
| sub DrawTriangleFan(points, pointCount, color) | Draw a triangle fan defined by points (first vertex is the center) |
| sub DrawTriangleLines(v1, v2, v3, color) | Draw triangle outline (vertex in counter-clockwise order!) |
| sub DrawTriangleStrip(points, pointCount, color) | Draw a triangle strip defined by points |
| sub DrawTriangleStrip3D(points, pointCount, color) | Draw a triangle strip defined by points |
| sub EnableCursor() | Enables cursor (unlock cursor) |
| func EncodeDataBase64(data, dataSize, outputSize) | Encode data to Base64 string, memory must be MemFree() |
| sub EndBlendMode() | End blending mode (reset to default: alpha blending) |
| sub EndDrawing() | End canvas drawing and swap buffers (double buffering) |
| sub EndMode2D() | Ends 2D mode with custom camera |
| sub EndMode3D() | Ends 3D mode and returns to default 2D orthographic mode |
| sub EndScissorMode() | End scissor mode |
| sub EndShaderMode() | End custom shader drawing (use default shader) |
| sub EndTextureMode() | Ends drawing to render texture |
| sub EndVrStereoMode() | End stereo rendering (requires VR simulator) |
| func ExportFontAsCode(font, fileName) | Export font as code file, returns true on success |
| func ExportImage(image, fileName) | Export image data to file, returns true on success |
| func ExportImageAsCode(image, fileName) | Export image as code file defining an array of bytes, returns true on success |
| func ExportMesh(mesh, fileName) | Export mesh data to file, returns true on success |
| func ExportWave(wave, fileName) | Export wave data to file, returns true on success |
| func ExportWaveAsCode(wave, fileName) | Export wave sample data to code (.h), returns true on success |
| func Fade(color, alpha) | Get color with alpha applied, alpha goes from 0.0f to 1.0f |
| func FileExists(fileName) | Check if file exists |
| func GenImageCellular(width, height, tileSize) | Generate image: cellular algorithm, bigger tileSize means bigger cells |
| func GenImageChecked(width, height, checksX, checksY, col1, col2) | Generate image: checked |
| func GenImageColor(width, height, color) | Generate image: plain color |
| func GenImageGradientH(width, height, left, right) | Generate image: horizontal gradient |
| func GenImageGradientRadial(width, height, density, inner, outer) | Generate image: radial gradient |
| func GenImageGradientV(width, height, top, bottom) | Generate image: vertical gradient |
| func GenImageWhiteNoise(width, height, factor) | Generate image: white noise |
| sub GenMeshBinormals(mesh) | Compute mesh binormals |
| func GenMeshCone(radius, height, slices) | Generate cone/pyramid mesh |
| func GenMeshCube(width, height, length) | Generate cuboid mesh |
| func GenMeshCubicmap(cubicmap, cubeSize) | Generate cubes-based map mesh from image data |
| func GenMeshCylinder(radius, height, slices) | Generate cylinder mesh |
| func GenMeshHeightmap(heightmap, size) | Generate heightmap mesh from image data |
| func GenMeshHemiSphere(radius, rings, slices) | Generate half-sphere mesh (no bottom cap) |
| func GenMeshKnot(radius, size, radSeg, sides) | Generate trefoil knot mesh |
| func GenMeshPlane(width, length, resX, resZ) | Generate plane mesh (with subdivisions) |
| func GenMeshPoly(sides, radius) | Generate polygonal mesh |
| func GenMeshSphere(radius, rings, slices) | Generate sphere mesh (standard sphere) |
| sub GenMeshTangents(mesh) | Compute mesh tangents |
| func GenMeshTorus(radius, size, radSeg, sides) | Generate torus mesh |
| sub GenTextureMipmaps(texture) | Generate GPU mipmaps for a texture |
| func GetApplicationDirectory() | Get the directory if the running application (uses static string) |
| func GetCameraMatrix(camera) | Get camera transform matrix (view matrix) |
| func GetCameraMatrix2D(camera) | Get camera 2d transform matrix |
| func GetCharPressed() | Get char pressed (unicode), call it multiple times for chars queued, returns 0 when the queue is empty |
| func GetClipboardText() | Get clipboard text content |
| func GetCodepoint(text, bytesProcessed) | Get next codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure |
| func GetCodepointCount(text) | Get total number of codepoints in a UTF-8 encoded string |
| func GetCollisionRec(rec1, rec2) | Get collision rectangle for two rectangles collision |
| func GetColor(hexValue) | Get Color structure from hexadecimal value |
| func GetCurrentMonitor() | Get current connected monitor |
| func GetDirectoryPath(filePath) | Get full path for a given fileName with path (uses static string) |
| func GetFileExtension(fileName) | Get pointer to extension for a filename string (includes dot: '.png') |
| func GetFileLength(fileName) | Get file length in bytes (NOTE: GetFileSize() conflicts with windows.h) |
| func GetFileModTime(fileName) | Get file modification time (last write time) |
| func GetFileName(filePath) | Get pointer to filename for a path string |
| func GetFileNameWithoutExt(filePath) | Get filename string without extension (uses static string) |
| func GetFontDefault() | Get the default Font |
| func GetFPS() | Get current FPS |
| func GetFrameTime() | Get time in seconds for last frame drawn (delta time) |
| func GetGamepadAxisCount(gamepad) | Get gamepad axis count for a gamepad |
| func GetGamepadAxisMovement(gamepad, axis) | Get axis movement value for a gamepad axis |
| func GetGamepadButtonPressed() | Get the last gamepad button pressed |
| func GetGamepadName(gamepad) | Get gamepad internal name id |
| func GetGestureDetected() | Get latest detected gesture |
| func GetGestureDragAngle() | Get gesture drag angle |
| func GetGestureDragVector() | Get gesture drag vector |
| func GetGestureHoldDuration() | Get gesture hold time in milliseconds |
| func GetGesturePinchAngle() | Get gesture pinch angle |
| func GetGesturePinchVector() | Get gesture pinch delta |
| func GetGlyphAtlasRec(font, codepoint) | Get glyph rectangle in font atlas for a codepoint (unicode character), fallback to '?' if not found |
| func GetGlyphIndex(font, codepoint) | Get glyph index position in font for a codepoint (unicode character), fallback to '?' if not found |
| func GetImageAlphaBorder(image, threshold) | Get image alpha border rectangle |
| func GetImageColor(image, x, y) | Get image pixel color at (x, y) position |
| func GetKeyPressed() | Get key pressed (keycode), call it multiple times for keys queued, returns 0 when the queue is empty |
| func GetModelBoundingBox(model) | Compute model bounding box limits (considers all meshes) |
| func GetMonitorCount() | Get number of connected monitors |
| func GetMonitorHeight(monitor) | Get specified monitor height (max available by monitor) |
| func GetMonitorName(monitor) | Get the human-readable, UTF-8 encoded name of the primary monitor |
| func GetMonitorPhysicalHeight(monitor) | Get specified monitor physical height in millimetres |
| func GetMonitorPhysicalWidth(monitor) | Get specified monitor physical width in millimetres |
| func GetMonitorPosition(monitor) | Get specified monitor position |
| func GetMonitorRefreshRate(monitor) | Get specified monitor refresh rate |
| func GetMonitorWidth(monitor) | Get specified monitor width (max available by monitor) |
| func GetMouseDelta() | Get mouse delta between frames |
| func GetMousePosition() | Get mouse position XY |
| func GetMouseRay(mousePosition, camera) | Get a ray trace from mouse position |
| func GetMouseWheelMove() | Get mouse wheel movement Y |
| func GetMouseX() | Get mouse position X |
| func GetMouseY() | Get mouse position Y |
| func GetMusicTimeLength(music) | Get music time length (in seconds) |
| func GetMusicTimePlayed(music) | Get current music time played (in seconds) |
| func getPhysicsbodiescount() | n/a |
| func getPhysicsbody() | n/a |
| func getPhysicsshapetype() | n/a |
| func getPhysicsshapevertex() | n/a |
| func getPhysicsshapeverticescount() | n/a |
| func GetPixelColor(srcPtr, format) | Get Color from a source pixel pointer of certain format |
| func GetPixelDataSize(width, height, format) | Get pixel data size in bytes for certain format |
| func GetPrevDirectoryPath(dirPath) | Get previous directory path for a given path (uses static string) |
| func GetRandomValue(min, max) | Get a random value between min and max (both included) |
| func GetRayCollisionBox(ray, box) | Get collision info between ray and box |
| func GetRayCollisionMesh(ray, mesh, transform) | Get collision info between ray and mesh |
| func GetRayCollisionQuad(ray, p1, p2, p3, p4) | Get collision info between ray and quad |
| func GetRayCollisionSphere(ray, center, radius) | Get collision info between ray and sphere |
| func GetRayCollisionTriangle(ray, p1, p2, p3) | Get collision info between ray and triangle |
| func GetRenderHeight() | Get current render height (it considers HiDPI) |
| func GetRenderWidth() | Get current render width (it considers HiDPI) |
| func GetScreenHeight() | Get current screen height |
| func GetScreenToWorld2D(position, camera) | Get the world space position for a 2d camera screen space position |
| func GetScreenWidth() | Get current screen width |
| func GetShaderLocation(shader, uniformName) | Get shader uniform location |
| func GetShaderLocationAttrib(shader, attribName) | Get shader attribute location |
| func GetSoundsPlaying() | Get number of sounds playing in the multichannel |
| func GetTime() | Get elapsed time in seconds since InitWindow() |
| func GetTouchPointCount() | Get number of touch points |
| func GetTouchPointId(index) | Get touch point identifier for given index |
| func GetTouchPosition(index) | Get touch position XY for a touch point index (relative to screen size) |
| func GetTouchX() | Get touch position X for touch point 0 (relative to screen size) |
| func GetTouchY() | Get touch position Y for touch point 0 (relative to screen size) |
| func GetWindowHandle() | Get native window handle |
| func GetWindowPosition() | Get window position XY on monitor |
| func GetWindowScaleDPI() | Get window scale DPI factor |
| func GetWorkingDirectory() | Get current working directory (uses static string) |
| func GetWorldToScreen(position, camera) | Get the screen space position for a 3d world space position |
| func GetWorldToScreen2D(position, camera) | Get the screen space position for a 2d camera world space position |
| func GetWorldToScreenEx(position, camera, width, height) | Get size position for a 3d world space position |
| func guibutton() | n/a |
| func guicheckbox() | n/a |
| func guicolorbaralpha() | n/a |
| func guicolorbarhue() | n/a |
| func guicolorpicker() | n/a |
| func guicombobox() | n/a |
| func guidisable() | n/a |
| func guidropdownbox() | n/a |
| func guidummyrec() | n/a |
| func guienable() | n/a |
| func guifade() | n/a |
| func guigetstyle() | n/a |
| func guigrid() | n/a |
| func guigroupbox() | n/a |
| func guilabel() | n/a |
| func guilabelbutton() | n/a |
| func guiline() | n/a |
| func guilistview() | n/a |
| func guilistviewex() | n/a |
| func guiloadstyle() | n/a |
| func guiloadstyledefault() | n/a |
| func guilock() | n/a |
| func guimessagebox() | n/a |
| func guipanel() | n/a |
| func guiprogressbar() | n/a |
| func guiscrollpanel() | n/a |
| func guisetstate() | n/a |
| func guisetstyle() | n/a |
| func guislider() | n/a |
| func guisliderbar() | n/a |
| func guispinner() | n/a |
| func guistatusbar() | n/a |
| func guitextbox() | n/a |
| func guitextboxmulti() | n/a |
| func guitextinputbox() | n/a |
| func guitoggle() | n/a |
| func guitogglegroup() | n/a |
| func guiunlock() | n/a |
| func guivaluebox() | n/a |
| func guiwindowbox() | n/a |
| sub HideCursor() | Hides cursor |
| sub ImageAlphaClear(image, color, threshold) | Clear alpha channel to desired color |
| sub ImageAlphaCrop(image, threshold) | Crop image depending on alpha value |
| sub ImageAlphaMask(image, alphaMask) | Apply alpha mask to image |
| sub ImageAlphaPremultiply(image) | Premultiply alpha channel |
| sub ImageClearBackground(dst, color) | Clear image background with given color |
| sub ImageColorBrightness(image, brightness) | Modify image color: brightness (-255 to 255) |
| sub ImageColorContrast(image, contrast) | Modify image color: contrast (-100 to 100) |
| sub ImageColorGrayscale(image) | Modify image color: grayscale |
| sub ImageColorInvert(image) | Modify image color: invert |
| sub ImageColorReplace(image, color, replace) | Modify image color: replace color |
| sub ImageColorTint(image, color) | Modify image color: tint |
| func ImageCopy(image) | Create an image duplicate (useful for transformations) |
| sub ImageCrop(image, crop) | Crop an image to a defined rectangle |
| sub ImageDither(image, rBpp, gBpp, bBpp, aBpp) | Dither image data to 16bpp or lower (Floyd-Steinberg dithering) |
| sub ImageDraw(dst, src, srcRec, dstRec, tint) | Draw a source image within a destination image (tint applied to source) |
| sub ImageDrawCircle(dst, centerX, centerY, radius, color) | Draw circle within an image |
| sub ImageDrawCircleV(dst, center, radius, color) | Draw circle within an image (Vector version) |
| sub ImageDrawLine(dst, startPosX, startPosY, endPosX, endPosY, color) | Draw line within an image |
| sub ImageDrawLineV(dst, start, end, color) | Draw line within an image (Vector version) |
| sub ImageDrawPixel(dst, posX, posY, color) | Draw pixel within an image |
| sub ImageDrawPixelV(dst, position, color) | Draw pixel within an image (Vector version) |
| sub ImageDrawRectangle(dst, posX, posY, width, height, color) | Draw rectangle within an image |
| sub ImageDrawRectangleLines(dst, rec, thick, color) | Draw rectangle lines within an image |
| sub ImageDrawRectangleRec(dst, rec, color) | Draw rectangle within an image |
| sub ImageDrawRectangleV(dst, position, size, color) | Draw rectangle within an image (Vector version) |
| sub ImageDrawText(dst, text, posX, posY, fontSize, color) | Draw text (using default font) within an image (destination) |
| sub ImageDrawTextEx(dst, font, text, position, fontSize, spacing, tint) | Draw text (custom sprite font) within an image (destination) |
| sub ImageFlipHorizontal(image) | Flip image horizontally |
| sub ImageFlipVertical(image) | Flip image vertically |
| sub ImageFormat(image, newFormat) | Convert image data to desired format |
| func ImageFromImage(image, rec) | Create an image from another image piece |
| sub ImageMipmaps(image) | Compute all mipmap levels for a provided image |
| sub ImageResize(image, newWidth, newHeight) | Resize image (Bicubic scaling algorithm) |
| sub ImageResizeCanvas(image, newWidth, newHeight, offsetX, offsetY, fill) | Resize canvas and fill with color |
| sub ImageResizeNN(image, newWidth, newHeight) | Resize image (Nearest-Neighbor scaling algorithm) |
| sub ImageRotateCCW(image) | Rotate image counter-clockwise 90deg |
| sub ImageRotateCW(image) | Rotate image clockwise 90deg |
| func ImageText(text, fontSize, color) | Create an image from text (default font) |
| func ImageTextEx(font, text, fontSize, spacing, tint) | Create an image from text (custom sprite font) |
| sub ImageToPOT(image, fill) | Convert image to POT (power-of-two) |
| sub InitAudioDevice() | Initialize audio device and context |
| func initPhysics() | n/a |
| sub InitWindow(width, height, title) | Initialize window and OpenGL context |
| func IsAudioDeviceReady() | Check if audio device has been initialized successfully |
| func IsAudioStreamPlaying(stream) | Check if audio stream is playing |
| func IsAudioStreamProcessed(stream) | Check if any audio stream buffers requires refill |
| func IsCursorHidden() | Check if cursor is not visible |
| func IsCursorOnScreen() | Check if cursor is on the screen |
| func IsFileDropped() | Check if a file has been dropped into window |
| func IsFileExtension(fileName, ext) | Check file extension (including point: .png, .wav) |
| func IsGamepadAvailable(gamepad) | Check if a gamepad is available |
| func IsGamepadButtonDown(gamepad, button) | Check if a gamepad button is being pressed |
| func IsGamepadButtonPressed(gamepad, button) | Check if a gamepad button has been pressed once |
| func IsGamepadButtonReleased(gamepad, button) | Check if a gamepad button has been released once |
| func IsGamepadButtonUp(gamepad, button) | Check if a gamepad button is NOT being pressed |
| func IsGestureDetected(gesture) | Check if a gesture have been detected |
| func IsKeyDown(key) | Check if a key is being pressed |
| func IsKeyPressed(key) | Check if a key has been pressed once |
| func IsKeyReleased(key) | Check if a key has been released once |
| func IsKeyUp(key) | Check if a key is NOT being pressed |
| func IsModelAnimationValid(model, anim) | Check model animation skeleton match |
| func IsMouseButtonDown(button) | Check if a mouse button is being pressed |
| func IsMouseButtonPressed(button) | Check if a mouse button has been pressed once |
| func IsMouseButtonReleased(button) | Check if a mouse button has been released once |
| func IsMouseButtonUp(button) | Check if a mouse button is NOT being pressed |
| func IsMusicStreamPlaying(music) | Check if music is playing |
| func IsSoundPlaying(sound) | Check if a sound is currently playing |
| func IsWindowFocused() | Check if window is currently focused (only PLATFORM_DESKTOP) |
| func IsWindowFullscreen() | Check if window is currently fullscreen |
| func IsWindowHidden() | Check if window is currently hidden (only PLATFORM_DESKTOP) |
| func IsWindowMaximized() | Check if window is currently maximized (only PLATFORM_DESKTOP) |
| func IsWindowMinimized() | Check if window is currently minimized (only PLATFORM_DESKTOP) |
| func IsWindowReady() | Check if window has been initialized successfully |
| func IsWindowResized() | Check if window has been resized last frame |
| func IsWindowState(flag) | Check if one specific window flag is enabled |
| func LoadAudioStream(sampleRate, sampleSize, channels) | Load audio stream (to stream raw audio pcm data) |
| func LoadCodepoints(text, count) | Load all codepoints from a UTF-8 text string, codepoints count returned by parameter |
| func LoadFileData(fileName, bytesRead) | Load file data as byte array (read) |
| func LoadFileText(fileName) | Load text data from file (read), returns a '\\0' terminated string |
| func LoadFont(fileName) | Load font from file into GPU memory (VRAM) |
| func LoadFontEx(fileName, fontSize, fontChars, glyphCount) | Load font from file with extended parameters, use NULL for fontChars and 0 for glyphCount to load the default character set |
| func LoadFontFromImage(image, key, firstChar) | Load font from Image (XNA style) |
| func LoadFontFromMemory(fileType, fileData, dataSize, fontSize, fontChars, glyphCount) | Load font from memory buffer, fileType refers to extension: i.e. '.ttf' |
| func LoadImage(fileName) | Load image from file into CPU memory (RAM) |
| func LoadImageAnim(fileName, frames) | Load image sequence from file (frames appended to image.data) |
| func LoadImageColors(image) | Load color data from image as a Color array (RGBA - 32bit) |
| func LoadImageFromMemory(fileType, fileData, dataSize) | Load image from memory buffer, fileType refers to extension: i.e. '.png' |
| func LoadImageFromScreen() | Load image from screen buffer and (screenshot) |
| func LoadImageFromTexture(texture) | Load image from GPU texture data |
| func LoadImagePalette(image, maxPaletteSize, colorCount) | Load colors palette from image as a Color array (RGBA - 32bit) |
| func LoadImageRaw(fileName, width, height, format, headerSize) | Load image from RAW file data |
| func LoadModel(fileName) | Load model from files (meshes and materials) |
| func LoadModelAnimations(fileName, animCount) | Load model animations from file |
| func LoadModelFromMesh(mesh) | Load model from generated mesh (default material) |
| func LoadMusicStream(fileName) | Load music stream from file |
| func LoadMusicStreamFromMemory(fileType, data, dataSize) | Load music stream from data |
| func LoadRenderTexture(width, height) | Load texture for rendering (framebuffer) |
| func LoadShader(vsFileName, fsFileName) | Load shader from files and bind default locations |
| func LoadShaderFromMemory(vsCode, fsCode) | Load shader from code strings and bind default locations |
| func LoadSound(fileName) | Load sound from file |
| func LoadSoundFromWave(wave) | Load sound from wave data |
| func LoadStorageValue(position) | Load integer value from storage file (from defined position) |
| func LoadTexture(fileName) | Load texture from file into GPU memory (VRAM) |
| func LoadTextureCubemap(image, layout) | Load cubemap from image, multiple image cubemap layouts supported |
| func LoadTextureFromImage(image) | Load texture from image data |
| func LoadWave(fileName) | Load wave data from file |
| func LoadWaveFromMemory(fileType, fileData, dataSize) | Load wave from memory buffer, fileType refers to extension: i.e. '.wav' |
| func LoadWaveSamples(wave) | Load samples data from wave as a 32bit float data array |
| sub MaximizeWindow() | Set window state: maximized, if resizable (only PLATFORM_DESKTOP) |
| func MeasureText(text, fontSize) | Measure string width for default font |
| func MeasureTextEx(font, text, fontSize, spacing) | Measure string size for Font |
| func MemAlloc(size) | Internal memory allocator |
| sub MemFree(ptr) | Internal memory free |
| func MemRealloc(ptr, size) | Internal memory reallocator |
| func meshboundingbox() | n/a |
| sub MinimizeWindow() | Set window state: minimized, if resizable (only PLATFORM_DESKTOP) |
| sub OpenURL(url) | Open URL with default system browser (if available) |
| sub PauseAudioStream(stream) | Pause audio stream |
| sub PauseMusicStream(music) | Pause music playing |
| sub PauseSound(sound) | Pause a sound |
| func Physicsaddforce() | n/a |
| func Physicsaddtorque() | n/a |
| func Physicsshapetype() | n/a |
| func Physicsshatter() | n/a |
| sub PlayAudioStream(stream) | Play audio stream |
| sub PlayMusicStream(music) | Start music playing |
| sub PlaySound(sound) | Play a sound |
| sub PlaySoundMulti(sound) | Play a sound (using multichannel buffer pool) |
| func pollevents() | n/a |
| sub PollInputEvents() | Register all input events |
| func resetPhysics() | n/a |
| sub RestoreWindow() | Set window state: not minimized/maximized (only PLATFORM_DESKTOP) |
| sub ResumeAudioStream(stream) | Resume audio stream |
| sub ResumeMusicStream(music) | Resume playing paused music |
| sub ResumeSound(sound) | Resume a paused sound |
| func SaveFileData(fileName, data, bytesToWrite) | Save data to file from byte array (write), returns true on success |
| func SaveFileText(fileName, text) | Save text data to file (write), string must be '\\0' terminated, returns true on success |
| func SaveStorageValue(position, value) | Save integer value to storage file (to defined position), returns true on success |
| sub SeekMusicStream(music, position) | Seek music to a position (in seconds) |
| sub SetAudioStreamBufferSizeDefault(size) | Default size for new audio streams |
| sub SetAudioStreamPan(stream, pan) | Set pan for audio stream (0.5 is centered) |
| sub SetAudioStreamPitch(stream, pitch) | Set pitch for audio stream (1.0 is base level) |
| sub SetAudioStreamVolume(stream, volume) | Set volume for audio stream (1.0 is max level) |
| sub SetCameraAltControl(keyAlt) | Set camera alt key to combine with mouse movement (free camera) |
| sub SetCameraMode(camera, mode) | Set camera mode (multiple camera modes available) |
| sub SetCameraMoveControls(keyFront, keyBack, keyRight, keyLeft, keyUp, keyDown) | Set camera move controls (1st person and 3rd person cameras) |
| sub SetCameraPanControl(keyPan) | Set camera pan key to combine with mouse movement (free camera) |
| sub SetCameraSmoothZoomControl(keySmoothZoom) | Set camera smooth zoom key to combine with mouse (free camera) |
| sub SetClipboardText(text) | Set clipboard text content |
| sub SetConfigFlags(flags) | Setup init configuration flags (view FLAGS) |
| sub SetExitKey(key) | Set a custom key to exit program (default is ESC) |
| func SetGamepadMappings(mappings) | Set internal gamepad mappings (SDL_GameControllerDB) |
| sub SetGesturesEnabled(flags) | Enable a set of gestures using flags |
| sub SetMasterVolume(volume) | Set master volume (listener) |
| func setmodeldiffusetexture() | n/a |
| sub SetModelMeshMaterial(model, meshId, materialId) | Set material for a mesh |
| sub SetMouseCursor(cursor) | Set mouse cursor |
| sub SetMouseOffset(offsetX, offsetY) | Set mouse offset |
| sub SetMousePosition(x, y) | Set mouse position XY |
| sub SetMouseScale(scaleX, scaleY) | Set mouse scaling |
| sub SetMusicPan(music, pan) | Set pan for a music (0.5 is center) |
| sub SetMusicPitch(music, pitch) | Set pitch for a music (1.0 is base level) |
| sub SetMusicVolume(music, volume) | Set volume for music (1.0 is max level) |
| func setPhysicsbodyangularvelocity() | n/a |
| func setPhysicsbodydynamicfriction() | n/a |
| func setPhysicsbodyenabled() | n/a |
| func setPhysicsbodyforce() | n/a |
| func setPhysicsbodyfreezeorient() | n/a |
| func setPhysicsbodyinertia() | n/a |
| func setPhysicsbodyinverseinertia() | n/a |
| func setPhysicsbodyinversemass() | n/a |
| func setPhysicsbodyisgrounded() | n/a |
| func setPhysicsbodymass() | n/a |
| func setPhysicsbodyorient() | n/a |
| func setPhysicsbodyposition() | n/a |
| func setPhysicsbodyrestitution() | n/a |
| func setPhysicsbodyrotation() | n/a |
| func setPhysicsbodystaticfriction() | n/a |
| func setPhysicsbodytorque() | n/a |
| func setPhysicsbodyusegravity() | n/a |
| func setPhysicsbodyvelocity() | n/a |
| func setPhysicsgravity() | n/a |
| func setPhysicstimestep() | n/a |
| sub SetPixelColor(dstPtr, color, format) | Set color formatted into destination pixel pointer |
| sub SetRandomSeed(seed) | Set the seed for the random number generator |
| sub SetShaderValue(shader, locIndex, value, uniformType) | Set shader uniform value |
| sub SetShaderValueMatrix(shader, locIndex, mat) | Set shader uniform value (matrix 4x4) |
| sub SetShaderValueTexture(shader, locIndex, texture) | Set shader uniform value for texture (sampler2d) |
| sub SetShaderValueV(shader, locIndex, value, uniformType, count) | Set shader uniform value vector |
| sub SetShapesTexture(texture, source) | Set texture and rectangle to be used on shapes drawing |
| sub SetSoundPan(sound, pan) | Set pan for a sound (0.5 is center) |
| sub SetSoundPitch(sound, pitch) | Set pitch for a sound (1.0 is base level) |
| sub SetSoundVolume(sound, volume) | Set volume for a sound (1.0 is max level) |
| sub SetTargetFPS(fps) | Set target FPS (maximum) |
| sub SetTextureFilter(texture, filter) | Set texture scaling filter mode |
| sub SetTextureWrap(texture, wrap) | Set texture wrapping mode |
| sub SetTraceLogLevel(logLevel) | Set the current threshold (minimum) log level |
| sub SetWindowIcon(image) | Set icon for window (only PLATFORM_DESKTOP) |
| sub SetWindowMinSize(width, height) | Set window minimum dimensions (for FLAG_WINDOW_RESIZABLE) |
| sub SetWindowMonitor(monitor) | Set monitor for the current window (fullscreen mode) |
| sub SetWindowOpacity(opacity) | Set window opacity [0.0f..1.0f] (only PLATFORM_DESKTOP) |
| sub SetWindowPosition(x, y) | Set window position on screen (only PLATFORM_DESKTOP) |
| sub SetWindowSize(width, height) | Set window dimensions |
| sub SetWindowState(flags) | Set window configuration state using flags (only PLATFORM_DESKTOP) |
| sub SetWindowTitle(title) | Set title for window (only PLATFORM_DESKTOP) |
| sub ShowCursor() | Shows cursor |
| sub StopAudioStream(stream) | Stop audio stream |
| sub StopMusicStream(music) | Stop music playing |
| sub StopSound(sound) | Stop playing a sound |
| sub StopSoundMulti() | Stop any sound playing (using multichannel buffer pool) |
| sub SwapScreenBuffer() | Swap back buffer with front buffer (screen drawing) |
| sub TakeScreenshot(fileName) | Takes a screenshot of current screen (filename extension defines format) |
| sub TextAppend(text, append, position) | Append text at specific position and move cursor! |
| func TextCodepointsToUTF8(codepoints, length) | Encode text as codepoints array into UTF-8 text string (WARNING: memory must be freed!) |
| func TextCopy(dst, src) | Copy one string to another, returns bytes copied |
| func TextFindIndex(text, find) | Find first text occurrence within a string |
| func TextFormat(text, args) | Text formatting with variables (sprintf() style) |
| func TextInsert(text, insert, position) | Insert text in a position (WARNING: memory must be freed!) |
| func TextIsEqual(text1, text2) | Check if two text string are equal |
| func TextLength(text) | Get text length, checks for '\\0' ending |
| func TextReplace(text, replace, by) | Replace text string (WARNING: memory must be freed!) |
| func TextSubtext(text, position, length) | Get a piece of a text string |
| func TextToInteger(text) | Get integer value from text (negative values not supported) |
| func TextToLower(text) | Get lower case version of provided string |
| func TextToPascal(text) | Get Pascal case notation version of provided string |
| func TextToUpper(text) | Get upper case version of provided string |
| sub ToggleFullscreen() | Toggle window state: fullscreen/windowed (only PLATFORM_DESKTOP) |
| sub UnloadAudioStream(stream) | Unload audio stream and free memory |
| sub UnloadCodepoints(codepoints) | Unload codepoints data from memory |
| sub UnloadFileData(data) | Unload file data allocated by LoadFileData() |
| sub UnloadFileText(text) | Unload file text data allocated by LoadFileText() |
| sub UnloadFont(font) | Unload font from GPU memory (VRAM) |
| sub UnloadImage(image) | Unload image from CPU memory (RAM) |
| sub UnloadImageColors(colors) | Unload color data loaded with LoadImageColors() |
| sub UnloadImagePalette(colors) | Unload colors palette loaded with LoadImagePalette() |
| sub UnloadMesh(mesh) | Unload mesh data from CPU and GPU |
| sub UnloadModel(model) | Unload model (including meshes) from memory (RAM and/or VRAM) |
| sub UnloadModelAnimation(anim) | Unload animation data |
| sub UnloadModelAnimations(animations, count) | Unload animation array data |
| sub UnloadModelKeepMeshes(model) | Unload model (but not meshes) from memory (RAM and/or VRAM) |
| sub UnloadMusicStream(music) | Unload music stream |
| sub UnloadRenderTexture(target) | Unload render texture from GPU memory (VRAM) |
| sub UnloadShader(shader) | Unload shader from GPU memory (VRAM) |
| sub UnloadSound(sound) | Unload sound |
| sub UnloadTexture(texture) | Unload texture from GPU memory (VRAM) |
| sub UnloadWave(wave) | Unload wave data |
| sub UnloadWaveSamples(samples) | Unload samples data loaded with LoadWaveSamples() |
| sub UpdateAudioStream(stream, data, frameCount) | Update audio stream buffers with data |
| sub UpdateCamera(camera) | Update camera position for selected mode |
| sub UpdateMeshBuffer(mesh, index, data, dataSize, offset) | Update mesh vertex data in GPU for a specific buffer index |
| sub UpdateModelAnimation(model, anim, frame) | Update model animation pose |
| sub UpdateMusicStream(music) | Updates buffers for music streaming |
| func updatePhysics() | n/a |
| sub UpdateSound(sound, data, sampleCount) | Update sound buffer with new data |
| sub UpdateTexture(texture, pixels) | Update GPU texture with new data |
| sub UpdateTextureRec(texture, rec, pixels) | Update GPU texture rectangle with new data |
| sub UploadMesh(mesh, dynamic) | Upload mesh vertex data in GPU and provide VAO/VBO ids |
| func waitevents() | n/a |
| sub WaitTime(ms) | Wait for some milliseconds (halt program execution) |
| func WaveCopy(wave) | Copy a wave to a new wave |
| sub WaveCrop(wave, initSample, finalSample) | Crop a wave to defined samples range |
| sub WaveFormat(wave, sampleRate, sampleSize, channels) | Convert wave data to desired format |
| func WindowShouldClose() | Check if KEY_ESCAPE pressed or Close icon pressed |

Unimplemented APIs
----------------

| Name    | Description   |
|---------|---------------|
| AttachAudioStreamProcessor | n/a |
| BeginVrStereoMode | Begin stereo rendering (requires VR simulator) |
| DetachAudioStreamProcessor | n/a |
| DrawMesh | Draw a 3d mesh with material and transform |
| DrawMeshInstanced | Draw multiple mesh instances with material and different transforms |
| GenImageFontAtlas | Generate image font atlas using chars info |
| GetDirectoryFiles | Get filenames in a directory path (memory must be freed) |
| GetDroppedFiles | Get dropped files names (memory must be freed) |
| GetGlyphInfo | Get glyph font info data for a codepoint (unicode character), fallback to '?' if not found |
| LoadFontData | Load font data for further use |
| LoadMaterialDefault | Load default material (Supports: DIFFUSE, SPECULAR, NORMAL maps) |
| LoadMaterials | Load materials from model file |
| LoadVrStereoConfig | Load VR stereo config for VR simulator device parameters |
| SetAudioStreamCallback | Audio thread callback to request new data |
| SetLoadFileDataCallback | Set custom file binary data loader |
| SetLoadFileTextCallback | Set custom file text data loader |
| SetMaterialTexture | Set texture for a material map type (MATERIAL_MAP_DIFFUSE, MATERIAL_MAP_SPECULAR...) |
| SetSaveFileDataCallback | Set custom file binary data saver |
| SetSaveFileTextCallback | Set custom file text data saver |
| SetTraceLogCallback | Set custom trace log |
| TextJoin | Join text strings with delimiter |
| TextSplit | Split text into multiple strings |
| UnloadFontData | Unload font chars info data (RAM) |
| UnloadMaterial | Unload material from GPU memory (VRAM) |
| UnloadVrStereoConfig | Unload VR stereo config |

