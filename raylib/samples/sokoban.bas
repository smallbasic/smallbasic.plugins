import raylib as rl
import raylibc as c
'import debug

const screenWidth = 900
const screenHeight = screenWidth * .75
const cubeSize = 1
const gridSize = 1

camera = {}
camera.position = [0, 20, 6]
camera.target = [0, 0, 0]
camera.up = [0, .1, 0]
camera.fovy = 60
camera.projection = c.CAMERA_PERSPECTIVE

rl.InitWindow(screenWidth, screenHeight,  "Sokoban")
rl.SetWindowPosition(965, 1200)
rl.SetTargetFPS(20)
rl.SetCameraMode(camera, c.CAMERA_FREE)
'run("xdotool windowactivate `xdotool search --onlyvisible --name \"Emacs\"`")

sub main
  local games = load_levels("sokoban.levels")
  local gameIndex = 0
  local game = create_game(games[gameIndex])
  local options = ""
  local i, delim

  for i = 1 to min(20, len(games))
    options += delim + i
    delim = ";"
  next

' while (!rl.WindowShouldClose() && !debug.IsSourceModified())
  while (!rl.WindowShouldClose())
    update_move(game)

    rl.UpdateCamera(camera)
    rl.BeginDrawing()
    rl.ClearBackground(c.WHITE)
    rl.BeginMode3D(camera)

    rl.DrawGrid(100, gridSize)
    draw_game(game)
    rl.EndMode3D()

    draw_status(game)

    if (rl.GuiDropDownBox([110, 10, 60, 20], options, cbx, is_open))
      if (cbx != gameIndex) then
        gameIndex = cbx
        game = create_game(games[gameIndex])
      endif
      is_open = !is_open
    endif

    rl.DrawFPS(10, 10)
    rl.EndDrawing()
  wend

  rl.CloseWindow()
end

'
' create the game board
'
func create_game(grid)
  local row, col, row_len, x, y, ch, game
  local blocks = []
  local borders = []
  local target = []
  local width = 0

  y = 0
  for row in grid
    row_len = len(row)
    width = max(width, row_len)
    for x = 1 to row_len
      ch = mid(row, x, 1)
      select case ch
      case "#"' ' border
        borders << [x, y]
      case "@" ' sokoban man
        game.soko_x = x
        game.soko_y = y
      case "." ' block target
        target << [x, y]
      case "$" ' moveable block
        blocks << [x, y]
      end select
    next i
    y++
  next row

  game.width = width / 2
  game.height = y / 2
  game.blocks = blocks
  game.borders = borders
  game.target = target
  game.grid = grid
  game.undo_top = 0
  game.game_over = false
  game.undo = []
  return game
end

func get_position(byref game, byref pt)
  local x, y
  [x, y] = pt
  x = x - game.width
  y = y - game.height
  return [x, .5, y]
end

'
' draw the game boundary
'
sub draw_wall(byref game, byref pt)
  rl.DrawCube(get_position(game, pt), cubeSize, cubeSize, cubeSize, c.GRAY)
end

'
' draw the block target
'
sub draw_target(byref game, byref pt)
  local position = get_position(game, pt)
  rl.DrawCube(position, 1, -.4, 1, c.YELLOW)
  rl.DrawCubeWires(position, 1, -.4, 1, c.BLACK)
end

'
' draw the movable block
'
sub draw_block(byref game, byref pt)
  rl.DrawCube(get_position(game, pt), cubeSize, cubeSize, cubeSize, c.GREEN)
end

'
' draw the sokoban man
'
sub draw_soko(byref game, byref pt)
  rl.DrawCube(get_position(game, pt), cubeSize, cubeSize, cubeSize, c.PURPLE)
end

'
' show the game status
'
sub draw_status(byref game)
  ' count the number of blocks over the targts
  local bl_len = len(game.blocks) - 1
  local num_over = 0

  for i = 0 to bl_len
    local block = game.blocks(i)
    local x = block(0)
    local y = block(1)
    if (mid(game.grid(y), x, 1) == ".") then
      num_over++
    endif
  next i

  if (num_over == bl_len + 1) then
    game.game_over = true
    rl.DrawText("Game OVER!", 340, 10, 20, c.RED)
  endif

  rl.DrawText(rl.textFormat("Moves: %d Pushes: %d", game.moves, game.pushes), 640, 10, 20, c.RED)
end

sub draw_game(byref game)
  local p
  for p in game.borders
    draw_wall(game, p)
  next
  for p in game.target
    draw_target(game, p)
  next
  for p in game.blocks
    draw_block(game, p)
  next
  p = [game.soko_x, game.soko_y]
  draw_soko(game, p)
end

'
' return whether the x/y location touches the border
'
func is_border(byref grid, x, y)
  return mid(grid(y), x, 1) == "#"
end

'
' loads any sokoban games found in filename
' for more games see: http://www.sourcecode.se/sokoban/levels.php
'
func load_levels(filename)
  tload filename, buffer
  local blockFound = false
  local buffLen = len(buffer) - 1
  local nextLine, i, firstChar, games, nextGame

  dim nextGame
  local games=[]

  for i = 0 to buffLen
    nextLine = buffer(i)
    firstChar = left(trim(nextLine), 1)
    if (firstChar == "#") then
      ' now visiting a game block
      blockFound = true
    else if firstChar == ";" then
      ' now visiting a comment
      if blockFound then
        ' store the previously visited game block
        games << nextGame
      endif
      blockFound = false
      if (len(nextLine) > 2) then
        ' comment names the next game
        erase nextGame
      endif
    endif
    if (blockFound) then
      ' append to the next game block
      nextGame << nextLine
    endif
  next i

  if blockFound then
    ' store the last game block
    games << nextGame
  endif

  return games
end

'
' move the block covered by soko
'
sub move_block(byref game, xdir, ydir, x, y)
  local i
  local bl_len = len(game.blocks) - 1

  for i = 0 to bl_len
    local block = game.blocks(i)
    if (block(0) = x && block(1) = y) then
      block(0) = block(0) + xdir
      block(1) = block(1) + ydir
      game.blocks(i) = block
      i = bl_len
    endif
  next i
end

'
' move up
'
sub move_up(byref game, is_push)
  game.soko_y--
  game.moves++
  if is_push then
    move_block game, 0, -1, game.soko_x, game.soko_y
    game.pushes++
  endif
end

'
' move down
'
sub move_down(byref game, is_push)
  game.soko_y++
  game.moves++
  if is_push then
    move_block game, 0, 1, game.soko_x, game.soko_y
    game.pushes++
  endif
end

'
' move left
'
sub move_left(byref game, is_push)
  game.soko_x--
  game.moves++
  if is_push then
    move_block game, -1, 0, game.soko_x, game.soko_y
    game.pushes++
  endif
end

'
' move right
'
sub move_right(byref game, is_push)
  game.soko_x++
  game.moves++
  if is_push then
    move_block game, 1, 0, game.soko_x, game.soko_y
    game.pushes++
  endif
end

'
' whether there is a block at the x/y location
'
func get_block(byref blocks, x, y)
  local i
  local result = false
  local bl_len = len(game.blocks) - 1

  for i = 0 to bl_len
    local block = game.blocks(i)
    if (block(0) = x && block(1) = y) then
      result = true
      i = bl_len
    endif
  next i

  return result
end

'
' play the game
'
sub update_move(byref game)
  if (rl.isKeyPressed(c.KEY_LEFT)) then
    if (is_border(game.grid, game.soko_x-1, game.soko_y) = false) then
      if (get_block(game.blocks, game.soko_x-1, game.soko_y) = false) then
        move_left game, false
        undo_push game, "R", 0, 0
      elseif (is_border(game.grid, game.soko_x-2, game.soko_y) = false) then
        if (get_block(game.blocks, game.soko_x-2, game.soko_y) = false) then
          move_left game, true
          undo_push game, "R", game.soko_x-1, game.soko_y
        endif
      endif
    endif
  elseif (rl.isKeyPressed(c.KEY_RIGHT)) then
    if (is_border(game.grid, game.soko_x+1, game.soko_y) = false) then
      if (get_block(game.blocks, game.soko_x+1, game.soko_y) = false) then
        move_right game, false
        undo_push game, "L", 0, 0
      elseif (is_border(game.grid, game.soko_x+2, game.soko_y) = false) then
        if (get_block(game.blocks, game.soko_x+2, game.soko_y) = false) then
          move_right game, true
          undo_push game, "L", game.soko_x+1, game.soko_y
        endif
      endif
    endif
  elseif (rl.isKeyPressed(c.KEY_UP)) then
    if (is_border(game.grid, game.soko_x, game.soko_y-1) = false) then
      if (get_block(game.blocks, game.soko_x, game.soko_y-1) = false) then
        move_up game, false
        undo_push game, "D", 0, 0
      elseif (is_border(game.grid, game.soko_x, game.soko_y-2) = false) then
        if (get_block(game.blocks, game.soko_x, game.soko_y-2) = false) then
          move_up game, true
          undo_push game, "D", game.soko_x, game.soko_y-1
        endif
      endif
    endif
  elseif (rl.isKeyPressed(c.KEY_DOWN)) then
    if (is_border(game.grid, game.soko_x, game.soko_y+1) = false) then
      if (get_block(game.blocks, game.soko_x, game.soko_y+1) = false) then
        move_down game, false
        undo_push game, "U", 0, 0
      elseif (is_border(game.grid, game.soko_x, game.soko_y+2) = false) then
        if (get_block(game.blocks, game.soko_x, game.soko_y+2) = false) then
          move_down game, true
          undo_push game, "U", game.soko_x, game.soko_y+1
        endif
      endif
    endif
  elseif (rl.isKeyPressed(c.KEY_U)) then
    undo game
  elseif (rl.isKeyPressed(c.KEY_E)) then
    game.game_over = true
  elseif (rl.isKeyPressed(c.KEY_P)) then
    logprint camera
  elseif (rl.isKeyPressed(c.KEY_A)) then
     camera.position[2]+=.5
  elseif (rl.isKeyPressed(c.KEY_S)) then
    camera.position[2]-=.5
  endif
end

'
' undo the last move
'
sub undo(byref game)
  local top = len(game.undo) - 1
  if (top != -1) then
    local undo_el = game.undo(top)
    local soko_dir = undo_el.soko_dir
    local block_x = undo_el.block_x
    local block_y = undo_el.block_y

    delete game.undo, top

    select case soko_dir
    case "U": move_up game, false
    case "D": move_down game, false
    case "L": move_left game, false
    case "R": move_right game, false
    end select

    if (block_x != 0 && block_y != 0) then
      ' move to the previous position
      select case soko_dir
      case "U": move_block game, 0, -1, block_x, block_y
      case "D": move_block game, 0,  1, block_x, block_y
      case "L": move_block game, -1, 0, block_x, block_y
      case "R": move_block game, 1,  0, block_x, block_y
      end select
    endif
  endif
end

'
' add an element to the undo stack
'
sub undo_push(byref game, soko_dir, block_x, block_y)
  local undo_el
  undo_el.soko_dir = soko_dir
  undo_el.block_x = block_x
  undo_el.block_y = block_y
  game.undo << undo_el
end

'
' program entry point
'
main
