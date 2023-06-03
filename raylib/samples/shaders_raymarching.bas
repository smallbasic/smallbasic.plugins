REM *******************************************************************************************
REM *
REM *   raylib [shaders] example - Raymarching shapes generation
REM *
REM *   NOTE: This example requires raylib OpenGL 3.3 for shaders support and only #version 330
REM *         is currently supported. OpenGL ES 2.0 platforms are not supported at the moment.
REM *
REM *   This example has been created using raylib 2.0 (www.raylib.com)
REM *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM *
REM *   Copyright (c) 2018 Ramon Santamaria (@raysan5)
REM *
REM ********************************************************************************************

import raylib as rl
import raylibc as c

const screenWidth = 800 
const screenHeight = 450 

rl.SetConfigFlags(FLAG_WINDOW_RESIZABLE) 
rl.InitWindow(screenWidth, screenHeight, "SmallBASIC raylib [shaders] example - raymarching shapes") 

camera.position = [2.5, 2.5, 3.0]     ' Camera position
camera.target = [0.0, 0.0, 0.7]       ' Camera looking at point
camera.up = [ 0.0, 1.0, 0.0]          ' Camera up vector (rotation towards target)
camera.fovy = 65.0                    ' Camera field-of-view Y

' Load raymarching shader
' NOTE: Defining 0 (NULL) for vertex shader forces usage of internal default vertex shader
' 
const resources = CWD + "raylib/examples/shaders/resources/"
shader = rl.LoadShader(0, resources + "shaders/glsl330/raymarching.fs") 

' Get shader locations for required uniforms
viewEyeLoc = rl.GetShaderLocation(shader, "viewEye") 
viewCenterLoc = rl.GetShaderLocation(shader, "viewCenter") 
runTimeLoc = rl.GetShaderLocation(shader, "runTime") 
resolutionLoc = rl.GetShaderLocation(shader, "resolution") 

resolution = [screenWidth, screenHeight]
rl.SetShaderValue(shader, resolutionLoc, resolution, c.UNIFORM_VEC2) 

runTime = 0.0

rl.SetTargetFPS(60)                        ' Set our game to run at 60 frames-per-second
while (!rl.WindowShouldClose())
  rl.UpdateCamera(camera,c.CAMERA_FREE)               ' Update camera

  cameraPos = [camera.position[0], camera.position[1], camera.position[2]]
  cameraTarget = [camera.target[0], camera.target[1], camera.target[2]]
  deltaTime = rl.GetFrameTime() 
  runTime += deltaTime 

  ' Set shader required uniform values
  rl.SetShaderValue(shader, viewEyeLoc, cameraPos, c.UNIFORM_VEC3) 
  rl.SetShaderValue(shader, viewCenterLoc, cameraTarget, c.UNIFORM_VEC3) 
  rl.SetShaderValue(shader, runTimeLoc, runTime, c.UNIFORM_FLOAT) 

  ' Check if screen is resized
  if (rl.IsWindowResized()) then
    screenWidth = rl.GetScreenWidth() 
    screenHeight = rl.GetScreenHeight() 
    resolution = [screenWidth, screenHeight]
    rl.SetShaderValue(shader, resolutionLoc, resolution, c.UNIFORM_VEC2) 
  endif

  rl.BeginDrawing() 
  rl.ClearBackground(RAYWHITE) 

  ' We only draw a white full-screen rectangle,
  ' frame is generated in shader using raymarching
  rl.BeginShaderMode(shader) 
  rl.DrawRectangle(0, 0, screenWidth, screenHeight, c.WHITE) 
  rl.EndShaderMode() 

  rl.DrawText("(c) Raymarching shader by Iigo Quilez. MIT License.", screenWidth - 280, screenHeight - 20, 10, c.BLACK) 
  rl.EndDrawing() 
wend

' De-Initialization
rl.UnloadShader(shader)            ' Unload shader
rl.CloseWindow()                   ' Close window and OpenGL context
