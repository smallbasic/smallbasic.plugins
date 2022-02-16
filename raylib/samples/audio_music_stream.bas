REM /*******************************************************************************************
REM *
REM *   raylib [audio] example - Music playing (streaming)
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

rl.InitWindow(screenWidth, screenHeight, "raylib [audio] example - music playing (streaming)")
rl.InitAudioDevice()              ' Initialize audio device

music = rl.LoadMusicStream("raylib/examples/audio/resources/country.mp3")

rl.PlayMusicStream(music)

timePlayed = 0.0
_pause = false

rl.SetTargetFPS(60)               ' Set our game to run at 60 frames-per-second
while (!rl.WindowShouldClose())    ' Detect window close button or ESC key
  rl.UpdateMusicStream(music)   ' Update music buffer with new stream data

  ' Restart music playing (stop and play)
  if (rl.IsKeyPressed(c.KEY_SPACE)) then
    rl.StopMusicStream(music)
    rl.PlayMusicStream(music)
  endif

  ' Pause/Resume music playing
  if (rl.IsKeyPressed(c.KEY_P)) then
    _pause = !_pause
    if (_pause) then
      rl.PauseMusicStream(music)
    else
      rl.ResumeMusicStream(music)
    endif
  endif

  ' Get timePlayed scaled to bar dimensions (400 pixels)
  timePlayed = rl.GetMusicTimePlayed(music) / rl.GetMusicTimeLength(music) * 400

  if (timePlayed > 400) then rl.StopMusicStream(music)
  rl.BeginDrawing()
  rl.ClearBackground(RAYWHITE)
  rl.DrawText("MUSIC SHOULD BE PLAYING!", 255, 150, 20, c.LIGHTGRAY)
  rl.DrawRectangle(200, 200, 400, 12, c.LIGHTGRAY)
  rl.DrawRectangle(200, 200, timePlayed, 12, c.MAROON)
  rl.DrawRectangleLines(200, 200, 400, 12, c.GRAY)
  rl.DrawText("PRESS SPACE TO RESTART MUSIC", 215, 250, 20, c.LIGHTGRAY)
  rl.DrawText("PRESS P TO PAUSE/RESUME MUSIC", 208, 280, 20, c.LIGHTGRAY)
  rl.EndDrawing()
wend

rl.UnloadMusicStream(music)   ' Unload music stream buffers from RAM
rl.CloseAudioDevice()         ' Close audio device (music streaming is automatically stopped)
rl.CloseWindow()              ' Close window and OpenGL context
