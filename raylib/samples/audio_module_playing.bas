REM /*******************************************************************************************
REM *
REM *   raylib [audio] example - Module playing (streaming)
REM *
REM *   This example has been created using raylib 1.5 (www.raylib.com)
REM *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM *
REM *   Copyright (c) 2016 Ramon Santamaria (@raysan5)
REM *
REM ********************************************************************************************/

import raylib as rl
import raylibc as c

MAX_CIRCLES = 64
const screenWidth = 800
const screenHeight = 450

rl.SetConfigFlags(c.FLAG_MSAA_4X_HINT)  ' NOTE: Try to enable MSAA 4X
rl.InitWindow(screenWidth, screenHeight, "raylib [audio] example - module playing (streaming)")
rl.InitAudioDevice()                    ' Initialize audio device

colors = [ c.ORANGE, c.RED, c.GOLD, c.LIME, c.BLUE, c.VIOLET, c.BROWN, c.LIGHTGRAY, c.PINK, c.YELLOW, c.GREEN, c.SKYBLUE, c.PURPLE, c.BEIGE ]

' Creates some circles for visual effect
dim circles(MAX_CIRCLES)

for i = MAX_CIRCLES to 0 step -1
  circles[i].alpha = 0.0
  circles[i].radius = rl.GetRandomValue(10, 40)
  circles[i].position.x = rl.GetRandomValue(circles[i].radius, (screenWidth - circles[i].radius))
  circles[i].position.y = rl.GetRandomValue(circles[i].radius, (screenHeight - circles[i].radius))
  circles[i].speed = rl.GetRandomValue(1, 100) / 2000
  circles[i].color = colors[rl.GetRandomValue(0, 13)]
next

music = rl.LoadMusicStream("raylib/examples/audio/resources/mini1111.xm")
music.looping = true
pitch = 1.0

rl.PlayMusicStream(music)

_timePlayed = 0
_pause = false

rl.SetTargetFPS(60)
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
  rl.UpdateMusicStream(music)      ' Update music buffer with new stream data

  ' Restart music playing (stop and play)
  if (rl.IsKeyPressed(c.KEY_SPACE)) then
    rl.StopMusicStream(music)
    rl.PlayMusicStream(music)
  endif

  ' _pause/Resume music playing
  if (rl.IsKeyPressed(c.KEY_P)) then
    _pause = !_pause
    if (_pause) then
      rl.PauseMusicStream(music)
    else
      rl.ResumeMusicStream(music)
    endif
  endif

  if (rl.IsKeyDown(c.KEY_DOWN)) then
    pitch -= 0.01
  else if (rl.IsKeyDown(c.KEY_UP)) then
    pitch += 0.01
  endif

  rl.SetMusicPitch(music, pitch)

  ' Get _timePlayed scaled to bar dimensions
  _timePlayed = rl.GetMusicTimePlayed(music) / rl.GetMusicTimeLength(music) * (screenWidth - 40)

  ' Color circles animation
  for i = MAX_CIRCLES to 0 step -1
    if (_pause) then exit for
    circles[i].alpha += circles[i].speed
    circles[i].radius += circles[i].speed * 10.0
    if (circles[i].alpha > 1.0) then circles[i].speed *= -1
    if (circles[i].alpha <= 0.0) then
      circles[i].alpha = 0.0
      circles[i].radius = rl.GetRandomValue(10, 40)
      circles[i].position.x = rl.GetRandomValue(circles[i].radius, (screenWidth - circles[i].radius))
      circles[i].position.y = rl.GetRandomValue(circles[i].radius, (screenHeight - circles[i].radius))
      circles[i].color = colors[rl.GetRandomValue(0, 13)]
      circles[i].speed = rl.GetRandomValue(1, 100) / 2000
    endif
  next

  rl.BeginDrawing()
  rl.ClearBackground(RAYWHITE)
  for i = MAX_CIRCLES to 0 step -1
    rl.DrawCircleV(circles[i].position, circles[i].radius, rl.Fade(circles[i].color, circles[i].alpha))
  next

  ' Draw time bar
  rl.DrawRectangle(20, screenHeight - 20 - 12, screenWidth - 40, 12, c.LIGHTGRAY)
  rl.DrawRectangle(20, screenHeight - 20 - 12, _timePlayed, 12, c.MAROON)
  rl.DrawRectangleLines(20, screenHeight - 20 - 12, screenWidth - 40, 12, c.GRAY)
  rl.EndDrawing()
wend

rl.UnloadMusicStream(music) ' Unload music stream buffers from RAM
rl.CloseAudioDevice()       ' Close audio device (music streaming is automatically stopped)
rl.CloseWindow()            ' Close window and OpenGL context
