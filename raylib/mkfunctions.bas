rem
rem generate functions.json for mkskeleton.bas
rem 
rem from input generated with:
rem clang -Xclang -dump-tokens raylib/src/raylib.h 2>&1 | awk -F"'" '/identifier|l_paren|r_paren|semi|void/ {print $2}' > functions.tokens
rem

tload "functions.tokens", s

const len_s = len(s) - 1
const states = ["init", "name", "arg", "l_paren", "r_paren"]

funcs = []
subs = []

sub reset
  state = "init"
  name = ""
  args = []
  is_func = true
end

sub mk_item
  local item
  item.name = name
  item.args = args
  if (is_func) then
    funcs << item
  else
    subs << item
  endif  
end

sub process
  local i, ch
  for i = 0 to len_s
    select case s[i]
    case "("
      state = "l_paren"
    case ")"
      state = "r_paren"
    case ";"
      mk_item
      reset
    case "void"
      if (state == "init") then
        is_func = false
      endif
    case else
      if (state = "l_paren") then
        ch = mid(s[i], 1, 1)
        if (ch > "Z") then
          'ignore class name
          args << s[i]
        endif
      else
        name = s[i]
      endif
    end select
  next i
end

func cmpFunc(l, r)
  local f1 = lower(l.name)
  local f2 = lower(r.name)
  return iff(f1 == f2, 0, iff(f1 > f2, 1, -1))
end

reset
process

sort funcs use cmpFunc(x,y)
sort subs use cmpFunc(x,y)

out.func = funcs
out.sub = subs

print out
