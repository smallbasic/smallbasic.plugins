REM /*******************************************************************************************
REM *
REM *   Physac - Physics restitution
REM *
REM *   Copyright (c) 2016-2018 Victor Fisac
REM *
REM ********************************************************************************************/

import raylib as rl
import raylibc as c

const screenWidth = 800
const screenHeight = 450

rl.SetConfigFlags(c.FLAG_MSAA_4X_HINT)
rl.InitWindow(screenWidth, screenHeight, "SmallBASIC Physac [raylib] - Physics restitution")

' Physac logo drawing position
logoX = screenWidth - rl.MeasureText("Physac", 30) - 10
logoY = 15

' Initialize physics and default physics bodies
rl.InitPhysics()

' Create ground rectangle physics body
ground = rl.CreatePhysicsBodyRectangle([screenWidth/2, screenHeight], screenWidth, 100, 10)

rl.setPhysBodyEnabled(ground, false) ' Disable body state to convert it to static (no dynamics, but collisions)
rl.setPhysBodyRestitution(ground, 1)

' Create circles physics body
circleA = rl.CreatePhysicsBodyCircle([screenWidth * 0.25, screenHeight / 2], 30, 10)
circleB = rl.CreatePhysicsBodyCircle([screenWidth * 0.5, screenHeight / 2], 30, 10)
circleC = rl.CreatePhysicsBodyCircle([screenWidth * 0.75, screenHeight / 2], 30, 10)

rl.setPhysBodyRestitution(circleA, 0)
rl.setPhysBodyRestitution(circleB, .5)
rl.setPhysBodyRestitution(circleC, 1)

' Restitution demo needs a very tiny physics time step for a proper simulation
rl.SetPhysicsTimeStep(.25)

rl.SetTargetFPS(60)                ' Set our game to run at 60 frames-per-second

while (!rl.WindowShouldClose())
  rl.RunPhysicsStep()
  
  if (rl.IsKeyPressed(asc("R"))) then    ' Reset physics input
    ' Reset circles physics bodies position and velocity
    rl.setPhysBodyPosition(circleA, [screenWidth * 0.25, screenHeight / 2])
    rl.setPhysBodyVelocity(circleA, [0, 0])
    rl.setPhysBodyPosition(circleB, [screenWidth * 0.5, screenHeight / 2])
    rl.setPhysBodyVelocity(circleB, [0, 0])
    rl.setPhysBodyPosition(circleC, [screenWidth * 0.75, screenHeight / 2])
    rl.setPhysBodyVelocity(circleC, [0, 0])
  endif

  rl.BeginDrawing()
  rl.ClearBackground(c.BLACK)
  rl.DrawFPS(screenWidth - 90, screenHeight - 30)

  ' Draw created physics bodies
  bodiesCount = rl.GetPhysicsBodiesCount()
  for i = 0  to bodiesCount - 1
    body = rl.GetPhysicsBody(i)
    vertexCount = rl.GetPhysicsShapeVerticesCount(i)
    for j = 0 to vertexCount - 1
      ' Get physics bodies shape vertices to draw lines
      ' Note: GetPhysicsShapeVertex() already calculates rotation transformations
      vertexA = rl.GetPhysicsShapeVertex(body, j)
      jj = iff(j + 1 < vertexCount, j + 1, 0)    ' Get next vertex or first to close the shape
      vertexB = rl.GetPhysicsShapeVertex(body, jj)
      rl.DrawLineV(vertexA, vertexB, c.GREEN)      ' Draw a line between two vertex positions
    next j
  next i
  
  posA = rl.getPhysBodyPosition(circleA)
  posB = rl.getPhysBodyPosition(circleB)
  posC = rl.getPhysBodyPosition(circleC)
  
  rl.DrawText("Restitution amount", (screenWidth - rl.MeasureText("Restitution amount", 30)) / 2, 75, 30, c.WHITE)
  rl.DrawText("0", posA.x - rl.MeasureText("0", 20)/2, posA.y - 7, 20, c.WHITE)
  rl.DrawText("0.5", posB.x - rl.MeasureText("0.5", 20)/2, posB.y - 7, 20, c.WHITE)
  rl.DrawText("1", posC.x - rl.MeasureText("1", 20)/2, posC.y - 7, 20, c.WHITE)
  rl.DrawText("Press 'R' to reset example", 10, 10, 10, c.WHITE)
  rl.DrawText("Physac", logoX, logoY, 30, c.WHITE)
  rl.DrawText("Powered by", logoX + 50, logoY - 7, 10, c.WHITE)
  rl.EndDrawing()
wend

'De-Initialization
rl.DestroyPhysicsBody(circleA)
rl.DestroyPhysicsBody(circleB)
rl.DestroyPhysicsBody(circleC)
rl.DestroyPhysicsBody(ground)
rl.ClosePhysics()        ' Unitialize physics
rl.CloseWindow()         ' Close window and OpenGL context
