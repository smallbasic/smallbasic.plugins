REM /*******************************************************************************************
REM *
REM *   raylib [shapes] example - Draw basic shapes 2d (rectangle, circle, line...)
REM *
REM *   This example has been created using raylib 1.0 (www.raylib.com)
REM *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM *
REM *   Copyright (c) 2014 Ramon Santamaria (@raysan5)
REM *
REM ********************************************************************************************/

import raylib as rl
import raylibc as c

const screenWidth = 800
const screenHeight = 450

rl.InitWindow(screenWidth, screenHeight, "raylib [shapes] example - basic shapes drawing")

rl.SetTargetFPS(60)               ' Set our game to run at 60 frames-per-second
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
  rl.BeginDrawing()
  rl.ClearBackground(c.RAYWHITE)
  rl.DrawText("some basic shapes available on raylib", 20, 20, 20, c.DARKGRAY)

  ' Circle shapes and lines
  rl.DrawCircle(screenWidth/5, 120, 35, c.DARKBLUE)
  rl.DrawCircleGradient(screenWidth/5, 220, 60, c.GREEN, c.SKYBLUE)
  rl.DrawCircleLines(screenWidth/5, 340, 80, c.DARKBLUE)

  ' Rectangle shapes and ines
  rl.DrawRectangle(screenWidth/4*2 - 60, 100, 120, 60, RED)
  rl.DrawRectangleGradientH(screenWidth/4*2 - 90, 170, 180, 130, MAROON, GOLD)
  rl.DrawRectangleLines(screenWidth/4*2 - 40, 320, 80, 60, ORANGE)  ' NOTE: Uses QUADSernally, not lines

  ' Triangle shapes and lines
  rl.DrawTriangle([screenWidth / 4.0 * 3.0, 80.0],  [screenWidth / 4.0 * 3.0 - 60.0, 150.0], [screenWidth / 4.0 * 3.0 + 60.0, 150.0], c.VIOLET)
  rl.DrawTriangleLines([screenWidth/4.0 * 3.0, 160.0], [screenWidth / 4.0 * 3.0 - 20.0, 230.0], [screenWidth / 4.0 * 3.0 + 20.0, 230.0], c.DARKBLUE)

  ' Polygon shapes and lines
  rl.DrawPoly([screenWidth / 4.0 * 3, 320], 6, 80, 0, c.BROWN)
  rl.DrawPolyLinesEx([screenWidth / 4.0 * 3, 320], 6, 80, 0, 6, c.BEIGE)

  ' NOTE: We draw all LINES based shapes together to optimizeernal drawing,
  ' this way, all LINES are rendered in a single draw pass
  rl.DrawLine(18, 42, screenWidth - 18, 42, c.BLACK)
  rl.EndDrawing()
wend

rl.CloseWindow()        ' Close window and OpenGL context
