REM ----------------------------------------------------------------------------------------
REM
REM   raylib [textures] example - Texture loading and drawing a part defined by a rectangle
REM
REM   This example has been created using raylib 1.6 (www.raylib.com)
REM   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM
REM   Copyright (c) 2014-2016 Ramon Santamaria (@raysan5)
REM
REM ----------------------------------------------------------------------------------------

import raylib as rl
import raylibc as c

local MAX_FRAME_SPEED = 14
local MIN_FRAME_SPEED = 1
local screenWidth = 800
local screenHeight = 450
local position = [350.0, 280.0]
local frameRec = {x:0, y:0, width:scarfy.width/6, height:scarfy.height}
local currentFrame = 0
local framesCounter = 0
local framesSpeed = 8

rl.InitWindow(screenWidth, screenHeight, "raylib [texture] example - texture rectangle")
REM  NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
local scarfy = rl.LoadTexture("../raylib/examples/textures/resources/scarfy.png")

rl.SetTargetFPS(60)

REM  Main game loop
while (!rl.WindowShouldClose())
  framesCounter = framesCounter + 1

  if (framesCounter >= (60/framesSpeed)) then
    framesCounter = 0
    currentFrame = currentFrame + 1
    if (currentFrame > 5) then currentFrame = 0
    frameRec.x = currentFrame * scarfy.width / 6
  endif

  if (rl.IsKeyPressed(c.KEY_RIGHT)) then
    framesSpeed = framesSpeed + 1
  elseif (rl.IsKeyPressed(c.KEY_LEFT)) then
    framesSpeed = framesSpeed - 1
  endif

  if (framesSpeed > MAX_FRAME_SPEED) then
    framesSpeed = MAX_FRAME_SPEED
  elseif (framesSpeed < MIN_FRAME_SPEED) then
    framesSpeed = MIN_FRAME_SPEED
  endif

  rl.BeginDrawing()
  rl.ClearBackground(c.RAYWHITE)
  rl.DrawTexture(scarfy, 15, 40, c.WHITE)
  rl.DrawRectangleLines(15, 40, scarfy.width, scarfy.height, c.LIME)
  rl.DrawRectangleLines(15 + frameRec.x, 40 + frameRec.y, frameRec.width, frameRec.height, c.RED)
  rl.DrawText("FRAME SPEED: ", 165, 210, 10, c.DARKGRAY)
  rl.DrawText(format("## FPS", framesSpeed), 575, 210, 10, c.DARKGRAY)
  rl.DrawText("PRESS RIGHT/LEFT KEYS to CHANGE SPEED!", 290, 240, 10, c.DARKGRAY)

  for i = 1 to MAX_FRAME_SPEED
    if (i <= framesSpeed) then
      rl.DrawRectangle(250 + 21*i, 205, 20, 20, c.RED)
    endif
    rl.DrawRectangleLines(250 + 21*i, 205, 20, 20, c.MAROON)
  next i

  rl.DrawTextureRec(scarfy, frameRec, position, c.WHITE)
  rl.DrawText("(c) Scarfy sprite by Eiden Marsal", screenWidth - 200, screenHeight - 20, 10, c.GRAY)
  rl.EndDrawing()
wend

rl.UnloadTexture(scarfy)
rl.CloseWindow()

