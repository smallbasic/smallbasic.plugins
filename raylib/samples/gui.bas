import raylib as rl
import raylibc as c
import debug

const screenWidth = 450
const screenHeight = 450

'rl.SetWindowState(0x00000800)
rl.InitWindow(screenWidth, screenHeight, "SmallBASIC raylib gui")
rl.SetWindowPosition(rl.GetMonitorWidth(0) - screenWidth - 20, (rl.GetMonitorHeight(0) - screenHeight) / 2)
rl.SetTargetFPS(60)

run("xdotool windowactivate `xdotool search --onlyvisible --name \"Emacs\"`")

'rl.UsndecorateWindow()
cp = c.YELLOW
text = "input text"

while (!rl.WindowShouldClose() && !debug.IsSourceModified())
  rl.BeginDrawing()
  rl.ClearBackground(c.RAYWHITE)

  x = 10: y = 10: h = 39

  if (rl.GuiButton([x, y, 80, 35], "My Button")) then
    showAlert=true
  endif
  if (showAlert) then
    select case rl.GuiMessageBox([100, 100, 280, 235], "Close Window", "Do you really want to quit?", "Yes;No")
    case -1
    case 0
      showAlert = false
    case 1
      pbv = 50
      showAlert = false
    case 2
      pbv = 0
      showAlert = false
    end select
  else
    y += h + 5: h = 15
    checked1 = rl.GuiCheckbox([x, y, h, h], "My Check", checked1)
    y += h + 5
    alpha = rl.guicolorbaralpha([x, y, 140, h], "", alpha)
    y += h + 5: h = 50
    hue = rl.guicolorbarhue([x, y, h, h], "", hue)
    y += h + 5: h = 110
    cp = rl.guicolorpicker([x, y, w, h], "", cp)
    y += h + 5: h = 30
    cba = rl.GuiComboBox([10, y, 120, h], "one;two;three;four;five", cba)
    y += h + 5
    if (rl.guidropdownbox([10, y, 120, h], "one;two;three;four;five", cbx, is_open)) then
      is_open = !is_open
    endif
    x = 180: y = 10: h = 100
    n = rl.guigrid([x, y, h, h], "", 20, 5)
    y += h + 5: h = 20
    n = rl.GuiLabelButton([x, y, 100, h], "hello!")
    y += h + 5: h = 80
    if (rl.GuiListView([x, y, 100, h], "list1;list2", scrollindex, listopen)) then
      listopen = true
    endif
    y += h + 5: h = 20
    'pbv = iff(pbv == 100, 0, pbv + 1)
    pv = rl.GuiProgressBar([x, y, 100, h], "L", "R", pbv, 0, 100)

    y += h + 5: h =120
    obj = rl.GuiTextInputBox([x, y, 200, 130], "Title", "Message", "Okay;well;okay", text, secret)
    text = obj.text
    secret = obj.secret
    if (obj.result == 1) then showAlert = true
    if (obj.result == 0) then text = "don't press x"

  endif

  rl.EndDrawing()
  rl.WaitEvents(800)
wend

rl.CloseWindow()
