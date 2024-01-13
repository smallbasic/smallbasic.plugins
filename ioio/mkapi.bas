rem
rem generate api methods from api.json
rem

tload "api.json", s, 1
api = array(s)

ioioApi = ["beginBatch", "disconnect", "endBatch", "hardReset", "softReset", "sync", "waitForConnect", "waitForDisconnect"]

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
  local cmd_name, err_name
  if (isstring(objName)) then
    cmd_name = lower(objName) + "_" + lname
    err_name = objName + "." + method.name
  else
    cmd_name = lname
    err_name = method.name
  endif

  print "static int cmd_" + cmd_name + "(var_s *self, int argc, slib_par_t *arg, var_s *retval) {"
  print "  int result = 0;"
  print "  if (argc != " + param_count + ") {"
  print "    error(retval, \"" + err_name + "\", " + param_count + ");"
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

sub generate_ioio_constructor()
  print "static void create_ioio(var_t *map) {"
  local s
  for s in ioioApi
    print "  v_create_callback(map, \"" + s + "\", cmd_" + lower(s) + ");"
  next
  print "}"
  print
end

sub generate_open_function(byref obj)
  print "static int cmd_open" + lower(obj.name) + "(int argc, slib_par_t *params, var_t *retval) {"
  print "  int result;"
  print "  int pin = get_param_int(argc, params, 0, 0);"
  print "  int id = ++nextId;"
  print "  IOClass &instance = _classMap[id];"
  print "  if (instance.create(CLASS_" + upper(obj.name) + ") &&"
  print "      instance.open(pin, retval)) {"
  print "    map_init_id(retval, id, CLASS_IOCLASS);"
  print "    create_" + lower(obj.name) + "(retval);"
  print "    if (get_param_int(argc, params, 1, 0)) {"
  print "      create_ioio(retval);"
  print "    }"
  print "    result = 1;"
  print "  } else {"
  print "    _classMap.erase(id);"
  print "    error(retval, \"open" + obj.name + "() failed\");"
  print "    result = 0;"
  print "  }"
  print "  return result;"
  print "}"
  print
end

for obj in api
  for method in obj.methods
    generate_command(obj.name, method)
  next
next

for s in ioioApi
  method.name = s
  method.rtn = "void"
  method.arg = "void"  
  generate_command(0, method)
next

for obj in api
  generate_constructor(obj)
next

generate_ioio_constructor()

for obj in api
  generate_open_function(obj)
next
