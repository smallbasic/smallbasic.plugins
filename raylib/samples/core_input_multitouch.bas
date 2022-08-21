'*******************************************************************************************
'*
'*   raylib [core] example - Input multitouch
'*
'*   Example originally created with raylib 2.1, last time updated with raylib 2.5
'*
'*   Example contributed by Berni (@Berni8k) and reviewed by Ramon Santamaria (@raysan5)
'*
'*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
'*   BSD-like license that allows static linking with closed source software
'*
'*   Copyright (c) 2019-2022 Berni (@Berni8k) and Ramon Santamaria (@raysan5)
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
const MAX_TOUCH_POINTS = 10

dim touchPositions[MAX_TOUCH_POINTS]

rl.InitWindow(screenWidth, screenHeight,  "raylib [core] example - input multitouch")

rl.SetTargetFPS(60)               ' Set our game to run at 60 frames-per-second
'--------------------------------------------------------------------------------------

' Main game loop
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
    ' Update
    '----------------------------------------------------------------------------------
    ' Get multiple touchpoints
    for i = 0 to MAX_TOUCH_POINTS
        touchPositions[i] = rl.GetTouchPosition(i)
    next
    '----------------------------------------------------------------------------------
    ' Draw
    '----------------------------------------------------------------------------------
    rl.BeginDrawing()

        rl.ClearBackground(c.RAYWHITE)

        for i = 0 to MAX_TOUCH_POINTS
            ' Make sure point is not (0, 0) as this means there is no touch for it
            if((touchPositions[i].x > 0) AND (touchPositions[i].y > 0)) then
                ' Draw circle and touch index number
                rl.DrawCircleV(touchPositions[i], 34, c.ORANGE)
                rl.DrawText(rl.TextFormat("%d", i), touchPositions[i].x - 10, touchPositions[i].y - 70, 40, c.BLACK)
            endif
        next

        rl.DrawText("touch the screen at multiple locations to get multiple balls", 10, 10, 20, c.DARKGRAY)


    rl.EndDrawing()
    '----------------------------------------------------------------------------------
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.CloseWindow()        ' Close window and OpenGL context
'--------------------------------------------------------------------------------------

 
