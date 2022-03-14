REM /*******************************************************************************************
REM *
REM *   raylib [core] example - 3d camera first person
REM *
REM *   This example has been created using raylib 1.3 (www.raylib.com)
REM *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM *
REM *   Copyright (c) 2015 Ramon Santamaria (@raysan5)
REM *
REM ********************************************************************************************/

import raylib as rl
import raylibc as c
import debug

const MAX_COLUMNS 20
const screenWidth = 800
const screenHeight = 450

rl.InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera first person")

' Define the camera to look into our 3d world (position, target, up vector)
camera = {}
camera.position = [4.0, 2.0, 4.0]
camera.target = [0.0, 1.8, 0.0]
camera.up = [0.0, 1.0, 0.0]
camera.fovy = 60.0
camera.projection = c.CAMERA_PERSPECTIVE

' Generates some random columns
dim heights(MAX_COLUMNS)
dim positions(MAX_COLUMNS)
dim colors(MAX_COLUMNS)

for i = 0 to MAX_COLUMNS - 1
  heights[i] = rl.GetRandomValue(1, 12)
  positions[i] = [rl.GetRandomValue(-15, 15), heights[i]/2.0, rl.GetRandomValue(-15, 15)]
  colors[i] = [rl.GetRandomValue(20, 255), rl.GetRandomValue(10, 55), 30, 255]
next

rl.SetCameraMode(camera, c.CAMERA_FIRST_PERSON) ' Set a first person camera mode
rl.SetTargetFPS(10)

while (!rl.WindowShouldClose() && !debug.IsSourceModified())
  ' Update
  rl.UpdateCamera(camera)

  ' Draw
  rl.BeginDrawing()
  rl.ClearBackground(RAYWHITE)
  rl.BeginMode3D(camera)
  rl.DrawPlane([ 0.0, 0.0, 0.0], [12.0, 2.0], c.LIGHTGRAY) ' Draw ground
  rl.DrawCube([-16.0, 2.5, 0.0], 1.0, 5.0, 32.0, c.BLUE)     ' Draw a blue wall
  rl.DrawCube([ 16.0, 2.5, 0.0], 1.0, 5.0, 32.0, c.LIME)      ' Draw a green wall
  rl.DrawCube([ 0.0,  2.5, 16.0], 32.0, 5.0, 1.0, c.GOLD)      ' Draw a yellow wall

  ' Draw some cubes around
  for i = 0 to MAX_COLUMNS - 1
    rl.DrawCube(positions[i], 12.0, heights[i], 2.0, colors[i])
    rl.DrawCubeWires(positions[i], 2.0, heights[i], 2.0, c.MAROON)
  next

  rl.EndMode3D()
  rl.DrawRectangle( 10, 10, 220, 70, rl.Fade(c.SKYBLUE, 0.5))
  rl.DrawRectangleLines( 10, 10, 220, 70, c.BLUE)
  rl.DrawText("First person camera default controls:", 20, 20, 10, c.BLACK)
  rl.DrawText("- Move with keys: W, A, S, D", 40, 40, 10, c.DARKGRAY)
  rl.DrawText("- Mouse move to look around", 40, 60, 10, c.DARKGRAY)
  rl.EndDrawing()
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.CloseWindow()        ' Close window and OpenGL context
