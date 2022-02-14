rem
rem generate README.md from main.cpp
rem

dim code

sub load(file)
  local i, s, sline

  tload file, s
  for i = 0 to len(s) - 1
    sline = s[i]
    if (INSTR(0, sline, "  {") == 1) then
      code << DISCLOSE(sline, "\"\"")
    endif
  next i
end

load("main.cpp")
load("proc-def.h")
load("func-def.h")

tload "raylib/parser/raylib_api.json", s, 1
api = array(s)
functions = {}
for fun in api("functions")
  name = upper(fun.name)
  functions[name] = fun
next

print "*Raylib* " + trim(command)
print "======="
print "raylib is a simple and easy-to-use library to enjoy videogames programming."
print
print "https://www.raylib.com/
print
print "Implemented APIs (" + len(code) + ")"
print "----------------"
print ""
print "| Name    | Description   |"
print "|---------|---------------|"

sort code
for i = 0 to len(code) - 1
  description = functions(code[i]).description
  if (description == 0) then
    description = "n/a"
  endif
  returnType = iff(functions(code[i]).returnType == "void", "sub", "func")
  params = ""
  comma = ""
  name = functions(code[i]).name
  if (name == 0) then
    name = lower(code(i))
  endif
  for param in functions(code[i]).params
    params += comma + param.name
    comma = ", "
  next param
  print "| " + returnType + " " + name + "(" + params + ") | " + description + " |"
next i

