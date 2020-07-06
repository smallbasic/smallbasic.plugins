import glfw

# Initialize the library
if not glfw.init() then
  throw "glfw init failed"
endif

# Create a windowed mode window and its OpenGL context
wnd = glfw.create_window(640, 480, "SmallBASIC - GLFW")
if not wnd then
  glfw.terminate()
  throw "glfw create window failed"
endif

maxfps = 0

sub render
  r += 1
  if r >= 256 then r = 0
  if r >= 15 then r = 0
  g += 2
  if g >= 256 then g = 0
  b += 3
  if b >= 256 then b = 0
  color 8, rgb(r, g, b)

  cls
  glfw.line_width(lw)
  lw = iff(lw == 20, 1, lw+1)

  pset 10,10 color 7
  pset 11,11 color 7
  pset 12,12 color 7
  line 20,40,140,40 color r
  line 20,80,140,80 color r
  line 60,0,60,120 color r
  line 100,0,100,120 color r

  rect 100, 100, 200, 200 color r
  rect 130, 130, 230, 230 color r filled
end

# Loop until the user closes the window
while not glfw.window_should_close(wnd)
  t=TICKS
  #  key = glfw.key_event();

  render()

  # Swap front and back buffers
  glfw.swap_buffers(wnd)

  # Poll for and process events
  glfw.poll_events()

  t2 = (ticks-t)
  if (t2 > 0) then
    fps=1000/t2
    if (fps>maxfps) then
      maxfps = fps
    endif
  endif
wend

glfw.terminate()
print format("Fps:  ###.##", fps)
print format("Best:  ####.##", maxfps)

