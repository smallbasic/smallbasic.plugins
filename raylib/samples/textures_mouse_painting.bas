REM ----------------------------------------------------------------------------------------
REM
REM   raylib [textures] example - Mouse painting
REM
REM   This example has been created using raylib 2.5 (www.raylib.com)
REM   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM
REM   Example contributed by Chris Dill (@MysteriousSpace) and reviewed by Ramon Santamaria (@raysan5)
REM
REM   Copyright (c) 2019 Chris Dill (@MysteriousSpace) and Ramon Santamaria (@raysan5)
REM
REM ----------------------------------------------------------------------------------------

import raylib as rl
import raylibc as c

const MAX_COLORS_COUNT = 23
const screenWidth = 800
const screenHeight = 450

rl.InitWindow(screenWidth, screenHeight, "SmallBASIC [raylib textures] example - mouse painting")

' Colours to choose from
colors = [c.RAYWHITE, c.YELLOW, c.GOLD, c.ORANGE, c.PINK, c.RED, c.MAROON, c.GREEN, c.LIME, c.DARKGREEN,
   c.SKYBLUE, c.BLUE, c.DARKBLUE, c.PURPLE, c.VIOLET, c.DARKPURPLE, c.BEIGE, c.BROWN, c.DARKBROWN,
   c.LIGHTGRAY, c.GRAY, c.DARKGRAY, c.BLACK]

dim colorsRecs(MAX_COLORS_COUNT)

for i = 0  to MAX_COLORS_COUNT - 1
  colorsRecs[i].x = 10 + 30 * i + 2 * i
  colorsRecs[i].y = 10
  colorsRecs[i].width = 30
  colorsRecs[i].height = 30
next i

colorSelected = 0
colorSelectedPrev = colorSelected
colorMouseHover = 0
brushSize = 20

btnSaveRec = [750, 10, 40, 30]
btnSaveMouseHover = false
showSaveMessage = false
saveMessageCounter = 0

' Create a RenderTexture2D to use as a canvas
target = rl.LoadRenderTexture(screenWidth, screenHeight)

' Clear render texture before entering the game loop
rl.BeginTextureMode(target)
rl.ClearBackground(colors[0])
rl.EndTextureMode()

rl.SetTargetFPS(60)

while (!rl.WindowShouldClose())
  mousePos = rl.GetMousePosition()

  ' Move between colors with keys
  if (rl.IsKeyPressed(c.KEY_RIGHT)) then
    colorSelected++
  elseif (rl.IsKeyPressed(c.KEY_LEFT)) then
    colorSelected--
  endif

  if (colorSelected >= MAX_COLORS_COUNT) then
    colorSelected = MAX_COLORS_COUNT - 1
  elseif (colorSelected < 0) then
    colorSelected = 0
  endif

  ' Choose color with mouse
  for i = 0 to MAX_COLORS_COUNT - 1
    if (rl.CheckCollisionPointRec(mousePos, colorsRecs[i])) then
      colorMouseHover = i
      exit for
    else
      colorMouseHover = -1
    endif
  next i

  if ((colorMouseHover >= 0) && rl.IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) then
    colorSelected = colorMouseHover
    colorSelectedPrev = colorSelected
  endif

  ' Change brush size
  brushSize += rl.GetMouseWheelMove() * 5
  if (brushSize < 2) then brushSize = 2
  if (brushSize > 50) then brushSize = 50

  if (rl.IsKeyPressed(c.KEY_C)) then
    ' Clear render texture to clear color
    rl.BeginTextureMode(target)
    rl.ClearBackground(colors[0])
    rl.EndTextureMode()
  endif

  if (rl.IsMouseButtonDown(c.MOUSE_LEFT_BUTTON) || (rl.GetGestureDetected() == c.GESTURE_DRAG)) then
    ' Paint circle into render texture
    ' NOTE: To avoid discontinuous circles, we could store
    ' previous-next mouse points and just draw a line using brush size
    rl.BeginTextureMode(target)
    if (mousePos.y > 50) then 
      rl.DrawCircle(mousePos.x, mousePos.y, brushSize, colors[colorSelected])
    endif
    rl.EndTextureMode()
  endif

  if (rl.IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) then
    colorSelected = 0
    ' Erase circle from render texture
    rl.BeginTextureMode(target)
    if (mousePos.y > 50) then 
      rl.DrawCircle(mousePos.x, mousePos.y, brushSize, colors[0])
    endif
    rl.EndTextureMode()
  else
    colorSelected = colorSelectedPrev
  endif

  ' Check mouse hover save button
  if (rl.CheckCollisionPointRec(mousePos, btnSaveRec)) then
    btnSaveMouseHover = true
  else
    btnSaveMouseHover = false
  endif

  ' Image saving logic
  ' NOTE: Saving painted texture to a default named image
  if ((btnSaveMouseHover && rl.IsMouseButtonReleased(c.MOUSE_LEFT_BUTTON)) || rl.IsKeyPressed(c.KEY_S)) then
    local img = rl.GetTextureData(target.texture)
    rl.ImageFlipVertical(img)
    rl.ExportImage(img, "my_amazing_texture_painting.png")
    rl.UnloadImage(img)
    showSaveMessage = true
  endif
  if (showSaveMessage) then
    ' On saving, show a full screen message for 2 seconds
    saveMessageCounter++
    if (saveMessageCounter > 240) then
      showSaveMessage = false
      saveMessageCounter = 0
    endif
  endif

  rl.BeginDrawing()
  rl.ClearBackground(c.RAYWHITE)

  ' NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
  rl.DrawTextureRec(target.texture, [0, 0, target.texture.width, -target.texture.height], [0, 0], c.WHITE)

  ' Draw drawing circle for reference
  if (mousePos.y > 50) then
    if (rl.IsMouseButtonDown(c.MOUSE_RIGHT_BUTTON)) then
      rl.DrawCircleLines(mousePos.x, mousePos.y, brushSize, c.GRAY)
    else
      rl.DrawCircle(rl.GetMouseX(), rl.GetMouseY(), brushSize, colors[colorSelected])
    endif
  endif

  ' Draw top panel
  rl.DrawRectangle(0, 0, rl.GetScreenWidth(), 50, c.RAYWHITE)
  rl.DrawLine(0, 50, rl.GetScreenWidth(), 50, c.LIGHTGRAY)

  ' Draw color selection rectangles
  for i = 0 to MAX_COLORS_COUNT - 1
    rl.DrawRectangleRec(colorsRecs[i], colors[i])
  next i

  rl.DrawRectangleLines(10, 10, 30, 30, c.LIGHTGRAY)

  if (colorMouseHover >= 0) then 
    rl.DrawRectangleRec(colorsRecs[colorMouseHover], rl.Fade(c.WHITE, 0.6))
  endif

  rectx = [colorsRecs[colorSelected].x - 2, colorsRecs[colorSelected].y - 2, colorsRecs[colorSelected].width + 4, colorsRecs[colorSelected].height + 4]
  rl.DrawRectangleLinesEx(rectx, 2, c.BLACK)

  ' Draw save image button
  colx = IFF(btnSaveMouseHover, c.RED, c.BLACK)
  rl.DrawRectangleLinesEx(btnSaveRec, 2, colx)
  rl.DrawText("SAVE!", 755, 20, 10, colx)

  ' Draw save image message
  if (showSaveMessage) then
    rl.DrawRectangle(0, 0, rl.GetScreenWidth(), rl.GetScreenHeight(), rl.Fade(c.RAYWHITE, 0.8))
    rl.DrawRectangle(0, 150, rl.GetScreenWidth(), 80, c.BLACK)
    rl.DrawText("IMAGE SAVED:  my_amazing_texture_painting.png", 150, 180, 20, c.RAYWHITE)
  endif

  rl.EndDrawing()
wend

print "done"

' De-Initialization
rl.UnloadRenderTexture(target)     ' Unload render texture
rl.CloseWindow()                   ' Close window and OpenGL context

