REM ----------------------------------------------------------------------------------------
REM
REM
REM   raylib [textures] example - Background scrolling
REM
REM   This example has been created using raylib 2.0 (www.raylib.com)
REM   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM
REM   Copyright (c) 2019 Ramon Santamaria (@raysan5)
REM
REM ----------------------------------------------------------------------------------------

import raylib as rl
import raylibc as c

const screenWidth = 800
const screenHeight = 450

rl.InitWindow(screenWidth, screenHeight, "raylib [textures] example - background scrolling")

REM NOTE: Be careful, background width must be equal or bigger than screen width
REM if not, texture should be draw more than two times for scrolling effect

const resources = CWD + "raylib/examples/textures/resources/"

background = rl.LoadTexture(resources + "cyberpunk_street_background.png")
midground = rl.LoadTexture(resources + "cyberpunk_street_midground.png")
foreground = rl.LoadTexture(resources + "cyberpunk_street_foreground.png")

scrollingBack = 0.0
scrollingMid = 0.0
scrollingFore = 0.0

rl.SetTargetFPS(60)

while (!rl.WindowShouldClose())
  scrollingBack -= 0.1
  scrollingMid -= 0.5
  scrollingFore -= 1.0

  REM NOTE: Texture is scaled twice its size, so it sould be considered on scrolling
  if (scrollingBack <= -background.width * 2) then scrollingBack = 0
  if (scrollingMid <= -midground.width * 2) then scrollingMid = 0
  if (scrollingFore <= -foreground.width * 2) then scrollingFore = 0

  rl.BeginDrawing()
  rl.ClearBackground(rl.GetColor(0x052c46ff))

  REM Draw background image twice
  REM NOTE: Texture is scaled twice its size
  rl.DrawTextureEx(background, [scrollingBack, 20], 0.0, 2.0, c.WHITE)
  rl.DrawTextureEx(background, [background.width * 2 + scrollingBack, 20], 0.0, 2.0, c.WHITE)

  REM Draw midground image twice
  rl.DrawTextureEx(midground, [scrollingMid, 20], 0.0, 2.0, c.WHITE)
  rl.DrawTextureEx(midground, [midground.width * 2 + scrollingMid, 20], 0.0, 2.0, c.WHITE)

  REM Draw foreground image twice
  rl.DrawTextureEx(foreground, [scrollingFore, 70], 0.0, 2.0, c.WHITE)
  rl.DrawTextureEx(foreground, [foreground.width * 2 + scrollingFore, 70], 0.0, 2.0, WHITE)

  rl.DrawText("BACKGROUND SCROLLING & PARALLAX", 10, 10, 20, c.RED)
  rl.DrawText("(c) Cyberpunk Street Environment by Luis Zuno (@ansimuz)", screenWidth - 330, screenHeight - 20, 10, c.RAYWHITE)
  rl.EndDrawing()
wend

rl.UnloadTexture(background)
rl.UnloadTexture(midground)
rl.UnloadTexture(foreground)
rl.CloseWindow()
