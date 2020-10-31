REM ----------------------------------------------------------------------------------------
REM 
REM   raylib [textures] example - Image processing
REM 
REM   NOTE: Images are loaded in CPU memory (RAM) textures are loaded in GPU memory (VRAM)
REM 
REM   This example has been created using raylib 1.7 (www.raylib.com)
REM   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM 
REM   Copyright (c) 2017 Ramon Santamaria (@raysan5)
REM 
REM ----------------------------------------------------------------------------------------

import raylib as rl
import raylibc as c

const NUM_PROCESSES = 8
const COLOR_NONE = 0
const COLOR_GRAYSCALE = 1
const COLOR_TINT = 2
const COLOR_INVERT = 3
const COLOR_CONTRAST = 4
const COLOR_BRIGHTNESS = 5
const FLIP_VERTICAL = 6
const FLIP_HORIZONTAL = 7
const processText = ["NO PROCESSING", "COLOR GRAYSCALE","COLOR TINT", "COLOR INVERT", &
                     "COLOR CONTRAST", "COLOR BRIGHTNESS", "FLIP VERTICAL", "FLIP HORIZONTAL"]
const screenWidth = 800
const screenHeight = 450

rl.InitWindow(screenWidth, screenHeight, "raylib [textures] example - image processing")
REM NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)

img = rl.LoadImage(CWD + "raylib/examples/textures/resources/parrots.png") ' Loaded in CPU memory (RAM)
rl.ImageFormat(img, c.UNCOMPRESSED_R8G8B8A8)    ' Format image to RGBA 32bit (required for texture update)
texture = rl.LoadTextureFromImage(img)               ' Image converted to texture, GPU memory (VRAM)
currentProcess = COLOR_NONE
textureReload = false
dim selectRecs(NUM_PROCESSES)

for i = 0 to NUM_PROCESSES - 1
  selectRecs[i] = [40, 50 + 32 * i, 150, 30]
next i

rl.SetTargetFPS(60)

REM Main game loop
while (!rl.WindowShouldClose())
  if (rl.IsKeyPressed(c.KEY_DOWN)) then
    currentProcess++
    if (currentProcess == NUM_PROCESSES) then 
      currentProcess = 0
    endif
    textureReload = true
  elseif (rl.IsKeyPressed(c.KEY_UP)) then
    currentProcess--
    if (currentProcess < 0) then currentProcess = NUM_PROCESSES - 1
    textureReload = true
  endif

  if (textureReload) then
    rl.UnloadImage(img)
    img = rl.LoadImage(CWD + "raylib/examples/textures/resources/parrots.png") ' Re-load image data
    
    rem NOTE: Image processing is a costly CPU process to be done every frame,
    rem If image processing is required in a frame-basis, it should be done
    rem with a texture and by shaders
    select case currentProcess 
    case COLOR_GRAYSCALE
      rl.ImageColorGrayscale(img)
    case COLOR_TINT
      rl.ImageColorTint(img, c.GREEN)
    case COLOR_INVERT
      rl.ImageColorInvert(img)
    case COLOR_CONTRAST
      rl.ImageColorContrast(img, -40)
    case COLOR_BRIGHTNESS
      rl.ImageColorBrightness(img, -80)
    case FLIP_VERTICAL
      rl.ImageFlipVertical(img)
    case FLIP_HORIZONTAL 
      rl.ImageFlipHorizontal(img)
    end select

    if (currentProcess  > 0) then 
      pixels = rl.GetImageData(img)
      rl.UpdateTexture(texture, pixels)
    endif
    textureReload = false
  endif

  rl.BeginDrawing()
  rl.ClearBackground(c.RAYWHITE)
  rl.DrawText("IMAGE PROCESSING:", 40, 30, 10, c.DARKGRAY)

  for i = 0 to NUM_PROCESSES - 1
    if (i == currentProcess) then
      rl.DrawRectangleRec(selectRecs[i], c.SKYBLUE)
      rl.DrawRectangleLines(selectRecs[i][0], selectRecs[i][1], selectRecs[i][2], selectRecs[i][3], c.BLUE)
      rl.DrawText(processText[i], selectRecs[i][0] + selectRecs[i][2]/2 - rl.MeasureText(processText[i], 10)/2, selectRecs[i][1] + 11, 10, c.DARKBLUE)
    else
      rl.DrawRectangleRec(selectRecs[i], c.LIGHTGRAY)
      rl.DrawRectangleLines(selectRecs[i][0], selectRecs[i][1], selectRecs[i][2], selectRecs[i][3], c.GRAY)
      rl.DrawText(processText[i], selectRecs[i][0] + selectRecs[i][2]/2 - rl.MeasureText(processText[i], 10)/2, selectRecs[i][1] + 11, 10, c.DARKGRAY)
    endif
  next i
  rl.DrawTexture(texture, screenWidth - texture.width - 60, screenHeight/2 - texture.height/2, c.WHITE)
  rl.DrawRectangleLines(screenWidth - texture.width - 60, screenHeight/2 - texture.height/2, texture.width, texture.height, c.BLACK)
  rl.EndDrawing()
wend

rl.UnloadTexture(texture)       ' Unload texture from VRAM
rl.UnloadImage(img)             ' Unload image from RAM
rl.CloseWindow()                ' Close window and OpenGL context

