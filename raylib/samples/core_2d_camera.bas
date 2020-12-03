REM /*******************************************************************************************
REM *
REM *   raylib [core] example - 2d camera
REM *
REM *   This example has been created using raylib 1.5 (www.raylib.com)
REM *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM *
REM *   Copyright (c) 2016 Ramon Santamaria (@raysan5)
REM *
REM ********************************************************************************************/

import raylib as rl
import raylibc as c

const MAX_BUILDINGS = 100
const screenWidth = 800 
const screenHeight = 450 

rl.InitWindow(screenWidth, screenHeight, "SmallBASIC raylib [core] example - 2d camera") 

player = {x:400, y:280, width:40, height:40}
dim buildings(MAX_BUILDINGS)
dim buildColors(MAX_BUILDINGS)

spacing = 0 

for i = 0 to MAX_BUILDINGS - 1
  buildings[i].width = rl.GetRandomValue(50, 200) 
  buildings[i].height = rl.GetRandomValue(100, 800) 
  buildings[i].y = screenHeight - 130 - buildings[i].height 
  buildings[i].x = -6000 + spacing 
  spacing += buildings[i].width 
  buildColors[i] = [rl.GetRandomValue(200, 240), rl.GetRandomValue(200, 240), rl.GetRandomValue(200, 250)]
next i

camera.target.x = player.x + 20
camera.target.y = player.y + 20
camera.offset.x = screenWidth / 2
camera.offset.y = screenHeight / 2
camera.rotation = 0.0
camera.zoom = 1.0

rl.SetTargetFPS(60)                    ' Set our game to run at 60 frames-per-second
while (!rl.WindowShouldClose())
  ' Player movement
  if (rl.IsKeyDown(c.KEY_RIGHT)) then 
    player.x += 12 
  else if (rl.IsKeyDown(c.KEY_LEFT)) then
    player.x -= 12 
  endif
  
  ' Camera target follows player
  camera.target.x = player.x + 20
  camera.target.y = player.y + 20

  ' Camera rotation controls
  if (rl.IsKeyDown(asc("A"))) then
    camera.rotation--
  else if (rl.IsKeyDown(asc("S"))) then
    camera.rotation++ 
  endif
  
  ' Limit camera rotation to 80 degrees (-40 to 40)
  if (camera.rotation > 40) then
    camera.rotation = 40 
  else if (camera.rotation < -40) then
    camera.rotation = -40 
  endif
  
  ' Camera zoom controls
  camera.zoom += (rl.GetMouseWheelMove() * 0.05) 

  if (camera.zoom > 3.0) then
    camera.zoom = 3.0
  else if (camera.zoom < 0.1) then
    camera.zoom = 0.1
  endif
  
  ' Camera reset (zoom and rotation)
  if (rl.IsKeyPressed(c.KEY_R)) then
    camera.zoom = 1.0
    camera.rotation = 0.0
  endif

  rl.BeginDrawing() 
  rl.ClearBackground(c.RAYWHITE) 
  rl.BeginMode2D(camera) 
  rl.DrawRectangle(-6000, 320, 13000, 8000, c.DARKGRAY) 
  for i = 0 to MAX_BUILDINGS - 1
    rl.DrawRectangleRec(buildings[i], buildColors[i]) 
  next i
  
  rl.DrawRectangleRec(player, c.RED) 
  rl.DrawLine(camera.target.x, -screenHeight * 10, camera.target.x, screenHeight * 10, c.GREEN) 
  rl.DrawLine(-screenWidth * 10, camera.target.y, screenWidth * 10, camera.target.y, c.GREEN) 
  rl.EndMode2D() 

  rl.DrawText("SCREEN AREA", 640, 10, 20, c.RED) 
  rl.DrawRectangle(0, 0, screenWidth, 5, c.RED) 
  rl.DrawRectangle(0, 5, 5, screenHeight - 10, c.RED) 
  rl.DrawRectangle(screenWidth - 5, 5, 5, screenHeight - 10, c.RED) 
  rl.DrawRectangle(0, screenHeight - 5, screenWidth, 5, c.RED) 
  rl.DrawRectangle( 10, 10, 250, 113, rl.Fade(c.SKYBLUE, 0.5)) 
  rl.DrawRectangleLines( 10, 10, 250, 113, c.BLUE) 
  rl.DrawText("Free 2d camera controls:", 20, 20, 10, c.BLACK) 
  rl.DrawText("- Right/Left to move Offset", 40, 40, 10, c.DARKGRAY) 
  rl.DrawText("- Mouse Wheel to Zoom in-out", 40, 60, 10, c.DARKGRAY) 
  rl.DrawText("- A / S to Rotate", 40, 80, 10, c.DARKGRAY) 
  rl.DrawText("- R to reset Zoom and Rotation", 40, 100, 10, c.DARKGRAY) 
  rl.EndDrawing() 
wend

rl.CloseWindow()         ' Close window and OpenGL context
