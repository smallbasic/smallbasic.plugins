'*******************************************************************************************
'*
'*   raylib [core] example - Window should close
'*
'*   Example originally created with raylib 4.2, last time updated with raylib 4.2
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

rl.InitWindow(screenWidth, screenHeight, "raylib [core] example - window should close")

rl.SetExitKey(c.KEY_NULL)       ' Disable KEY_ESCAPE to close window, X-button still works
    
exitWindowRequested = false     ' Flag to request window to exit
exitWindow = false              ' Flag to set window to exit

rl.SetTargetFPS(60)             ' Set our game to run at 60 frames-per-second
'--------------------------------------------------------------------------------------

' Main game loop
while (!exitWindow)    ' Detect window close button or ESC key
    ' Update
    '----------------------------------------------------------------------------------
    ' Detect if X-button or KEY_ESCAPE have been pressed to close window
    if(rl.WindowShouldClose() OR rl.IsKeyPressed(c.KEY_ESCAPE)) then
        exitWindowRequested = true
    endif
    
    if(exitWindowRequested) then
        ' A request for close window has been issued, we can save data before closing
        ' or just show a message asking for confirmation
        
        if(rl.IsKeyPressed(c.KEY_Y)) then
            exitWindow = true
        else if (rl.IsKeyPressed(c.KEY_N)) then
            exitWindowRequested = false
        endif
    endif
    '----------------------------------------------------------------------------------

    ' Draw
    '----------------------------------------------------------------------------------
    rl.BeginDrawing()

        rl.ClearBackground(c.RAYWHITE)

        if(exitWindowRequested) then
            rl.DrawRectangle(0, 100, screenWidth, 200, c.BLACK)
            rl.DrawText("Are you sure you want to exit program? [Y/N]", 40, 180, 30, c.WHITE)
        else
            rl.DrawText("Try to close the window to get confirmation message!", 120, 200, 20, c.LIGHTGRAY)
        endif

    rl.EndDrawing()
    '----------------------------------------------------------------------------------
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.CloseWindow()        ' Close window and OpenGL context
'--------------------------------------------------------------------------------------

 
