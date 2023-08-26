rem
rem gtk-server module for SmallBASIC
rem

unit gtk

import gtkserver as gtks

export g_window
export g_attach
export g_button
export g_check
export g_radio
export g_entry
export g_password
export g_label
export g_droplist
export g_text
export g_separator
export g_frame
export g_list
export g_get_text
export g_get_value
export g_set_value
export g_focus
export g_hide
export g_show
export g_event
export g_async
export g_version
export g_logging
export g_set_text
export g_fg_color
export g_bg_color
export g_disable
export g_enable

gtk_init=0
gtk_container={}
gtk_type={}
gtk_start_iter={}
gtk_end_iter={}
gtk_text_view={}
gtk_combo_bound={}
gtk_list_store={}
gtk_list_iter={}
gtk_list_select={}
gtk_list_array={}

gtks.gtk("gtk_init NULL NULL")

rem
rem UTF-8 conversion for high ASCII
rem
func g_UTF8(st)
  local t, x, b1, b2

  t = 1
  while t <= LEN(st)
    x = ASC(MID(st, t, 1))
    if x > 127 then
      b1 = CHR((x AND 192) / 64 + 192)
      b2 = CHR((x AND 63) + 128)
      st = LEFT(st, t - 1) + b1 + b2 + RIGHT(st, LEN(st) - t - 1)
      t+=1
    end if
    t+=1
  Wend
  return st
end

rem
rem Window creation starts here
rem
func g_window(title, xsize, ysize)
  local win = gtks.gtk("gtk_window_new 0")
  gtks.gtk("gtk_window_set_title %d \"%s\"", win, title)
  gtks.gtk("gtk_widget_set_size_request %d %d %d", win, xsize, ysize)
  gtks.gtk("gtk_window_set_resizable %d 0", win)
  gtks.gtk("gtk_widget_set_name %d %d", win, win)
  gtks.gtk("gtk_widget_show %d", win)

  local fixed = gtks.gtk("gtk_fixed_new")
  gtks.gtk("gtk_container_add %d %d", win, fixed)
  gtks.gtk("gtk_widget_show %d", fixed)

  GTK_CONTAINER[win] = fixed
  GTK_TYPE[win] = "window"

  return win
end

rem
rem Parentize
rem
sub g_attach(widget, parent, x, y)
  local fixed = GTK_CONTAINER[parent]
  gtks.gtk("gtk_fixed_put %s %s %s %s", fixed, widget, x, y)
end

rem
rem Button creation starts here
rem
func g_button(text, xsize, ysize)
  local but = gtks.gtk("gtk_button_new_with_label %s", text)
  gtks.gtk("gtk_widget_set_size_request %s %s %s", but, xsize, ysize)
  gtks.gtk("gtk_widget_set_name %s %s", but, but)
  gtks.gtk("gtk_widget_show %s", but)
  GTK_TYPE[but] = "button"
  return but
end

rem
rem Check Button creation starts here
rem
func g_check(text, xsize, ysize)
  local chk = gtks.gtk("gtk_check_button_new_with_label \"" + text + "\"")
  gtks.gtk("gtk_widget_set_size_request %s %s %s", chk, xsize, ysize)
  gtks.gtk("gtk_widget_set_name %s %s", chk, chk)
  gtks.gtk("gtk_widget_show %s", chk)
  GTK_TYPE[chk] = "check"
  return chk
end

rem
rem Radio Button creation starts here
rem
func g_radio(text, xsize, ysize, group)
  local value = gtks.gtk("gtk_radio_button_new_with_label_from_widget %s %s", "\"" + STR(group) + "\"", "\"" + text + "\"")
  gtks.gtk("gtk_widget_set_size_request %s %s %s", value, xsize, ysize)
  gtks.gtk("gtk_widget_set_name %s %s", value, value)
  gtks.gtk("gtk_widget_show %s", value)
  GTK_TYPE[value] = "radio"
  return value
end

rem
rem Entry creation starts here
rem
func g_entry(xsize, ysize)
  local ent = gtks.gtk("gtk_entry_new")
  gtks.gtk("gtk_widget_set_size_request %s %s %s", ent, xsize, ysize)
  gtks.gtk("gtk_widget_set_name %s %s", ent, ent)
  gtks.gtk("gtk_widget_show %s", ent)
  GTK_TYPE[ent] = "entry"
  return ent
end

rem
rem Password creation starts here
rem
func g_password(xsize, ysize)
  local pwd = gtks.gtk("gtk_entry_new")
  gtks.gtk("gtk_widget_set_size_request %s %s %s", pwd, xsize, ysize)
  gtks.gtk("gtk_entry_set_visibility %s", pwd, 0)
  gtks.gtk("gtk_widget_set_name %s %s", pwd, pwd)
  gtks.gtk("gtk_widget_show %s", pwd)
  GTK_TYPE[pwd] = "password"
  return pwd
end

rem
rem Label creation starts here
rem
func g_label(text, xsize, ysize)
  local lab = gtks.gtk("gtk_label_new \"" + text + "\"")
  gtks.gtk("gtk_widget_set_size_request %s %s %s", lab, xsize, ysize)
  gtks.gtk("gtk_widget_set_name %s %s", lab, lab)
  gtks.gtk("gtk_widget_show %s", lab)
  GTK_TYPE[lab] = "label"
  return lab
end

rem
rem DropList creation starts here
rem
func g_droplist(text, xsize, ysize)
  local drop, i
  drop = gtks.gtk("gtk_combo_box_text_new")
  if not ismap(text) then
    PRINT "WARNING: Pass an array to create a droplist!\n"
    GTK_COMBO_BOUND[drop] = 0
  else
    for i in text
      gtks.gtk("gtk_combo_box_text_append %s %s", drop, "\"" + text[i] + "\"")
    next i
    GTK_COMBO_BOUND[drop] = text
  end if

  gtks.gtk("gtk_combo_box_set_active %s 0", drop)
  gtks.gtk("gtk_widget_set_size_request %s %s %s", drop, xsize, ysize)
  gtks.gtk("gtk_widget_set_name %s %s", drop, drop)
  gtks.gtk("gtk_widget_show %s %s", drop)
  GTK_TYPE[drop] = "droplist"
  return drop
end

rem
rem Multiline text starts here
rem
func g_text(xsize, ysize)
  local buf, vw, sw
  buf = gtks.gtk("gtk_text_buffer_new NULL")
  vw = gtks.gtk("gtk_text_view_new_with_buffer %s", buf)
  gtks.gtk("gtk_text_view_set_wrap_mode %s", vw, 1)
  gtks.gtk("gtk_widget_set_name %s %s", vw, vw)
  gtks.gtk("gtk_widget_show %s", vw)

  sw = gtks.gtk("gtk_scrolled_window_new NULL NULL")
  gtks.gtk("gtk_scrolled_window_set_policy %s %s %s", sw, 1, 1)
  gtks.gtk("gtk_scrolled_window_set_shadow_type %s %s", sw, 1)
  gtks.gtk("gtk_container_add %s %s", sw, vw)
  gtks.gtk("gtk_text_view_set_editable %s %s", vw, 1)
  gtks.gtk("gtk_text_view_set_wrap_mode %s %s", vw, 2)
  gtks.gtk("gtk_widget_set_size_request %s %s", sw, xsize, ysize)
  gtks.gtk("gtk_widget_show %s %s", sw)

  GTK_START_ITER[sw] = gtks.gtk("gtk_server_opaque")
  GTK_end_ITER[sw] = gtks.gtk("gtk_server_opaque")
  GTK_TEXT_VIEW[sw] = vw
  GTK_CONTAINER[sw] = buf
  GTK_TYPE[sw] = "text"
  return sw
end

rem
rem Separator creation starts here
rem
func g_separator(xsize)
  local sep = gtks.gtk("gtk_hseparator_new")
  gtks.gtk("gtk_widget_set_size_request %s %s %s", sep, xsize, 0)
  gtks.gtk("gtk_widget_set_name %s %s", sep, sep)
  gtks.gtk("gtk_widget_show %s", sep)
  GTK_TYPE[sep] = "separator"
  return sep
end

rem
rem frame) creation starts here
rem
func g_frame(text, xsize, ysize)
  local frm = gtks.gtk("gtk_frame_new NULL")
'  gtks.gtk("gtk_frame_set_label %s \"%s\"", frm, text)
  gtks.gtk("gtk_widget_set_size_request %s %s %s", frm, xsize, ysize)
  gtks.gtk("gtk_widget_set_name %s %s", frm, frm)
  gtks.gtk("gtk_widget_show %s %s", frm)
  GTK_TYPE[frm] = "frame"
  return frm
end

rem
rem List widget
rem
func g_list(text, xsize, ysize)
  local lst, iter, tree, column, sel, sw, i
  iter = gtks.gtk("gtk_server_opaque")
  lst = gtks.gtk("gtk_list_store_new 1 64")
  tree = gtks.gtk("gtk_tree_view_new_with_model %s", lst)
  gtks.gtk("gtk_tree_view_set_headers_visible %s 0", tree)
  gtks.gtk("gtk_widget_set_name %s %s", tree, tree)
  gtks.gtk("gtk_server_connect %s %s %s 1", tree, "button-press-event", tree)

  sel = gtks.gtk("gtk_tree_view_get_selection %s", tree)
  gtks.gtk("gtk_tree_selection_set_mode %s 1", sel)
  column = gtks.gtk("gtk_tree_view_column_new_with_attributes dummy %s text 0 NULL", gtks.gtk("gtk_cell_renderer_text_new"))
  gtks.gtk("gtk_tree_view_append_column %s %s", tree, column)

  sw = gtks.gtk("gtk_scrolled_window_new NULL NULL")
  gtks.gtk("gtk_scrolled_window_set_policy %s 1 1", sw)
  gtks.gtk("gtk_scrolled_window_set_shadow_type %s 1", sw)
  gtks.gtk("gtk_container_add %s %s", sw, tree)
  gtks.gtk("gtk_widget_set_size_request %s %s %s", sw, xsize, ysize)
  gtks.gtk("gtk_widget_show_all %s", sw)

  if !ismap(text) then
    PRINT "WARNING: Pass an array to create a list!\n"
  else
    for i in text
      gtks.gtk("gtk_list_store_append %s %s", lst, iter)
      gtks.gtk("gtk_list_store_set %s \"%s\" 0 %s -1", lst, iter, text[i])
    next i
    GTK_LIST_ARRAY[sw] = text
  end if
  GTK_LIST_STORE[sw] = lst
  GTK_LIST_ITER[sw] = iter
  GTK_LIST_SELECT[sw] = sel
  GTK_CONTAINER[sw] = tree
  GTK_TYPE[sw] = "list"
  return sw
end

rem
rem Get text from widget
rem
func g_get_text(widget)
  local arr
  local result
  if GTK_TYPE[widget] = "window" then get_text = gtks.gtk("gtk_window_get_title", widget)
  if GTK_TYPE[widget] = "button" then get_text = gtks.gtk("gtk_button_get_label", widget)
  if GTK_TYPE[widget] = "check" then get_text = gtks.gtk("gtk_button_get_label", widget)
  if GTK_TYPE[widget] = "radio" then get_text = gtks.gtk("gtk_button_get_label", widget)
  if GTK_TYPE[widget] = "entry" then get_text = gtks.gtk("gtk_entry_get_text", widget)
  if GTK_TYPE[widget] = "password" then get_text = gtks.gtk("gtk_entry_get_text", widget)
  if GTK_TYPE[widget] = "label" then get_text = gtks.gtk("gtk_label_get_text", widget)
  if GTK_TYPE[widget] = "droplist" then get_text = gtks.gtk("gtk_combo_box_get_active_text", widget)
  if GTK_TYPE[widget] = "text" then
    gtks.gtk("gtk_text_buffer_get_start_iter", GTK_CONTAINER[widget], GTK_START_ITER[widget])
    gtks.gtk("gtk_text_buffer_get_end_iter", GTK_CONTAINER[widget], GTK_end_ITER[widget])
    get_text = gtks.gtk("gtk_text_buffer_get_text", GTK_CONTAINER[widget], GTK_START_ITER[widget], GTK_end_ITER[widget], 1)
  end if
  if GTK_TYPE[widget] = "separator" then PRINT "WARNING: Cannot get text of " + GTK_TYPE[widget] + " widget!\n"
  if GTK_TYPE[widget] = "frame" then get_text = gtks.gtk("gtk_frame_get_label", widget)
  if GTK_TYPE[widget] = "list" then
    if VAL(gtks.gtk("gtk_tree_selection_get_selected", GTK_LIST_SELECT[widget], "NULL", GTK_LIST_ITER[widget])) then
      if GTK_LIST_ARRAY[widget] <> 0 then
        arr = GTK_LIST_ARRAY[widget]
        result = arr[LBOUND(arr) + VAL(gtks.gtk("gtk_tree_model_get_string_from_iter", GTK_LIST_STORE[widget], GTK_LIST_ITER[widget]))]
      end if
    end if
  end if
  return result
end

rem
rem Set text in widget
rem
sub g_set_text(widget, text)
  local mark, i
  if GTK_TYPE[widget] = "window" then gtks.gtk("gtk_window_set_title", widget,  "\"" + text + "\"")
  if GTK_TYPE[widget] = "button" then gtks.gtk("gtk_button_set_label", widget, "\"" + text + "\"")
  if GTK_TYPE[widget] = "check" then gtks.gtk("gtk_button_set_label", widget, "\"" + text + "\"")
  if GTK_TYPE[widget] = "radio" then gtks.gtk("gtk_button_set_label", widget, "\"" + text + "\"")
  if GTK_TYPE[widget] = "entry" then gtks.gtk("gtk_entry_set_text", widget, "\"" + text + "\"")
  if GTK_TYPE[widget] = "password" then gtks.gtk("gtk_entry_set_text", widget, "\"" + text + "\"")
  if GTK_TYPE[widget] = "label" then gtks.gtk("gtk_label_set_text", widget, "\"" + text + "\"")
  if GTK_TYPE[widget] = "droplist" then
    if ismap(GTK_COMBO_BOUND[widget]) then
      for i in GTK_COMBO_BOUND[widget]
        gtks.gtk("gtk_combo_box_text_remove %d 0", widget)
      next i
    end if
    if !ismap(text) then
      PRINT "WARNING: Pass an array to recreate a droplist!\n"
      GTK_COMBO_BOUND[drop] = 0
    else
      for i in text
        gtks.gtk("gtk_combo_box_text_append %s %s", widget, "\"" + text[i] + "\"")
      next i
      GTK_COMBO_BOUND[drop] = text
    end if
    gtks.gtk("gtk_combo_box_set_active %d 0", widget)
  end if
  if GTK_TYPE[widget] = "text" then
    gtks.gtk("gtk_text_buffer_set_text", GTK_CONTAINER[widget], "\"" + text + "\"", -1)
    gtks.gtk("gtk_text_buffer_get_end_iter", GTK_CONTAINER[widget], GTK_end_ITER[widget])
    mark = gtks.gtk("gtk_text_buffer_create_mark", GTK_CONTAINER[widget], "mymark", GTK_end_ITER[widget], 0)
    gtks.gtk("gtk_text_view_scroll_to_mark", GTK_TEXT_VIEW[widget], mark, 0, 1, 0.0, 1.0)
    gtks.gtk("gtk_text_buffer_delete_mark", GTK_CONTAINER[widget], mark)
  end if
  if GTK_TYPE[widget] = "separator" then PRINT "WARNING: Cannot set text of " + GTK_TYPE[widget] + " widget!\n"
  if GTK_TYPE[widget] = "frame" then gtks.gtk("gtk_frame_set_label", widget, "\"" + text + "\"")
  if GTK_TYPE[widget] = "list" then
    gtks.gtk("gtk_list_store_clear", GTK_LIST_STORE[widget])
    if LBOUND(text) = undef then
      PRINT "WARNING: Pass an array to create a list!\n"
      GTK_LIST_ARRAY = undef
    else
      for i = LBOUND(text) TO UBOUND(text)
        gtks.gtk("gtk_list_store_append", GTK_LIST_STORE[widget], GTK_LIST_ITER[widget])
        gtks.gtk("gtk_list_store_set", GTK_LIST_STORE[widget], GTK_LIST_ITER[widget], 0, "\"" + text[i] + "\"", -1)
      next i
      GTK_LIST_ARRAY[widget] = text
    end if
  end if
end

rem
rem Find selection of chek/option button
rem
func g_get_value(widget)
  if GTK_TYPE[widget] = "check" then
    get_value = VAL(gtks.gtk("gtk_toggle_button_get_active %s", widget))
  else if GTK_TYPE[widget] = "radio" then
    get_value = VAL(gtks.gtk("gtk_toggle_button_get_active %s", widget))
  else if GTK_TYPE[widget] = "droplist" then
    get_value = VAL(gtks.gtk("gtk_combo_box_get_active %s", widget))
  else if GTK_TYPE[widget] = "text" then
    get_value = VAL(gtks.gtk("gtk_text_buffer_get_line_count %s", GTK_CONTAINER[widget]))
  else if GTK_TYPE[widget] = "list" then
    if VAL(gtks.gtk("gtk_tree_selection_get_selected %s %s %s", GTK_LIST_SELECT[widget], "NULL", GTK_LIST_ITER[widget])) then
      get_value = VAL(gtks.gtk("gtk_tree_model_get_string_from_iter %s %s", GTK_LIST_STORE[widget], GTK_LIST_ITER[widget]))
    end if
  else
    PRINT "WARNING: Cannot get status of " + GTK_TYPE[widget] + " widget!\n"
  end if
end

rem
rem Set selection of check/option button
rem
sub g_set_value(widget, n)
  local path, mark
  if GTK_TYPE[widget] = "check" then
    gtks.gtk("gtk_toggle_button_set_active %s %s", widget, 1)
  else if GTK_TYPE[widget] = "radio" then
    gtks.gtk("gtk_toggle_button_set_active %s %s", widget, 1)
  else if GTK_TYPE[widget] = "droplist" then
    gtks.gtk("gtk_combo_box_set_active %s %d", widget, n)
  else if GTK_TYPE[widget] = "text" then
    gtks.gtk("gtk_text_buffer_get_iter_at_line", GTK_CONTAINER[widget], GTK_end_ITER[widget], n)
    mark = gtks.gtk("gtk_text_buffer_create_mark", GTK_CONTAINER[widget], "mymark", GTK_end_ITER[widget], 0)
    gtks.gtk("gtk_text_view_scroll_to_mark", GTK_TEXT_VIEW[widget], mark, 0, 1, 0.0, 1.0)
    gtks.gtk("gtk_text_buffer_delete_mark", GTK_CONTAINER[widget], mark)
  else if GTK_TYPE[widget] = "list" then
    path = gtks.gtk("gtk_tree_path_new_from_string %s", n)
    gtks.gtk("gtk_tree_selection_select_path %s %s", GTK_LIST_SELECT[widget], path)
    gtks.gtk("gtk_tree_path_free %s", path)
  else
    PRINT "WARNING: Cannot activate " + GTK_TYPE[widget] + " widget!\n"
  end if
end

rem
rem Focus to widget
rem
sub g_focus(widget)
  gtks.gtk("gtk_widget_grab_focus", widget)
end

rem
rem Foreground colors
rem
sub g_fg_color(widget, r, g, b)
  local tmp, gtksettings
  gtksettings = gtks.gtk("gtk_settings_get_default")

  if GTK_TYPE[widget] = "window" then PRINT "WARNING: Cannot set foreground color of window widget!\n"
  if GTK_TYPE[widget] = "button" then
    gtks.gtk("gtk_rc_parse_string %s %s %s %s %s %s %s %s", "\"style \\\"" + widget + "\\\" [ fg[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ fg[PRELIGHT] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ fg[ACTIVE] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "*\\\" style \\\"" + widget + "\\\"\"")
  end if
  if GTK_TYPE[widget] = "check" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ fg[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ fg[PRELIGHT] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ fg[ACTIVE] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "*\\\" style \\\"" + widget + "\\\"\"")
  end if
  if GTK_TYPE[widget] = "radio" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ fg[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ fg[PRELIGHT] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ fg[ACTIVE] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "*\\\" style \\\"" + widget + "\\\"\"")
  end if
  if GTK_TYPE[widget] = "entry" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ text[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ GtkWidget::cursor_color = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  if GTK_TYPE[widget] = "password" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ text[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ GtkWidget::cursor_color = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  if GTK_TYPE[widget] = "label" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ fg[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  if GTK_TYPE[widget] = "droplist" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ text[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ text[PRELIGHT] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "*\\\" style \\\"" + widget + "\\\"\"")
  end if
  if GTK_TYPE[widget] = "text" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ text[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ GtkWidget::cursor_color = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + GTK_TEXT_VIEW[widget] + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  if GTK_TYPE[widget] = "separator" then PRINT "WARNING: Cannot set foreground color of separator widget!\n"
  if GTK_TYPE[widget] = "frame" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ fg[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "*\\\" style \\\"" + widget + "\\\"\"")
  end if
  if GTK_TYPE[widget] = "list" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ text[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + GTK_CONTAINER[widget] + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  
  gtks.gtk("gtk_rc_reset_styles", gtksettings)
end

rem
rem Background colors
rem
sub g_bg_color(widget, r, g, b)
  local tmp, gtksettings
  
  gtksettings = gtks.gtk("gtk_settings_get_default")
  
  if GTK_TYPE[widget] = "window" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ bg[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"" + widget + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  if GTK_TYPE[widget] = "button" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ bg[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    if r < 60415 then r = r + 5120 
    if g < 60415 then g = g + 5120
    if b < 60415 then b = b + 5120
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ bg[PRELIGHT] = [" + r + ", " + g + ", " + b + "] ]\"")
    if r > 5120 then r = r - 5120
    if g > 5120 then g = g - 5120
    if b > 5120 then b = b - 5120
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ bg[ACTIVE] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "*\\\" style \\\"" + widget + "\\\"\"")
  end if
  if GTK_TYPE[widget] = "check" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ bg[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    if r < 60415 then r = r + 5120 
    if g < 60415 then g = g + 5120
    if b < 60415 then b = b + 5120
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ bg[PRELIGHT] = [" + r + ", " + g + ", " + b + "] ]\"")
    if r > 5120 then r = r - 5120
    if g > 5120 then g = g - 5120
    if b > 5120 then b = b - 5120
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ bg[ACTIVE] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "*\\\" style \\\"" + widget + "\\\"\"")
  end if
  if GTK_TYPE[widget] = "radio" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ bg[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    if r < 60415 then r = r + 5120 
    if g < 60415 then g = g + 5120
    if b < 60415 then b = b + 5120
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ bg[PRELIGHT] = [" + r + ", " + g + ", " + b + "] ]\"")
    if r > 5120 then r = r - 5120
    if g > 5120 then g = g - 5120
    if b > 5120 then b = b - 5120
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ bg[ACTIVE] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "*\\\" style \\\"" + widget + "\\\"\"")
  end if
  if GTK_TYPE[widget] = "entry" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ base[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  if GTK_TYPE[widget] = "password" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ base[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  if GTK_TYPE[widget] = "text" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ base[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + GTK_TEXT_VIEW[widget] + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  if GTK_TYPE[widget] = "droplist" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ bg[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    if r < 60415 then r = r + 5120 
    if g < 60415 then g = g + 5120
    if b < 60415 then b = b + 5120
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ bg[PRELIGHT] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "*\\\" style \\\"" + widget + "\\\"\"")
  end if
  
  if GTK_TYPE[widget] = "label" then PRINT "WARNING: Cannot set background color of label widget!\n"
  if GTK_TYPE[widget] = "separator" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ bg[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  if GTK_TYPE[widget] = "frame" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ bg[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  if GTK_TYPE[widget] = "list" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ base[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + GTK_CONTAINER[widget] + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  
  gtks.gtk("gtk_rc_reset_styles", gtksettings)
end

rem
rem Disable widget
rem

sub g_disable(widget)
  if GTK_TYPE[widget] = "window" then PRINT "WARNING: Cannot disable " + GTK_TYPE[widget] + " widget!\n"
  if GTK_TYPE[widget] = "button" then gtks.gtk("gtk_widget_set_sensitive %s 0", widget)
  if GTK_TYPE[widget] = "check" then gtks.gtk("gtk_widget_set_sensitive %s 0", widget)
  if GTK_TYPE[widget] = "radio" then gtks.gtk("gtk_widget_set_sensitive %s 0", widget)
  if GTK_TYPE[widget] = "entry" then gtks.gtk("gtk_widget_set_sensitive %s 0", widget)
  if GTK_TYPE[widget] = "password" then gtks.gtk("gtk_widget_set_sensitive %s", widget)
  if GTK_TYPE[widget] = "label" then PRINT "WARNING: Cannot disable " + GTK_TYPE[widget] + " widget!\n"
  if GTK_TYPE[widget] = "droplist" then gtks.gtk("gtk_widget_set_sensitive %s", widget)
  if GTK_TYPE[widget] = "text" then gtks.gtk("gtk_text_view_set_editable %s 0", GTK_TEXT_VIEW[widget])
  if GTK_TYPE[widget] = "separator" then PRINT "WARNING: Cannot disable " + GTK_TYPE[widget] + " widget!\n"
  if GTK_TYPE[widget] = "frame" then PRINT "WARNING: Cannot disable " + GTK_TYPE[widget] + " widget!\n"
  if GTK_TYPE[widget] = "list" then gtks.gtk("gtk_widget_set_sensitive %s 0", widget)
end

rem
rem Enable widget
rem
sub g_enable(widget)
  if GTK_TYPE[widget] = "window" then PRINT "WARNING: Cannot enable " + GTK_TYPE[widget] + " widget!\n"
  if GTK_TYPE[widget] = "button" then gtks.gtk("gtk_widget_set_sensitive", widget, 1)
  if GTK_TYPE[widget] = "check" then gtks.gtk("gtk_widget_set_sensitive", widget, 1)
  if GTK_TYPE[widget] = "radio" then gtks.gtk("gtk_widget_set_sensitive", widget, 1)
  if GTK_TYPE[widget] = "entry" then gtks.gtk("gtk_widget_set_sensitive", widget, 1)
  if GTK_TYPE[widget] = "password" then gtks.gtk("gtk_widget_set_sensitive", widget, 1)
  if GTK_TYPE[widget] = "label" then PRINT "WARNING: Cannot enable " + GTK_TYPE[widget] + " widget!\n"
  if GTK_TYPE[widget] = "droplist" then gtks.gtk("gtk_widget_set_sensitive", widget, 1)
  if GTK_TYPE[widget] = "text" then gtks.gtk("gtk_text_view_set_editable", GTK_TEXT_VIEW[widget], 1)
  if GTK_TYPE[widget] = "separator" then PRINT "WARNING: Cannot enable " + GTK_TYPE[widget] + " widget!\n"
  if GTK_TYPE[widget] = "frame" then PRINT "WARNING: Cannot enable " + GTK_TYPE[widget] + " widget!\n"
  if GTK_TYPE[widget] = "list" then gtks.gtk("gtk_widget_set_sensitive", widget, 1)
end

rem
rem Hide widget
rem
sub g_hide(widget)
  gtks.gtk("gtk_widget_hide %d", widget)
end

rem
rem Show widget
rem
sub g_show(widget)
  gtks.gtk("gtk_widget_show %d", widget)
end

rem
rem Synchronous mainloop here
rem
func g_event()
  return gtks.gtk("gtk_server_callback wait")
end

rem
rem Asynchronous mainloop here
rem
func g_async()
  return gtks.gtk("gtk_server_callback update")
end

rem
rem Get version
rem
func g_version()
  return gtks.gtk("gtk_server_version")
end

rem
rem set logging
rem
sub g_logging(arg)
  gtks.gtk("log %s", arg)
end

