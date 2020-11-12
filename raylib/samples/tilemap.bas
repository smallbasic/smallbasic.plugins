import raylib as rl
import raylibc as c

const screenWidth = 800
const screenHeight = 450

rl.InitWindow(screenWidth, screenHeight, "raylib example.")

const mymap= [
  [1, 1, 1, 1, 1],
  [1, 0, 0, 0, 1],
  [1, 1, 1, 1, 1]
]

rl.SetTargetFPS(60)

while (!rl.WindowShouldClose())
  rl.BeginDrawing()
  rl.ClearBackground(c.RAYWHITE)
  for y = 0 to 2
    for x = 0 to 4
      if (mymap[y][x] == 1) then
        rl.DrawRectangle(x * 32, y * 32, 32, 32, c.BLUE)
      endif
    next x
  next y
  rl.DrawText("Example of a minimal tilemap.", 100, 180, 40, c.LIGHTGRAY)
  rl.EndDrawing()
wend

rl.CloseWindow()
