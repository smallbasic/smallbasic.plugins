option predef grmode 640x480

import nuklear as nk

ClickCounter = 0

while 1
    if nk.windowBegin("Button", 0, 0, "100%", "100%") then

        nk.layoutRow("dynamic", 30, 1)
        if nk.button("You clicked me " + ClickCounter + " times") then
           ClickCounter++
        endif

    endif
    nk.windowEnd()
    nk.waitEvents()
wend
