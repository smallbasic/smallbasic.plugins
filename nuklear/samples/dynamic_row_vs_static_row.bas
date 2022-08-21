option predef grmode 640x480

import nuklear as nk

radio = {value: "A"}

while 1
    if nk.windowBegin("Radio Buttons", 0, 0, "100%", "100%") then

        nk.layoutRow("static",  90, 310, 2)   ' fix sized elements
        nk.radio("Radio Button A", radio)
        nk.radio("Radio Button B", radio)

        nk.layoutRow("dynamic", 90, 2)        ' dynamic sized elements
        nk.radio("Radio Button C", radio)
        nk.radio("Radio Button D", radio)

        if(radio.value = "A") then
            ' Do something useful
        endif

    endif
    nk.windowEnd()
    nk.waitEvents()
wend
