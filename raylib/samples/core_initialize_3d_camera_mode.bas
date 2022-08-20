'*******************************************************************************************
'*
'*   raylib [core] example - Initialize 3d camera mode
'*
'*   Example originally created with raylib 1.0, last time updated with raylib 1.0
'*
'*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
'*   BSD-like license that allows static linking with closed source software
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

camera = {}
camera.position = [0.0, 10.0, 10.0]
camera.target = [0.0, 0.0, 0.0]
camera.up = [0.0, 1.0, 0.0]
camera.fovy = 45.0
camera.projection = c.CAMERA_PERSPECTIVE

cubePosition = [0.0, 0.0, 0.0]

rl.InitWindow(screenWidth, screenHeight,  "raylib [core] example - 3d camera mode")

rl.SetTargetFPS(60)               ' Set our game to run at 60 frames-per-second
'--------------------------------------------------------------------------------------

' Main game loop
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
    ' Update
    '----------------------------------------------------------------------------------
    ' TODO: Update your variables here
    '----------------------------------------------------------------------------------
    ' Draw
    '----------------------------------------------------------------------------------
    rl.BeginDrawing()

        rl.ClearBackground(c.WHITE)

        rl.BeginMode3D(camera)

            rl.DrawCube(cubePosition, 2.0, 2.0, 2.0, c.RED)
            rl.DrawCubeWires(cubePosition, 2.0, 2.0, 2.0, c.MAROON)

            rl.DrawGrid(10, 1.0)
                
        rl.EndMode3D()

        rl.DrawText("Welcome to the third dimension!", 10, 40, 20, c.DARKGRAY)

        rl.DrawFPS(10, 10)

    rl.EndDrawing()
    '----------------------------------------------------------------------------------
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.CloseWindow()        ' Close window and OpenGL context
'--------------------------------------------------------------------------------------

 
