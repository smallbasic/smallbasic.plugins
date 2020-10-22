import raylib
import logo

const screenWidth = 1280
const screenHeight = 720

rem Initialization (Note windowTitle is unused on Android)
raylib.InitWindow(screenWidth, screenHeight, "JUST DO [GGJ15]")

rem raylib.togglefullscreen()

rem Load global data here (assets that must be available in all screens, i.e. fonts) 
raylib.InitAudioDevice()
    
const levelWin = raylib.LoadSound("resources/win.wav")
const music = raylib.LoadMusicStream("resources/ambient.ogg")
    
rem Setup and Init first screen
currentScreen = 0

rem Set our game to run at 60 frames-per-second
raylib.SetTargetFPS(60)  

rem logo.init()

rem  Main game loop
while (!raylib.WindowShouldClose()) 
  raylib.BeginDrawing()
  raylib.ClearBackground(0)

  logo.update()
  logo.drawScreen()

  raylib.EndDrawing()
wend

rem De-Initialization
raylib.UnloadSound(levelWin)
raylib.UnloadMusicStream(music)
raylib.CloseAudioDevice()
raylib.CloseWindow()

