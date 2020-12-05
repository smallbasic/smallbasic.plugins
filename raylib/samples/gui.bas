import raylib as rl
import raylibc as c
import debug

const screenWidth = 800
const screenHeight = 450

rl.InitWindow(screenWidth, screenHeight, "SmallBASIC raylib gui")
rl.SetWindowPosition(800, 20)
rl.SetTargetFPS(60)
'rl.UndecorateWindow()

cp = c.GREEN

while (!rl.WindowShouldClose() && !debug.IsSourceModified())
  rl.BeginDrawing()
  rl.ClearBackground(c.RAYWHITE)
  
  x = 10: y = 10: h = 35

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
    alpha = rl.guicolorbaralpha([x, y, 140, h], alpha)
    y += h + 5: h = 50
    hue = rl.guicolorbarhue([x, y, h, h], hue)
    y += h + 5: h = 110
    cp = rl.guicolorpicker([x, y, w, h], cp)
    y += h + 5: h = 30
    cba = rl.GuiComboBox([10, y, 120, h], "one;two;three;four;five", cba)
    y += h + 5
    if (rl.guidropdownbox([10, y, 120, h], "one;two;three;four;five", cbx, is_open)) then
      is_open = !is_open
    endif
    x = 180: y = 10: h = 100
    n = rl.guigrid([x, y, h, h], 20, 5)
    y += h + 5: h = 20
    n = rl.GuiLabelButton([x, y, 100, h], "hello!")
    y += h + 5: h = 80
    if (rl.GuiListView([x, y, 100, h], "list1;list2", scrollindex, listopen)) then
      listopen = true
    endif
    y += h + 5: h = 20
    'pbv = iff(pbv == 100, 0, pbv + 1)
    pv = rl.GuiProgressBar([x, y, 100, h], "L", "R", pbv, 0, 100)

  endif

  rl.EndDrawing()
  rl.WaitEvents(100)
wend

rl.CloseWindow()

