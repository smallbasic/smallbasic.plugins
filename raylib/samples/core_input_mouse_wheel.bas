'*******************************************************************************************
'*
'*   raylib [core] examples - Mouse wheel input
'*
'*   Example originally created with raylib 1.1, last time updated with raylib 1.3
'*
'*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
'*   BSD-like license that allows static linking with closed source software
'*
'*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
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

boxPositionY = screenHeight/2 - 40
scrollSpeed = 4            ' Scrolling speed in pixels

rl.InitWindow(screenWidth, screenHeight,  "raylib [core] example - input mouse wheel")

rl.SetTargetFPS(60)               ' Set our game to run at 60 frames-per-second
'--------------------------------------------------------------------------------------

' Main game loop
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
    ' Update
    '----------------------------------------------------------------------------------
    boxPositionY -= rl.GetMouseWheelMove()*scrollSpeed
    '----------------------------------------------------------------------------------
    ' Draw
    '----------------------------------------------------------------------------------
    rl.BeginDrawing()

        rl.ClearBackground(c.RAYWHITE)

        rl.DrawRectangle(screenWidth/2 - 40, boxPositionY, 80, 80, c.MAROON)

        rl.DrawText("Use mouse wheel to move the cube up and down!", 10, 10, 20, c.GRAY)
        rl.DrawText(rl.TextFormat("Box position Y: %03i", boxPositionY), 10, 40, 20, c.LIGHTGRAY)

    rl.EndDrawing()
    '----------------------------------------------------------------------------------
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.CloseWindow()        ' Close window and OpenGL context
'--------------------------------------------------------------------------------------

 
