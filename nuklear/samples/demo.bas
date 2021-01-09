option predef grmode 640x480
import nuklear as nk

local radio = {value: "Easy"}
slider.value = .6

c1=rgb(0x28, 0x2f, 0x38)
c2=rgb(0x60, 0x7d, 0x8b)

fnt= [[1,0,1,1,1,1,1],
[0,0,0,0,0,1,1],
[1,1,1,0,1,1,0],
[1,1,1,0,0,1,1],
[0,1,0,1,0,1,1],
[1,1,1,1,0,0,1],
[1,1,1,1,1,0,1],
[1,0,0,0,0,1,1],
[1,1,1,1,1,1,1],
[1,1,1,1,0,1,1]]

sub dn(n, p)
  ox=20
  oy=20
  w=10
  h=10
  dw=xmax / 10
  dh=ymax / 15
  p0=p * 100
  x0=ox+p0+10
  x1=x0+dw
  y0=oy
  y1=y0+dh
  y2=y1+dh

  if n>9 then throw n
  if fnt[n][0]==1 then
    rect x0+w,y0,x1,y0+h filled
  endif
  if fnt[n][1]==1 then
    rect x0+w,y1,x1,y1+h filled
  endif
  if fnt[n][2]==1 then
    rect x0+w,y2,x1,y2+h filled
  endif
  if fnt[n][3]==1 then
    rect x0,y0+h,x0+w,y1 filled
  endif
  if fnt[n][4]==1 then
    rect x0,y1+h,x0+w,y2 filled
  endif
  if fnt[n][5]==1 then
    rect x1,y0+h,x1+w,y1 filled
  endif
  if fnt[n][6]==1 then
    rect x1,y1+h,x1+w,y2 filled
  endif
end

sub printNumber(n)
  cls
  dn(floor(n/1000),0)
  dn(floor(n mod 1000/100), 1)
  dn(floor(n mod 100/10), 2)
  dn(floor(n mod 10/1), 3)
  nk.swapBuffers()
end

nk.lineWidth(1)
color c1,c2

while 1
  if nk.windowBegin("Show!", 5, 5, "100% - 10", "100% - 10") then
    nk.layoutRow("static", 30, 80, 1)
    if (nk.button("Button")) then
      for i = 50 to 0 step -1
        printNumber(i)
        delay 100
      next i
      delay 1000
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

