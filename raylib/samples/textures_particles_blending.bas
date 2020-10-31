REM ----------------------------------------------------------------------------------------
REM 
REM   raylib example - particles trail blending
REM 
REM   This example has been created using raylib 1.6 (www.raylib.com)
REM   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM 
REM   Copyright (c) 2014-2016 Ramon Santamaria (@raysan5)
REM 
REM ----------------------------------------------------------------------------------------

import raylib as rl
import raylibc as c

const MAX_PARTICLES = 200
local screenWidth = 800
local screenHeight = 450

rl.InitWindow(screenWidth, screenHeight, "raylib [textures] example - particles trail blending")

REM Particles pool, reuse them!
dim mouseTail(MAX_PARTICLES)

randomize timer

REM Initialize particles
for i = 0 to MAX_PARTICLES
  mouseTail[i].position = [0, 0]
  mouseTail[i].col = rgb(((rnd * 1000) % 255), ((rnd * 1000) % 255), ((rnd * 1000) % 255))
  mouseTail[i].alpha = 1.0
  mouseTail[i].size = 1 + ((rnd * 1000) % 20)
  mouseTail[i].rotation = 1 + ((rnd * 1000) % 360) 
  mouseTail[i].active = false
next i

local gravity = 3.0
local smoke = rl.LoadTexture(CWD + "raylib/examples/textures/resources/spark_flame.png")
local blending = c.BLEND_ALPHA

rl.SetTargetFPS(60)

while (!rl.WindowShouldClose())
  rem Activate one particle every frame and Update active particles
  rem NOTE: Particles initial position should be mouse position when activated
  rem NOTE: Particles fall down with gravity and rotation... and disappear after 2 seconds (alpha = 0)
  rem NOTE: When a particle disappears, active = false and it can be reused.
  
  for i = 0 to MAX_PARTICLES
    if (!mouseTail[i].active) then
      mouseTail[i].active = true
      mouseTail[i].alpha = 1.0
      mouseTail[i].position = GetMousePosition()
      exit for
    endif
  next i

  for i = 0 to MAX_PARTICLES
    if (mouseTail[i].active) then
      mouseTail[i].position.y = mouseTail[i].position.y + gravity
      mouseTail[i].alpha = mouseTail[i].alpha - 0.01
      if (mouseTail[i].alpha <= 0.0) then 
        mouseTail[i].active = false 
      endif
      mouseTail[i].rotation = mouseTail[i].rotation + 5.0
    endif
  next i

  if (rl.IsKeyPressed(c.KEY_SPACE)) then
    if (blending == c.BLEND_ALPHA) then 
      blending = c.BLEND_ADDITIVE
    else 
      blending = c.BLEND_ALPHA 
    endif
  endif

  rl.BeginDrawing()
  rl.ClearBackground(c.DARKGRAY)
  rl.BeginBlendMode(blending)

  rem Draw active particles
  for i = 0 to MAX_PARTICLES
    if (mouseTail[i].active) then
      sourceRec = [0, 0, smoke.width, smoke.height]
      destRec =  [mouseTail[i].position.x, mouseTail[i].position.y, smoke.width * mouseTail[i].size, smoke.height * mouseTail[i].size]
      origin = [smoke.width * mouseTail[i].size / 2,  smoke.height * mouseTail[i].size / 2]
      rotation = 0
      tint = rl.Fade(mouseTail[i].col, mouseTail[i].alpha)
      rl.DrawTexturePro(smoke, sourceRec, destRec, origin, mouseTail[i].rotation, tint)
    endif
  next i

  rl.EndBlendMode()
  rl.DrawText("PRESS SPACE to CHANGE BLENDING MODE", 180, 20, 20, c.BLACK)

  if (blending == c.BLEND_ALPHA) then 
    rl.DrawText("ALPHA BLENDING", 290, screenHeight - 40, 20, c.BLACK)
  else 
    rl.DrawText("ADDITIVE BLENDING", 280, screenHeight - 40, 20, c.RAYWHITE) 
  endif

  rl.EndDrawing()
wend

rl.UnloadTexture(smoke)
rl.CloseWindow()

