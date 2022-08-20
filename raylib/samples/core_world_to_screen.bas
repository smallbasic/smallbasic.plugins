'*******************************************************************************************
'*
'*   raylib [core] example - World to screen
'*
'*   Example originally created with raylib 1.3, last time updated with raylib 1.4
'*
'*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
'*   BSD-like license that allows static linking with closed source software
'*
'*   Copyright (c) 2015-2022 Ramon Santamaria (@raysan5)
'*
'*   Converted to SmallBASIC by Joerg Siebenmorgen, 2022
'*   https://github.com/smallbasic/smallbasic.plugins/blob/master/raylib/samples
'* 
'********************************************************************************************/

import raylib as rl
import raylibc as c

' Initialization
'--------------------------------------------------------------------------------------
const screenWidth = 800
const screenHeight = 450

' Define the camera to look into our 3d world
camera = {}
camera.position = [10.0, 10.0, 10.0]    ' Camera position
camera.target = [0.0, 0.0, 0.0]         ' Camera looking at point
camera.up = [0.0, 1.0, 0.0]             ' Camera up vector (rotation towards target)
camera.fovy = 45.0                      ' Camera field-of-view Y
camera.projection = c.CAMERA_PERSPECTIVE ' Camera mode type

cubePosition = [0.0, 0.0, 0.0]

rl.InitWindow(screenWidth, screenHeight,  "raylib [core] example - world to screen")

rl.SetCameraMode(camera, c.CAMERA_FREE) ' Set a free camera mode

rl.SetTargetFPS(60)               ' Set our game to run at 60 frames-per-second
'--------------------------------------------------------------------------------------

' Main game loop
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
    ' Update
    '----------------------------------------------------------------------------------
    rl.UpdateCamera(camera)
    
    ' Calculate cube screen space position (with a little offset to be in top)
    cubeScreenPosition = rl.GetWorldToScreen([cubePosition[0], cubePosition[1] + 2.5, cubePosition[2]], camera)
    '----------------------------------------------------------------------------------
    ' Draw
    '----------------------------------------------------------------------------------
    rl.BeginDrawing()

        rl.ClearBackground(c.WHITE)

        rl.BeginMode3D(camera)
            rl.DrawCube(cubePosition, 2, 2, 2, c.RED)
            rl.DrawCubeWires(cubePosition, 2, 2, 2, c.MAROON)
            rl.DrawGrid(10, 1.0)
        rl.EndMode3D()
        
        rl.DrawText("Enemy: 100 / 100", cubeScreenPosition.x - rl.MeasureText("Enemy: 100/100", 20)/2, cubeScreenPosition.y, 20, c.BLACK)
        rl.DrawText("Text is always on top of the cube", (screenWidth - rl.MeasureText("Text is always on top of the cube", 20))/2, 25, 20, c.GRAY)


    rl.EndDrawing()
    '----------------------------------------------------------------------------------
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.CloseWindow()        ' Close window and OpenGL context
'--------------------------------------------------------------------------------------

 
