'*******************************************************************************************
'*
'*   raylib [core] example - Input Gestures Detection
'*
'*   Example originally created with raylib 1.4, last time updated with raylib 4.2
'*
'*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
'*   BSD-like license that allows static linking with closed source software
'*
'*   Copyright (c) 2016-2022 Ramon Santamaria (@raysan5)
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
const MAX_GESTURE_STRINGS = 20

touchPosition = { 0, 0 }
touchArea = [220, 10, screenWidth - 230.0, screenHeight - 20.0]
gesturesCount = 0
dim gestureStrings[MAX_GESTURE_STRINGS]
currentGesture = GESTURE_NONE
lastGesture = GESTURE_NONE


rl.InitWindow(screenWidth, screenHeight,  "raylib [core] example - input gestures")

' rl.SetGesturesEnabled(0b0000000000001001)   ' Enable only some gestures to be detected

rl.SetTargetFPS(60)               ' Set our game to run at 60 frames-per-second
'--------------------------------------------------------------------------------------

' Main game loop
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
    ' Update
    '----------------------------------------------------------------------------------
    lastGesture = currentGesture
    currentGesture = rl.GetGestureDetected()
    touchPosition = rl.GetTouchPosition(0)

    if(rl.CheckCollisionPointRec(touchPosition, touchArea) AND (currentGesture != c.GESTURE_NONE)) then
        if(currentGesture != lastGesture) then
            'Store gesture string
            select case currentGesture
                case c.GESTURE_TAP
                    gestureStrings[gesturesCount] = "GESTURE TAP"
                case c.GESTURE_DOUBLETAP
                    gestureStrings[gesturesCount] = "GESTURE DOUBLETAP"
                case c.GESTURE_HOLD
                    gestureStrings[gesturesCount] = "GESTURE HOLD"
                case c.GESTURE_DRAG
                    gestureStrings[gesturesCount] = "GESTURE DRAG"
                case c.GESTURE_SWIPE_RIGHT
                    gestureStrings[gesturesCount] = "GESTURE SWIPE RIGHT"
                case c.GESTURE_SWIPE_LEFT
                    gestureStrings[gesturesCount] = "GESTURE SWIPE LEFT"
                case c.GESTURE_SWIPE_UP
                    gestureStrings[gesturesCount] = "GESTURE SWIPE UP"
                case c.GESTURE_SWIPE_DOWN
                    gestureStrings[gesturesCount] = "GESTURE SWIPE DOWN"
                case c.GESTURE_PINCH_IN
                    gestureStrings[gesturesCount] = "GESTURE PINCH IN"
                case c.GESTURE_PINCH_OUT
                    gestureStrings[gesturesCount] = "GESTURE PINCH OUT"
            end select
    
            gesturesCount++

            ' Reset gestures strings
            if(gesturesCount >= MAX_GESTURE_STRINGS) then
                for i = 0 to MAX_GESTURE_STRINGS
                    gestureStrings[i] = ""
                next
                gesturesCount = 0
            endif
        endif
    endif
    '----------------------------------------------------------------------------------
    ' Draw
    '----------------------------------------------------------------------------------
    rl.BeginDrawing()

        rl.ClearBackground(c.RAYWHITE)

        rl.DrawRectangleRec(touchArea, c.GRAY)
        rl.DrawRectangle(225, 15, screenWidth - 240, screenHeight - 30, c.RAYWHITE)

        rl.DrawText("GESTURES TEST AREA", screenWidth - 270, screenHeight - 40, 20, rl.Fade(c.GRAY, 0.5))

        for i = 0 to gesturesCount
            if (i%2 == 0) then
                rl.DrawRectangle(10, 30 + 20*i, 200, 20, rl.Fade(c.LIGHTGRAY, 0.5))
            else
                rl.DrawRectangle(10, 30 + 20*i, 200, 20, rl.Fade(c.LIGHTGRAY, 0.3))
            endif
            if(i < gesturesCount - 1) then
                rl.DrawText(gestureStrings[i], 35, 36 + 20*i, 10, c.DARKGRAY)
            else
                rl.DrawText(gestureStrings[i], 35, 36 + 20*i, 10, c.MAROON)
            endif
        next

        rl.DrawRectangleLines(10, 29, 200, screenHeight - 50, c.GRAY)
        rl.DrawText("DETECTED GESTURES", 50, 15, 10, c.GRAY)

        if(currentGesture != GESTURE_NONE) then
            rl.DrawCircleV(touchPosition, 30, c.MAROON)
        endif


    rl.EndDrawing()
    '----------------------------------------------------------------------------------
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.CloseWindow()        ' Close window and OpenGL context
'--------------------------------------------------------------------------------------

 
