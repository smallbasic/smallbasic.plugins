REM /*******************************************************************************************
REM *
REM *   Physac - Physics friction
REM *
REM *   Copyright (c) 2016-2018 Victor Fisac
REM *
REM ********************************************************************************************/

import raylib as rl
import raylibc as c

const screenWidth = 800
const screenHeight = 450

rl.SetConfigFlags(c.FLAG_MSAA_4X_HINT)
rl.InitWindow(screenWidth, screenHeight, "SmallBASIC Physac [raylib] - Physics friction")

' Physac logo drawing position
logoX = screenWidth - rl.MeasureText("Physac", 30) - 10
logoY = 15

' Initialize physics and default physics bodies
rl.InitPhysics()

' Create floor rectangle physics body
ground = rl.CreatePhysicsBodyRectangle([screenWidth/2, screenHeight], screenWidth, 100, 10)
rl.SetPhysicsBodyEnabled(ground, false) ' Disable body state to convert it to static (no dynamics, but collisions)

wall = rl.CreatePhysicsBodyRectangle([screenWidth/2, screenHeight*0.8], 10, 80, 10)
rl.SetPhysicsBodyEnabled(wall, false) ' Disable body state to convert it to static (no dynamics, but collisions)

' Create left ramp physics body
rectLeft = rl.CreatePhysicsBodyRectangle([25, screenHeight - 5], 250, 250, 10)
rl.SetPhysicsBodyEnabled(rectLeft, false) ' Disable body state to convert it to static (no dynamics, but collisions)
rl.SetPhysicsBodyRotation(rectLeft, 30 * c.DEG2RAD)

' Create right ramp  physics body
rectRight = rl.CreatePhysicsBodyRectangle([screenWidth - 25, screenHeight - 5], 250, 250, 10)
rl.SetPhysicsBodyEnabled(rectRight, false) ' Disable body state to convert it to static (no dynamics, but collisions)
rl.SetPhysicsBodyRotation(rectRight, 330 * c.DEG2RAD)

' Create dynamic physics bodies
bodyA = rl.CreatePhysicsBodyRectangle([35, screenHeight * 0.6], 40, 40, 10)
rl.SetPhysicsBodyStaticFriction(bodyA, 0.1)
rl.SetPhysicsBodyDynamicFriction(bodyA, 0.1)
rl.SetPhysicsBodyRotation(bodyA, 30 * c.DEG2RAD)

bodyB = rl.CreatePhysicsBodyRectangle([screenWidth - 35, screenHeight * 0.6], 40, 40, 10)
rl.SetPhysicsBodyStaticFriction(bodyB, 1.0)
rl.SetPhysicsBodyDynamicFriction(bodyB, 1.0)
rl.SetPhysicsBodyRotation(bodyB, 330 * c.DEG2RAD)

rl.SetTargetFPS(60)               ' Set our game to run at 60 frames-per-second
'--------------------------------------------------------------------------------------

' Main game loop
while (!rl.WindowShouldClose())
  rl.RunPhysicsStep()

  if (rl.IsKeyPressed(asc("R"))) then   ' Reset physics input
    ' Reset dynamic physics bodies position, velocity and rotation
    rl.setPhysicsBodyPosition(bodyA, [35, screenHeight * 0.6])
    rl.setPhysicsBodyVelocity(bodyA, [0, 0])
    rl.setPhysicsBodyAngularVelocity(bodyA, 0)
    rl.SetPhysicsBodyRotation(bodyA, 30 * c.DEG2RAD)
    rl.setPhysicsBodyPosition(bodyB, [screenWidth - 35, screenHeight * 0.6])
    rl.setPhysicsBodyVelocity(bodyB, [0, 0])
    rl.setPhysicsBodyAngularVelocity(bodyB, 0)
    rl.SetPhysicsBodyRotation(bodyB, 330 * c.DEG2RAD)
  endif
  
  rl.BeginDrawing()
  rl.ClearBackground(c.BLACK)
  rl.DrawFPS(screenWidth - 90, screenHeight - 30)

  ' Draw created physics bodies
  bodiesCount = rl.GetPhysicsBodiesCount()
  for i = 0  to bodiesCount - 1
    body = rl.GetPhysicsBody(i)
    if (ismap(body)) then    
      vertexCount = rl.GetPhysicsShapeVerticesCount(i)
      for j = 0 to vertexCount - 1
        ' Get physics bodies shape vertices to draw lines
        ' Note: GetPhysicsShapeVertex() already calculates rotation transformations
        vertexA = rl.GetPhysicsShapeVertex(body, j)
        jj = iff(j + 1 < vertexCount, j + 1, 0)    ' Get next vertex or first to close the shape
        vertexB = rl.GetPhysicsShapeVertex(body, jj)
        rl.DrawLineV(vertexA, vertexB, c.GREEN)      ' Draw a line between two vertex positions
      next j
    endif
  next i

  posA = bodyA.position
  posB = bodyB.position
  
  rl.DrawRectangle(0, screenHeight - 49, screenWidth, 49, c.BLACK)
  rl.DrawText("Friction amount", (screenWidth - rl.MeasureText("Friction amount", 30)) / 2, 75, 30, c.WHITE)
  rl.DrawText("0.1", posA.x - rl.MeasureText("0.1", 20) / 2, posA.y - 7, 20, c.WHITE)
  rl.DrawText("1", posB.x - rl.MeasureText("1", 20) / 2, posB.y - 7, 20, c.WHITE)
  rl.DrawText("Press 'R' to reset example", 10, 10, 10, c.WHITE)
  rl.DrawText("Physac", logoX, logoY, 30, c.WHITE)
  rl.DrawText("Powered by", logoX + 50, logoY - 7, 10, c.WHITE)
  rl.EndDrawing()
wend

rl.ClosePhysics()       ' Unitialize physics
rl.CloseWindow()        ' Close window and OpenGL context
