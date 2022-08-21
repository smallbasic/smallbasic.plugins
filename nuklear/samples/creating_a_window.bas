option predef grmode 640x480    ' Set window size

import nuklear as nk            ' Import Nuklear plugin

while 1
    if nk.windowBegin("Hello World", 0, 0, "100%", "100%") then     ' Create window
        ' Render GUI elements and do other useful thinks
    endif
    nk.windowEnd()              ' Free window
    nk.waitEvents()             ' Pause until event
wend
