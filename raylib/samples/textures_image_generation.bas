REM ----------------------------------------------------------------------------------------
REM
REM   raylib [textures] example - Procedural images generation
REM
REM   This example has been created using raylib 1.8 (www.raylib.com)
REM   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM
REM   Copyright (c) 2O17 Wilhem Barbier (@nounoursheureux)
REM
REM ----------------------------------------------------------------------------------------

import raylib as rl
import raylibc as c

NUM_TEXTURES = 7

const screenWidth = 800
const screenHeight = 450

rl.InitWindow(screenWidth, screenHeight, "SmallBASIC/raylib [textures] example - procedural images generation")

verticalGradient = rl.GenImageGradientV(screenWidth, screenHeight, c.RED, c.BLUE)
horizontalGradient = rl.GenImageGradientH(screenWidth, screenHeight, c.RED, c.BLUE)
radialGradient = rl.GenImageGradientRadial(screenWidth, screenHeight, 0.0, c.WHITE, c.BLACK)
checked = rl.GenImageChecked(screenWidth, screenHeight, 32, 32, c.RED, c.BLUE)
whiteNoise = rl.GenImageWhiteNoise(screenWidth, screenHeight, 0.5)
perlinNoise = rl.GenImagePerlinNoise(screenWidth, screenHeight, 50, 50, 4.0)
cellular = rl.GenImageCellular(screenWidth, screenHeight, 32)

dim textures(NUM_TEXTURES)
textures[0] = rl.LoadTextureFromImage(verticalGradient)
textures[1] = rl.LoadTextureFromImage(horizontalGradient)
textures[2] = rl.LoadTextureFromImage(radialGradient)
textures[3] = rl.LoadTextureFromImage(checked)
textures[4] = rl.LoadTextureFromImage(whiteNoise)
textures[5] = rl.LoadTextureFromImage(perlinNoise)
textures[6] = rl.LoadTextureFromImage(cellular)

rl.UnloadImage(verticalGradient)
rl.UnloadImage(horizontalGradient)
rl.UnloadImage(radialGradient)
rl.UnloadImage(checked)
rl.UnloadImage(whiteNoise)
rl.UnloadImage(perlinNoise)
rl.UnloadImage(cellular)

currentTexture = 0

rl.SetTargetFPS(60)
while (!rl.WindowShouldClose())
  if (rl.IsMouseButtonPressed(c.MOUSE_LEFT_BUTTON) OR rl.IsKeyPressed(c.KEY_RIGHT)) then
    currentTexture = (currentTexture + 1) % NUM_TEXTURES   ' Cycle between the textures
  endif

  rl.BeginDrawing()
  rl.ClearBackground(RAYWHITE)
  rl.DrawTexture(textures[currentTexture], 0, 0, c.WHITE)
  rl.DrawRectangle(30, 400, 325, 30, rl.Fade(c.SKYBLUE, 0.5))
  rl.DrawRectangleLines(30, 400, 325, 30, rl.Fade(c.WHITE, 0.5))
  rl.DrawText("MOUSE LEFT BUTTON to CYCLE PROCEDURAL TEXTURES", 40, 410, 10, c.WHITE)

  select case (currentTexture)
  case 0: rl.DrawText("VERTICAL GRADIENT", 560, 10, 20, c.RAYWHITE)
  case 1: rl.DrawText("HORIZONTAL GRADIENT", 540, 10, 20, c.RAYWHITE)
  case 2: rl.DrawText("RADIAL GRADIENT", 580, 10, 20, c.LIGHTGRAY)
  case 3: rl.DrawText("CHECKED", 680, 10, 20, c.RAYWHITE)
  case 4: rl.DrawText("WHITE NOISE", 640, 10, 20, c.RED)
  case 5: rl.DrawText("PERLIN NOISE", 630, 10, 20, c.RAYWHITE)
  case 6: rl.DrawText("CELLULAR", 670, 10, 20, c.RAYWHITE)
  end select
  rl.EndDrawing()
wend

REM Unload textures data (GPU VRAM)
for i = 0 to NUM_TEXTURES - 1
  rl.UnloadTexture(textures[i])
next i

rl.CloseWindow()
