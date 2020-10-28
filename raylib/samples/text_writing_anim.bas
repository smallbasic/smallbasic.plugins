REM ----------------------------------------------------------------------------------------
REM 
REM   raylib [text] example - Text Writing Animation
REM 
REM   This example has been created using raylib 1.6 (www.raylib.com)
REM   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM 
REM   Copyright (c) 2014-2016 Ramon Santamaria (@raysan5)
REM 
REM ----------------------------------------------------------------------------------------

REM  Initialization
REM ----------------------------------------------------------------------------------------

import raylib as rl
import raylib_const as c

local screenWidth = 800
local screenHeight = 450
local message = "This sample illustrates a text writing\nanimation effect! Check it out! ;)"
local framesCounter = 0

rl.InitWindow(screenWidth, screenHeight, "raylib [text] example - text writing anim")
rl.SetTargetFPS(60)

while (!rl.WindowShouldClose())
  if (rl.IsKeyDown(c.KEY_SPACE)) then
    framesCounter += 8
  else 
    framesCounter++
  endif    

  if (rl.IsKeyPressed(c.KEY_ENTER)) then 
    framesCounter = 0
  endif
  
  if (rl.IsKeyPressed(c.KEY_ESCAPE)) then 
    exit loop
  endif
  
  if (framesCounter/10 > len(message)) then
    framesCounter = 0
  endif
  
  rl.BeginDrawing()
  rl.ClearBackground(c.RAYWHITE)

  rl.DrawText(mid(message, 1, framesCounter/10), 210, 160, 20, c.MAROON)
  rl.DrawText("PRESS [ENTER] to RESTART!", 240, 280, 20, c.LIGHTGRAY)
  rl.DrawText("PRESS [SPACE] to SPEED UP!", 239, 300, 20, c.LIGHTGRAY)

  rl.EndDrawing()
wend

rl.CloseWindow()
