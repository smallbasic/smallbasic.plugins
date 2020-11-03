REM ----------------------------------------------------------------------------------------
REM
REM   raylib [shaders] example - Apply a postprocessing shader to a scene
REM
REM   NOTE: This example requires raylib OpenGL 3.3 or ES2 versions for shaders support,
REM         OpenGL 1.1 does not support shaders, recompile raylib to OpenGL 3.3 version.
REM
REM   NOTE: Shaders used in this example are #version 330 (OpenGL 3.3), to test this example
REM         on OpenGL ES 2.0 platforms (Android, Raspberry Pi, HTML5), use #version 100 shaders
REM         raylib comes with shaders ready for both versions, check raylib/shaders install folder
REM
REM   This example has been created using raylib 1.7 (www.raylib.com)
REM   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM
REM   Copyright (c) 2017 Ramon Santamaria (@raysan5)
REM
REM ----------------------------------------------------------------------------------------

import raylib as rl
import raylibc as c

const MAX_POSTPRO_SHADERS = 12
const FX_GRAYSCALE = 0
const FX_POSTERIZATION = 1
const FX_DREAM_VISION = 2
const FX_PIXELIZER = 3
const FX_CROSS_HATCHING = 4
const FX_CROSS_STITCHING = 5
const FX_PREDATOR_VIEW = 6
const FX_SCANLINES = 7
const FX_FISHEYE = 8
const FX_SOBEL = 9
const FX_BLOOM = 10
const FX_BLUR = 11

const postproShaderText = [
  "GRAYSCALE",
  "POSTERIZATION",
  "DREAM_VISION",
  "PIXELIZER",
  "CROSS_HATCHING",
  "CROSS_STITCHING",
  "PREDATOR_VIEW",
  "SCANLINES",
  "FISHEYE",
  "SOBEL",
  "BLOOM",
  "BLUR",
]

const screenWidth = 800
const screenHeight = 450

rl.SetConfigFlags(c.FLAG_MSAA_4X_HINT)      ' Enable Multi Sampling Anti Aliasing 4x (if available)
rl.InitWindow(screenWidth, screenHeight, "raylib [shaders] example - postprocessing shaders")

const resources = CWD + "raylib/examples/shaders/resources/"
const camera = [[2.0, 3.0, 2.0], [0.0, 1.0, 0.0], [0.0, 1.0, 0.0], 45.0, 0]
const dwarf = rl.LoadModel(resources + "models/church.obj")              ' Load OBJ model
const texture = rl.LoadTexture(resources + "models/church_diffuse.png")  ' Load model texture (diffuse map)
const position = [0.0, 0.0, 0.0]                                         ' Set model position

rl.SetModelDiffuseTexture(dwarf, texture)                                ' Set dwarf model diffuse texture

REM  Load all postpro shaders
REM  NOTE 1: All postpro shader use the base vertex shader
REM  NOTE 2: We load the correct shader depending on GLSL version
dim shaders(12)

shaders[FX_GRAYSCALE] = rl.LoadShader(resources + "shaders/glsl330/base.vs", resources + "shaders/glsl330/grayscale.fs")
shaders[FX_POSTERIZATION] = rl.LoadShader(resources + "shaders/glsl330/base.vs", resources + "shaders/glsl330/posterization.fs")
shaders[FX_DREAM_VISION] = rl.LoadShader(resources + "shaders/glsl330/base.vs", resources + "shaders/glsl330/dream_vision.fs")
shaders[FX_PIXELIZER] = rl.LoadShader(resources + "shaders/glsl330/base.vs", resources + "shaders/glsl330/pixelizer.fs")
shaders[FX_CROSS_HATCHING] = rl.LoadShader(resources + "shaders/glsl330/base.vs", resources + "shaders/glsl330/cross_hatching.fs")
shaders[FX_CROSS_STITCHING] = rl.LoadShader(resources + "shaders/glsl330/base.vs", resources + "shaders/glsl330/cross_stitching.fs")
shaders[FX_PREDATOR_VIEW] = rl.LoadShader(resources + "shaders/glsl330/base.vs", resources + "shaders/glsl330/predator.fs")
shaders[FX_SCANLINES] = rl.LoadShader(resources + "shaders/glsl330/base.vs", resources + "shaders/glsl330/scanlines.fs")
shaders[FX_FISHEYE] = rl.LoadShader(resources + "shaders/glsl330/base.vs", resources + "shaders/glsl330/fisheye.fs")
shaders[FX_SOBEL] = rl.LoadShader(resources + "shaders/glsl330/base.vs", resources + "shaders/glsl330/sobel.fs")
shaders[FX_BLOOM] = rl.LoadShader(resources + "shaders/glsl330/base.vs", resources + "shaders/glsl330/bloom.fs")
shaders[FX_BLUR] = rl.LoadShader(resources + "shaders/glsl330/base.vs", resources + "shaders/glsl330/blur.fs")

currentShader = FX_GRAYSCALE

REM  Create a RenderTexture2D to be used for render to texture
const target = rl.LoadRenderTexture(screenWidth, screenHeight)

rl.SetCameraMode(camera, c.CAMERA_ORBITAL)  ' Set an orbital camera mode
rl.SetTargetFPS(60)                        ' Set our game to run at 60 frames-per-second

REM  Main game loop
while (!rl.WindowShouldClose())
  rl.UpdateCamera(camera)           ' Update camera

  if (rl.IsKeyPressed(c.KEY_RIGHT)) then 
    currentShader++
  elseif (rl.IsKeyPressed(c.KEY_LEFT)) then 
    currentShader--
  endif

  if (currentShader >= MAX_POSTPRO_SHADERS) then 
    currentShader = FX_GRAYSCALE
  elseif (currentShader == -1) then 
    currentShader = FX_BLUR
  endif

  rl.BeginDrawing()
  rl.ClearBackground(c.RAYWHITE)
  rl.BeginTextureMode(target)                   ' Enable drawing to texture
  rl.BeginMode3D(camera)
  rl.DrawModel(dwarf, position, 2.0, c.WHITE)   ' Draw 3d model with texture
  rl.DrawGrid(10, 1.0)                          ' Draw a grid
  rl.EndMode3D()
  rl.EndTextureMode()                           ' End drawing to texture (now we have a texture available for next passes)

  rl.BeginShaderMode(shaders[currentShader])
  ' NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
  rl.DrawTextureRec(target.texture, [0, 0, target.texture.width, -target.texture.height], [0, 0], c.WHITE)
  rl.EndShaderMode()

  rl.DrawRectangle(0, 9, 580, 30, rl.Fade(c.LIGHTGRAY, 0.7))
  rl.DrawText("(c) Dwarf 3D model by David Moreno", screenWidth - 200, screenHeight - 20, 10, c.DARKGRAY)
  rl.DrawText("CURRENT POSTPRO SHADER:", 10, 15, 20, c.BLACK)
  rl.DrawText(postproShaderText[currentShader], 330, 15, 20, c.RED)
  rl.DrawText("< >", 540, 10, 30, c.DARKBLUE)
  rl.DrawFPS(700, 15)
  rl.EndDrawing()
wend

REM  Unload all postpro shaders
for i = 0 to MAX_POSTPRO_SHADERS - 1
  rl.UnloadShader(shaders[i])
next i

rl.UnloadTexture(texture)         ' Unload texture
rl.UnloadModel(dwarf)             ' Unload model
rl.UnloadRenderTexture(target)    ' Unload render texture
rl.CloseWindow()                  ' Close window and OpenGL context
