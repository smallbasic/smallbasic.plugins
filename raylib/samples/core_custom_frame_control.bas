'*******************************************************************************************
'*
'*   raylib [core] example - custom frame control
'*
'*   NOTE: WARNING: This is an example for advance users willing to have full control over
'*   the frame processes. By default, EndDrawing() calls the following processes:
'*       1. Draw remaining batch data: rlDrawRenderBatchActive()
'*       2. SwapScreenBuffer()
'*       3. Frame time control: WaitTime()
'*       4. PollInputEvents()
'*
'*   To avoid steps 2, 3 and 4, flag SUPPORT_CUSTOM_FRAME_CONTROL can be enabled in
'*   config.h (it requires recompiling raylib). This way those steps are up to the user.
'*
'*   Note that enabling this flag invalidates some functions:
'*       - GetFrameTime()
'*       - SetTargetFPS()
'*       - GetFPS()
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

rl.InitWindow(screenWidth, screenHeight, "raylib [core] example - custom frame control")


previousTime = rl.GetTime()             ' Previous time measure
currentTime = 0.0                       ' Current time measure
updateDrawTime = 0.0                    ' Update + Draw time
waitTime = 0.0                          ' Wait time (if target fps required)
deltaTime = 1.0                         ' Frame time (Update + Draw + Wait time)

timeCounter = 0.0                       ' Accumulative time counter (seconds)
position = 0.0                          ' Circle position
doPause = false                         ' Pause control flag

targetFPS = 60                          ' Our initial target fps
'--------------------------------------------------------------------------------------

' Main game loop
while (!rl.WindowShouldClose())         ' Detect window close button or ESC key
    ' Update
    '----------------------------------------------------------------------------------
    rl.PollInputEvents()                ' Poll input events (SUPPORT_CUSTOM_FRAME_CONTROL)
        
    if(rl.IsKeyPressed(c.KEY_SPACE)) then
        doPause = !doPause
    endif
    
    if(rl.IsKeyPressed(c.KEY_UP)) then
        targetFPS += 5
    else if(rl.IsKeyPressed(c.KEY_DOWN)) then
        targetFPS -= 5
    endif
    
    if(targetFPS < 5) then
        targetFPS = 5
    endif

    if(!doPause) then
        position += 200 * deltaTime     ' We move at 200 pixels per second
        if(position >= rl.GetScreenWidth()) then
            position = 0
        endif
        timeCounter += deltaTime        ' We count time (seconds)
    endif
    '----------------------------------------------------------------------------------

    ' Draw
    '----------------------------------------------------------------------------------
    rl.BeginDrawing()

        rl.ClearBackground(c.RAYWHITE)

        for i = 0 to rl.GetScreenWidth()/200
            rl.DrawRectangle(200*i, 0, 1, rl.GetScreenHeight(), c.SKYBLUE)
        next
            
        rl.DrawCircle(position, rl.GetScreenHeight()/2 - 25, 50, c.RED)
        
        rl.DrawText(rl.TextFormat("%03.0f ms", timeCounter*1000.0), position - 40, rl.GetScreenHeight()/2 - 100, 20, c.MAROON)
        rl.DrawText(rl.TextFormat("PosX: %03.0f", position), position - 50, rl.GetScreenHeight()/2 + 40, 20, c.BLACK)
        
        rl.DrawText("Circle is moving at a constant 200 pixels/sec,\nindependently of the frame rate.", 10, 10, 20, c.DARKGRAY)
        rl.DrawText("PRESS SPACE to PAUSE MOVEMENT", 10, rl.GetScreenHeight() - 60, 20, c.GRAY)
        rl.DrawText("PRESS UP | DOWN to CHANGE TARGET FPS", 10, rl.GetScreenHeight() - 30, 20, c.GRAY)
        rl.DrawText(rl.TextFormat("TARGET FPS: %i", targetFPS), rl.GetScreenWidth() - 220, 10, 20, c.LIME)
        rl.DrawText(rl.TextFormat("CURRENT FPS: %f", 1.0/deltaTime), rl.GetScreenWidth() - 220, 40, 20, c.GREEN)

    rl.EndDrawing()
    
    ' NOTE: In case raylib is configured to SUPPORT_CUSTOM_FRAME_CONTROL, 
    ' Events polling, screen buffer swap and frame time control must be managed by the user

    ' rl.SwapScreenBuffer()         ' Flip the back buffer to screen (front buffer)
    
    currentTime = rl.GetTime()
    updateDrawTime = currentTime - previousTime
    
    if(targetFPS > 0) then          ' We want a fixed frame rate
        waitTime = 1.0/targetFPS - updateDrawTime
        if(waitTime > 0.0) then
            rl.WaitTime(waitTime*1000.0)
            currentTime = rl.GetTime()
            deltaTime = currentTime - previousTime
        endif
    else
        deltaTime = updateDrawTime    ' Framerate could be variable
    endif
    
    previousTime = currentTime
    '----------------------------------------------------------------------------------
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.CloseWindow()        ' Close window and OpenGL context
'--------------------------------------------------------------------------------------

 
