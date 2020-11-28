REM /*******************************************************************************************
REM *
REM *   raylib [textures] example - Texture source and destination rectangles
REM *
REM *   This example has been created using raylib 1.3 (www.raylib.com)
REM *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM *
REM *   Copyright (c) 2015 Ramon Santamaria (@raysan5)
REM *
REM ********************************************************************************************/

import raylib as rl
import raylibc as c

const screenWidth = 800 
const screenHeight = 450 

rl.InitWindow(screenWidth, screenHeight, "raylib [textures] examples - texture source and destination rectangles") 

'  NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)

const resources = CWD + "raylib/examples/textures/resources/"
scarfy = rl.LoadTexture(resources + "scarfy.png")         '  Texture loading

frameWidth = scarfy.width / 6 
frameHeight = scarfy.height 

'  Source rectangle (part of the texture to use for drawing)
sourceRec = [0.0, 0.0, frameWidth, frameHeight]

'  Destination rectangle (screen rectangle where drawing part of texture)
destRec = [screenWidth/2, screenHeight/2, frameWidth*2, frameHeight*2]

'  Origin of the texture (rotation/scale point), it's relative to destination rectangle size
origin = [frameWidth, frameHeight]

rotation = 0 

rl.SetTargetFPS(60) 

while (!rl.WindowShouldClose())
  rotation++ 
  rl.BeginDrawing() 
  rl.ClearBackground(c.RAYWHITE) 

  '  NOTE: Using DrawTexturePro() we can easily rotate and scale the part of the texture we draw
  '  sourceRec defines the part of the texture we use for drawing
  '  destRec defines the rectangle where our texture part will fit (scaling it to fit)
  '  origin defines the point of the texture used as reference for rotation and scaling
  '  rotation defines the texture rotation (using origin as rotation point)
  rl.DrawTexturePro(scarfy, sourceRec, destRec, origin, rotation, c.WHITE) 

  rl.DrawLine(destRec[0], 0, destRec[0], screenHeight, c.GRAY) 
  rl.DrawLine(0, destRec[1], screenWidth, destRec[1], c.GRAY) 
  rl.DrawText("(c) Scarfy sprite by Eiden Marsal", screenWidth - 200, screenHeight - 20, 10, c.GRAY) 

  rl.EndDrawing() 
wend

rl.UnloadTexture(scarfy)         '  Texture unloading
rl.CloseWindow()                 '  Close window and OpenGL context
