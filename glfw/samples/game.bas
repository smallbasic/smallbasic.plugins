import glfw

# Initialize the library
if not glfw.init() then
  throw "failed to init"
endif

# Create a windowed mode window and its OpenGL context
wnd = glfw.create_window(640, 480, "Hello World!")
if not wnd then
  glfw.terminate()
  throw "failed to create window"
endif

# Loop until the user closes the window
while not glfw.window_should_close(wnd)
  # Render here

  # Swap front and back buffers
  glfw.swap_buffers(wnd)

  # Poll for and process events
  glfw.poll_events()
wend

glfw.terminate()
print "done!"

