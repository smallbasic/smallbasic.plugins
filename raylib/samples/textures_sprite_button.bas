REM /*******************************************************************************************
REM *
REM *   raylib [textures] example - sprite button
REM *
REM *   This example has been created using raylib 2.5 (www.raylib.com)
REM *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM *
REM *   Copyright (c) 2019 Ramon Santamaria (@raysan5)
REM *
REM ********************************************************************************************/

import raylib as rl
import raylibc as c

const NUM_FRAMES = 3       ' Number of frames (rectangles) for the button sprite texture
const screenWidth = 800
const screenHeight = 450

rl.InitWindow(screenWidth, screenHeight, "SmallBASIC raylib [textures] example - sprite button")
rl.InitAudioDevice()      ' Initialize audio device

resources = CWD + "raylib/examples/textures/resources/"
fxButton = rl.LoadSound(resources + "buttonfx.wav")   ' Load button sound
button = rl.LoadTexture(resources + "button.png") ' Load button texture

' Define frame rectangle for drawing
frameHeight = button.height/NUM_FRAMES
sourceRec = [0, 0, button.width, frameHeight]
btnBounds = [screenWidth/2 - button.width/2, screenHeight/2 - button.height/NUM_FRAMES/2, button.width, frameHeight]
btnState = 0               ' Button state: 0-NORMAL, 1-MOUSE_HOVER, 2-PRESSED
btnAction = false          ' Button action should be activated

rl.SetTargetFPS(60)
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
  mousePoint = rl.GetMousePosition()
  btnAction = false

  ' Check button state
  if (rl.CheckCollisionPointRec(mousePoint, btnBounds)) then
    if (rl.IsMouseButtonDown(c.MOUSE_LEFT_BUTTON)) then
      btnState = 2
    else 
      btnState = 1
    endif
    if (rl.IsMouseButtonReleased(c.MOUSE_LEFT_BUTTON)) then 
      btnAction = true
    endif
  else 
    btnState = 0
  endif

  if (btnAction) then
    print "sound!"
    rl.PlaySound(fxButton)
  endif
  
  ' Calculate button frame rectangle to draw depending on button state
  sourceRec[1] = btnState * frameHeight
  rl.BeginDrawing()
  rl.ClearBackground(c.RAYWHITE)
  rl.DrawTextureRec(button, sourceRec, btnBounds, c.WHITE) ' Draw button frame
  rl.EndDrawing()
wend

rl.UnloadTexture(button)  ' Unload button texture
rl.UnloadSound(fxButton)  ' Unload sound
rl.CloseAudioDevice()     ' Close audio device
rl.CloseWindow()          ' Close window and OpenGL context
