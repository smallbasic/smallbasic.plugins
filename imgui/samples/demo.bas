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

show_demo_window = false
show_another_window = false
clear_color = [0.45, 0.55, 0.60, 1.00]

sub show_window
  if (ImGui.BeginFullScreen("Hello, world!")) then       ' Create a window called "Hello, world!" and append into it.
    ImGui.Text("This is some useful text.")              ' Display some text (you can use a format strings too)
    ImGui.Checkbox("Demo Window", show_demo_window)      ' Edit bools storing our window open/close state
    ImGui.Checkbox("Another Window", show_another_window)
    ImGui.SliderFloat("float", f, 0.0, 1.0)              ' Edit 1 float using a slider from 0.0f to 1.0f
    n = ImGui.ColorEdit3("clear color", clear_color)     ' Edit 3 floats representing a color
    if (ImGui.Button("Button")) then                     ' Buttons return true when clicked (most widgets return true when edited/activated)
      counter++
    endif  
    ImGui.SameLine()
    ImGui.Text("counter = %d", counter)
    ImGui.Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0 / ImGui.Framerate(), ImGui.Framerate())
  endif  
  ImGui.End()
end

# Loop until the user closes the window
while not ImGui.window_should_close(wnd)
  ImGui.wait_events()
  ImGui.newframe()
  show_window()
  ImGui.Render(wnd)
wend

