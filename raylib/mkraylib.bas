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
  case else
   result = "get_param_int"
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

sub print_func_map(byref fun, map_param)
  print "static int cmd_" + lower(fun.name) + "(int argc, slib_par_t *params, var_t *retval) {"
  print "  int result;"
  print "  int id = " + get_param_name(fun.params[map_param]) + "(argc, params," + map_param + ");"
  print "  if (id != -1) {"
  local i = 0
  local args = ""
  local param
  for param in fun.params
    if (i > 0) then args += ", "
    if (i != map_param) then
      print "    auto " + lower(param.name) + " = " + get_param_name(param) + "(argc, params, " + i + ", 0);"
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

sub print_func(byref fun)
  print "static int cmd_" + lower(fun.name) + "(int argc, slib_par_t *params, var_t *retval) {"
  local i = 0
  local args = ""
  local param
  for param in fun.params
    print "  auto " + lower(param.name) + " = " + get_param_name(param) + "(argc, params, " + i + ", 0);"
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
    if (instr(param.type, "*") > 0) then
      result = true
      exit for
    endif
  next
  return result
end

for fun in api("functions")
  if (fun.returnType == "void" and not has_ptr_arg(fun)) then
    map_param = get_map_param(fun)
    if (map_param != -1) then
      print_func_map(fun, map_param)
    else
      print_func(fun)
    endif
  endif
next

