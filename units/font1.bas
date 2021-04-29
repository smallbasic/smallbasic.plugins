'
' Original work by [B+=MGA] 2016
'

Unit Font1

Export drawtext

'
' drawtext x, y, message
'   x,y = top left of message pixel position on screen
'   scale - pixel height of character cell, .5 * scale is character width of cell
'   message = text to be displayed
'
' Returns whether message was clipped
'
' drawtext requires 2 other subs for drawing thick lines (tl) and arcs (ac)
' the varaibles: thick (line thickness), lx,ly (last x and y) and dr (drawing radius)
' are shared between these 3 subs
' scale for 1x2 cells
'
sub drawtext(x, y, scale, mess)
  local i, c, px
  local thick = 1 / 40 * scale
  local dr = 1 / 6 * scale - .5 * thick
  local gf = .04 * scale
  local p1 = .16 * scale
  local p2 = .32 * scale
  local p3 = .48 * scale
  local p4 = .64 * scale
  local lm = len(mess)
  local py = y + .5 * thick

  for i= 1 to lm
    c = mid(mess, i, 1)
    px = x + (i - 1) * scale *.5 + .5 * thick
    select case c
    case "a":ac px+p1,py+p3,0,360:lx=px+p2:ly=py+p2-gf:tl 0,p2:tl 0,2*gf
    case "b":ac px+p1,py+p3,0,360:lx=px:ly=py:tl 0,p4
    case "c":ac px+p1,py+p3,40,320
    case "d":ac px+p1,py+p3,0,360:lx=px+p2:ly=py:tl 0,p4
    case "e":ac px+p1,py+p3,45,360:lx=px:ly=py+p3:tl p2,0
    case "f":ac px+p2,py+p1+gf,180,270:lx=px:ly=py+p2:tl p2,0:lx=px+p1:ly=py+p1:tl 0,p3
    case "g":ac px+p1,py+p3,0,360:lx=px+p2:ly=py+p2:tl 0,p2:ac px+p1,py+p4,0,90:lx=px:ly=py+p4+p1:tl p1,0
    case "h":ac px+p1,py+p3,180,360:lx=px:ly=py:tl 0,p4:lx=px+p2:ly=py+p3:tl 0,p1
    case "i":lx=px+p1-gf:ly=py+p2:tl gf,0:tl 0,p2:tl -gf,0:tl 2*gf,0
             lx=px+p1-gf:ly=py+p1:tl gf,0:tl 0,-gf:tl -gf,0:tl 0,gf
    case "j":lx=px+p1-gf:ly=py+p2:tl gf,0:tl 0,p2:ac px,py+p4,0,120
             lx=px+p1-gf:ly=py+p1:tl gf,0:tl 0,-gf:tl -gf,0:tl 0,gf
    case "k":lx=px:ly=py:tl 0,p4:lx=px+p2:ly=py+p2:tl -p2,p1:tl p2,p1
    case "l":lx=px+p1-gf:ly=py:tl gf,0:tl 0,p4:tl gf,0:tl -2*gf,0
    case "m":lx=px:ly=py+p2:tl 0,p2:lx=px+p1:ly=py+p4:tl 0,-p2:tl -p1,p1:lx=px+p2:ly=py+p4:tl 0,-p2:tl -p1,p1
    case "n":ac px+p1,py+p3,180,360:lx=px:ly=py+p2-gf:tl 0,p2+gf:lx=px+p2:ly=py+p3:tl 0,p1
    case "o":ac px+p1,py+p3,0,360
    case "p":ac px+p1,py+p3,0,360:lx=px:ly=py+p2-gf:tl 0,p4-2*gf
    case "q":ac px+p1,py+p3,0,360:lx=px+p2:ly=py+p2-gf:tl 0,p4-2*gf:tl 2*gf,0
    case "r":lx=px:ly=py+p2:tl 0,p2:ac px+p1,py+p3,180,340
    case "s":dr=1/12*scale:ac px+.5*p1,py+p2+dr,90,270:ac px+1.5*p1,py+p3+dr,270,450:dr=1/6*scale
             lx=px+.5*p1:ly=py+p2:tl p1*1.5,0:lx=px:ly=py+p4:tl p1*1.5,0:lx=px+.5*p1:ly=py+p3:tl p1,0
    case "t":lx=px+p1:ly=py:tl 0,p4:tl 2*gf,0:lx=px:ly=py+p2:tl p2,0
    case "u":ac px+p1,py+p3,0,180:lx=px:ly=py+p2:tl 0,p1*1.5:lx=px+p2:ly=py+p2:tl 0,p2+gf
    case "v":lx=px:ly=py+p2:tl p1,p2+gf:tl p1,-p2-1.5*gf
    case "w":lx=px-gf:ly=py+p2:tl gf+p1*.5,p2:tl .5*p1,-p2:tl p1*.5,p2:tl p1*.5+gf,-p2
    case "x":lx=px:ly=py+p2:tl p2,p2:lx=px:ly=py+p4:tl p2,-p2
    case "y":lx=px:ly=py+p2:tl p1,p2:lx=px+p2:ly=py+p2:tl -p2,p3+gf
    case "z":lx=px:ly=py+p2:tl p2,0:tl -p2,p2:tl p2,0
    case "A":lx=px:ly=py+p4:tl p1-gf,-p4:tl 2*gf,0:tl p1-gf,p4:lx=px+p1-2*gf:ly=py+p2:tl p1,0
    case "B":ac px+p1,py+p1,270,450:ac px+p1,py+p3,270,450
             lx=px:ly=py:tl 0,p4:tl p1,0:lx=px:ly=py+p2:tl p1,0:lx=px:ly=py:tl p1,0
    case "C":ac px+p1,py+p1,180,320:ac px+p1,py+p3,40,180:lx=px:ly=py+p1:tl 0,p2
    case "D":ac px+p1,py+p1,270,360:ac px+p1,py+p3,0,90
             lx=px+p1:ly=py:tl -p1,0:tl 0,p4:tl p1,0:lx=px+p2:ly=py+p1:tl 0,p2
    case "E":lx=px+p2:ly=py:tl -p2,0:tl 0,p4:tl p2,0:lx=px:ly=py+p2:tl p1+2*gf,0
    case "F":lx=px+p2:ly=py:tl -p2,0:tl 0,p4:lx=px:ly=py+p2:tl p1+2*gf,0
    case "G":ac px+p1,py+p1,180,345:ac px+p1,py+p3,0,180
             lx=px:ly=py+p1:tl 0,p2:lx=px+p2:ly=py+p4+gf:tl 0,-p2-gf:tl -p1,0
    case "H":lx=px:ly=py:tl 0,p4:tl 0,-p2:tl p2,0:tl 0,p2:tl 0,-p4
    case "I":lx=px+p1*.5:ly=py:tl p1,0:lx=px+p1*.5:ly=py+p4:tl p1,0:lx=px+p1:ly=py:tl 0,p4
    case "J":ac px+p1,py+p3,0,180:lx=px+p1*1.5:ly=py:tl p1*.5,0:tl 0,p3
    case "K":lx=px:ly=py:tl 0,p4:lx=px+p2:ly=py:tl -p2,p2:tl p2,p2
    case "L":lx=px:ly=py:tl 0,p4:tl p2,0
    case "M":lx=px:ly=py+p4:tl 0,-p4:tl p1,p2:tl p1,-p2:tl 0,p4
    case "N":lx=px:ly=py+p4:tl 0,-p4:tl p2,p4:tl 0,-p4
    case "O":ac px+p1,py+p1,180,360:ac px+p1,py+p3,0,180:lx=px:ly=py+p1:tl 0,p2:lx=px+p2:ly=py+p1:tl 0,p2
    case "P":ac px+p1,py+p1,270,450:lx=px+p1:ly=py:tl -p1,0:tl 0,p4:lx=px:ly=py+p2:tl p1,0
    case "Q":ac px+p1,py+p1,180,360:ac px+p1,py+p3,0,180
             lx=px:ly=py+p1:tl 0,p2:lx=px+p2:ly=py+p1:tl 0,p2:lx=px+p1:ly=py+p2:tl p1+gf,p2+gf
    case "R":ac px+p1,py+p1,270,450:lx=px+p1:ly=py:tl -p1,0:tl 0,p4:lx=px:ly=py+p2:tl p1,0:tl p1,p2
    case "S":ac px+p1,py+p1,90,360:ac px+p1,py+p3,270,540
    case "T":lx=px:ly=py:tl p2,0:lx=px+p1:ly=py:tl 0,p4
    case "U":ac px+p1,py+p3,0,180:lx=px:ly=py:tl 0,p3:lx=px+p2:ly=py:tl 0,p3
    case "V":lx=px:ly=py:tl p1,p4:tl p1,-p4
    case "W":lx=px-gf:ly=py:tl gf,p4:tl p1,-p2:tl p1,p2:tl gf,-p4
    case "X":lx=px:ly=py:tl p2,p4:lx=px+p2:ly=py:tl -p2,p4
    case "Y":lx=px:ly=py:tl p1,p2:tl 0,p2:lx=px+p2:ly=py:tl -p1,p2
    case "Z":lx=px:ly=py:tl p2,0:tl -p2,p4:tl p2,0
    case "0":ac px+p1,py+p1,180,360:ac px+p1,py+p3,0,180:lx=px:ly=py+p1:tl 0,p2:tl p2,-p2:tl 0,p2
    case "1":lx=px:ly=py+p1:tl p1,-p1:tl 0,p4:tl -p1,0:tl p2,0
    case "2":ac px+p1,py+p1,180,360:lx=px+p2:ly=py+p1:tl -p2,p3:tl p2,0
    case "3":ac px+p1,py+p1,180,450:ac px+p1,py+p3,270,540
    case "4":lx=px+p1:ly=py:tl -p1,p2:tl p2,0:tl 0,p2:lx=px+p2:ly=py:tl 0,p2
    case "5":ac px+p1,py+p3,270,520:lx=px+p1:tl -p1,0:tl 0,-p2:tl p2,0
    case "6":ac px+p1,py+p3,0,360:lx=px:ly=py+p3:tl p1,-p3
    case "7":lx=px:ly=py:tl p2,0:tl -p1,p4
    case "8":ac px+p1,py+p1,0,360:ac px+p1,py+p3,0,360
    case "9":ac px+p1,py+p1,0,360:lx=px+p2:ly=py+p1:tl -p1,p3
    case ".":lx=px:ly=py+p4:tl gf,0:tl 0,-gf:tl -gf,0:tl 0,gf
    case "!":lx=px:ly=py+p4:tl gf,0:tl 0,-gf:tl -gf,0:tl 0,gf::lx=px:ly=py:tl 0,p2
    case ",":lx=px+gf:ly=py+p4:tl -gf,0:tl 0,-gf:tl gf,0:tl 0,gf:tl -gf,.5*p1+gf
    case "?":lx=px+p1:ly=py+p4:tl gf,0:tl 0,-gf:tl -gf,0:tl 0,gf:ac px+p1,py+p1,180,450:lx=px+p1:ly=py+p2:tl 0,p1-gf
    end select
  next
end

'ac is for arc, x,y is radius center, das=degree angle start, dae=degree angle end
sub ac(x, y, das, dae)
  'note dr, drawing radius has to be global, use COLOR globally sets color
  'note thick also made globals by POST sub
  local a,x1,y1,stepper
  if dr then
    if int(thick)=0 then stepper=1/(dr*pi) else stepper=(thick/2)/(dr*pi/2)
    for a=das to dae step stepper
      x1=dr*cos(rad(a)) : y1=dr*sin(rad(a))
      if int(thick)<1 then  pset x+x1,y+y1 else circle x+x1,y+y1,thick filled
    next
  fi
end

'tl stands for thick line in the LINE STEP x,y format
sub tl(stepx,stepy) 'tl=thickline
  'lastx, lasty globals for last drawn position
  'thick has to be global
  'note thick=0 still draws a line, use COLOR so line is drawn from this global
  local length,dx,dy,i
  length=((stepx)^2 +(stepy)^2)^.5
  if length then
    dx=stepx/length : dy=stepy/length
    for i=0 to length
      circle lx+dx*i,ly+dy*i,thick filled
    next
  end if
  lx=lx+stepx : ly=ly+stepy
end
