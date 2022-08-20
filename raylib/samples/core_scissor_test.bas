'*******************************************************************************************
'*
'*   raylib [core] example - Scissor test
'*
'*   Example originally created with raylib 2.5, last time updated with raylib 3.0
'*
'*   Example contributed by Chris Dill (@MysteriousSpace) and reviewed by Ramon Santamaria (@raysan5)
'*
'*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
'*   BSD-like license that allows static linking with closed source software
'*
'*   Copyright (c) 2019-2022 Chris Dill (@MysteriousSpace)
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

scissorArea = {x: 0, y: 0, width:300, height:300}
scissorMode = true

rl.InitWindow(screenWidth, screenHeight, "raylib [core] example - scissor test")

rl.SetTargetFPS(60)               ' Set our game to run at 60 frames-per-second
'--------------------------------------------------------------------------------------

' Main game loop
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
    ' Update
    '----------------------------------------------------------------------------------
    if(rl.IsKeyPressed(c.KEY_S)) then
        scissorMode = !scissorMode
    endif

    'Centre the scissor area around the mouse position
    scissorArea.x = rl.GetMouseX() - scissorArea.width/2
    scissorArea.y = rl.GetMouseY() - scissorArea.height/2
    '----------------------------------------------------------------------------------

    ' Draw
    '----------------------------------------------------------------------------------
    rl.BeginDrawing()

        rl.ClearBackground(c.RAYWHITE)

        if(scissorMode) then
            rl.BeginScissorMode(scissorArea.x, scissorArea.y, scissorArea.width, scissorArea.height)
        endif

        ' Draw full screen rectangle and some text
        ' NOTE: Only part defined by scissor area will be rendered
        rl.DrawRectangle(0, 0, rl.GetScreenWidth(), rl.GetScreenHeight(), c.RED)
        rl.DrawText("Move the mouse around to reveal this text!", 190, 200, 20, c.LIGHTGRAY)

        if(scissorMode) then
            rl.EndScissorMode()
        endif

        rl.DrawRectangleLinesEx(scissorArea, 1, c.BLACK)
        rl.DrawText("Press S to toggle scissor test", 10, 10, 20, c.BLACK)

    rl.EndDrawing()
    '----------------------------------------------------------------------------------
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.CloseWindow()        ' Close window and OpenGL context
'--------------------------------------------------------------------------------------

 
