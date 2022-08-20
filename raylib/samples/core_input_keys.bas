'*******************************************************************************************
'*
'*   raylib [core] example - Keyboard input
'*
'*   Example originally created with raylib 1.0, last time updated with raylib 1.0
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

ballPosition = { x: 400, y: 225 }

rl.InitWindow(screenWidth, screenHeight, "raylib [core] example - keyboard input")

rl.SetTargetFPS(60)               ' Set our game to run at 60 frames-per-second
'--------------------------------------------------------------------------------------

' Main game loop
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
    ' Update
    '----------------------------------------------------------------------------------
    if (rl.IsKeyDown(c.KEY_RIGHT)) then
        ballPosition.x += 2.0
    endif
    if (rl.IsKeyDown(c.KEY_LEFT)) then
        ballPosition.x -= 2.0
    endif
    if (rl.IsKeyDown(c.KEY_UP)) then
        ballPosition.y -= 2.0
    endif
    if (rl.IsKeyDown(c.KEY_DOWN)) then
        ballPosition.y += 2.0
    endif
    '----------------------------------------------------------------------------------

    ' Draw
    '----------------------------------------------------------------------------------
    rl.BeginDrawing()

        rl.ClearBackground(c.RAYWHITE)

        rl.DrawText("move the ball with arrow keys", 10, 10, 20, c.DARKGRAY)

        rl.DrawCircleV(ballPosition, 50, c.MAROON)

    rl.EndDrawing()
    '----------------------------------------------------------------------------------
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.CloseWindow()        ' Close window and OpenGL context
'--------------------------------------------------------------------------------------

 
