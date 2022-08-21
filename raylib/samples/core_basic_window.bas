'*******************************************************************************************
'*
'*   raylib [core] example - Basic window
'*
'*   Welcome to raylib!
'*
'*   To test examples, just press F6 and execute raylib_compile_execute script
'*   Note that compiled executable is placed in the same folder as .c file
'*
'*   You can find all basic examples on C:\raylib\raylib\examples folder or
'*   raylib official webpage: www.raylib.com
'*
'*   Enjoy using raylib. :)
'*
'*   Example originally created with raylib 1.0, last time updated with raylib 1.0
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

rl.InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window")

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

        rl.ClearBackground(c.RAYWHITE)

        rl.DrawText("Congrats! You created your first window!", 190, 200, 20, c.LIGHTGRAY)

    rl.EndDrawing()
    '----------------------------------------------------------------------------------
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.CloseWindow()        ' Close window and OpenGL context
'--------------------------------------------------------------------------------------

 
