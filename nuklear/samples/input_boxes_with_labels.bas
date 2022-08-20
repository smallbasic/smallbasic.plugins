option predef grmode 640x480

import nuklear as nk

FirstName = {value: ""}
Surname  = {value: ""}

while 1
    if nk.windowBegin("Input box with labels", 0, 0, "100%", "100%") then

        nk.layoutRowBegin("static", 25, 2)  ' Static row -> fixed width
            nk.LayoutRowPush(92)            ' Next element will have a width of 92px
            nk.label("First Name:")
            nk.LayoutRowPush(371)           ' Next element will have a width of 371px
            nk.edit("field", FirstName)
        nk.layoutRowEnd()

        nk.layoutRowBegin("dynamic", 25, 2) ' Dynamic row -> width depends on window size
            nk.LayoutRowPush(0.15)          ' Next element will have a width of 15%
            nk.label("Surname:")
            nk.LayoutRowPush(0.60)          ' Next element will have a width of 85%
            nk.edit("field", Surname)
        nk.layoutRowEnd()

    endif
    nk.windowEnd()
    nk.waitEvents()
wend
