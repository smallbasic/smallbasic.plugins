import imgui

# Initialize the library
if not imgui.init() then
  throw "imgui init failed"
endif

# Create a windowed mode window and its OpenGL context
wnd = imgui.create_window(640, 480, "SmallBASIC - IMGUI")
if not wnd then
  throw "imgui create window failed"
endif

maxfps = 0

# Loop until the user closes the window
while not imgui.window_should_close(wnd)
  imgui.poll_events()
  imgui.newframe()
  
  
  imgui.render(wnd)
wend

