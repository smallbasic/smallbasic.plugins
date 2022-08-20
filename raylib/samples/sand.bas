'
' Falling sand!
'
' https://news.ycombinator.com/item?id=31309616
' https://jason.today/falling-sand
'

import raylib as rl
import raylibc as c

randomize timer

const screenWidth = 800
const screenHeight = 450
const width = 40
const height = 40
const maxLife = 125

' hue, saturation, lightness
const SAND_COLOR = [0.103, 0.595, 0.883]
const BACKGROUND = [10, 50, 80]

func grid_init(width, height)
  result = {}
  result.width = width
  result.height = height
  result.grid = []

  dim result.grid(width, height)
  dim result.life(width, height)
  
  return result
end

sub grid_clear(byref grid)
  dim grid.grid(width, height)
end

sub grid_set(byref grid, y, x, col)
  grid.grid[y, x] = col
end

sub grid_swap(byref grid, a, b)
  swap grid.grid[a[0], a[1]], grid.grid[b[0], b[1]]
end

func grid_is_empty(byref grid, pixel)
  local y, x
  [y, x] = pixel
  return iff(x < grid.width and y < grid.height and grid.grid[y, x] == 0, true, false)
end

sub grid_draw(byref grid)
  local x, y, x1, y1
  local w = screenWidth / grid.width
  local h = screenHeight / grid.height
  local grow = rnd * 1000 % 3

  rl.BeginDrawing()
  rl.ClearBackground(BACKGROUND)
  
  for y = 0 to grid.height
    for x = 0 to grid.width
      if (grid.grid[y, x] != 0) then
        x1 = x * w
        y1 = y * h
        rl.DrawCircle(x1, y1, iff(rnd > 0.5, w - grow, w + grow), grid.grid[y, x])
        grid.life[y, x]++
        if (grid.life[y, x] > maxLife) then 
          grid.grid[y, x] = 0
          grid.life[y, x] = 0
        endif
      endif
    next
  next


  rl.DrawFPS(10, 10)
  rl.EndDrawing()
end

sub grid_update_pixel(byref grid, pixel)
  local y, x: [y, x] = pixel
  local below = [y + 1, x]
  local belowLeft = [y + 1, x - 1]
  local belowRight = [y + 1, x + 1]

  if rnd > 0.5 then 
    swap belowLeft, belowRight
  endif
  
  ' If there are no pixels below, move it down.
  if (grid_is_empty(grid, below)) then
    grid_swap(grid, below, pixel)
  else if (grid_is_empty(grid, belowLeft)) then
    grid_swap(grid, belowLeft, pixel)
  else if (grid_is_empty(grid, belowRight)) then    
    grid_swap(grid, belowRight, pixel)
  endif
end

'
' go through each pixel one by one and apply the rule
'
sub grid_update(byref grid)
  local y, x, 
  for y = grid.height - 2 to 0 step - 1
    for x = grid.width - 2 to 0 step - 1
      grid_update_pixel(grid, [y, x])
    next
  next
end

' https://stackoverflow.com/questions/2353211/hsl-to-rgb-color-conversion
'
func hslToRgb(h, s, l)
  local r, g, b, q, p

  func hue2rgb(m1, m2, h)
    local result
    if (h < 0) then h += 1.0
    if (h > 1) then h -= 1.0
    if (h < 0.1) then 
      result = m1 + (m2 - m1) * 6.0 * h
    else if (h < 0.5) then 
      result = m2
    else if (h < 0.6) then 
      result = m1 + (m2 - m1) * (0.6 - h) * 6.0
    else
      result = m1
    endif
    return result
  end

  if (s == 0) then
    r = l
    g = l
    b = l
  else
    q = iff(l < 0.5, (l * (1 + s)), (l + s - l * s))
    p = 2.0 * l - q
    r = hue2rgb(p, q, h + 0.3)
    g = hue2rgb(p, q, h)
    b = hue2rgb(p, q, h - 0.3)
  endif

  return rgbf(r, g, b)
end

func random(low, high)
  local result = (rnd * 10000)
  return low + (result % abs(high - low))
end

'
' Constrains a value between a minimum and maximum value.
'
func constrain(n, low, high)
  return iff(n < low, low, iff(n > high, high, n))
end

func vary_color(c)
  local saturation, lightness
 
  saturation = c[1] * 100 + random(-20, 0)
  saturation = constrain(saturation, 0, 100)
  lightness = c[2] * 100 + random(-10, 10)
  lightness = constrain(lightness, 0, 100)
  
  return hslToRgb(c[0], saturation / 100, lightness / 100)
end

sub main
  local x, y, pos
  
  rl.InitWindow(screenWidth, screenHeight, "Sand!")
  rl.SetTargetFPS(60)
  
  grid = grid_init(width, height)
 
  sub update
    grid_update(grid)
    grid_draw(grid)
  end
  
  while (!rl.WindowShouldClose())
    lives = 0
    if rl.IsMouseButtonDown(c.MOUSE_BUTTON_LEFT) then
      pos = rl.GetMousePosition()
      if (pos.x < screenWidth and pos.x > 0) then
        x = int(pos.x * grid.width / screenWidth)
        y = int(pos.y * grid.height / screenHeight)
        grid_set(grid, y, x, vary_color(SAND_COLOR))
      endif
      update()
    else 
      update()
    endif  
  wend
  
  rl.CloseWindow() 
end

main
