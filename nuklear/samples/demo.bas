option predef grmode 300x250
import nuklear as nk

local radio = {value: "Easy"}
slider.value = .6

while 1
  if nk.windowBegin("Show!", 5, 5, "100% - 10", "100% - 10") then
    nk.layoutRow("static", 30, 80, 1)
    if (nk.button("Button")) then
        print "pressed"
    endif

    nk.layoutRow("dynamic", 0, 2)
    nk.radio("Easy", radio)
    nk.radio("Hard", radio)

    nk.layoutRowBegin("static", 30, 2)
      nk.layoutRowPush(50)
      nk.label("Volume:", "left")
      nk.layoutRowPush(110)
      nk.slider(0, slider, 1, .1)
    nk.layoutRowEnd()
   nk.windowEnd()
  endif  
wend
    
