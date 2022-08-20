'*******************************************************************************************
'*
'*   raylib [core] example - Mouse input
'*
'*   Example originally created with raylib 1.0, last time updated with raylib 4.0
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

ballPosition = { x: -100, y: -100 }
ballColor = c.DARKBLUE

rl.InitWindow(screenWidth, screenHeight,  "raylib [core] example - mouse input")

rl.SetTargetFPS(60)               ' Set our game to run at 60 frames-per-second
'--------------------------------------------------------------------------------------

' Main game loop
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
    ' Update
    '----------------------------------------------------------------------------------
    ballPosition = rl.GetMousePosition()

    if (rl.IsMouseButtonPressed(c.MOUSE_BUTTON_LEFT)) then
        ballColor = c.MAROON
    else if (rl.IsMouseButtonPressed(c.MOUSE_BUTTON_MIDDLE)) then
        ballColor = c.LIME
    else if (rl.IsMouseButtonPressed(c.MOUSE_BUTTON_RIGHT)) then
        ballColor = c.DARKBLUE
    else if (rl.IsMouseButtonPressed(c.MOUSE_BUTTON_SIDE)) then
        ballColor = c.PURPLE
    else if (rl.IsMouseButtonPressed(c.MOUSE_BUTTON_EXTRA)) then
        ballColor = c.YELLOW
    else if (rl.IsMouseButtonPressed(c.MOUSE_BUTTON_FORWARD)) then
        ballColor = c.ORANGE
    else if (rl.IsMouseButtonPressed(c.MOUSE_BUTTON_BACK)) then
        ballColor = c.BEIGE
    endif
    '----------------------------------------------------------------------------------
    ' Draw
    '----------------------------------------------------------------------------------
    rl.BeginDrawing()

        rl.ClearBackground(c.RAYWHITE)

        rl.DrawCircleV(ballPosition, 40, ballColor)
        
        rl.DrawText("move ball with mouse and click mouse button to change color", 10, 10, 20, c.DARKGRAY)

    rl.EndDrawing()
    '----------------------------------------------------------------------------------
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.CloseWindow()        ' Close window and OpenGL context
'--------------------------------------------------------------------------------------

 
