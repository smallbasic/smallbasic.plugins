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
  case "boundingbox": result = "get_param_bounding_box"
  case "bounds": result = "get_param_rect"
  case "camera": result = "get_camera_3d"
  case "camera2d": result = "get_camera_2d"
  case "camera3d": result = "get_camera_3d"
  case "char *": result = "get_param_str"
  case "color": result = "get_param_color"
  case "const char *": result = "get_param_str"
  case "const unsigned char *": result = "get_param_str"
  case "float": result = "get_param_num"
  case "font": result = "get_font_id"
  case "image": result = "get_image_id"
  case "int": result = "get_param_int"
  case "label": result = "get_param_str"
  case "matrix": result = "get_matrix_id"
  case "mesh": result = "get_mesh_id"
  case "model": result = "get_model_id"
  case "modelanimation": result = "get_model_animation_id"
  case "music": result = "get_music_id"
  case "physicsbody": result = "get_physics_body_id"
  case "ray": result = "get_param_ray"
  case "rectangle": result = "get_param_rect"
  case "rendertexture2d": result = "get_render_texture_id"
  case "shader": result = "get_param_shader"
  case "sound": result = "get_sound_id"
  case "text": result = "get_param_str"
  case "texture2d": result = "get_texture_id"
  case "unsigned char *": result = "get_param_str"
  case "unsigned int": result = "get_param_int"
  case "vector2": result = "get_param_vec2"
  case "vector3": result = "get_param_vec3"
  case "vector4": result = "get_param_vec4"
  case "wave": result = "get_param_wave"
  case else: throw "unknown param [" + param.type + "]"
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
  case "mesh": result = "_meshMap"
  case "model": result = "_modelMap"
  case "modelanimation": result = "_modelAnimationMap"
  case "music": result = "_musicMap"
  case "physicsbody": result = "_physicsMap"
  case "rendertexture2d": result = "_renderMap"
  case "sound": result = "_soundMap"
  case "texture2d": result = "_textureMap"
  case else: throw "unknown  map [_" + param.type + "Map]"
  end select
  return result
end

func get_v_set_name(byref fun)
  local result
  select case lower(trim(fun.returnType))
  case "bool": result = "v_setint"
  case "char *": result = "v_setstr"
  case "color": result = "v_setcolor"
  case "const char *": result = "v_setstr"
  case "double": result = "v_setreal"
  case "float": result = "v_setreal"
  case "font": result = "v_setfont"
  case "image": result = "v_setimage"
  case "int": result = "v_setint"
  case "long": result = "v_setint"
  case "matrix": result = "v_setmatrix"
  case "mesh": result = "v_setmesh"
  case "model": result = "v_setmodel"
  case "music": result = "v_setmusic"
  case "physicsbody": result = "v_setphysics"
  case "ray": result = "v_setray"
  case "raycollision": result = "v_setraycollision"
  case "rectangle": result = "v_setrect"
  case "rendertexture2d": result = "v_setrendertexture2d"
  case "shader": result = "v_setshader"
  case "shader": result = "v_setshader"
  case "sound": result = "v_setsound"
  case "texture2d": result = "v_settexture2d"
  case "unsigned char *": result = "v_setstr"
  case "unsigned int": result = "v_setint"
  case "vector2": result = "v_setvec2"
  case "vector3": result = "v_setvec3"
  case "vector4": result = "v_setvec4"
  case "wave": result = "v_setwave"
  case else: throw "unknown return [" + fun.returnType + "] "
  end select
  return result
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

func is_unsupported_type(type)
  return (type == "AudioStream" || &
          type == "BoundingBox" || &
          type == "Camera *" || &
          type == "Color *" || &
          type == "const GlyphInfo *" || &
          type == "GlyphInfo *" || &
          type == "GlyphInfo" || &
          type == "Image *" || &
          type == "LoadFileDataCallback" || &
          type == "LoadFileTextCallback" || &
          type == "Material *" || &
          type == "Material" || &
          type == "Mesh *" || &
          type == "Model *" || &
          type == "ModelAnimation *" || &
          type == "ModelAnimation*" || &
          type == "NPatchInfo" || &
          type == "SaveFileDataCallback" || &
          type == "SaveFileTextCallback" || &
          type == "Texture2D *" || &
          type == "TextureCubemap" || &
          type == "TraceLogCallback" || &
          type == "Vector2 *" || &
          type == "Vector3 *" || &
          type == "VrStereoConfig" || &
          type == "Wave *" || &
          type == "char **" || &
          type == "const char **" || &
          type == "const void *" || &
          type == "float *" || &
          type == "unsigned int *" || &
          type == "int *" || &
          type == "void *" || &
          type == "")
end

func is_unsupported(byref fun)
  local result = false
  local i = 0
  local param
  for param in fun.params
    if (is_unsupported_type(param.type) || (instr(param.type, "*") > 0 and not param.type == "const char *")) then
      result = true
      exit for
    endif
  next
  if (is_unsupported_type(fun.returnType)) then
    rem NOTE: main.cpp includes non-generated cmd_loadmodelanimations
    result = true
  endif
  return result
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

sub print_proc_main
  local fun, map_param
  for fun in api("functions")
    if (fun.returnType == "void" and not is_unsupported(fun)) then
      map_param = get_map_param(fun)
      if (map_param != -1) then
        print_proc_map(fun, map_param)
      else
        print_proc(fun)
      endif
    endif
  next
end

sub print_func(byref fun)
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
  print "  auto fnResult = " + fun.name + "(" + args + ");"
  print "  " + get_v_set_name(fun) + "(retval, fnResult);"
  print "  return 1;"
  print "}"
  print
end

sub print_func_map(byref fun, map_param)
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
  print "    auto fnResult = " + fun.name + "(" + args + ");"
  print "    " + get_v_set_name(fun) + "(retval, fnResult);"
  print "    result = 1;"
  print "  } else {"
  print "    result = 0;"
  print "  }"
  print "  return result;"
  print "}"
  print
end

sub print_func_main
  local fun, map_param
  for fun in api("functions")
    if (fun.returnType != "void" and not is_unsupported(fun)) then
      map_param = get_map_param(fun)
      if (map_param != -1) then
        print_func_map(fun, map_param)
      else
        print_func(fun)
      endif
    endif
  next
end

sub print_def(proc_def)
  local fun, n
  for fun in api("functions")
    if (((fun.returnType == "void") == proc_def) and not is_unsupported(fun)) then
      print "  {" + len(fun.params) + ", " + len(fun.params) + ", \"" + upper(fun.name) + "\", cmd_" + lower(fun.name) + "},"
    endif
  next
end

if trim(command) == "proc.h" then
  print_proc_main
else if trim(command) == "proc-def.h" then
  print_def(true)
elseif trim(command) == "func.h" then
  print_func_main
else if trim(command) == "func-def.h" then
  print_def(false)
endif

