rem Simple calculator example, adapted from the original nuklear demo.
rem adapted from love-nuklear example, see: https://github.com/keharriso/love-nuklear.git

import nuklear as nk

local ops = {'+', '-', '*', '/'}
local a, b, op = '0'

sub clear()
  a = 0
  b = 0
  op = '0'
end

sub digit(d)
  if op then
    if (b == Nil or b == "0") then
      b = d
    else
      b = b.d
    endif
  else if a == "0" then
    a = d
  else
    a = a.d
  endif
end

sub equals()
'  if not tonumber(b) then
'    return
'  end
'  if op == "+" then
'    a, b, op = tostring(tonumber(a) + tonumber(b))
'  elseif op == "-" then
'    a, b, op = tostring(tonumber(a) - tonumber(b))
'  elseif op == "*" then
'    a, b, op = tostring(tonumber(a) * tonumber(b))
'  elseif op == "/" then
'    a, b, op = tostring(tonumber(a) / tonumber(b))
'  end
end

sub operator(byref o)
  if op then
    equals()
  endif
  op = o
end

func display()
  return b or a
end

sub main
  if nk.windowBegin("Calculator", 50, 50, 180, 250, "border", "movable", "title") then
    'nk.layoutRow("dynamic", 35, 1)
    'nk.label(display(), "right")
    'nk.layoutRow("dynamic", 35, 4)
    for i = 1 to 16
      if i >= 13 and i < 16 then
        if i == 13 then
          'if nk.button("C") then
          '  clear()
          'endif
          'if nk.button("0") then
          '  digit("0")
          'endif
          'if nk.button("=") then
          '  equals()
          'endif
        endif
      'elseif i % 4 ~= 0 then
      '  local d = tostring(math.floor(i / 4) * 3 + (i % 4))
      '  if nk.button(d) then
      '    digit(d)
      '  end
      'else
      '  local o = ops[math.floor(i / 4)]
      '  if nk.button(o) then
      '    operator(o)
      '  end
      endif
    next i
  endif
  nk.windowEnd()
end

while 1
 main
wend
