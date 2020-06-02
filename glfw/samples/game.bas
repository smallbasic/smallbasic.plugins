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

t1=timer


# Loop until the user closes the window
while not glfw.window_should_close(wnd)
  # Render here
  t1 = TICKS

  #  key = glfw.key_event();
  r += 1
  if r >= 256 then r = 0
  if r >= 15 then r = 0
  g += 2
  if g >= 256 then g = 0
  b += 3
  if b >= 256 then b = 0
  color 8, rgb(r, g, b)

  cls
  pset 10,10 color 7
  pset 11,11 color 7
  pset 12,12 color 7
  line 20,40,140,40 color r
  line 20,80,140,80 color r
  line 60,0,60,120 color r
  line 100,0,100,120 color r

  # Swap front and back buffers
  glfw.swap_buffers(wnd)

  # Poll for and process events
  glfw.wait_events()

  t2 = ticks - t1
  if (t2 > 0) then
    fps = 1000 / t2
    # print format("Fps:  ###.##", fps)
  endif
wend

glfw.terminate()
print "done!"

