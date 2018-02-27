rem adapted from love-nuklear example, see: https://github.com/keharriso/love-nuklear.git
rem An overview of most of the supported widgets.
option predef grmode 850x600

import nuklear as nk

local checkA = {value: false}
local checkB = {value: true}
local radio = {value: "A"}
local selectA = {value: false}
local selectB = {value: true}
local slider = {value: 0.2}
local progress = {value: 5}
local colorPicker = {value: "#ff0000"}
local property = {value: 6}
local edit = {value: "Edit text"}
local comboA = {value: 1, items: ["A", "B", "C"]}

while 1
  if nk.windowBegin("Overview", 10, 10, "100% - 20", "100% - 20") then
    nk.menubarBegin()
    nk.layoutRow("dynamic", 30, 1)
    if nk.menuBegin("Menu", nil, 120, 90) then
      nk.layoutRow("dynamic", 40, 1)
      nk.menuItem("Item A")
      nk.menuItem("Item B")
      nk.menuItem("Item C")
      nk.menuEnd()
    endif
    nk.menubarEnd()
    nk.layoutRow("dynamic", 500, 3)
    nk.groupBegin("Group 1", "border")
      nk.layoutRow("dynamic", 30, 1)
      nk.label("Left label")
      nk.label("Centered label", "centered")
      nk.label("Right label", "right")
      nk.label("Colored label", "left", "#ff0000")
      nk.label("A very long wrapped label - The quick brown fox jumps over the ...", "right", "wrap")
      if nk.treePush("tab", "Tree Tab") then
        if nk.treePush("node", "Tree Node 1") then
          nk.label("Label 1")
          nk.treePop()
        endif
        if nk.treePush("node", "Tree Node 2") then
          nk.label("Label 2")
          nk.treePop()
        endif
        nk.treePop()
      endif
      nk.spacing(1)
      if nk.button("Button") then
        print("button pressed!")
      endif
      nk.spacing(1)
      nk.checkbox("Checkbox A", checkA)
      nk.checkbox("Checkbox B", checkB)
    nk.groupEnd()
    nk.groupBegin("Group 2", "border")
      nk.layoutRow("dynamic", 30, 1)
      nk.label("Radio buttons:")
      nk.layoutRow("dynamic", 30, 3)
      nk.radio("A", radio)
      nk.radio("B", radio)
      nk.radio("C", radio)
      nk.layoutRow("dynamic", 30, 1)
      nk.selectable("Selectable A", selectA)
      nk.selectable("Selectable B", selectB)
      nk.layoutRow("dynamic", 30, 1)
      nk.label("Slider:")
      nk.slider(1, progress, 10, .5)
      nk.label("Progress:")
      nk.progress(progress, 10, true)
      nk.layoutRow("dynamic", 30, 2)
      nk.spacing(2)
      nk.label("Color picker:")
      nk.button(colorPicker.value)
      nk.layoutRow("dynamic", 90, 1)
      nk.colorPicker(colorPicker)
    nk.groupEnd()
    nk.groupBegin("Group 3", "border")
      nk.layoutRow("dynamic", 30, 1)
      nk.property("Property", 0, property, 10, 0.25, 0.05)
      nk.spacing(1)
      nk.label("Edit:")
      nk.layoutRow("dynamic", 90, 1)
      nk.edit("box", edit)
      nk.layoutRow("dynamic", 5, 1)
      nk.spacing(1)
      nk.layoutRow("dynamic", 30, 1)
      nk.label("Combobox:")
      nk.combobox(comboA)
      nk.layoutRow("dynamic", 5, 1)
      nk.spacing(1)
      nk.layoutRow("dynamic", 30, 1)
      if nk.widgetIsHovered() then
        nk.tooltip("Test tooltip")
      endif
      [x, y, w, h] = nk.widgetBounds()
      if nk.contextualBegin(100, 100, x, y, w, h) then
        nk.layoutRow("dynamic", 30, 1)
        nk.contextualItem("Item A")
        nk.contextualItem("Item B")
        nk.contextualEnd()
      endif
      nk.label("Contextual (Right click me)")
    nk.groupEnd()
  endif
  nk.windowEnd()
wend
