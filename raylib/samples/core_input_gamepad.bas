'*******************************************************************************************
'*
'*   raylib [core] example - Gamepad input
'*
'*   NOTE: This example requires a Gamepad connected to the system
'*         raylib is configured to work with the following gamepads:
'*                - Xbox 360 Controller (Xbox 360, Xbox One)
'*                - PLAYSTATION(R)3 Controller
'*         Check raylib.h for buttons configuration
'*
'*   Example originally created with raylib 1.1, last time updated with raylib 4.2
'*
'*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
'*   BSD-like license that allows static linking with closed source software
'*
'*   Copyright (c) 2013-2022 Ramon Santamaria (@raysan5)
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

rl.InitWindow(screenWidth, screenHeight,  "raylib [core] example - gamepad input")

rl.SetTargetFPS(60)               ' Set our game to run at 60 frames-per-second
'--------------------------------------------------------------------------------------

' Main game loop
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
    ' Update
    '----------------------------------------------------------------------------------
    '...
    '----------------------------------------------------------------------------------
    ' Draw
    '----------------------------------------------------------------------------------
    rl.BeginDrawing()

        rl.ClearBackground(c.RAYWHITE)

        if (rl.IsGamepadAvailable(0))
            
            rl.DrawText(rl.TextFormat("GP1: %s", rl.GetGamepadName(0)), 10, 10, 10, c.BLACK)

            rl.DrawText(rl.TextFormat("DETECTED AXIS [%i]:", rl.GetGamepadAxisCount(0)), 10, 50, 10, c.MAROON)

            for i = 0 to rl.GetGamepadAxisCount(0)
                rl.DrawText(rl.TextFormat("AXIS %i: %.02f", i, rl.GetGamepadAxisMovement(0, i)), 20, 70 + 20*i, 10, c.DARKGRAY)
            next

            if (rl.GetGamepadButtonPressed() != -1) then
                rl.DrawText(rl.TextFormat("DETECTED BUTTON: %i", rl.GetGamepadButtonPressed()), 10, 430, 10, c.RED)
            else 
                rl.DrawText("DETECTED BUTTON: NONE", 10, 430, 10, c.GRAY)
            endif
        
        else
            rl.DrawText("GP1: NOT DETECTED", 10, 10, 10, c.GRAY)
        endif
    rl.EndDrawing()
    '----------------------------------------------------------------------------------
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.CloseWindow()        ' Close window and OpenGL context
'--------------------------------------------------------------------------------------

 
