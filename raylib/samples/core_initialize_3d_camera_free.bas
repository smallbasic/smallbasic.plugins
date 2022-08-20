'*******************************************************************************************
'*
'*   raylib [core] example - Initialize 3d camera free
'*
'*   Example originally created with raylib 1.3, last time updated with raylib 1.3
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

rl.InitWindow(screenWidth, screenHeight,  "raylib [core] example - 3d camera mode")

rl.SetCameraMode(camera, c.CAMERA_FREE) ' Set a free camera mode

rl.SetTargetFPS(60)               ' Set our game to run at 60 frames-per-second
'--------------------------------------------------------------------------------------

' Main game loop
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
    ' Update
    '----------------------------------------------------------------------------------
    rl.UpdateCamera(camera)
    
    if(rl.IsKeyDown(c.KEY_Z)) then
        camera.target = [0.0, 0.0, 0.0]
    endif
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

        rl.DrawRectangle( 10, 10, 320, 133, rl.Fade(c.SKYBLUE, 0.5))
        rl.DrawRectangleLines( 10, 10, 320, 133, c.BLUE)

        rl.DrawText("Free camera default controls:", 20, 20, 10, c.BLACK)
        rl.DrawText("- Mouse Wheel to Zoom in-out", 40, 40, 10, c.DARKGRAY)
        rl.DrawText("- Mouse Wheel Pressed to Pan", 40, 60, 10, c.DARKGRAY)
        rl.DrawText("- Alt + Mouse Wheel Pressed to Rotate", 40, 80, 10, c.DARKGRAY)
        rl.DrawText("- Alt + Ctrl + Mouse Wheel Pressed for Smooth Zoom", 40, 100, 10, c.DARKGRAY)
        rl.DrawText("- Z to zoom to (0, 0, 0)", 40, 120, 10, c.DARKGRAY)

    rl.EndDrawing()
    '----------------------------------------------------------------------------------
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.CloseWindow()        ' Close window and OpenGL context
'--------------------------------------------------------------------------------------

 
