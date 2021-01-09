REM /*******************************************************************************************
REM *
REM *   raylib [textures] example - sprite explosion
REM *
REM *   This example has been created using raylib 2.5 (www.raylib.com)
REM *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM *
REM *   Copyright (c) 2019 Anata and Ramon Santamaria (@raysan5)
REM *
REM ********************************************************************************************/

import raylib as rl
import raylibc as c

const NUM_FRAMES_PER_LINE =   5
const NUM_LINES =             5

const screenWidth = 800
const screenHeight = 450

rl.InitWindow(screenWidth, screenHeight, "raylib [textures] example - sprite explosion")
rl.InitAudioDevice()

' Load explosion sound
const resources = CWD + "raylib/examples/textures/resources/"
fxBoom = rl.LoadSound(resources + "boom.wav")

' Load explosion texture
explosion = rl.LoadTexture(resources + "explosion.png")

' Init variables for animation
frameWidth = explosion.width / NUM_FRAMES_PER_LINE   ' Sprite one frame rectangle width
frameHeight = explosion.height / NUM_LINES           ' Sprite one frame rectangle height
currentFrame = 0
currentLine = 0

frameRec = [0, 0, frameWidth, frameHeight]
position = [0.0, 0.0]
active = true
framesCounter = 0

rl.SetTargetFPS(120)
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
  ' Check for mouse button pressed and activate explosion (if not active)
  if (rl.IsMouseButtonPressed(c.MOUSE_LEFT_BUTTON) && !active) then
    position = rl.GetMousePosition()
    active = true
    position.x -= frameWidth/2
    position.y -= frameHeight/2
    rl.PlaySound(fxBoom)
  endif

  ' Compute explosion animation frames
  if (active) then
    framesCounter++
    if (framesCounter > 2) then
      currentFrame++
      if (currentFrame >= NUM_FRAMES_PER_LINE) then
        currentFrame = 0
        currentLine++
        if (currentLine >= NUM_LINES) then
          currentLine = 0
          active = false
        endif
      endif
      framesCounter = 0
    endif
  endif

  frameRec[0] = frameWidth * currentFrame
  frameRec[1] = frameHeight * currentLine

  rl.BeginDrawing()
  rl.ClearBackground(c.RAYWHITE)

  ' Draw explosion required frame rectangle
  if (active) then
    rl.DrawTextureRec(explosion, frameRec, position, c.WHITE)
  endif

  rl.EndDrawing()
wend

rl.UnloadTexture(explosion)   ' Unload texture
rl.UnloadSound(fxBoom)        ' Unload sound
rl.CloseAudioDevice()
rl.CloseWindow()              ' Close window and OpenGL context
