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
  case "bool": result = "get_param_int"
  case "camera": result = "get_camera_3d"
  case "camera2d": result = "get_camera_2d"
  case "camera3d": result = "get_camera_3d"
  case "char *": result = "(char *)get_param_str"
  case "color": result = "get_param_color"
  case "const char *": result = "get_param_str"
  case "const unsigned char *": result = "(const unsigned char *)get_param_str"
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
  case "unsigned char *": result = "(unsigned char *)get_param_str"
  case "unsigned int": result = "get_param_int"
  case "vector2": result = "get_param_vec2"
  case "vector3": result = "get_param_vec3"
  case "vector4": result = "get_param_vec4"
  case "wave": result = "get_wave_id"
  case "camera *": result = "(Camera *)get_param_int_t"
  case "color *": result = "(Color *)get_param_int_t"
  case "image *": result = "get_image_id"
  case "mesh *": result = "(Mesh *)get_param_int_t"
  case "model *": result = "(Model *)get_param_int_t"
  case "modelanimation *": result = "(ModelAnimation *)get_param_int_t"
  case "modelanimation*": result = "(ModelAnimation *)get_param_int_t"
  case "texture2d *": result = "(Texture2D *)get_param_int_t"
  case "vector2 *": result = "(Vector2 *)get_param_vec2_array"
  case "vector3 *": result = "(Vector3 *)get_param_vec3_array"
  case "wave *": result = "(Wave *)get_param_int_t"
  case "const void *": result = "(const void *)get_param_int_t"
  case "float *": result = "(float *)get_param_int_t"
  case "unsigned int *": result = "(unsigned int *)get_param_int_t"
  case "int *": result = "(int *)get_param_int_t"
  case "void *": result = "(void *)get_param_int_t"
  case "const int *": result = "(const int *)get_param_int_t"
  case else: throw "unknown param [" + param.type + "]"
  end select
  return result
end

func has_default_param(byref param)
  local name = get_param_name(param)
  return instr(name, "get_param_int") > 0 || &
         instr(name, "get_param_num") > 0 || &
         instr(name, "get_param_str") > 0
end

func get_map_name(byref param)
  local result
  select case lower(trim(param.type))
  case "font": result = "_fontMap"
  case "image": result = "_imageMap"
  case "image *": result = "&_imageMap"
  case "matrix": result = "_matrixMap"
  case "mesh": result = "_meshMap"
  case "model": result = "_modelMap"
  case "modelanimation": result = "_modelAnimationMap"
  case "music": result = "_musicMap"
  case "physicsbody": result = "_physicsMap"
  case "rendertexture2d": result = "_renderMap"
  case "sound": result = "_soundMap"
  case "texture2d": result = "_textureMap"
  case "wave": result = "_waveMap"
  case else: throw "unknown  map [_" + param.type + "Map]"
  end select
  return result
end

func get_v_set_name(byref fun)
  local result
  select case lower(trim(fun.returnType))
  case "bool": result = "v_setint"
  case "boundingbox": result = "v_setboundingbox"
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
  case "camera *": result = "v_setint"
  case "color *": result = "v_setint"
  case "image *": result = "v_setint"
  case "mesh *": result = "v_setint"
  case "model *": result = "v_setint"
  case "modelanimation *": result = "v_setint"
  case "modelanimation*": result = "v_setint"
  case "texture2d *": result = "v_setint"
  case "vector2 *": result = "v_setint"
  case "vector3 *": result = "v_setint"
  case "wave *": result = "v_setint"
  case "const void *": result = "v_setint"
  case "float *": result = "v_setint"
  case "unsigned int *": result = "v_setint"
  case "int *": result = "v_setint"
  case "void *": result = "v_setint"
  case else: throw "unknown return [" + fun.returnType + "] "
  end select
  return result
end

func get_result_cast(byref fun)
  local result
  select case lower(trim(fun.returnType))
  case "char *": result = "(const char *)"
  case "const char *": result = "(const char *)"
  case "unsigned char *": result = "(const char *)"
  case "camera *": result = "(var_int_t)"
  case "color *": result = "(var_int_t)"
  case "image *": result = "(var_int_t)"
  case "mesh *": result = "(var_int_t)"
  case "model *": result = "(var_int_t)"
  case "modelanimation *": result = "(var_int_t)"
  case "modelanimation*": result = "(var_int_t)"
  case "texture2d *": result = "(var_int_t)"
  case "vector2 *": result = "(var_int_t)"
  case "vector3 *": result = "(var_int_t)"
  case "wave *": result = "(var_int_t)"
  case "const void *": result = "(var_int_t)"
  case "float *": result = "(var_int_t)"
  case "unsigned int *": result = "(var_int_t)"
  case "int *": result = "(var_int_t)"
  case "void *": result = "(var_int_t)"
  case else: result = ""
  end select
  return result
end

func is_map_param(type)
  return type == "Font" || &
         type == "Image" || &
         type == "Image *" || &
         type == "Matrix" || &
         type == "Mesh" || &
         type == "Model" || &
         type == "ModelAnimation" || &
         type == "Music" || &
         type == "RenderTexture2D" || &
         type == "Sound" || &
         type == "Texture2D" || &
         type == "Wave"
end

func has_map_param(byref fun)
  local result = false
  local param
  for param in fun.params
    if (is_map_param(param.type)) then
      result = true
      exit for
    endif
  next
  return result
end

func is_unsupported_type(type)
  return type == "AudioStream" || &
         type == "const GlyphInfo *" || &
         type == "GlyphInfo *" || &
         type == "GlyphInfo" || &
         type == "LoadFileDataCallback" || &
         type == "LoadFileTextCallback" || &
         type == "Material *" || &
         type == "Material" || &
         type == "NPatchInfo" || &
         type == "SaveFileDataCallback" || &
         type == "SaveFileTextCallback" || &
         type == "TextureCubemap" || &
         type == "TraceLogCallback" || &
         type == "VrStereoConfig" || &
         type == "char **" || &
         type == "const char **" || &
         type == ""
end

func is_unsupported(byref fun)
  local result = false
  local i = 0
  local param
  for param in fun.params
    if (is_unsupported_type(param.type) || (instr(param.type, "*") > 0 and not param.type == "const char *")) then
      rem print "  // not generated: " + fun.name + " " + param.type
      result = true
      exit for
    endif
  next
  if (is_unsupported_type(fun.returnType)) then
    rem NOTE: main.cpp includes non-generated cmd_loadmodelanimations
    result = true
  endif
  if (fun.name == "GetMeshBoundingBox" || &
      fun.name == "LoadModelAnimations" || &
      fun.name == "LoadRenderTexture" || &
      fun.name == "LoadShader" || &
      fun.name == "SetShaderValue" || &
      fun.name == "UpdateCamera" || &
      fun.name == "UpdateTexture") then
    ' non-generated version in main.cpp
    result = true
  endif
  return result
end

sub print_description
  print "//"
  print "// " + fun.description
  print "//"
end

sub print_func_map(byref fun)
  print_description
  print "static int cmd_" + lower(fun.name) + "(int argc, slib_par_t *params, var_t *retval) {"
  print "  int result;"

  local ifStatement = "  if ("
  local andClause = ""
  local i = 0
  local param

  for param in fun.params
    if (is_map_param(param.type)) then
      print "  int " + lower(param.name) + "_id = " + get_param_name(param) + "(argc, params, " + i + ", retval);"
      ifStatement += andClause + lower(param.name) + "_id != -1"
      andClause = " && "
    endif
    i++
  next
  print ifStatement + ") {"

  i = 0
  local args = ""
  local def_arg

  for param in fun.params
    if (i > 0) then args += ", "
    if (!is_map_param(param.type)) then
      def_arg = iff(has_default_param(param), ", 0", "")
      print "    auto " + param.name + " = " + get_param_name(param) + "(argc, params, " + i + def_arg + ");"
      args += param.name
    else
      args += get_map_name(param) + ".at(" + lower(param.name) + "_id)"
    endif
    i++
  next

  if (fun.returnType == "void") then
    print "    " + fun.name + "(" + args + ");"
  else
    print "    auto fnResult = " + get_result_cast(fun) + fun.name + "(" + args + ");"
    print "    " + get_v_set_name(fun) + "(retval, " + "fnResult);"
  endif
  print "    result = 1;"
  print "  } else {"
  print "    result = 0;"
  print "  }"
  print "  return result;"
  print "}"
  print
end

sub print_func(byref fun)
  print_description
  print "static int cmd_" + lower(fun.name) + "(int argc, slib_par_t *params, var_t *retval) {"
  local i = 0
  local args = ""
  local param
  local def_arg
  for param in fun.params
    def_arg = iff(has_default_param(param), ", 0", "")
    print "  auto " + param.name + " = " + get_param_name(param) + "(argc, params, " + i + def_arg + ");"
    if (i > 0) then args += ", "
    args += param.name
    i++
  next

  if (fun.returnType == "void") then
    print "  " + fun.name + "(" + args + ");"
  else
    print "  auto fnResult = " + get_result_cast(fun) + fun.name + "(" + args + ");"
    print "  " + get_v_set_name(fun) + "(retval, fnResult);"
  endif
  print "  return 1;"
  print "}"
  print
end

sub print_proc_main
  local fun
  for fun in api("functions")
    if (fun.returnType == "void" and not is_unsupported(fun)) then
      if (has_map_param(fun)) then
        print_func_map(fun)
      else
        print_func(fun)
      endif
    endif
  next
end

sub print_func_main
  local fun, map_param
  for fun in api("functions")
    if (fun.returnType != "void" and not is_unsupported(fun)) then
      if (has_map_param(fun)) then
        print_func_map(fun)
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
      n = iff(isarray(fun.params), len(fun.params), 0)
      print "  {" + n + ", " + n + ", \"" + upper(fun.name) + "\", cmd_" + lower(fun.name) + "},"
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

