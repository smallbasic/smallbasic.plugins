rem Simple calculator example, adapted from the original nuklear demo.
rem adapted from love-nuklear example, see: https://github.com/keharriso/love-nuklear.git

import nuklear as nk

local ops = ["+","-","*","/"]
local numbers = [1,2,3,0,4,5,6,0,7,8,9]
local a, b, c, oper

sub clear()
  a = 0
  b = 0
  c = "0"
end

sub digit(d)
  if (c == "0") then c = ""
  c += str(d)
end

sub equals()
  b = val(c)
  select case oper
  case "+"
    c = str(a + b)
  case "-"
    c = str(a - b)
  case "/"
    c = str(a / b)
  case "*"
    c = str(a * b)
  end select
end

sub operator(o)
  a = val(c)
  oper = o
  c = "0"
end

sub main
  if nk.windowBegin("Calculator", 10, 10, 180, 250, "border", "movable", "title", "no_scrollbar") then
    nk.layoutRow("dynamic", 35, 1)
    nk.label(c, "right")
    nk.layoutRow("dynamic", 35, 4)
    for i = 0 to 15
      if i >= 12 and i < 15 then
        if i == 13 then
          if nk.button("C") then
            clear()
          endif
          if nk.button("0") then
            digit("0")
          endif
          if nk.button("=") then
            equals()
          endif
        endif
      elseif (i+1) % 4 == 0 then
        local o = ops[int(i / 4)]
        if nk.button(o) then
          operator(o)
        endif
      else
        local n = numbers[i]
        if nk.button(n) then
          digit(n)
        endif
      endif
    next i
  endif
  nk.windowEnd()
end

while 1
 main
wend
