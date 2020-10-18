import raylib

const screenWidth = 1280
const screenHeight = 720

rem Initialization (Note windowTitle is unused on Android)
raylib.InitWindow(screenWidth, screenHeight, "JUST DO [GGJ15]")

rem Load global data here (assets that must be available in all screens, i.e. fonts) 
raylib.InitAudioDevice()
    
levelWin = raylib.LoadSound("resources/win.wav")
music = raylib.LoadMusicStream("resources/ambient.ogg")
    
rem Setup and Init first screen
currentScreen = 0
raylib.InitLogoScreen()

rem Set our game to run at 60 frames-per-second
raylib.SetTargetFPS(60)  
    
rem  Main game loop
while (!raylib.WindowShouldClose()) 
  rem UpdateDrawFrame()
  raylib.BeginDrawing()
  raylib.ClearBackground(RAYWHITE)
  raylib.EndDrawing()
wend

rem De-Initialization
raylib.UnloadSound(levelWin)
raylib.UnloadMusicStream(music)
raylib.CloseAudioDevice()
raylib.CloseWindow()

