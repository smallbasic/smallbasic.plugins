option predef grmode 640x480

import nuklear as nk

radio = {value: "A"}

while 1
    if nk.windowBegin("Radio Buttons", 0, 0, "100%", "100%") then

        nk.layoutRow("dynamic", 90, 2)
        nk.radio("Radio Button A", radio)
        nk.radio("Radio Button B", radio)

        if(radio.value = "A") then
            ' Do something useful
        endif

    endif
    nk.windowEnd()
    nk.waitEvents()
wend
