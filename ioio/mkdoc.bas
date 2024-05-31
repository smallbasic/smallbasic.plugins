rem
rem generate api methods from api.json
rem

tload "api.json", s, 1
api = array(s)

func get_signature(method)
  local result

  if (method.rtn == "void") then
    result = "void"
  else if (method.rtn == "boolean" || method.rtn == "int") then
    result = "int"
  else if (method.rtn == "float") then
    result = "float"
  endif

  result += " " + method.name + "("

  if (method.signature != 0) then
    result += method.signature
  else if (method.arg == "void") then
    result += "void"
  else if (method.arg == "boolean" || method.arg == "int") then
    result += "int"
  else if (method.arg == "float") then
    result += "float"
  endif

  result += ")"

  return result
end

print "# IOIO for SmallBASIC"
print
print "see: https://github.com/ytai/ioio/wiki"
print

for obj in api
  print "## " + obj.name
  print
  print obj.comment
  if (obj.name != "IOIO") then
    print
    signature = "(pin)"
    if (obj.signature) then signature = "(" + obj.signature + ")"
    print "`io = ioio.open" + obj.name + signature + "`"
  endif
  print ""
  print "| Name    | Description   |"
  print "|---------|---------------|"
  for method in obj.methods
    print "|" + get_signature(method) + "|" + method.comment  + "|"
  next
  print
next

