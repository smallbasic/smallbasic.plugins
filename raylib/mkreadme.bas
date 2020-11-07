rem
rem generate README.md from main.cpp
rem

tload "main.cpp", s

dim code
for i = 0 to len(s) - 1
  sline = s[i]
  if (INSTR(0, sline, "  {") == 1) then
    code << DISCLOSE(sline, "\"\"")
  endif
next i

print "*raylib*"
print "======="
print "raylib is a simple and easy-to-use library to enjoy videogames programming."
print
print "https://www.raylib.com/
print 
print "Implemented APIs (" + len(code) + ")"
print "----------------"
print ""
print "```"
sort code
for i = 0 to len(code) - 1
  ? code[i]
next i
print "```"
