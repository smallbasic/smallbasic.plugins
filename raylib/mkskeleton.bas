rem
rem generate a skelton main.cpp from json input
rem
rem input file (functions.json) structure:
rem {
rem   "func": [{
rem       "name": "InitWindow",
rem       "args", ["screenWidth", "screenHeight", "title"]
rem     }, ...
rem   ]
rem   "sub": [{
rem       "name": "InitAudioDevice",
rem       "args": []
rem     }
rem   ]
rem }
rem

tload "functions.json", s, 1
skelton = array(s)
keywords = []
map = {}
const NL = chr(10)

func get_param_name(a)
  local result
  select case a
  case "alpha": result = "get_param_num"
  case "color": result = "get_param_color"
  case "text": result = "get_param_str"
  case "bounds": result = "get_param_rect"
  case "label": result = "get_param_str"
  case else
   result = "get_param_int"
  end select
  return result
end

func_api = "FUNC_SIG lib_func[] = {"
funcs = ""
for fun in skelton("func")
  funcs += NL + "static int cmd_" + lower(fun.name) + "(int argc, slib_par_t *params, var_t *retval) {"
  i = 0
  args = ""
  commented = iff(lower(left(fun.name, 2)) == "is", "", "//")
  'if (len(fun.args) < 4) then commented = ""
  for a in fun.args
    funcs += NL + "  " + commented + "auto " + a + " = " + get_param_name(a) + "(argc, params, " + i + ", 0);"
    if (i > 0) then args += ", "
    args += a
    i++
  next a
  funcs += NL + "  " + commented + "auto fnResult = " + fun.name + "(" + args + ");"
  funcs += NL + "  " + commented + "v_setint(retval, fnResult);"
  funcs += NL + "  return 1;"
  funcs += NL + "}" + NL
  func_api += NL + "  " + commented + "{" + len(fun.args) + ", " + len(fun.args) + ", \"" + upper(fun.name) + "\", cmd_" + lower(fun.name) + "},"
next fun
func_api += NL + "};" + NL

proc_api = "FUNC_SIG lib_proc[] = {"
for proc in skelton("sub")
  funcs += NL + "static int cmd_" + lower(proc.name) + "(int argc, slib_par_t *params, var_t *retval) {"
  i = 0
  args = ""
  commented = iff(len(fun.args) < 1, "", "//")
  for a in proc.args
    funcs += NL + "  " + commented + " auto " + a + " = " + get_param_name(a) + "(argc, params, " + i + ", 0);"
    if (i > 0) then args += ", "
    args += a
    i++
  next a
  if (len(proc.args) == 0) then
    funcs += NL + "    " + proc.name + "(" + args + ");"
    proc_api += NL + "   {0, 0, \"" + upper(proc.name) + "\", cmd_" + lower(proc.name) + "},"
  else
    funcs += NL + "    " + commented + " " + proc.name + "(" + args + ");"
    proc_api += NL + "  " + commented + " {" + len(proc.args) + ", " + len(proc.args) + ", \"" + upper(proc.name) + "\", cmd_" + lower(proc.name) + "},"
  endif
  funcs += NL + "  return 1;"
  funcs += NL + "}" + NL
next proc
proc_api += NL + "};" + NL

sub print_api
  print "SBLIB_API int sblib_proc_count() {"
  print "  return (sizeof(lib_proc) / sizeof(lib_proc[0]));"
  print "}"
  print ""
  print "SBLIB_API int sblib_func_count() {"
  print "  return (sizeof(lib_func) / sizeof(lib_func[0]));"
  print "}"
  print ""
  print "SBLIB_API int sblib_proc_getname(int index, char *proc_name) {"
  print "  int result;"
  print "  if (index < sblib_proc_count()) {"
  print "    strcpy(proc_name, lib_proc[index]._name);"
  print "    result = 1;"
  print "  } else {"
  print "    result = 0;"
  print "  }"
  print "  return result;"
  print "}"
  print ""
  print "SBLIB_API int sblib_func_getname(int index, char *proc_name) {"
  print "  int result;"
  print "  if (index < sblib_func_count()) {"
  print "    strcpy(proc_name, lib_func[index]._name);"
  print "    result = 1;"
  print "  } else {"
  print "    result = 0;"
  print "  }"
  print "  return result;"
  print "}"
  print ""
  print "SBLIB_API int sblib_proc_exec(int index, int argc, slib_par_t *params, var_t *retval) {"
  print "  int result;"
  print "  if (index >= 0 && index < sblib_proc_count()) {"
  print "    if (argc < lib_proc[index]._min || argc > lib_proc[index]._max) {"
  print "      if (lib_proc[index]._min == lib_proc[index]._max) {"
  print "        error(retval, lib_proc[index]._name, lib_proc[index]._min);"
  print "      } else {"
  print "        error(retval, lib_proc[index]._name, lib_proc[index]._min, lib_proc[index]._max);"
  print "      }"
  print "      result = 0;"
  print "    } else {"
  print "      result = lib_proc[index]._command(argc, params, retval);"
  print "    }"
  print "  } else {"
  print "    fprintf(stderr, \"PROC index error [%d]\\n\", index);"
  print "    result = 0;"
  print "  }"
  print "  return result;"
  print "}"
  print ""
  print "SBLIB_API int sblib_func_exec(int index, int argc, slib_par_t *params, var_t *retval) {"
  print "  int result;"
  print "  if (index >= 0 && index < sblib_func_count()) {"
  print "    if (argc < lib_func[index]._min || argc > lib_func[index]._max) {"
  print "      if (lib_func[index]._min == lib_func[index]._max) {"
  print "        error(retval, lib_func[index]._name, lib_func[index]._min);"
  print "      } else {"
  print "        error(retval, lib_func[index]._name, lib_func[index]._min, lib_func[index]._max);"
  print "      }"
  print "      result = 0;"
  print "    } else {"
  print "      result = lib_func[index]._command(argc, params, retval);"
  print "    }"
  print "  } else {"
  print "    fprintf(stderr, \"FUNC index error [%d]\\n\", index);"
  print "    result = 0;"
  print "  }"
  print "  return result;"
  print "}"
  print ""
  print "SBLIB_API int sblib_events(int wait_flag, int *w, int *h) {"
  print "  return 0;"
  print "}"
  print ""
  print "SBLIB_API int sblib_init(void) {"
  print "  return 1;"
  print "}"
  print ""
  print "SBLIB_API void sblib_close(void) {"
  print "}"
end

print funcs
print func_api
print proc_api
print_api
