'*******************************************************************************************
'*
'*   raylib [core] example - window flags
'*
'*   Example originally created with raylib 3.5, last time updated with raylib 3.5
'*
'*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
'*   BSD-like license that allows static linking with closed source software
'*
'*   Copyright (c) 2020-2022 Ramon Santamaria (@raysan5)
'*
'*   Converted to SmallBASIC by Joerg Siebenmorgen, 2022
'*   https://github.com/smallbasic/smallbasic.plugins/blob/master/raylib/samples
'* 
'********************************************************************************************

import raylib as rl
import raylibc as c

' Initialization
'--------------------------------------------------------------------------------------
const screenWidth = 800
const screenHeight = 450


' Possible window flags
' FLAG_VSYNC_HINT
' FLAG_FULLSCREEN_MODE    
' FLAG_WINDOW_RESIZABLE
' FLAG_WINDOW_UNDECORATED
' FLAG_WINDOW_TRANSPARENT
' FLAG_WINDOW_HIDDEN
' FLAG_WINDOW_MINIMIZED   -> Not supported on window creation
' FLAG_WINDOW_MAXIMIZED   -> Not supported on window creation
' FLAG_WINDOW_UNFOCUSED
' FLAG_WINDOW_TOPMOST
' FLAG_WINDOW_HIGHDPI     
' FLAG_WINDOW_ALWAYS_RUN
' FLAG_MSAA_4X_HINT

rl.InitWindow(screenWidth, screenHeight, "raylib [core] example - window flags")

ballPosition = {}
ballPosition.x = rl.GetScreenWidth() / 2
ballPosition.y = rl.GetScreenHeight() / 2
ballSpeed = { x: 5.0, y: 4.0}
ballRadius = 20

framesCounter = 0

rl.SetTargetFPS(60)               ' Set our game to run at 60 frames-per-second

'--------------------------------------------------------------------------------------

' Main game loop
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
    ' Update
    '----------------------------------------------------------------------------------
    if (rl.IsKeyPressed(c.KEY_F)) then
        rl.ToggleFullscreen()  ' modifies window size when scaling!
    endif

    if(rl.IsKeyPressed(c.KEY_R)) then
        if(rl.IsWindowState(c.FLAG_WINDOW_RESIZABLE)) then
            rl.ClearWindowState(c.FLAG_WINDOW_RESIZABLE)
        else
            rl.SetWindowState(c.FLAG_WINDOW_RESIZABLE)
        endif
    endif
    
    if (rl.IsKeyPressed(c.KEY_D)) then
        if(rl.IsWindowState(c.FLAG_WINDOW_UNDECORATED)) then
            rl.ClearWindowState(c.FLAG_WINDOW_UNDECORATED)
        else
            rl.SetWindowState(c.FLAG_WINDOW_UNDECORATED)
        endif
    endif

    if(rl.IsKeyPressed(c.KEY_H)) then
        if(!rl.IsWindowState(c.FLAG_WINDOW_HIDDEN)) then
            rl.SetWindowState(c.FLAG_WINDOW_HIDDEN)
        endif

        framesCounter = 0
    endif

    if(rl.IsWindowState(c.FLAG_WINDOW_HIDDEN)) then
        framesCounter++
        if(framesCounter >= 240) then
            rl.ClearWindowState(c.FLAG_WINDOW_HIDDEN) ' Show window after 3 seconds
        endif
    endif
    
    if(rl.IsKeyPressed(c.KEY_N)) then
        if(!rl.IsWindowState(c.FLAG_WINDOW_MINIMIZED)) then
            rl.MinimizeWindow()
        endif
        
        framesCounter = 0
    endif

    if(rl.IsWindowState(c.FLAG_WINDOW_MINIMIZED)) then
        framesCounter++
        if(framesCounter >= 240) then
            rl.RestoreWindow()  ' Restore window after 3 seconds
        endif
    endif

    if(rl.IsKeyPressed(c.KEY_M)) then
        ' NOTE: Requires FLAG_WINDOW_RESIZABLE enabled!
        if(rl.IsWindowState(c.FLAG_WINDOW_MAXIMIZED)) then
            rl.RestoreWindow()
        else
            rl.MaximizeWindow()
        endif
    endif

    if(rl.IsKeyPressed(c.KEY_U)) then
        if(rl.IsWindowState(c.FLAG_WINDOW_UNFOCUSED)) then
            rl.ClearWindowState(cFLAG_WINDOW_UNFOCUSED)
        else
            rl.SetWindowState(c.FLAG_WINDOW_UNFOCUSED)
        endif
    endif

    if(rl.IsKeyPressed(c.KEY_T)) then
        if(rl.IsWindowState(c.FLAG_WINDOW_TOPMOST)) then
            rl.ClearWindowState(c.FLAG_WINDOW_TOPMOST)
        else
            rl.SetWindowState(c.FLAG_WINDOW_TOPMOST)
        endif
    endif
    
    if(rl.IsKeyPressed(c.KEY_A)) then
        if(rl.IsWindowState(c.FLAG_WINDOW_ALWAYS_RUN)) then
            rl.ClearWindowState(c.FLAG_WINDOW_ALWAYS_RUN)
        else
            rl.SetWindowState(c.FLAG_WINDOW_ALWAYS_RUN)
        endif
    endif

    if(rl.IsKeyPressed(c.KEY_V)) then
        if(rl.IsWindowState(c.FLAG_VSYNC_HINT)) then
            rl.ClearWindowState(c.FLAG_VSYNC_HINT)
        else
            rl.SetWindowState(c.FLAG_VSYNC_HINT)
        endif
    endif

    ' Bouncing ball logic
    ballPosition.x += ballSpeed.x
    ballPosition.y += ballSpeed.y
    if((ballPosition.x >= (rl.GetScreenWidth() - ballRadius)) OR (ballPosition.x <= ballRadius)) then
        ballSpeed.x *= -1.0
    endif
    if((ballPosition.y >= (rl.GetScreenHeight() - ballRadius)) OR (ballPosition.y <= ballRadius)) then
        ballSpeed.y *= -1.0
    endif
    '----------------------------------------------------------------------------------
    ' Draw
    '----------------------------------------------------------------------------------
    rl.BeginDrawing()

        if(rl.IsWindowState(c.FLAG_WINDOW_TRANSPARENT)) then
            rl.ClearBackground(c.BLANK)
        else
            rl.ClearBackground(c.RAYWHITE)
        endif

        rl.DrawCircleV(ballPosition, ballRadius, c.MAROON)
        rl.DrawRectangleLinesEx([0, 0, rl.GetScreenWidth(), rl.GetScreenHeight()], 4, c.RAYWHITE)

        rl.DrawCircleV(rl.GetMousePosition(), 10, c.DARKBLUE)

        rl.DrawFPS(10, 10)

        rl.DrawText(rl.TextFormat("Screen Size: [%i, %i]", rl.GetScreenWidth(), rl.GetScreenHeight()), 10, 40, 10, c.GREEN)

        ' Draw window state info
        rl.DrawText("Following flags can be set after window creation:", 10, 60, 10, c.GRAY)
        
        if(rl.IsWindowState(c.FLAG_FULLSCREEN_MODE)) then
            rl.DrawText("[F] FLAG_FULLSCREEN_MODE: on", 10, 80, 10, c.LIME)
        else
            rl.DrawText("[F] FLAG_FULLSCREEN_MODE: off", 10, 80, 10, c.MAROON)
        endif
        
        if(rl.IsWindowState(c.FLAG_WINDOW_RESIZABLE)) then
            rl.DrawText("[R] FLAG_WINDOW_RESIZABLE: on", 10, 100, 10, c.LIME)
        else
            rl.DrawText("[R] FLAG_WINDOW_RESIZABLE: off", 10, 100, 10, c.MAROON)
        endif
        
        if(rl.IsWindowState(c.FLAG_WINDOW_UNDECORATED)) then
            rl.DrawText("[D] FLAG_WINDOW_UNDECORATED: on", 10, 120, 10, c.LIME)
        else
            rl.DrawText("[D] FLAG_WINDOW_UNDECORATED: off", 10, 120, 10, c.MAROON)
        endif
        
        if(rl.IsWindowState(c.FLAG_WINDOW_HIDDEN)) then
            rl.DrawText("[H] FLAG_WINDOW_HIDDEN: on", 10, 140, 10, c.LIME)
        else
            rl.DrawText("[H] FLAG_WINDOW_HIDDEN: off", 10, 140, 10, c.MAROON)
        endif
        
        if(rl.IsWindowState(c.FLAG_WINDOW_MINIMIZED)) then
            rl.DrawText("[N] FLAG_WINDOW_MINIMIZED: on", 10, 160, 10, c.LIME)
        else
            rl.DrawText("[N] FLAG_WINDOW_MINIMIZED: off", 10, 160, 10, c.MAROON)
        endif
        
        if(rl.IsWindowState(c.FLAG_WINDOW_MAXIMIZED)) then
            rl.DrawText("[M] FLAG_WINDOW_MAXIMIZED: on", 10, 180, 10, c.LIME)
        else
            rl.DrawText("[M] FLAG_WINDOW_MAXIMIZED: off", 10, 180, 10, c.MAROON)
        endif
        
        if(rl.IsWindowState(c.FLAG_WINDOW_UNFOCUSED)) then
            rl.DrawText("[G] FLAG_WINDOW_UNFOCUSED: on", 10, 200, 10, c.LIME)
        else
            rl.DrawText("[U] FLAG_WINDOW_UNFOCUSED: off", 10, 200, 10, c.MAROON)
        endif
        
        if(rl.IsWindowState(c.FLAG_WINDOW_TOPMOST)) then
            rl.DrawText("[T] FLAG_WINDOW_TOPMOST: on", 10, 220, 10, c.LIME)
        else
            rl.DrawText("[T] FLAG_WINDOW_TOPMOST: off", 10, 220, 10, c.MAROON)
        endif
        if(rl.IsWindowState(c.FLAG_WINDOW_ALWAYS_RUN)) then
            rl.DrawText("[A] FLAG_WINDOW_ALWAYS_RUN: on", 10, 240, 10, c.LIME)
        else
            rl.DrawText("[A] FLAG_WINDOW_ALWAYS_RUN: off", 10, 240, 10, c.MAROON)
        endif
        
        if(rl.IsWindowState(c.FLAG_VSYNC_HINT)) then
            rl.DrawText("[V] FLAG_VSYNC_HINT: on", 10, 260, 10, c.LIME)
        else
            rl.DrawText("[V] FLAG_VSYNC_HINT: off", 10, 260, 10, c.MAROON)
        endif

        rl.DrawText("Following flags can only be set before window creation:", 10, 300, 10, c.GRAY)
        
        if(rl.IsWindowState(c.FLAG_WINDOW_HIGHDPI)) then
            rl.DrawText("FLAG_WINDOW_HIGHDPI: on", 10, 320, 10, c.LIME)
        else
            rl.DrawText("FLAG_WINDOW_HIGHDPI: off", 10, 320, 10, c.MAROON)
        endif
        
        if(rl.IsWindowState(c.FLAG_WINDOW_TRANSPARENT)) then
            rl.DrawText("FLAG_WINDOW_TRANSPARENT: on", 10, 340, 10, c.LIME)
        else
            rl.DrawText("FLAG_WINDOW_TRANSPARENT: off", 10, 340, 10, c.MAROON)
        endif
        
        if(rl.IsWindowState(c.FLAG_MSAA_4X_HINT)) then
            rl.DrawText("FLAG_MSAA_4X_HINT: on", 10, 360, 10, c.LIME)
        else
            rl.DrawText("FLAG_MSAA_4X_HINT: off", 10, 360, 10, c.MAROON)
        endif

    rl.EndDrawing()
    '----------------------------------------------------------------------------------
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.CloseWindow()        ' Close window and OpenGL context
'--------------------------------------------------------------------------------------

 
