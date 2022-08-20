option predef grmode 640x480

import nuklear as nk

while 1
    if nk.windowBegin("Menu", 0, 0, "100%", "100%") then

        nk.menubarBegin()
            nk.layoutRow("dynamic", 20, 1)
            if nk.menuBegin("Menu", nil, 100, 80) then
                nk.layoutRow("dynamic", 20, 1)
                if(nk.menuItem("Item A")) then
                    ' Menu item clicked - do something
                endif
                if(nk.menuItem("Item B")) then
                    ' Menu item clicked - do something
                endif
                if(nk.menuItem("Item C")) then
                    ' Menu item clicked - do something
                endif
                nk.menuEnd()
            endif
        nk.menubarEnd()

    endif
    nk.windowEnd()
    nk.waitEvents()
wend
