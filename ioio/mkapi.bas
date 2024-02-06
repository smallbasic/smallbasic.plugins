rem
rem generate api methods from api.json
rem

tload "api.json", s, 1
api = array(s)

func get_method_name(method)
  local result

  if (method.rtn == "void") then
    result = "invokeVoid"
  else if (method.rtn == "int") then
    result = "invokeInt"
  else if (method.rtn == "boolean") then
    result = "invokeBool"
  else if (method.rtn == "float") then
    result = "invokeFloat"
  endif

  if (method.arg == "void") then
    result += "Void"
  else if (method.arg == "int") then
    result += "Int"
  else if (method.arg == "boolean") then
    result += "Bool"
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

  if (objName == "IOIO") then
    print "static int cmd_" + cmd_name + "(int argc, slib_par_t *arg, var_s *retval) {"
  else
    print "static int cmd_" + cmd_name + "(var_s *self, int argc, slib_par_t *arg, var_s *retval) {"
  endif
  print "  int result = 0;"
  print "  if (argc != " + param_count + ") {"
  print "    error(retval, \"" + err_name + "\", " + param_count + ");"
  print "  } else {"
 
  local getter, indent
  if (objName == "IOIO") then
    getter = "ioioTask->"
    indent = "    "
  else 
    getter = "_ioTaskMap.at(id)."
    indent = "      "
    print "    int id = get_io_class_id(self, retval);"
    print "    if (id != -1) {"
  endif    

  local argument = ""
  if (method.arg == "boolean" || method.arg == "int") then
    print indent + "auto value = get_param_int(argc, arg, 0, 0);"
    argument = ", value"
  else if (method.arg == "float") then
    print indent + "auto value = get_param_num(argc, arg, 0, 0);"
    argument = ", value"
  endif

  if (method.rtn == "void") then
    print indent + "result = " + getter + invoke + "(\"" + method.name + "\"" + argument + ", retval);"
  else if (method.rtn == "boolean" || method.rtn == "int") then
    print indent + "result = " + getter + invoke + "(\"" + method.name + "\"" + argument + ", retval);"
  else if (method.rtn == "float") then
    print indent + "result = " + getter + invoke + "(\"" + method.name + "\"" + argument + ", retval);"
  endif
  if (objName != "IOIO") then
    print "    }"
  endif
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

sub generate_open_function(byref obj)
  local pin
  local openFunc = "open(pin1, "
  print "static int cmd_open" + lower(obj.name) + "(int argc, slib_par_t *params, var_t *retval) {"
  print "  int result;"
  print "  int pin1 = get_param_int(argc, params, 0, -1);"
  if (isnumber(obj.pins) && obj.pins > 0) then
    openFunc = "open" + obj.pins + "(pin1, "
    for pin = 2 to obj.pins
      openFunc += "pin" + pin + ", "
      print "  int pin" + pin + " = get_param_int(argc, params, " + (pin - 1) + ", -1);"
    next 
  endif
  print "  int id = ++nextId;"
  print "  IOTask &instance = _ioTaskMap[id];"
  print "  if (instance.create(CLASS_" + upper(obj.name) + ") &&"
  print "      instance." + openFunc + "retval)) {"
  print "    map_init_id(retval, id, CLASS_IOTASK_ID);"
  print "    create_" + lower(obj.name) + "(retval);"
  print "    result = 1;"
  print "  } else {"
  print "    _ioTaskMap.erase(id);"
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

for obj in api
  if (len(obj.methods) > 0 && obj.name != "IOIO") then
    generate_constructor(obj)
  endif
next

for obj in api
  if (obj.name != "IOIO") then
    generate_open_function(obj)
  endif    
next

