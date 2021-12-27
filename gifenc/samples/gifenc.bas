import gifenc as ge

const w = 120
const h = 90
const palette = [0x00, 0x66, 0x00, & ' 0 -> black
                 0xFF, 0x00, 0x00, & ' 1 -> red
                 0x22, 0xFF, 0x00, & ' 2 -> green
                 0x00, 0x00, 0xFF]   ' 3 -> blue

const bgIndex = -1 'none
const loopCount = 0
const gif = ge.create("example.gif", w, h, palette, bgIndex, loopCount)

for i = 0 to 8
  frame = []
  for j = 0 to w * h
    frame << (i * 3 + j) / 6 % 4
  next j
  ge.add_frame(gif, 10, frame)
next i

ge.close(gif)
