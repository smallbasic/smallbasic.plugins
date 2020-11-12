REM *******************************************************************************************
REM
REM    raylib [shapes] example - draw ring (with gui options)
REM
REM    This example has been created using raylib 2.5 (www.raylib.com)
REM    raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM
REM    Example contributed by Vlad Adrian (@demizdor) and reviewed by Ramon Santamaria (@raysan5)
REM
REM    Copyright (c) 2018 Vlad Adrian (@demizdor) and Ramon Santamaria (@raysan5)
REM
REM *******************************************************************************************

import raylib as rl
import raylibc as c

const screenWidth = 800 
const  screenHeight = 450 

rl.InitWindow(screenWidth, screenHeight, "raylib [shapes] example - draw ring") 

center = [(rl.GetScreenWidth() - 300) / 2, rl.GetScreenHeight() / 2]
innerRadius = 80.0
outerRadius = 190.0
startAngle = 0 
endAngle = 360 
segments = 0 
drawRing = true 
drawRingLines = false 
drawCircleLines = false 

rl.SetTargetFPS(60)

while (!rl.WindowShouldClose())
  rl.BeginDrawing() 
  rl.ClearBackground(c.RAYWHITE) 
  rl.DrawLine(500, 0, 500, rl.GetScreenHeight(), rl.Fade(c.LIGHTGRAY, 0.6)) 
  rl.DrawRectangle(500, 0, rl.GetScreenWidth() - 500, rl.GetScreenHeight(), rl.Fade(c.LIGHTGRAY, 0.3)) 

  if (drawRing) then rl.DrawRing(center, innerRadius, outerRadius, startAngle, endAngle, segments, rl.Fade(c.MAROON, 0.3)) 
  if (drawRingLines) then rl.DrawRingLines(center, innerRadius, outerRadius, startAngle, endAngle, segments, rl.Fade(c.BLACK, 0.4)) 
  if (drawCircleLines) then rl.DrawCircleSectorLines(center, outerRadius, startAngle, endAngle, segments, rl.Fade(c.BLACK, 0.4)) 

  ' Draw GUI controls
  startAngle = rl.GuiSliderBar([600, 40, 120, 20], "StartAngle", startAngle, -450, 450, true) 
  endAngle = rl.GuiSliderBar([600, 70, 120, 20], "EndAngle", endAngle, -450, 450, true) 
  innerRadius = rl.GuiSliderBar([600, 140, 120, 20], "InnerRadius", innerRadius, 0, 100, true) 
  outerRadius = rl.GuiSliderBar([600, 170, 120, 20], "OuterRadius", outerRadius, 0, 200, true) 
  segments = rl.GuiSliderBar([600, 240, 120, 20], "Segments", segments, 0, 100, true) 
  drawRing = rl.GuiCheckBox(([600, 320, 20, 20], "Draw Ring", drawRing) 
  drawRingLines = rl.GuiCheckBox([600, 350, 20, 20], "Draw RingLines", drawRingLines) 
  drawCircleLines = rl.GuiCheckBox([600, 380, 20, 20], "Draw CircleLines", drawCircleLines) 

  rl.DrawText(Format("MODE: ###", iff(segments >= 4, "MANUAL", "AUTO"), 600, 270, 10, iff(segments >= 4, c.MAROON, c.DARKGRAY)) 
  rl.DrawFPS(10, 10) 
  rl.EndDrawing() 
wend

rl.CloseWindow()

