'*******************************************************************************************
'*
'*   raylib [core] example - smooth pixel-perfect camera
'*
'*   Example originally created with raylib 3.7, last time updated with raylib 4.0
'*   
'*   Example contributed by Giancamillo Alessandroni (@NotManyIdeasDev) and
'*   reviewed by Ramon Santamaria (@raysan5)
'*
'*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
'*   BSD-like license that allows static linking with closed source software
'*
'*   Copyright (c) 2021-2022 Giancamillo Alessandroni (@NotManyIdeasDev) and Ramon Santamaria (@raysan5)
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

const virtualScreenWidth = 160
const virtualScreenHeight = 90

const virtualRatio = screenWidth/virtualScreenWidth

rl.InitWindow(screenWidth, screenHeight, "raylib [core] example - smooth pixel-perfect camera")

worldSpaceCamera.target.x = 0       ' Game world camera
worldSpaceCamera.target.y = 0
worldSpaceCamera.offset.x = 0
worldSpaceCamera.offset.y = 0
worldSpaceCamera.rotation = 0.0
worldSpaceCamera.zoom = 1.0

screenSpaceCamera.target.x = 0      ' Smoothing camera
screenSpaceCamera.target.y = 0
screenSpaceCamera.offset.x = 0
screenSpaceCamera.offset.y = 0
screenSpaceCamera.rotation = 0.0
screenSpaceCamera.zoom = 1.0

target = rl.LoadRenderTexture(virtualScreenWidth, virtualScreenHeight) ' This is where we'll draw all our objects.

rec01 = { x: 70.0, y: 35.0, width: 20.0, height: 20.0 }
rec02 = { x: 90.0, y: 55.0, width: 30.0, height: 10.0 }
rec03 = { x: 80.0, y: 65.0, width: 15.0, height: 25.0 }

' The target's height is flipped (in the source Rectangle), due to OpenGL reasons
sourceRec = { x: 0.0, y: 0.0}
sourceRec.width = target.texture.width
sourceRec.height = -target.texture.height

destRec.x = -virtualRatio
destRec.y = -virtualRatio
destRec.width = screenWidth + (virtualRatio*2)
destRec.height = screenHeight + (virtualRatio*2)

origin = { x: 0.0, y: 0.0 }

rotation = 0.0

cameraX = 0.0
cameraY = 0.0



rl.SetTargetFPS(60)               ' Set our game to run at 60 frames-per-second
'--------------------------------------------------------------------------------------

' Main game loop
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
    ' Update
    '----------------------------------------------------------------------------------
    rotation += 60.0 * rl.GetFrameTime()   ' Rotate the rectangles, 60 degrees per second

    ' Make the camera move to demonstrate the effect
    cameraX = sin(rl.GetTime()) * 50.0 - 10.0
    cameraY = cos(rl.GetTime()) * 30.0

    ' Set the camera's target to the values computed above
    screenSpaceCamera.target.x = cameraX
    screenSpaceCamera.target.y = cameraY

    ' Round worldSpace coordinates, keep decimals into screenSpace coordinates
    worldSpaceCamera.target.x = int(screenSpaceCamera.target.x)
    screenSpaceCamera.target.x -= worldSpaceCamera.target.x
    screenSpaceCamera.target.x *= virtualRatio

    worldSpaceCamera.target.y = int(screenSpaceCamera.target.y)
    screenSpaceCamera.target.y -= worldSpaceCamera.target.y
    screenSpaceCamera.target.y *= virtualRatio

    '----------------------------------------------------------------------------------
    ' Draw
    '----------------------------------------------------------------------------------
    rl.BeginTextureMode(target)
        rl.ClearBackground(c.RAYWHITE)

        rl.BeginMode2D(worldSpaceCamera)
            rl.DrawRectanglePro(rec01, origin, rotation, c.BLACK)
            rl.DrawRectanglePro(rec02, origin, -rotation, c.RED)
            rl.DrawRectanglePro(rec03, origin, rotation + 45.0, c.BLUE)
        rl.EndMode2D()
    rl.EndTextureMode()
        
    rl.BeginDrawing()
        rl.ClearBackground(c.RED)

        rl.BeginMode2D(screenSpaceCamera)
            rl.DrawTexturePro(target.texture, sourceRec, destRec, origin, 0.0, c.WHITE)
        rl.EndMode2D()

        rl.DrawText(rl.TextFormat("Screen resolution: %ix%i", screenWidth, screenHeight), 10, 10, 20, c.DARKBLUE)
        rl.DrawText(rl.TextFormat("World resolution: %ix%i", virtualScreenWidth, virtualScreenHeight), 10, 40, 20, c.DARKGREEN)
        rl.DrawFPS(rl.GetScreenWidth() - 95, 10)

    rl.EndDrawing()
    '----------------------------------------------------------------------------------
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.UnloadRenderTexture(target)    ' Unload render texture
rl.CloseWindow()        ' Close window and OpenGL context
'--------------------------------------------------------------------------------------

 
