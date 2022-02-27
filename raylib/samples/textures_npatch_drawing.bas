REM /*******************************************************************************************
REM *
REM *   raylib [textures] example - N-patch drawing
REM *
REM *   NOTE: Images are loaded in CPU memory (RAM); textures are loaded in GPU memory (VRAM)
REM *
REM *   This example has been created using raylib 2.0 (www.raylib.com)
REM *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM *
REM *   Example contributed by Jorge A. Gomes (@overdev) and reviewed by Ramon Santamaria (@raysan5)
REM *
REM *   Copyright (c) 2018 Jorge A. Gomes (@overdev) and Ramon Santamaria (@raysan5)
REM *
REM ********************************************************************************************/

import raylib as rl
import raylibc as c

const screenWidth = 800
const screenHeight = 450

rl.InitWindow(screenWidth, screenHeight, "raylib [textures] example - N-patch drawing")

const resources = CWD + "raylib/examples/textures/resources/"

' NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
nPatchTexture = rl.LoadTexture(resources + "ninepatch_button.png")

origin = [0, 0]

' Position and size of the n-patches
dstRec1 = { x:480.0, y:160.0, width: 32.0, height:32.0 }
dstRec2 = { x:160.0, y:160.0, width: 32.0, height:32.0 }
dstRecH = { x:160.0, y:93.0,  width: 32.0, height:32.0 }
dstRecV = { x:92.0, y:160.0,  width: 32.0, height:32.0 }

' A 9-patch (NPATCH_NINE_PATCH) changes its sizes in both axis
ninePatchInfo1 = { source: {x:0, y:0, width: 64, height: 64}, left:12, top: 40, right: 12, bottom: 12, layout: 0}
ninePatchInfo2 = { source: {x:0, y:128, width: 64, height: 64}, left:16, top: 16, right: 16, bottom: 16, layout: 0}

' A horizontal 3-patch (NPATCH_THREE_PATCH_HORIZONTAL) changes its sizes along the x axis only
h3PatchInfo = { source: {x:0, y:64, width: 64, height: 64}, left:8, top: 8, right: 8, bottom: 8, layout: 2}

' A vertical 3-patch (NPATCH_THREE_PATCH_VERTICAL) changes its sizes along the y axis only
v3PatchInfo = { source: {x:0, y:192, width: 64, height: 64}, left:6, top: 6, right: 6, bottom: 6, layout: 1}

rl.SetTargetFPS(60)
while (!rl.WindowShouldClose())
  ' Update
  mousePosition = rl.GetMousePosition()

  ' Resize the n-patches based on mouse position
  dstRec1.width = mousePosition.x - dstRec1.x
  dstRec1.height = mousePosition.y - dstRec1.y
  dstRec2.width = mousePosition.x - dstRec2.x
  dstRec2.height = mousePosition.y - dstRec2.y
  dstRecH.width = mousePosition.x - dstRecH.x
  dstRecV.height = mousePosition.y - dstRecV.y

  ' Set a minimum width and/or height
  if (dstRec1.width < 1.0) then dstRec1.width = 1.0
  if (dstRec1.width > 300.0) then dstRec1.width = 300.0
  if (dstRec1.height < 1.0) then dstRec1.height = 1.0
  if (dstRec2.width < 1.0) then dstRec2.width = 1.0
  if (dstRec2.width > 300.0) then dstRec2.width = 300.0
  if (dstRec2.height < 1.0) then dstRec2.height = 1.0
  if (dstRecH.width < 1.0) then dstRecH.width = 1.0
  if (dstRecV.height < 1.0) then dstRecV.height = 1.0

  ' Draw
  '----------------------------------------------------------------------------------
  rl.BeginDrawing()
  rl.ClearBackground(c.WHITE)

  ' Draw the n-patches
  rl.DrawTextureNPatch(nPatchTexture, ninePatchInfo2, dstRec2, origin, 0.0, c.WHITE)
  rl.DrawTextureNPatch(nPatchTexture, ninePatchInfo1, dstRec1, origin, 0.0, c.WHITE)
  rl.DrawTextureNPatch(nPatchTexture, h3PatchInfo, dstRecH, origin, 0.0, c.WHITE)
  rl.DrawTextureNPatch(nPatchTexture, v3PatchInfo, dstRecV, origin, 0.0, c.WHITE)

  ' Draw the source texture
  rl.DrawRectangleLines(5, 88, 74, 266, c.BLUE)
  rl.DrawTexture(nPatchTexture, 10, 93, c.WHITE)
  rl.DrawText("TEXTURE", 15, 360, 10, c.DARKGRAY)

  rl.DrawText("Move the mouse to stretch or shrink the n-patches", 10, 20, 20, c.DARKGRAY)
  rl.EndDrawing()
wend

rl.UnloadTexture(nPatchTexture)   ' Texture unloading
rl.CloseWindow()                  ' Close window and OpenGL context
