import ImGui

# Initialize the library
if not ImGui.init() then
  throw "ImGui init failed"
endif

# Create a windowed mode window and its OpenGL context
wnd = ImGui.create_window(640, 480, "SmallBASIC - IMGUI")
if not wnd then
  throw "ImGui create window failed"
endif

counter = 0

# Loop until the user closes the window
while not ImGui.window_should_close(wnd)
  ImGui.poll_events()
  ImGui.newframe()
  counter++

  if (ImGui.Begin("New window")) then
    ImGui.Text("This is some useful text.")
    ImGui.Text("counter = %d", counter)
  endif
  ImGui.End()

  ImGui.Render(wnd)
wend

