'*******************************************************************************************
'*
'*   raylib [core] example - window scale letterbox (and virtual mouse)
'*
'*   Example originally created with raylib 2.5, last time updated with raylib 4.0
'*
'*   Example contributed by Anata (@anatagawa) and reviewed by Ramon Santamaria (@raysan5)
'*
'*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
'*   BSD-like license that allows static linking with closed source software
'*
'*   Copyright (c) 2019-2022 Anata (@anatagawa) and Ramon Santamaria (@raysan5)
'*
'*   Converted to SmallBASIC by Joerg Siebenmorgen, 2022
'*   https://github.com/smallbasic/smallbasic.plugins/blob/master/raylib/samples
'* 
'********************************************************************************************/

import raylib as rl
import raylibc as c

' Initialization
'--------------------------------------------------------------------------------------
const windowWidth = 800
const windowHeight = 450

virtualMouse = {}

' Enable config flags for resizable window and vertical synchro
rl.SetConfigFlags(c.FLAG_WINDOW_RESIZABLE | c.FLAG_VSYNC_HINT)
rl.InitWindow(windowWidth, windowHeight, "raylib [core] example - window scale letterbox")
rl.SetWindowMinSize(320, 240)

gameScreenWidth = 640
gameScreenHeight = 480

' Render texture initialization, used to hold the rendering result so we can easily resize it
target = rl.LoadRenderTexture(gameScreenWidth, gameScreenHeight)
rl.SetTextureFilter(target.texture, c.TEXTURE_FILTER_BILINEAR)  ' Texture scale filter to use

dim colors(10)
for i = 0 to 10
    colors[i] = [rl.GetRandomValue(100, 250), rl.GetRandomValue(50, 150), rl.GetRandomValue(10, 100), 255 ]
next

rl.SetTargetFPS(60)               ' Set our game to run at 60 frames-per-second
'--------------------------------------------------------------------------------------

' Main game loop
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
    ' Update
    '----------------------------------------------------------------------------------
    ' Compute required framebuffer scaling
    scale = min(rl.GetScreenWidth()/gameScreenWidth, rl.GetScreenHeight()/gameScreenHeight)

    if(rl.IsKeyPressed(c.KEY_SPACE)) then
        ' Recalculate random colors for the bars
        for i = 0 to 10
            colors[i] = [rl.GetRandomValue(100, 250), rl.GetRandomValue(50, 150), rl.GetRandomValue(10, 100), 255]
        next
    endif

    ' Update virtual mouse (clamped mouse value behind game screen)
    mouse = rl.GetMousePosition()
  
    virtualMouse.x = (mouse.x - (rl.GetScreenWidth() - (gameScreenWidth * scale)) * 0.5)/scale
    virtualMouse.y = (mouse.y - (rl.GetScreenHeight() - (gameScreenHeight * scale)) * 0.5)/scale
    
    ' Vector2Clamp implementation in SmallBASIC
    virtualMouse.x = min(gameScreenWidth, max(0, virtualMouse.x))
    virtualMouse.y = min(gameScreenHeight, max(0, virtualMouse.y))
    
    ' Apply the same transformation as the virtual mouse to the real mouse (i.e. to work with raygui)
    ' rl.SetMouseOffset(-(rl.GetScreenWidth() - (gameScreenWidth*scale))*0.5, -(rl.GetScreenHeight() - (gameScreenHeight*scale))*0.5)
    ' rl.SetMouseScale(1/scale, 1/scale)
    '----------------------------------------------------------------------------------

    ' Draw
    '----------------------------------------------------------------------------------
    rl.BeginTextureMode(target)
        rl.ClearBackground(c.RAYWHITE)  ' Clear render texture background color

        for i = 0 to 10
            rl.DrawRectangle(0, (gameScreenHeight/10)*i, gameScreenWidth, gameScreenHeight/10, colors[i])
        next

        rl.DrawText("If executed inside a window,\nyou can resize the window,\nand see the screen scaling!", 10, 25, 20, c.WHITE)
        'rl.DrawText(rl.TextFormat("Default Mouse: [%i , %i]", mouse.x, mouse.y), 350, 25, 20, c.GREEN)
        rl.DrawText("Default Mouse: [" + mouse.x + " , " + mouse.y + "]", 350, 25, 20, c.GREEN)
        'rl.DrawText(rl.TextFormat("Virtual Mouse: [%i , %i]", virtualMouse.x, virtualMouse.y), 350, 55, 20, c.YELLOW)
        rl.DrawText("Virtual Mouse: [" + int(virtualMouse.x) + " , " + int(virtualMouse.y) + "]", 350, 55, 20, c.YELLOW)
    rl.EndTextureMode()
        
    rl.BeginDrawing()
        rl.ClearBackground(c.BLACK)     ' Clear screen background

        ' Draw render texture to screen, properly scaled
        rl.DrawTexturePro(target.texture, [0.0, 0.0, target.texture.width, -target.texture.height], &
                       [(rl.GetScreenWidth() - (gameScreenWidth*scale))*0.5, (rl.GetScreenHeight() - (gameScreenHeight*scale))*0.5, &
                       gameScreenWidth*scale, gameScreenHeight*scale], [0, 0], 0.0, c.WHITE)
    rl.EndDrawing()
    '----------------------------------------------------------------------------------
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.CloseWindow()        ' Close window and OpenGL context
'--------------------------------------------------------------------------------------

 
