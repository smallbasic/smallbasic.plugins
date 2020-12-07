REM /*******************************************************************************************
REM *
REM *   Physac - Physics movement
REM *
REM *   Copyright (c) 2016-2018 Victor Fisac
REM *
REM ********************************************************************************************/

import raylib as rl
import raylibc as c

const VELOCITY = 0.5
const screenWidth = 800 
const screenHeight = 450 

rl.SetConfigFlags(c.FLAG_MSAA_4X_HINT) 
rl.InitWindow(screenWidth, screenHeight, "SmallBASIC Physac [raylib] - Physics movement") 

' Physac logo drawing position
logoX = screenWidth - rl.MeasureText("Physac", 30) - 10 
logoY = 15 

' Initialize physics and default physics bodies
rl.InitPhysics() 

' Create ground and walls rectangle physics body
ground = rl.CreatePhysicsBodyRectangle([ screenWidth/2, screenHeight], screenWidth, 100, 10) 
platformLeft = rl.CreatePhysicsBodyRectangle([screenWidth*0.25, screenHeight*0.6], screenWidth*0.25, 10, 10) 
platformRight = rl.CreatePhysicsBodyRectangle([screenWidth*0.75, screenHeight*0.6], screenWidth*0.25, 10, 10) 
wallLeft = rl.CreatePhysicsBodyRectangle([ -5, screenHeight/2], 10, screenHeight, 10) 
wallRight = rl.CreatePhysicsBodyRectangle([ screenWidth + 5, screenHeight/2], 10, screenHeight, 10)

' Disable dynamics to ground and walls physics bodies
rl.SetPhysBodyEnabled(ground, false)
rl.SetPhysBodyEnabled(platformleft, false)
rl.SetPhysBodyEnabled(platformright, false)
rl.SetPhysBodyEnabled(wallLeft, false)
rl.SetPhysBodyEnabled(wallRight, false)
 
' Create movement physics body
body = rl.CreatePhysicsBodyRectangle([screenWidth/2, screenHeight/2], 50, 50, 1) 
rl.SetPhysBodyFreezeOrient(body, true)       ' Constrain body rotation to avoid little collision torque amounts

rl.SetTargetFPS(60)                ' Set our game to run at 60 frames-per-second
while (!rl.WindowShouldClose())
  rl.RunPhysicsStep() 

  if (rl.IsKeyPressed(asc("R"))) then   ' Reset physics input
    ' Reset movement physics body position, velocity and rotation
    rl.setPhysBodyPosition(body, [screenWidth/2, screenHeight/2])
    rl.setPhysBodyVelocity(body, [ 0, 0 ])
    rl.SetPhysicsBodyRotation(body, 0)
  endif

  ' Horizontal movement input
  if (rl.IsKeyDown(c.KEY_RIGHT)) then
    body.velocity.x = VELOCITY 
    rl.SetPhysBodyVelocity(body, body.velocity)
  else if (rl.IsKeyDown(c.KEY_LEFT)) then
    body.velocity.x = -VELOCITY 
    rl.SetPhysBodyVelocity(body, body.velocity)
  endif
  
  ' Vertical movement input checking if player physics body is grounded
  if (rl.IsKeyDown(c.KEY_UP) && body.isGrounded) then 
    body.velocity.y = -VELOCITY * 4
    rl.SetPhysBodyVelocity(body, body.velocity)
  endif

  rl.BeginDrawing() 
  rl.ClearBackground(c.BLACK) 
  rl.DrawFPS(screenWidth - 90, screenHeight - 30) 

  ' Draw created physics bodies
  bodiesCount = rl.GetPhysicsBodiesCount() 
  for i = 0 to bodiesCount - 1
    body = rl.GetPhysicsBody(i) 
    vertexCount = rl.GetPhysicsShapeVerticesCount(i) 
    for j = 0 to vertexCount - 1
      ' Get physics bodies shape vertices to draw lines
      ' Note: GetPhysicsShapeVertex() already calculates rotation transformations
      vertexA = rl.GetPhysicsShapeVertex(body, j)
      jj = iff(j + 1 < vertexCount, j + 1, 0)    ' Get next vertex or first to close the shape
      vertexB = rl.GetPhysicsShapeVertex(body, jj) 
      rl.DrawLineV(vertexA, vertexB, c.GREEN)    ' Draw a line between two vertex positions
    next j
  next i

  rl.DrawText("Use 'ARROWS' to move player", 10, 10, 10, c.WHITE) 
  rl.DrawText("Press 'R' to reset example", 10, 30, 10, c.WHITE) 
  rl.DrawText("Physac", logoX, logoY, 30, c.WHITE) 
  rl.DrawText("Powered by", logoX + 50, logoY - 7, 10, c.WHITE) 
  rl.EndDrawing() 
wend

rl.ClosePhysics()        ' Unitialize physics
rl.CloseWindow()         ' Close window and OpenGL context
