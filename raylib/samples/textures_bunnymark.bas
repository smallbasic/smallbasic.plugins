REM -----------------------------------------------------------------------------------------
REM 
REM    raylib [textures] example - Bunnymark
REM 
REM    This example has been created using raylib 1.7 (www.raylib.com)
REM    raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM 
REM    Copyright (c) 2014-2019 Ramon Santamaria (@raysan5)
REM 
REM -----------------------------------------------------------------------------------------

import raylib as rl
import raylibc as c

local MAX_BUNNIES = 100000    ' 100K bunnies limit

REM  This is the maximum amount of elements (quads) per batch
REM  NOTE: This value is defined in [rlgl] module and can be changed there
local MAX_BATCH_ELEMENTS = 8192
local screenWidth = 800
local screenHeight = 450

func newBunny(pos, spd, col)
  local bunny
  bunny.position = pos
  bunny.speed = spd
  bunny.col = col
  return bunny
end

sub updateBunny(byref o)
  o.position.x = o.position.x + o.speed.x
  o.position.y = o.position.y + o.speed.y
  if ((o.position.x + texBunny.width/2) > rl.GetScreenWidth() || (o.position.x + texBunny.width/2) < 0) then
    o.speed.x = o.speed.x * -1
  endif
  if ((o.position.y + texBunny.height/2) > rl.GetScreenHeight() || (o.position.y + texBunny.height/2 - 40) < 0) then
    o.speed.y = o.speed.y * -1
  endif
end

rl.InitWindow(screenWidth, screenHeight, "raylib [textures] example - bunnymark")
texBunny = rl.LoadTexture(CWD + "raylib/examples/textures/resources/wabbit_alpha.png")

randomize timer
dim bunnies
rl.SetTargetFPS(60)

while (!rl.WindowShouldClose())
  if rl.IsMouseButtonDown(c.MOUSE_LEFT_BUTTON) then
    for i = 0 to 100
      if (len(bunnies) < MAX_BUNNIES) then
        speed.x = ((rnd * 1000) % 60)
        speed.y = ((rnd * 1000) % 60)
        col = rgb(((rnd * 1000) % 255), ((rnd * 1000) % 255), ((rnd * 1000) % 255))
        bunnies << newBunny(rl.GetMousePosition(), speed, col)
      endif
    next i
  endif

  # Update bunnies
  for i = 0 to len(bunnies) - 1
    updateBunny(bunnies[i])
  next i

  rl.BeginDrawing()
  rl.ClearBackground(RAYWHITE);

  for i = 0 to len(bunnies) - 1
    ' NOTE: When internal batch buffer limit is reached (MAX_BATCH_ELEMENTS),
    ' a draw call is launched and buffer starts being filled again;
    ' before issuing a draw call, updated vertex data from internal CPU buffer is send to GPU...
    ' Process of sending data is costly and it could happen that GPU data has not been completely
    ' processed for drawing while new data is tried to be sent (updating current in-use buffers)
    ' it could generates a stall and consequently a frame drop, limiting the number of drawn bunnies
    rl.DrawTexture(texBunny, floor(bunnies[i].position.x), floor(bunnies[i].position.y), bunnies[i].col)
  next i

  rl.DrawRectangle(0, 0, screenWidth, 40, c.BLACK)
  rl.DrawText("bunnies: " + len(bunnies), 120, 10, 20, c.GREEN)
  rl.DrawText("batched draw calls: " + (1 + len(bunnies) / MAX_BATCH_ELEMENTS), 320, 10, 20, c.MAROON)
  rl.DrawText(Format("bunnies: #####", len(bunnies)), 120, 10, 20, c.GREEN)
  rl.DrawText(Format("batched draw calls: ###", 1 + len(bunnies)/MAX_BATCH_ELEMENTS), 320, 10, 20, c.MAROON)
  rl.DrawFPS(10, 10)
  rl.EndDrawing()
wend

rl.UnloadTexture(texBunny)    ' Unload bunny texture
rl.CloseWindow()              ' Close window and OpenGL context

