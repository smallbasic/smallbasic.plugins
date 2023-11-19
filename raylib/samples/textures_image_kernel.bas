REM /*******************************************************************************************
REM *
REM *   raylib [textures] example - Image loading and texture creation
REM *
REM *   NOTE: Images are loaded in CPU memory (RAM); textures are loaded in GPU memory (VRAM)
REM *
REM *   Example originally created with raylib 1.3, last time updated with raylib 1.3
REM *
REM *   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
REM *   BSD-like license that allows static linking with closed source software
REM *
REM *   Copyright (c) 2015-2023 Karim Salem (@kimo-s)
REM *
REM ********************************************************************************************/

import raylib as rl
import raylibc as c

sub normalizeKernel(byref kernel, size)
  local _sum = 0
  local i
  for i = 0 to size - 1
    _sum += kernel[i]
  next

  if (_sum != 0) then
    for i = 0 to size - 1
      kernel[i] /= _sum
    next
  endif
end

const resources = CWD + "raylib/examples/textures/resources/"
const _image = rl.LoadImage(resources + "cat.png")
const screenWidth = 800
const screenHeight = 450

rl.InitWindow(screenWidth, screenHeight, "raylib [textures] example - image convolution")

gaussiankernel = [1.0, 2.0, 1.0, 2.0, 4.0, 2.0, 1.0, 2.0, 1.0]
sobelkernel = [1.0, 0.0, -1.0, 2.0, 0.0, -2.0,1.0, 0.0, -1.0]
sharpenkernel = [0.0, -1.0, 0.0, -1.0, 5.0, -1.0, 0.0, -1.0, 0.0]

normalizeKernel(gaussiankernel, 9)
normalizeKernel(sharpenkernel, 9)
normalizeKernel(sobelkernel, 9)

catSharpend = rl.ImageCopy(_image)
rl.ImageKernelConvolution(catSharpend, sharpenkernel)

catSobel = rl.ImageCopy(_image)
rl.ImageKernelConvolution(catSobel, sobelkernel)

catGaussian = rl.ImageCopy(_image)
for i = 0 to 6
  rl.ImageKernelConvolution(catGaussian, gaussiankernel)
next

rl.ImageCrop(_image, [0, 0, 200, 450])
rl.ImageCrop(catGaussian, [0, 0, 200, 450])
rl.ImageCrop(catSobel, [0, 0, 200, 450])
rl.ImageCrop(catSharpend, [0, 0, 200, 450])
texture = rl.LoadTextureFromImage(_image)          ' Image converted to texture, GPU memory (VRAM)
catSharpendTexture = rl.LoadTextureFromImage(catSharpend)
catSobelTexture = rl.LoadTextureFromImage(catSobel)
catGaussianTexture = rl.LoadTextureFromImage(catGaussian)

rl.UnloadImage(_image)   ' Once image has been converted to texture and uploaded to VRAM, it can be unloaded from RAM
rl.UnloadImage(catGaussian)
rl.UnloadImage(catSobel)
rl.UnloadImage(catSharpend)

rl.SetTargetFPS(60)
while (!rl.WindowShouldClose())
  rl.BeginDrawing()
  rl.ClearBackground(c.RAYWHITE)
  rl.DrawTexture(catSharpendTexture, 0, 0, c.WHITE)
  rl.DrawText("Sharpend", 0, 0, 20, c.RED)
  rl.DrawTexture(catSobelTexture, 200, 0, c.WHITE)
  rl.DrawText("Sobel", 200, 0, 20, c.RED)
  rl.DrawTexture(catGaussianTexture, 400, 0, c.WHITE)
  rl.DrawText("Gaussian", 400, 0, 20, c.RED)
  rl.DrawTexture(texture, 600, 0, c.WHITE)
  rl.DrawText("Original", 600, 0, 20, c.RED)
  rl.EndDrawing()
wend

rl.UnloadTexture(texture)
rl.UnloadTexture(catGaussianTexture)
rl.UnloadTexture(catSobelTexture)
rl.UnloadTexture(catSharpendTexture)
rl.CloseWindow()

