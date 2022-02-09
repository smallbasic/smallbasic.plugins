rem
rem generate a skelton main.cpp from json input
rem

tload "raylib/parser/raylib_api.json", s, 1
api = array(s)

func comparator(l, r)
  local f1 = lower(l.name)
  local f2 = lower(r.name)
  return iff(f1 == f2, 0, iff(f1 > f2, 1, -1))
end

sort api("functions") use comparator(x, y)

func get_param_name(byref param)
  local result
  select case lower(trim(param.type))
  case "alpha": result = "get_param_num"
  case "color": result = "get_param_color"
  case "text": result = "get_param_str"
  case "const char *": result = "get_param_str"
  case "bounds": result = "get_param_rect"
  case "label": result = "get_param_str"
  case "camera3d": result = "get_camera_3d"
  case "font": result = "get_font_id"
  case "image": result = "get_image_id"
  case "model": result = "get_model_id"
  case "modelanimation": result = "get_model_animation_id"
  case "music": result = "get_music_id"
  case "physicsbody": result = "get_physics_body_id"
  case "rendertexture2d": result = "get_render_texture_id"
  case "sound": result = "get_sound_id"
  case "texture2d": result = "get_texture_id"
  case "boundingbox": result = "get_param_bounding_box"
  case else
   result = "get_param_int"
  end select
  return result
end

func has_default_param(byref param)
  local result
  select case lower(trim(param.type))
  case "int": result = true
  case "int *": result = true
  case "text": result = true
  case "const char *": result = true
  case else
   result = false
  end select
  return result
end

func get_map_name(byref param)
  local result
  select case lower(trim(param.type))
  case "font": result = "_fontMap"
  case "image": result = "_imageMap"
  case "model": result = "_modelMap"
  case "modelanimation": result = "_modelAnimationMap"
  case "music": result = "_musicMap"
  case "physicsbody": result = "_physicsMap"
  case "rendertexture2d": result = "_renderMap"
  case "sound": result = "_soundMap"
  case "texture2d": result = "_textureMap"
  case else: result = lower(trim(param.type)) + "Map"
  end select
  return result
end

func get_v_set_name(byref fun)
  local result
  select case lower(trim(fun.returnType))
  case "color": result = "v_setcolor"
  case "font": result = "v_setfont"
  case "image": result = "v_setimage"
  case "int": result = "v_setint"
  case "mesh": result = "v_setmesh"
  case "model": result = "v_setmodel"
  case "modelanimation": result = "v_setmodel_animation"
  case "physicsbody": result = "v_setphysics"
  case "raycollision": result = "v_setraycollision"
  case "float": result = "v_setreal"
  case "rectangle": result = "v_setrect"
  case "Shader": result = "v_setshader"
  case "char": result = "v_setstr"
  case "texture2d": result = "v_settexture2d"
  case "vector2": result = "v_setvec2"
  case "vector3": result = "v_setvec3"
  case else: result = "unknown name" + fun.name
  end select
  return result
end

sub print_proc_map(byref fun, map_param)
  print "static int cmd_" + lower(fun.name) + "(int argc, slib_par_t *params, var_t *retval) {"
  print "  int result;"
  print "  int id = " + get_param_name(fun.params[map_param]) + "(argc, params, " + map_param + ", retval);"
  print "  if (id != -1) {"
  local i = 0
  local args = ""
  local param
  local def_arg
  for param in fun.params
    if (i > 0) then args += ", "
    if (i != map_param) then
      def_arg = iff(has_default_param(param), ", 0", "")
      print "    auto " + lower(param.name) + " = " + get_param_name(param) + "(argc, params, " + i + def_arg + ");"
      args += param.name
    else
      args += get_map_name(fun.params[map_param]) + ".at(id)"
    endif
    i++
  next
  print "    " + fun.name + "(" + args + ");"
  print "    result = 1;"
  print "  } else {"
  print "    result = 0;"
  print "  }"
  print "  return result;"
  print "}"
  print
end

sub print_proc(byref fun)
  print "static int cmd_" + lower(fun.name) + "(int argc, slib_par_t *params, var_t *retval) {"
  local i = 0
  local args = ""
  local param
  local def_arg
  for param in fun.params
    def_arg = iff(has_default_param(param), ", 0", "")
    print "  auto " + lower(param.name) + " = " + get_param_name(param) + "(argc, params, " + i + def_arg + ");"
    if (i > 0) then args += ", "
    args += param.name
    i++
  next
  print "  " + fun.name + "(" + args + ");"
  print "  return 1;"
  print "}"
  print
end

func get_map_param(byref fun)
  local result = -1
  local i = 0
  local param
  for param in fun.params
    if (param.type == "Font" || &
        param.type == "Image" || &
        param.type == "Mesh" || &
        param.type == "Model" || &
        param.type == "ModelAnimation" || &
        param.type == "RenderTexture2D" || &
        param.type == "Sound" || &
        param.type == "Texture2D") then
      result = i
      exit for
    endif
    i++
  next
  return result
end

func has_ptr_arg(byref fun)
  local result = false
  local i = 0
  local param
  for param in fun.params
    if (instr(param.type, "*") > 0 and not param.type == "const char *") then
      result = true
      exit for
    endif
  next
  return result
end

sub print_proc_main
  local fun, map_param
  for fun in api("functions")
    if (fun.returnType == "void" and not has_ptr_arg(fun)) then
      map_param = get_map_param(fun)
      if (map_param != -1) then
        print_proc_map(fun, map_param)
      else
        print_proc(fun)
      endif
    endif
  next
end

sub print_func_main
  local fun, map_param
  for fun in api("functions")
    if (fun.returnType == "void" and not has_ptr_arg(fun)) then
      map_param = get_map_param(fun)
      if (map_param != -1) then
        print_proc_map(fun, map_param)
      else
        print_proc(fun)
      endif
    endif
  next
end

sub print_def(proc_def)
  local fun, n
  for fun in api("functions")
    if (((fun.returnType == "void") == proc_def) and not has_ptr_arg(fun)) then
      print "  {" + len(fun.params) + ", " + len(fun.params) + ", \"" + upper(fun.name) + "\", cmd_" + lower(fun.name) + "},"
    endif
  next
end

if trim(command) == "proc" then
  print_proc_main
else if trim(command) == "proc-def" then
  print_def(true)
elseif trim(command) == "func" then
  print_func_main
else if trim(command) == "func-def" then
  print_def(false)
endif


