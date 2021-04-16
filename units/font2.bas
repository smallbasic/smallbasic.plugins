REM
REM ROUTINE TO DRAW CHARACTERS WITH DRAW COMMANDREM By Miguel A. MAroto 2019 (from routines of TurboBasic of 2001)
REM 
REM The text are in variable letters
REM  xc1 and yc1  are the initial position of the text
REM  Escala is the scale of the text (NO OPERATIVE NOW !!!!!!!)
REM ----------------------------------------------------------------------
REM ! ONLY CAN DRAW CAPITAL LETTERS AND SINGNS (not all) !
REM Each character has a cudricule of X=5, Y=9
REM The drawing of the character start from left-down position and ends at right-down
REM !!! ALL THE SCALE RELATED COMMANDS ARE DISABLED  !!!!!
REM   DEFINITION OF THE MOVEMENTS NEEDED TO DRAW EACH CHARACTER WITH THE DRAW COMMAND
REM the number of each chararter in the array car(n) is the same of the ASCII code
REM

Unit Fonts2

Export drawtext

dim car (120)
car(32)="br5":REM SPACE
car(40)="br4 bu10 g2 d7 f2 bu1 br3 ":REM (
car(41)="br1 bu10 f2 d7 g2 bu1 br4  ":REM )
car(42)="br3 bu4 nu3 nd3 nl3 nr3 nf2 nh2 ne2 ng2 bd4 br2 ":REM *
car(43)="br3 bu4 nu3 nd3 nl3 nr3  bd4 br2 ":REM +
car(44)="br3 bu1 g2 be1 br3 ":REM ,
car(45)="br3 bu4 nl3 nr3  bd4 br2":REM -
car(46)="br3 u1 r1 d1 l1 br2 ":REM .
car(47)="e6 bd6 ":REM /
car(48)="br4 bu1 g1 l2 h1 u7 e1 r2 f1 d7 bf1":REM 0
car(49)="br1 r4 l2 u9 g2 r2 bd7 br1":REM 1
car(50)="bu8 e1 r3 f1 d1 g5 d2 r5 u1 bd1":REM 2
car(51)="bu1 f1 r3 e1 u3 h1 l3 br3 e1 u2 h1 l3 g1 br5 bd8 ":REM 3
car(52)="br3 r2 l1 u9 g4 d1 r5 bd4 ":REM 4
car(53)="bu1 f1 r3 e1 u3 h1 l4 u4 r5 bd9 ":REM 5
car(54)="br4 bu9 l1 g2 d6 f1 r3 e1 u3 h1 l4 br4 bd4 ":REM 6
car(55)="br2 u3 e3 u3 l5 d1 br5 bd8 ":REM 7
car(56)="bu8 d2 f1 g1 d3 f1 r3 e1 u3 h1 l3 br3 e1 u2 h1 l3 g1 br5 bd8":REM 8
car(57)="br1 r2 e2 u6 h1 l3 g1 d2 f1 r4 bd5 ":REM
car(58)="br3 bu1 u1 bu3 u1 bd5 br2":REM :
car(59)="br3 bu1 g2 be1 br1 bu4 u1 bd5 br3 ":REM ;
car(60)="br4 bu7 g3 f3 br1 bd1 ":REM <
car(61)="br1 bu3 r3 bu2 l3 br4 bd5 ":REM =
car(62)="br1 bu7 f3 g3 bd1 br4 ":REM >
car(63)="br3 bu4 nl3 nr3  bd4 br2":REM -
car(64)="br3 bu4 nl3 nr3  bd4 br2":REM -
car(65)="u6 e3 f3 d6 u4 l6 d4 br6":REM A
car(66)="r5 e1 u3 h1 l4 br4 e1 u2 h1 l5 r1 d9 br5 ":REM B
car(67)="br5 bu1 g1 l3 h1 u7 e1 r3 f1 bd8 ":REM C
car(68)="r5 e1 u7 h1 l5 r1 d9 br5":REM D
car(69)="br5 bu1 g1 e1 d1 l5 r1 u5 r2 u1 d2 u1 l2 u4 l1 r5 d1 h1 br1 bd9":REM E
car(70)="br1 r2 l1 u5 r3 u1 d2 u1 l3 u4 l1 r5 d1 h1 br1 bd9 bl2":REM F
car(71)="br3 bu4 r2 d4 u1 g1 l3 h1 u7 e1 r3 f1 bd8 ":REM G
car(72)="u9 bd4 r5 u4 d9":REM H
car(73)="br1 r2 l1 u9 l1 r2 ":REM I
car(74)="br1 bu2 d1 f1 r1 e1 u8 l2 r4 ":REM J
car(75)="r1 u9 l1 br4 d2 g3 f3 d1 bd1":REM K
car(76)="br5 bu1 g1 e1 d1 l5 r1 u9 l1 r2 br3" : REM L
car(77)="u9 f3 e3 d9 ":REM M
car(78)="u9  f5 u5 d9 ":REM N
car(79)="br1 h1 u7 e1 r3 f1 d7 g1 l3 br4 ":REM O
car(80)="br1 r2 bl1 u9 r3 f1 d3 g1 l3 br4 ":REM P
car(81)="br1 h1 u7 e1 r3 f1 d7 g1 l3 br3 bu1 f2 bu1 br1 ":REM Q
car(82)="br1 r2 bl1 u9 l1 r4 f1 d3 g1 l3 br2 f2 d2":REM R
car(83)="bu2 d1 f1 r3 e1 u1 h5 u1 e1 r3 f1 d1 bd8":REM S
car(84)="br1 r2 l1 u9 l3 d1 e1 r5 d1  h1 bf1 bd9 ":REM T
car(85)="bu9 d8 f1 r3 e1 u8 bd9":REM U
car(86)="bu9 d6 f3 e3 u6 bd9":REM V
car(87)="bu9 d7 f2 e1 u2 d2 f1 e2 u7 bd9":REM W
car(88)="bu9 d2 f5 d2 bl5 u2 e5 u2 bd9":REM X
car(89)="br1 r2 l1 u5 e2 u2 bl4 d2 f2 bd5 br2":REM Y
car(90)="br5 bu1 g1 be1 d1 l5 u2 e5 u2 l5 d1 e1 br5 bd9 ":REM Z

REM adjust the separation between chararters to 5 pixels to scale  1:1
separator = .4

sub _draw(s)
  if (s != "0") then
    draw(s)
  endif     
end

sub drawtext(xc, yc, scale, bold, text)
  REM set the initial point for the drawing
  local S, position, moverse, bit
  
  for S = 1 to len(text)
    REM Set the initial position that will be moved to the right for each character
    position ="bm" + str(xc) + "," + str(yc)
    _draw(position)

    REM Extract each character one by one and I draw it
    bit = mid(text, S, 1)
    
    REM Draw the first stroke of the character
    _draw(car[asc(bit)])
    
    REM Go back the width  of a character plus 1 (with scale 1:1)
    xc += 1
    REM origin of the characters plus 1

    if (bold) then 
      moverse="bm" + str(xc) + "," + str(yc)
      _draw(moverse)
      _draw(car[asc(bit)])
    endif
    
    xc = int(xc + 1 + separator * (scale / 4))
  next S
end

