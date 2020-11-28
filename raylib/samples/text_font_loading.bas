REM /*******************************************************************************************
REM *
REM *   raylib [text] example - Font loading
REM *
REM *   raylib can load fonts from multiple file formats:
REM *
REM *     - TTF/OTF > Sprite font atlas is generated on loading, user can configure
REM *                 some of the generation parameters (size, characters to include)
REM *     - BMFonts > Angel code font fileformat, sprite font image must be provided
REM *                 together with the .fnt file, font generation cna not be configured
REM *     - XNA Spritefont > Sprite font image, following XNA Spritefont conventions,
REM *                 Characters in image must follow some spacing and order rules
REM *
REM *   This example has been created using raylib 2.6 (www.raylib.com)
REM *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
REM *
REM *   Copyright (c) 2016-2019 Ramon Santamaria (@raysan5)
REM *
REM ********************************************************************************************/

import raylib as rl
import raylibc as c

const screenWidth = 800 
const screenHeight = 450 

rl.InitWindow(screenWidth, screenHeight, "SmallBASIC raylib [text] example - font loading") 

' Define characters to draw
' NOTE: raylib supports UTF-8 encoding, following list is actually codified as UTF8 internally
msg = "!\"#$%&'()*+,-./0123456789: <=>?@ABCDEFGHI\nJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmn\nopqrstuvwxyz{|}~¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓ\nÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷\nøùúûüýþÿ"

' NOTE: Textures/Fonts MUST be loaded after Window initialization (OpenGL context is required)

' BMFont (AngelCode) : Font data and image atlas have been generated using external program
const resources = CWD + "raylib/examples/text/resources/"
fontBm = rl.LoadFont(resources + "pixantiqua.fnt") 

' TTF font : Font data and atlas are generated directly from TTF
' NOTE: We define a font base size of 32 pixels tall and up-to 250 characters
fontTtf = rl.LoadFontEx(resources + "pixantiqua.ttf", 32, 0, 250) 

useTtf = false 
rl.SetTargetFPS(60)                ' Set our game to run at 60 frames-per-second
while (!rl.WindowShouldClose())
  if (rl.IsKeyDown(c.KEY_SPACE)) then'
    useTtf = true 
  else 
    useTtf = false 
  endif
  
  rl.BeginDrawing() 
  rl.ClearBackground(c.RAYWHITE) 
  rl.DrawText("Hold SPACE to use TTF generated font", 20, 20, 20, c.LIGHTGRAY) 
  if (!useTtf) then
    rl.DrawTextEx(fontBm, msg, [20.0, 100.0], fontBm.baseSize, 2, c.MAROON) 
    rl.DrawText("Using BMFont (Angelcode) imported", 20, rl.GetScreenHeight() - 30, 20, c.GRAY) 
  else
    rl.DrawTextEx(fontTtf, msg, [20.0, 100.0], fontTtf.baseSize, 2, c.LIME) 
    rl.DrawText("Using TTF font generated", 20, rl.GetScreenHeight() - 30, 20, c.GRAY) 
  endif
  rl.EndDrawing() 
wend

rl.UnloadFont(fontBm)      ' AngelCode Font unloading
rl.UnloadFont(fontTtf)     ' TTF Font unloading
rl.CloseWindow()           ' Close window and OpenGL context
