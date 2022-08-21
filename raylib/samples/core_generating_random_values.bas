'*******************************************************************************************
'*
'*   raylib [core] example - Generate random values
'*
'*   Example originally created with raylib 1.1, last time updated with raylib 1.1
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

rl.InitWindow(screenWidth, screenHeight, "raylib [core] example - generate random values")

' rl.SetRandomSeed(0xaabbccff)   ' Set a custom random seed if desired, by default: "time(NULL)"

randValue = rl.GetRandomValue(-8, 5)   ' Get a random integer number between -8 and 5 (both included)
    
framesCounter = 0                      ' Variable used to count frames

rl.SetTargetFPS(60)                    ' Set our game to run at 60 frames-per-second
'--------------------------------------------------------------------------------------

' Main game loop
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
    ' Update
    '----------------------------------------------------------------------------------
    framesCounter++

    ' Every two seconds (120 frames) a new random value is generated
    if(((framesCounter/120)%2) == 1) then
        randValue = rl.GetRandomValue(-8, 5)
        framesCounter = 0
    endif
    '----------------------------------------------------------------------------------

    ' Draw
    '----------------------------------------------------------------------------------
    rl.BeginDrawing()

        rl.ClearBackground(c.RAYWHITE)

        rl.DrawText("Every 2 seconds a new random value is generated:", 130, 100, 20, c.MAROON)

        rl.DrawText(rl.TextFormat("%i", randValue), 360, 180, 80, c.LIGHTGRAY)

    rl.EndDrawing()
    '----------------------------------------------------------------------------------
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.CloseWindow()        ' Close window and OpenGL context
'--------------------------------------------------------------------------------------

 
