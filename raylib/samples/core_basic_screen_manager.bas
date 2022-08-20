'*******************************************************************************************
'*
'*   raylib [core] examples - basic screen manager
'*
'*   NOTE: This example illustrates a very simple screen manager based on a states machines
'*
'*   Example originally created with raylib 4.0, last time updated with raylib 4.0
'*
'*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
'*   BSD-like license that allows static linking with closed source software
'*
'*   Copyright (c) 2021-2022 Ramon Santamaria (@raysan5)
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

const LOGO = 0
const TITLE = 1
const GAMEPLAY = 2
const ENDING = 3

rl.InitWindow(screenWidth, screenHeight, "raylib [core] example - basic screen manager")

' TODO: Initialize all required variables and load all required data here!
currentScreen = LOGO
framesCounter = 0          ' Useful to count frames

rl.SetTargetFPS(60)               ' Set our game to run at 60 frames-per-second
'--------------------------------------------------------------------------------------

' Main game loop
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
    ' Update
    '----------------------------------------------------------------------------------
    select case currentScreen
        case LOGO
            ' TODO: Update LOGO screen variables here!
            framesCounter++    ' Count frames

            ' Wait for 2 seconds (120 frames) before jumping to TITLE screen
            if(framesCounter > 120) then
                currentScreen = TITLE
            endif
        case TITLE
            ' TODO: Update TITLE screen variables here!

            ' Press enter to change to GAMEPLAY screen
            if(rl.IsKeyPressed(c.KEY_ENTER) OR rl.IsGestureDetected(c.GESTURE_TAP)) then
                currentScreen = GAMEPLAY
            endif
        case GAMEPLAY
            ' TODO: Update GAMEPLAY screen variables here!

            ' Press enter to change to ENDING screen
            if(rl.IsKeyPressed(c.KEY_ENTER) OR rl.IsGestureDetected(c.GESTURE_TAP)) then
                currentScreen = ENDING
            endif
        case ENDING
            ' TODO: Update ENDING screen variables here!

            ' Press enter to return to TITLE screen
            if(rl.IsKeyPressed(c.KEY_ENTER) OR rl.IsGestureDetected(c.GESTURE_TAP)) then
                currentScreen = TITLE
            endif
    end select
    '----------------------------------------------------------------------------------

    ' Draw
    '----------------------------------------------------------------------------------
    rl.BeginDrawing()

        rl.ClearBackground(c.RAYWHITE)

        select case currentScreen
            case LOGO
                ' TODO: Draw LOGO screen here!
                rl.DrawText("LOGO SCREEN", 20, 20, 40, c.LIGHTGRAY)
                rl.DrawText("WAIT for 2 SECONDS...", 290, 220, 20, c.GRAY)
            case TITLE
                ' TODO: Draw TITLE screen here!
                rl.DrawRectangle(0, 0, screenWidth, screenHeight, c.GREEN)
                rl.DrawText("TITLE SCREEN", 20, 20, 40, c.DARKGREEN)
                rl.DrawText("PRESS ENTER or TAP to JUMP to GAMEPLAY SCREEN", 120, 220, 20, c.DARKGREEN)
            case GAMEPLAY
                ' TODO: Draw GAMEPLAY screen here!
                rl.DrawRectangle(0, 0, screenWidth, screenHeight, c.PURPLE)
                rl.DrawText("GAMEPLAY SCREEN", 20, 20, 40, c.MAROON)
                rl.DrawText("PRESS ENTER or TAP to JUMP to ENDING SCREEN", 130, 220, 20, c.MAROON)
            case ENDING
                ' TODO: Draw ENDING screen here!
                rl.DrawRectangle(0, 0, screenWidth, screenHeight, c.BLUE)
                rl.DrawText("ENDING SCREEN", 20, 20, 40, c.DARKBLUE)
                rl.DrawText("PRESS ENTER or TAP to RETURN to TITLE SCREEN", 120, 220, 20, c.DARKBLUE)
        end select

    rl.EndDrawing()
    '----------------------------------------------------------------------------------
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.CloseWindow()        ' Close window and OpenGL context
'--------------------------------------------------------------------------------------

 
