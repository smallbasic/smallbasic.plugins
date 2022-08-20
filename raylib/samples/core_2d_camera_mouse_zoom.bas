'*******************************************************************************************
'*
'*   raylib [core] example - 2d camera mouse zoom
'*
'*   Example originally created with raylib 4.2, last time updated with raylib 4.2
'*
'*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
'*   BSD-like license that allows static linking with closed source software
'*
'*   Copyright (c) 2022 Jeffery Myers (@JeffM2501)
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

camera.target.x = 0
camera.target.y = 0
camera.offset.x = screenWidth/2
camera.offset.y = screenHeight/2
camera.rotation = 0.0
camera.zoom = 1.0

zoomIncrement = 0.125

rl.InitWindow(screenWidth, screenHeight,  "raylib [core] example - 2d camera mouse zoom")

rl.SetTargetFPS(60)               ' Set our game to run at 60 frames-per-second
'--------------------------------------------------------------------------------------

' Main game loop
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
    ' Update
    '----------------------------------------------------------------------------------
    ' Translate based on mouse right click
    if(rl.IsMouseButtonDown(c.MOUSE_BUTTON_RIGHT)) then
        delta = rl.GetMouseDelta()
         
        delta.x = delta.x * (-1.0/camera.zoom)
        delta.y = delta.y * (-1.0/camera.zoom)
        
        camera.target.x = camera.target.x + delta.x
        camera.target.y = camera.target.y + delta.y
    endif

    ' Zoom based on mouse wheel
    wheel = rl.GetMouseWheelMove()
    if(wheel != 0) then
        ' Get the world point that is under the mouse
        mouseWorldPos = rl.GetScreenToWorld2D(rl.GetMousePosition(), camera)

        ' Set the offset to where the mouse is
        camera.offset = rl.GetMousePosition()

        ' Set the target to match, so that the camera maps the world space point 
        ' under the cursor to the screen space point under the cursor at any zoom
        camera.target = mouseWorldPos
    
        camera.zoom += (wheel*zoomIncrement)
        if(camera.zoom < zoomIncrement) then
            camera.zoom = zoomIncrement
        endif
    endif
    '----------------------------------------------------------------------------------
    ' Draw
    '----------------------------------------------------------------------------------
    rl.BeginDrawing()

        rl.ClearBackground(c.BLACK)

        rl.BeginMode2D(camera)

            ' Draw a rectangle
            ' just so we have something in the XY plane
             rl.DrawRectangleLines(-300,-150, 600,300, c.WHITE)   

            ' Draw a reference circle
            rl.DrawCircle(0, 0, 50, c.YELLOW)
                
        rl.EndMode2D()

        rl.DrawText("Mouse right button drag to move, mouse wheel to zoom", 10, 10, 20, c.WHITE)

    rl.EndDrawing()
    '----------------------------------------------------------------------------------
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.CloseWindow()        ' Close window and OpenGL context
'--------------------------------------------------------------------------------------

 
