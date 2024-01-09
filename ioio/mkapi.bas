rem
rem generate api methods from api.json
rem

tload "api.json", s, 1
api = array(s)

func get_method_name(method)
  local result

  if (method.rtn == "void") then
    result = "invokeVoid"
  else if (method.rtn == "boolean" || method.rtn == "int") then
    result = "invokeInt"
  else if (method.rtn == "float") then
    result = "invokeFloat"
  endif

  if (method.arg == "void") then
    result += "Void"
  else if (method.arg == "boolean" || method.arg == "int") then
    result += "Int"
  else if (method.arg == "float") then
    result += "Float"
  endif

  return result
end

sub generate_command(objName, method)
  local lname = lower(method.name)
  local uname = upper(method.name)
  local param_count = iff(method.arg == "void", 0, 1)
  local invoke = get_method_name(method)

  print "static int cmd_" + lower(objName) + "_" + lname + "(var_s *self, int argc, slib_par_t *arg, var_s *retval) {"
  print "  int result = 0;"
  print "  if (argc != " + param_count + ") {"
  print "    error(retval, \"" + objName + "." + method.name + "\", " + param_count + ");"
  print "  } else {"
  print "    int id = get_io_class_id(self, retval);"
  print "    if (id != -1) {"

  local argument = ""
  if (method.arg == "boolean" || method.arg == "int") then
    print "      auto value = get_param_int(argc, arg, 0, 0);"
    argument = ", value"
  else if (method.arg == "float") then
    print "      auto value = get_param_num(argc, arg, 0, 0);"
    argument = ", value"
  endif

  if (method.rtn == "void") then
    print "      result = _classMap.at(id)." + invoke + "(\"" + method.name + "\"" + argument + ", retval);"
  else if (method.rtn == "boolean" || method.rtn == "int") then
    print "      result = _classMap.at(id)." + invoke + "(\"" + method.name + "\"" + argument + ", retval);"
  else if (method.rtn == "float") then
    print "      result = _classMap.at(id)." + invoke + "(\"" + method.name + "\"" + argument + ", retval);"
  endif
  print "    }"
  print "  }"
  print "  return result;"
  print "}"
  print
end

sub generate_constructor(byref obj)
  print "static void create_" + lower(obj.name) + "(var_t *map) {"
  local method
  for method in obj.methods
     print "  v_create_callback(map, \"" + method.name + "\", cmd_" + lower(obj.name) + "_" + lower(method.name) + ");"
  next
  print "}"
  print
end

for obj in api
  for method in obj.methods
    generate_command(obj.name, method)
  next
next

for obj in api
  generate_constructor(obj)
next
