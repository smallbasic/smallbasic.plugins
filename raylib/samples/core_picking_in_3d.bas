'*******************************************************************************************
'*
'*   raylib [core] example - Picking in 3d mode
'*
'*   Example originally created with raylib 1.3, last time updated with raylib 4.0
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

cubePosition = [0.0, 1.0, 0.0]
cubeSize = {x: 2.0, y: 2.0, z: 2.0}

ray = {direction: {x: 0, y: 0, z: 0}, position: {x: 0, y: 0, z: 0}}    ' Picking line ray

collision = {hit: 0}

rl.InitWindow(screenWidth, screenHeight,  "raylib [core] example - 3d picking")

rl.SetCameraMode(camera, c.CAMERA_FREE) ' Set a free camera mode

rl.SetTargetFPS(60)               ' Set our game to run at 60 frames-per-second
'--------------------------------------------------------------------------------------

' Main game loop
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
    ' Update
    '----------------------------------------------------------------------------------
    rl.UpdateCamera(camera)
    
    if(rl.IsMouseButtonPressed(c.MOUSE_BUTTON_LEFT)) then
        if (!collision.hit) then            
            ray = rl.GetMouseRay(rl.GetMousePosition(), camera)
           
            BoundingBox.min.x = cubePosition[0] - cubeSize.x/2
            BoundingBox.min.y = cubePosition[1] - cubeSize.y/2
            BoundingBox.min.z = cubePosition[2] - cubeSize.z/2
            BoundingBox.max.x = cubePosition[0] + cubeSize.x/2
            BoundingBox.max.y = cubePosition[1] + cubeSize.y/2
            BoundingBox.max.z = cubePosition[2] + cubeSize.z/2
            'Check collision between ray and box            
            collision = rl.GetRayCollisionBox(ray, BoundingBox)            
        else
            collision.hit = false
        endif
        
    endif
    '----------------------------------------------------------------------------------
    ' Draw
    '----------------------------------------------------------------------------------
    rl.BeginDrawing()

        rl.ClearBackground(c.WHITE)

        rl.BeginMode3D(camera)
            if(collision.hit) then
                rl.DrawCube(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, c.RED)
                rl.DrawCubeWires(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, c.MAROON)
                rl.DrawCubeWires(cubePosition, cubeSize.x + 0.2, cubeSize.y + 0.2, cubeSize.z + 0.2, c.GREEN)
            else
                rl.DrawCube(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, c.GRAY)
                rl.DrawCubeWires(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, c.DARKGRAY)
            endif        
            rl.DrawRay(ray, c.MAROON)
            rl.DrawGrid(10, 1.0)
        rl.EndMode3D()
        
        rl.DrawText("Try selecting the box with mouse!", 240, 10, 20, c.DARKGRAY)

        if(collision.hit) then
            rl.DrawText("BOX SELECTED", (screenWidth - rl.MeasureText("BOX SELECTED", 30)) / 2, screenHeight * 0.1, 30, c.GREEN)
        endif
        
        rl.DrawFPS(10, 10)

    rl.EndDrawing()
    '----------------------------------------------------------------------------------
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.CloseWindow()        ' Close window and OpenGL context
'--------------------------------------------------------------------------------------

 
