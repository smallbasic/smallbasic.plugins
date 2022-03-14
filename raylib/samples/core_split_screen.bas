REM /*******************************************************************************************
REM *
REM *   raylib [core] example - split screen
REM *
REM *   This example has been created using raylib 3.7 (www.raylib.com)
REM *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM *
REM *   Example contributed by Jeffery Myers (@JeffM2501) and reviewed by Ramon Santamaria (@raysan5)
REM *
REM *   Copyright (c) 2021 Jeffery Myers (@JeffM2501)
REM *
REM ********************************************************************************************/

import raylib as rl
import raylibc as c

textureGrid = {}
cameraPlayer1 = {}
cameraPlayer2 = {}
const screenWidth = 800
const screenHeight = 450

' Scene drawing
sub DrawScene()
  local count = 5
  local spacing = 4

  ' Grid of cube trees on a plane to make a "world"
  rl.DrawPlane([0, 0, 0], [50, 50], c.BEIGE) ' Simple world plane

  for x = -count * spacing to count * spacing step spacing
    for z = -count * spacing to count * spacing step spacing
      rl.DrawCubeTexture(textureGrid, [x, 1.5, z], 1, 1, 1, c.GREEN)
      rl.DrawCubeTexture(textureGrid, [x, 0.5, z], 0.25, 1, 0.25, c.BROWN)
    next
  next

  ' Draw a cube at each player's position
  rl.DrawCube(cameraPlayer1.position, 1, 1, 1, c.RED)
  rl.DrawCube(cameraPlayer2.position, 1, 1, 1, c.BLUE)
end

rl.InitWindow(screenWidth, screenHeight, "raylib [core] example - split screen")

' Generate a simple texture to use for trees
img = rl.GenImageChecked(256, 256, 32, 32, c.DARKGRAY, c.WHITE)
textureGrid = rl.LoadTextureFromImage(img)
rl.UnloadImage(img)
rl.SetTextureFilter(textureGrid, c.TEXTURE_FILTER_ANISOTROPIC_16X)
rl.SetTextureWrap(textureGrid, c.TEXTURE_WRAP_CLAMP)

' Setup player 1 camera and screen
cameraPlayer1.fovy = 45.0
cameraPlayer1.up.y = 1.0
cameraPlayer1.target.y = 1.0
cameraPlayer1.position.z = -3.0
cameraPlayer1.position.y = 1.0

screenPlayer1 = rl.LoadRenderTexture(screenWidth / 2, screenHeight)

' Setup player two camera and screen
cameraPlayer2.fovy = 45.0
cameraPlayer2.up.y = 1.0
cameraPlayer2.target.y = 3.0
cameraPlayer2.position.x = -3.0
cameraPlayer2.position.y = 3.0

screenPlayer2 = rl.LoadRenderTexture(screenWidth / 2, screenHeight)

' Build a flipped rectangle the size of the split view to use for drawing later
splitScreenRect = [0.0, 0.0, screenPlayer1.texture.width, -screenPlayer1.texture.height]

rl.SetTargetFPS(60)
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
  ' Update
  '----------------------------------------------------------------------------------
  ' If anyone moves this frame, how far will they move based on the time since the last frame
  ' this moves thigns at 10 world units per second, regardless of the actual FPS
  offsetThisFrame = 10.0 * rl.GetFrameTime()

  ' Move Player1 forward and backwards (no turning)
  if (rl.IsKeyDown(c.KEY_W)) then
    cameraPlayer1.position.z += offsetThisFrame
    cameraPlayer1.target.z += offsetThisFrame
  else if (rl.IsKeyDown(c.KEY_S)) then
    cameraPlayer1.position.z -= offsetThisFrame
    cameraPlayer1.target.z -= offsetThisFrame
  endif

  ' Move Player2 forward and backwards (no turning)
  if (rl.IsKeyDown(c.KEY_UP)) then
    cameraPlayer2.position.x += offsetThisFrame
    cameraPlayer2.target.x += offsetThisFrame
  else if (rl.IsKeyDown(c.KEY_DOWN)) then
    cameraPlayer2.position.x -= offsetThisFrame
    cameraPlayer2.target.x -= offsetThisFrame
  endif

  ' Draw
  '----------------------------------------------------------------------------------
  ' Draw Player1 view to the render texture
  rl.BeginTextureMode(screenPlayer1)
  rl.ClearBackground(c.SKYBLUE)
  rl.BeginMode3D(cameraPlayer1)
  DrawScene()
  rl.EndMode3D()
  rl.DrawText("PLAYER1 W/S to move", 10, 10, 20, c.RED)
  rl.EndTextureMode()

  ' Draw Player2 view to the render texture
  rl.BeginTextureMode(screenPlayer2)
  rl.ClearBackground(c.SKYBLUE)
  rl.BeginMode3D(cameraPlayer2)
  DrawScene()
  rl.EndMode3D()
  rl.DrawText("PLAYER2 UP/DOWN to move", 10, 10, 20, c.BLUE)
  rl.EndTextureMode()

  ' Draw both views render textures to the screen side by side
  rl.BeginDrawing()
  rl.ClearBackground(c.BLACK)
  rl.DrawTextureRec(screenPlayer1.texture, splitScreenRect, [0, 0], c.WHITE)
  rl.DrawTextureRec(screenPlayer2.texture, splitScreenRect, [screenWidth / 2.0, 0], c.WHITE)
  rl.EndDrawing()
wend

' De-Initialization
'--------------------------------------------------------------------------------------
rl.UnloadRenderTexture(screenPlayer1) ' Unload render texture
rl.UnloadRenderTexture(screenPlayer2) ' Unload render texture
rl.UnloadTexture(textureGrid)         ' Unload texture
rl.CloseWindow()                      ' Close window and OpenGL context
