'
' raylib [shapes] example - collision area
'
' This example has been created using raylib 2.5 (www.raylib.com)
' raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
'
' Copyright (c) 2013-2019 Ramon Santamaria (@raysan5)
'

import raylib as rl
import raylibc as c

const screenWidth = 800
const screenHeight = 450

rl.InitWindow(screenWidth, screenHeight, "raylib [shapes] example - collision area")

func mk_rect(x, y, w, h)
  local result
  result.x = x
  result.y = y
  result.width = w
  result.height = h
  return result
end  

' Box A: Moving box
boxA = mk_rect(10, rl.GetScreenHeight()/2 - 50, 200, 100)
boxASpeedX = 4

' Box B: Mouse moved box
boxB = mk_rect(rl.GetScreenWidth()/2 - 30, rl.GetScreenHeight()/2 - 30, 60, 60)

boxCollision = [ 0 ] ' Collision rectangle

screenUpperLimit = 40     ' Top menu limits
paused = false            ' Movement pause
collision = false         ' Collision detection

rl.SetTargetFPS(60)       ' Set our game to run at 60 frames-per-second

while (!rl.WindowShouldClose())
  ' Update
  '-----------------------------------------------------
  ' Move box if not paused
  if (!paused) then boxA.x += boxASpeedX

  ' Bounce box on x screen limits
  if (((boxA.x + boxA.width) >= rl.GetScreenWidth()) || (boxA.x <= 0)) then 
    boxASpeedX *= -1
  endif

  ' Update player-controlled-box (box02)
  boxB.x = rl.GetMouseX() - boxB.width / 2
  boxB.y = rl.GetMouseY() - boxB.height / 2

  ' Make sure Box B does not go out of move area limits
  if ((boxB.x + boxB.width) >= rl.GetScreenWidth()) then
    boxB.x = GetScreenWidth() - boxB.width
  elseif (boxB.x <= 0) 
    boxB.x = 0
  endif

  if ((boxB.y + boxB.height) >= rl.GetScreenHeight()) then
    boxB.y = rl.GetScreenHeight() - boxB.height
  elseif (boxB.y <= screenUpperLimit) then 
    boxB.y = screenUpperLimit
  endif
    
  ' Check boxes collision
  collision = rl.CheckCollisionRecs(boxA, boxB)

  ' Get collision rectangle (only on collision)
  if (collision) then boxCollision = rl.GetCollisionRec(boxA, boxB)

  ' Pause Box A movement
  if (rl.IsKeyPressed(c.KEY_SPACE)) then
    paused = !paused
  endif
  
  ' Draw
  rl.BeginDrawing()
  rl.ClearBackground(c.RAYWHITE)
  rectc = iff(collision==true, c.RED, c.BLACK)
  rl.DrawRectangle(0, 0, screenWidth, screenUpperLimit, rectc)
  rl.DrawRectangleRec(boxA, c.GOLD)
  rl.DrawRectangleRec(boxB, c.BLUE)

  if (collision) then
    ' Draw collision area
    rl.DrawRectangleRec(boxCollision, c.LIME)

    ' Draw collision message
    rl.DrawText("COLLISION!", rl.GetScreenWidth()/2 - rl.MeasureText("COLLISION!", 20)/2, screenUpperLimit/2 - 10, 20, c.BLACK)

    ' Draw collision area
    rl.DrawText(Format("Collision Area: ####", boxCollision.width * boxCollision.height), rl.GetScreenWidth()/2 - 100, screenUpperLimit + 10, 20, c.BLACK)
  endif

  rl.DrawFPS(10, 10)
  rl.EndDrawing()
wend

' De-Initialization
rl.CloseWindow()        ' Close window and OpenGL context
