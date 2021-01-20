REM /*******************************************************************************************
REM *
REM *   raylib [textures] example - Draw part of the texture tiled
REM *
REM *   This example has been created using raylib 3.0 (www.raylib.com)
REM *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM *
REM *   Copyright (c) 2020 Vlad Adrian (@demizdor) and Ramon Santamaria (@raysan5)
REM *
REM ********************************************************************************************/

import raylib as rl
import raylibc as c

const OPT_WIDTH   = 220       ' Max width for the options container
const MARGIN_SIZE =   8       ' Size for the margins
const COLOR_SIZE  =  16       ' Size of the color select buttons

screenWidth = 800
screenHeight = 450

rl.SetConfigFlags(FLAG_WINDOW_RESIZABLE)  ' Make the window resizable
rl.InitWindow(screenWidth, screenHeight, "SmallBASIC raylib [textures] example - Draw part of a texture tiled")

' NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
const resources = CWD + "raylib/examples/textures/resources/"
texPattern = rl.LoadTexture(resources + "patterns.png")
rl.SetTextureFilter(texPattern, c.FILTER_TRILINEAR)  ' Makes the texture smoother when upscaled

' Coordinates for all patterns inside the texture
recPattern = [
 [3, 3, 66, 66],
 [75, 3, 100, 100],
 [3, 75, 66, 66],
 [7, 156, 50, 50],
 [85, 106, 90, 45],
 [75, 154, 100, 60]
]

' Setup colors
const colors = [c.BLACK, c.MAROON, c.ORANGE, c.BLUE, c.PURPLE, c.BEIGE, c.LIME, c.RED, c.DARKGRAY, c.SKYBLUE]
const MAX_COLORS = len(colors)
dim colorRec(MAX_COLORS)

' Calculate rectangle for each color
x = 0
y = 0
for i = 0 to MAX_COLORS - 1
  colorRec[i].x = 2 + MARGIN_SIZE + x
  colorRec[i].y = 22 + 256 + MARGIN_SIZE + y
  colorRec[i].width = COLOR_SIZE * 2
  colorRec[i].height = COLOR_SIZE
  if (i == (MAX_COLORS/2 - 1)) then
    x = 0
    y += COLOR_SIZE + MARGIN_SIZE
  else
    x += (COLOR_SIZE*2 + MARGIN_SIZE)
  endif
next i

activePattern = 0
activeCol = 0
scale = 1.0
rotation = 0.0

rl.SetTargetFPS(60)
while (!rl.WindowShouldClose())
  screenWidth = rl.GetScreenWidth()
  screenHeight = rl.GetScreenHeight()

  ' Handle mouse
  if (rl.IsMouseButtonPressed(c.MOUSE_LEFT_BUTTON)) then
    mouse = rl.GetMousePosition()
    ' Check which pattern was clicked and set it as the active pattern
    for i = 0 to len(recPattern) - 1
      if (rl.CheckCollisionPointRec(mouse, [2 + MARGIN_SIZE + recPattern[i][0], 40 + MARGIN_SIZE + recPattern[i][1], recPattern[i][2], recPattern[i][3]])) then
        activePattern = i
        exit for
      endif
    next i

    ' Check to see which color was clicked and set it as the active color
    for i = 0 to MAX_COLORS -1
      if (rl.CheckCollisionPointRec(mouse, colorRec[i])) then
        activeCol = i
        exit for
      endif
    next i
  endif
'  ' Handle keys
'
  ' Change scale
  if (rl.IsKeyPressed(c.KEY_UP)) then scale += 0.25
  if (rl.IsKeyPressed(c.KEY_DOWN)) then scale -= 0.25
  if (scale > 10.0) then
    scale = 10.0
  else if (scale <= 0.0) then
    scale = 0.25
  endif

  ' Change rotation
  if (rl.IsKeyPressed(c.KEY_LEFT)) then rotation -= 25.0
  if (rl.IsKeyPressed(c.KEY_RIGHT)) then rotation += 25.0

  ' Reset
  if (rl.IsKeyPressed(c.KEY_SPACE)) then
    rotation = 0.0
    scale = 1.0
  endif

  rl.BeginDrawing()
  rl.ClearBackground(c.RAYWHITE)

  ' Draw the tiled area
  destRec = [OPT_WIDTH + MARGIN_SIZE, MARGIN_SIZE, screenWidth - OPT_WIDTH - 2 * MARGIN_SIZE, screenHeight - 2 * MARGIN_SIZE]
  rl.DrawTextureTiled(texPattern, recPattern[activePattern], destRec, [0.0, 0.0], rotation, scale, colors[activeCol])

  ' Draw options
  rl.DrawRectangle(MARGIN_SIZE, MARGIN_SIZE, OPT_WIDTH - MARGIN_SIZE, screenHeight - 2 * MARGIN_SIZE, rl.ColorAlpha(c.LIGHTGRAY, 0.5))
  rl.DrawText("Select Pattern", 2 + MARGIN_SIZE, 30 + MARGIN_SIZE, 10, c.BLACK)
  rl.DrawTexture(texPattern, 2 + MARGIN_SIZE, 40 + MARGIN_SIZE, c.BLACK)
  rl.DrawRectangle(2 + MARGIN_SIZE + recPattern[activePattern][0], 40 + MARGIN_SIZE + recPattern[activePattern][1], recPattern[activePattern][2], recPattern[activePattern][3], rl.ColorAlpha(c.DARKBLUE, 0.3))
  rl.DrawText("Select Color", 2+MARGIN_SIZE, 10+256+MARGIN_SIZE, 10, c.BLACK)
  for i = 0 to MAX_COLORS -1
    rl.DrawRectangleRec(colorRec[i], colors[i])
    if (activeCol == i) then rl.DrawRectangleLinesEx(colorRec[i], 3.0, rl.ColorAlpha(c.WHITE, 0.5))
  next i

  rl.DrawText("Scale (UP/DOWN to change)", 2 + MARGIN_SIZE, 80 + 256 + MARGIN_SIZE, 10, c.BLACK)
  rl.DrawText(rl.TextFormat("%.2fx", scale), 2 + MARGIN_SIZE, 92 + 256 + MARGIN_SIZE, 20, c.BLACK)
  rl.DrawText("Rotation (LEFT/RIGHT to change)", 2 + MARGIN_SIZE, 122 + 256 + MARGIN_SIZE, 10, c.BLACK)
  rl.DrawText(rl.TextFormat("%.0f degrees", rotation), 2 + MARGIN_SIZE, 134 + 256 + MARGIN_SIZE, 20, c.BLACK)
  rl.DrawText("Press [SPACE] to reset", 2 + MARGIN_SIZE, 164 + 256 + MARGIN_SIZE, 10, c.DARKBLUE)

  ' Draw FPS
  rl.DrawText(rl.TextFormat("%i FPS", rl.GetFPS()), 2 + MARGIN_SIZE, 2 + MARGIN_SIZE, 20, c.BLACK)
  rl.EndDrawing()
wend

rl.UnloadTexture(texPattern)         ' Unload texture
rl.CloseWindow()               ' Close window and OpenGL context
