REM /*******************************************************************************************
REM *
REM *   Physac - Physics demo
REM *
REM *   Copyright (c) 2016-2018 Victor Fisac
REM *
REM ********************************************************************************************/

import raylib as rl
import raylibc as c

const screenWidth = 800
const screenHeight = 450

rl.SetConfigFlags(c.FLAG_MSAA_4X_HINT)
rl.InitWindow(screenWidth, screenHeight, "SmallBASIC Physac [raylib] - Physics demo")

' Physac logo drawing position
logoX = screenWidth - rl.MeasureText("Physac", 30) - 10
logoY = 15
needsReset = false

' Initialize physics and default physics bodies
rl.InitPhysics()

' Create floor rectangle physics body
phy_floor = rl.CreatePhysicsBodyRectangle([screenWidth/2, screenHeight], 500, 100, 10)

' Create obstacle circle physics body
phy_circle = rl.CreatePhysicsBodyCircle([screenWidth / 2, screenHeight / 2], 45, 10)

' Disable body state to convert it to static (no dynamics, but collisions)
rl.enablePhysicsBody(phy_floor, false)
rl.enablePhysicsBody(phy_circle, false)

rl.SetTargetFPS(60)                ' Set our game to run at 60 frames-per-second
while (!rl.WindowShouldClose())
  rl.RunPhysicsStep()
  if (needsReset) then
    phy_floor = rl.CreatePhysicsBodyRectangle([screenWidth/2, screenHeight], 500, 100, 10)
    phy_circle = rl.CreatePhysicsBodyCircle([screenWidth/2, screenHeight/2], 45, 10)
    rl.enablePhysicsBody(phy_floor, false)
    rl.enablePhysicsBody(phy_circle, false)
    needsReset = false
  endif

  ' Reset physics input
  if (rl.IsKeyPressed(asc("R"))) then
    rl.ResetPhysics()
    needsReset = true
  endif

  ' Physics body creation inputs
  if (rl.isMouseButtonPressed(c.MOUSE_LEFT_BUTTON)) then
    n = rl.CreatePhysicsBodyPolygon(rl.GetMousePosition(), rl.GetRandomValue(20, 80), rl.GetRandomValue(3, 8), 10)
  elseif (rl.IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) then
    n = rl.CreatePhysicsBodyCircle(rl.GetMousePosition(), rl.GetRandomValue(10, 45), 10)
  endif

  ' Destroy falling physics bodies
  bodiesCount = rl.GetPhysicsBodiesCount()
  for i = bodiesCount - 1  to 0 step -1
    body = rl.GetPhysicsBody(i)
    if (body.position.y > screenHeight*2) then 
      rl.DestroyPhysicsBody(body)
    endif
  next i

  rl.BeginDrawing()
  rl.ClearBackground(c.BLACK)
  rl.DrawFPS(screenWidth - 90, screenHeight - 30)

  ' Draw created physics bodies
  bodiesCount = rl.GetPhysicsBodiesCount()
  for i = 0 to bodiesCount - 1
    body = rl.GetPhysicsBody(i)
    if (ismap(body)) then
      vertexCount = rl.GetPhysicsShapeVerticesCount(i)
      for j = 0 to vertexCount -1
        ' Get physics bodies shape vertices to draw lines
        ' Note: GetPhysicsShapeVertex() already calculates rotation transformations
        vertexA = rl.GetPhysicsShapeVertex(body, j)
        jj = iff(j + 1 < vertexCount, j + 1, 0)    ' Get next vertex or first to close the shape
        vertexB = rl.GetPhysicsShapeVertex(body, jj)
        rl.DrawLineV(vertexA, vertexB, c.GREEN)     ' Draw a line between two vertex positions
      next j
    endif
  next i

  rl.DrawText("Left mouse button to create a polygon", 10, 10, 10, c.WHITE)
  rl.DrawText("Right mouse button to create a circle", 10, 25, 10, c.WHITE)
  rl.DrawText("Press 'R' to reset example", 10, 40, 10, c.WHITE)
  rl.DrawText("Physac", logoX, logoY, 30, c.WHITE)
  rl.DrawText("Powered by", logoX + 50, logoY - 7, 10, c.WHITE)
  rl.EndDrawing()
wend

rl.ClosePhysics()        ' Unitialize physics
rl.CloseWindow()         ' Close window and OpenGL context
