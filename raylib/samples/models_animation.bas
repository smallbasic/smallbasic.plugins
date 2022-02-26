REM /*******************************************************************************************
REM *
REM *   raylib [models] example - Load 3d model with animations and play them
REM *
REM *   This example has been created using raylib 2.5 (www.raylib.com)
REM *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM *
REM *   Example contributed by Culacant (@culacant) and reviewed by Ramon Santamaria (@raysan5)
REM *
REM *   Copyright (c) 2019 Culacant (@culacant) and Ramon Santamaria (@raysan5)
REM *
REM ********************************************************************************************
REM *
REM * To export a model from blender, make sure it is not posed, the vertices need to be in the
REM * same position as they would be in edit mode.
REM * and that the scale of your models is set to 0. Scaling can be done from the export menu.
REM *
REM ********************************************************************************************/

import raylib as rl
import raylibc as c

const screenWidth = 800
const screenHeight = 450

rl.InitWindow(screenWidth, screenHeight, "raylib [models] example - model animation")

' Define the camera to look into our 3d world
camera = {}
camera.position = [10, 10, 10] ' Camera position
camera.target =   [0, 0, 0]    ' Camera looking at point
camera.up     =   [0, 1, 0]    ' Camera up vector (rotation towards target)
camera.fovy   =   45.0         ' Camera field-of-view Y
camera.projection = c.CAMERA_PERSPECTIVE   ' Camera mode type

const resources = CWD + "raylib/examples/models/resources/"

' Load animation data
anims = rl.LoadModelAnimations(resources + "models/iqm/guyanim.iqm")
animFrameCounter = 0

model = rl.LoadModel(resources + "models/iqm/guy.iqm")        ' Load the animated model mesh and basic data

texture = rl.LoadTexture(resources + "models/iqm/guytex.png") ' Load model texture and set material
rl.SetModelDiffuseTexture(model, texture)                     ' Set model material map texture

position = [0, 0, 0]       ' Set model position
rotationAxis = [0, 1, 0]
rotationAngle = -90
scale = [1, 1, 1]

rl.SetCameraMode(camera, c.CAMERA_FREE) ' Set free camera mode
rl.SetTargetFPS(60)                     ' Set our game to run at 60 frames-per-second

while (!rl.WindowShouldClose())
  rl.UpdateCamera(camera)

  ' Play animation when spacebar is held down
  if (rl.IsKeyDown(c.KEY_SPACE)) then
    animFrameCounter++
    rl.UpdateModelAnimation(model, anims[0], animFrameCounter)
    if (animFrameCounter >= anims[0].frameCount) then animFrameCounter = 0
  else if (rl.IsKeyDown(c.KEY_LEFT)) then
    rotationAxis[0]--
  else if (rl.IsKeyDown(c.KEY_RIGHT)) then
    rotationAxis[0]++
  else if (rl.IsKeyDown(c.KEY_UP)) then
    rotationAngle--
  else if (rl.IsKeyDown(c.KEY_DOWN)) then
    rotationAngle++
  endif

  rl.BeginDrawing()
  rl.ClearBackground(c.RAYWHITE)
  rl.BeginMode3D(camera)
  rl.DrawModelEx(model, position, [1, 0, 0], -90, [1, 1, 1], c.WHITE)
  rl.DrawModelEx(model, position, [1, 0, 0],  90, [1, 1, 1], c.YELLOW)
  rl.DrawModelEx(model, position, rotationAxis, rotationAngle, scale, c.GREEN)

  for i = 0 to model.boneCount - 1
    rl.DrawCube(anims[0].framePoses[animFrameCounter, i].translation, 0.2, 0.2, 0.2, c.RED)
  next i
  rl.DrawGrid(10, 1.0)         ' Draw a grid
  rl.EndMode3D()
  rl.DrawText("PRESS SPACE to PLAY MODEL ANIMATION", 10, 10, 20, c.MAROON)
  rl.DrawText("(c) Guy IQM 3D model by @culacant", screenWidth - 200, screenHeight - 20, 10, c.GRAY)
  rl.EndDrawing()
wend

rl.UnloadTexture(texture)     ' Unload texture
for i = 0 to len(anims) - 1
  rl.UnloadModelAnimation(anims[i])
next i
rl.UnloadModel(model)         ' Unload model
rl.CloseWindow()              ' Close window and OpenGL context
