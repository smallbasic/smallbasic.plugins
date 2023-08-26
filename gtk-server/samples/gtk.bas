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

gtks.debug(1)
'gtks.gtk("gtk_server_cfg -debug")
gtks.gtk("gtk_init 0 0")

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
  gtks.gtk("gtk_window_set_title %s \"%s\"", win, title)
  gtks.gtk("gtk_widget_set_size_request %s %u %u", win, xsize, ysize)
  gtks.gtk("gtk_window_set_resizable %s 1", win)
  gtks.gtk("gtk_widget_set_name %s %s", win, win)
  gtks.gtk("gtk_widget_show %s", win)

  local fixed = gtks.gtk("gtk_fixed_new")
  gtks.gtk("gtk_container_add %s %s", win, fixed)
  gtks.gtk("gtk_widget_show %s", fixed)

  gtk_container[win] = fixed
  gtk_type[win] = "window"

  return win
end

rem
rem Parentize
rem
sub g_attach(widget, parent, x, y)
  local fixed = gtk_container[parent]
  gtks.gtk("gtk_fixed_put %s %s %u %u", fixed, widget, x, y)
end

rem
rem Button creation starts here
rem
func g_button(text, xsize, ysize)
  local but = gtks.gtk("gtk_button_new_with_label %s", text)
  gtks.gtk("gtk_widget_set_size_request %s %u %u", but, xsize, ysize)
  gtks.gtk("gtk_widget_set_name %s %s", but, but)
  gtks.gtk("gtk_widget_show %s", but)
  gtk_type[but] = "button"
  return but
end

rem
rem Check Button creation starts here
rem
func g_check(text, xsize, ysize)
  local chk = gtks.gtk("gtk_check_button_new_with_label \"" + text + "\"")
  gtks.gtk("gtk_widget_set_size_request %s %u %u", chk, xsize, ysize)
  gtks.gtk("gtk_widget_set_name %s %s", chk, chk)
  gtks.gtk("gtk_widget_show %s", chk)
  gtk_type[chk] = "check"
  return chk
end

rem
rem Radio Button creation starts here
rem
func g_radio(text, xsize, ysize, group)
  local value = gtks.gtk("gtk_radio_button_new_with_label_from_widget %s %s", "\"" + STR(group) + "\"", "\"" + text + "\"")
  gtks.gtk("gtk_widget_set_size_request %s %u %u", value, xsize, ysize)
  gtks.gtk("gtk_widget_set_name %s %s", value, value)
  gtks.gtk("gtk_widget_show %s", value)
  gtk_type[value] = "radio"
  return value
end

rem
rem Entry creation starts here
rem
func g_entry(xsize, ysize)
  local ent = gtks.gtk("gtk_entry_new")
  gtks.gtk("gtk_widget_set_size_request %s %u %u", ent, xsize, ysize)
  gtks.gtk("gtk_widget_set_name %s %s", ent, ent)
  gtks.gtk("gtk_widget_show %s", ent)
  gtk_type[ent] = "entry"
  return ent
end

rem
rem Password creation starts here
rem
func g_password(xsize, ysize)
  local pwd = gtks.gtk("gtk_entry_new")
  gtks.gtk("gtk_widget_set_size_request %s %u %u", pwd, xsize, ysize)
  gtks.gtk("gtk_entry_set_visibility %s", pwd, 0)
  gtks.gtk("gtk_widget_set_name %s %s", pwd, pwd)
  gtks.gtk("gtk_widget_show %s", pwd)
  gtk_type[pwd] = "password"
  return pwd
end

rem
rem Label creation starts here
rem
func g_label(text, xsize, ysize)
  local lab = gtks.gtk("gtk_label_new \"" + text + "\"")
  gtks.gtk("gtk_widget_set_size_request %s %u %u", lab, xsize, ysize)
  gtks.gtk("gtk_widget_set_name %s %s", lab, lab)
  gtks.gtk("gtk_widget_show %s", lab)
  gtk_type[lab] = "label"
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
    gtk_combo_bound[drop] = 0
  else
    for i in text
      gtks.gtk("gtk_combo_box_text_append %s \"%s\"", drop, text[i])
    next i
    gtk_combo_bound[drop] = text
  end if

  gtks.gtk("gtk_combo_box_set_active %s 0", drop)
  gtks.gtk("gtk_widget_set_size_request %s %u %u", drop, xsize, ysize)
  gtks.gtk("gtk_widget_set_name %s %s", drop, drop)
  gtks.gtk("gtk_widget_show %s 1", drop)
  gtk_type[drop] = "droplist"
  return drop
end

rem
rem Multiline text starts here
rem
func g_text(xsize, ysize)
  local buf, vw, sw
  buf = gtks.gtk("gtk_text_buffer_new NULL")
  vw = gtks.gtk("gtk_text_view_new_with_buffer %s", buf)
  gtks.gtk("gtk_text_view_set_wrap_mode %s 1", vw)
  gtks.gtk("gtk_widget_set_name %s %s", vw, vw)
  gtks.gtk("gtk_widget_show %s", vw)

  sw = gtks.gtk("gtk_scrolled_window_new NULL NULL")
  gtks.gtk("gtk_scrolled_window_set_policy %s 1 1", sw)
  gtks.gtk("gtk_scrolled_window_set_shadow_type %s 1", sw)
  gtks.gtk("gtk_container_add %s %s", sw, vw)
  gtks.gtk("gtk_text_view_set_editable %s 1", vw)
  gtks.gtk("gtk_text_view_set_wrap_mode %s 2", vw)
  gtks.gtk("gtk_widget_set_size_request %s %u %u", sw, xsize, ysize)
  gtks.gtk("gtk_widget_show %s", sw)

  gtk_start_iter[sw] = gtks.gtk("gtk_server_opaque")
  gtk_end_iter[sw] = gtks.gtk("gtk_server_opaque")
  gtk_text_view[sw] = vw
  gtk_container[sw] = buf
  gtk_type[sw] = "text"
  return sw
end

rem
rem Separator creation starts here
rem
func g_separator(xsize)
  local sep = gtks.gtk("gtk_hseparator_new")
  gtks.gtk("gtk_widget_set_size_request %s %u %u", sep, xsize, 0)
  gtks.gtk("gtk_widget_set_name %s %s", sep, sep)
  gtks.gtk("gtk_widget_show %s", sep)
  gtk_type[sep] = "separator"
  return sep
end

rem
rem frame) creation starts here
rem
func g_frame(text, xsize, ysize)
  local frm = gtks.gtk("gtk_frame_new NULL")
  gtks.gtk("gtk_frame_set_label %s \"%s\"", frm, text)
  gtks.gtk("gtk_widget_set_size_request %s %u %u", frm, xsize, ysize)
  gtks.gtk("gtk_widget_set_name %s %s", frm, frm)
  gtks.gtk("gtk_widget_show %s", frm)
  gtk_type[frm] = "frame"
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
  gtks.gtk("gtk_server_connect %s button-press-event %s 1", tree, tree)

  sel = gtks.gtk("gtk_tree_view_get_selection %s", tree)
  gtks.gtk("gtk_tree_selection_set_mode %s 1", sel)
  column = gtks.gtk("gtk_tree_view_column_new_with_attributes dummy %s text 0 NULL", gtks.gtk("gtk_cell_renderer_text_new"))
  gtks.gtk("gtk_tree_view_append_column %s %s", tree, column)

  sw = gtks.gtk("gtk_scrolled_window_new NULL NULL")
  gtks.gtk("gtk_scrolled_window_set_policy %s 1 1", sw)
  gtks.gtk("gtk_scrolled_window_set_shadow_type %s 1", sw)
  gtks.gtk("gtk_container_add %s %s", sw, tree)
  gtks.gtk("gtk_widget_set_size_request %s %u %u", sw, xsize, ysize)
  gtks.gtk("gtk_widget_show_all %s", sw)

  if !ismap(text) then
    PRINT "WARNING: Pass an array to create a list!\n"
  else
    for i in text
      gtks.gtk("gtk_list_store_append %s %s", lst, iter)
      gtks.gtk("gtk_list_store_set %s %s 0 \"%s\" -1", lst, iter, text[i])
    next i
    gtk_list_array[sw] = text
  end if
  gtk_list_store[sw] = lst
  gtk_list_iter[sw] = iter
  gtk_list_select[sw] = sel
  gtk_container[sw] = tree
  gtk_type[sw] = "list"
  return sw
end

rem
rem Get text from widget
rem
func g_get_text(widget)
  local arr
  local result
  if gtk_type[widget] = "window" then get_text = gtks.gtk("gtk_window_get_title", widget)
  if gtk_type[widget] = "button" then get_text = gtks.gtk("gtk_button_get_label", widget)
  if gtk_type[widget] = "check" then get_text = gtks.gtk("gtk_button_get_label", widget)
  if gtk_type[widget] = "radio" then get_text = gtks.gtk("gtk_button_get_label", widget)
  if gtk_type[widget] = "entry" then get_text = gtks.gtk("gtk_entry_get_text", widget)
  if gtk_type[widget] = "password" then get_text = gtks.gtk("gtk_entry_get_text", widget)
  if gtk_type[widget] = "label" then get_text = gtks.gtk("gtk_label_get_text", widget)
  if gtk_type[widget] = "droplist" then get_text = gtks.gtk("gtk_combo_box_get_active_text", widget)
  if gtk_type[widget] = "text" then
    gtks.gtk("gtk_text_buffer_get_start_iter", gtk_container[widget], gtk_start_iter[widget])
    gtks.gtk("gtk_text_buffer_get_end_iter", gtk_container[widget], gtk_end_iter[widget])
    get_text = gtks.gtk("gtk_text_buffer_get_text", gtk_container[widget], gtk_start_iter[widget], gtk_end_iter[widget], 1)
  end if
  if gtk_type[widget] = "separator" then PRINT "WARNING: Cannot get text of " + gtk_type[widget] + " widget!\n"
  if gtk_type[widget] = "frame" then get_text = gtks.gtk("gtk_frame_get_label", widget)
  if gtk_type[widget] = "list" then
    if VAL(gtks.gtk("gtk_tree_selection_get_selected", gtk_list_select[widget], "NULL", gtk_list_iter[widget])) then
      if gtk_list_array[widget] <> 0 then
        arr = gtk_list_array[widget]
        result = arr[LBOUND(arr) + VAL(gtks.gtk("gtk_tree_model_get_string_from_iter", gtk_list_store[widget], gtk_list_iter[widget]))]
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
  if gtk_type[widget] = "window" then gtks.gtk("gtk_window_set_title %s \"%s\"", widget,  text)
  if gtk_type[widget] = "button" then gtks.gtk("gtk_button_set_label %s \"%s\"", widget, text)
  if gtk_type[widget] = "check" then gtks.gtk("gtk_button_set_label %s \"%s\"", widget, text)
  if gtk_type[widget] = "radio" then gtks.gtk("gtk_button_set_label %s \"%s\"", widget, text)
  if gtk_type[widget] = "entry" then gtks.gtk("gtk_entry_set_text %s \"%s\"", widget, text)
  if gtk_type[widget] = "password" then gtks.gtk("gtk_entry_set_text %s \"%s\"", widget, text)
  if gtk_type[widget] = "label" then gtks.gtk("gtk_label_set_text %s \"%s\"", widget, text)
  if gtk_type[widget] = "droplist" then
    if ismap(gtk_combo_bound[widget]) then
      for i in gtk_combo_bound[widget]
        gtks.gtk("gtk_combo_box_text_remove %s 0", widget)
      next i
    end if
    if !ismap(text) then
      PRINT "WARNING: Pass an array to recreate a droplist!\n"
      gtk_combo_bound[drop] = 0
    else
      for i in text
        gtks.gtk("gtk_combo_box_text_append %s \"%s\"", widget, text[i])
      next i
      gtk_combo_bound[drop] = text
    end if
    gtks.gtk("gtk_combo_box_set_active %s 0", widget)
  end if
  if gtk_type[widget] = "text" then
    gtks.gtk("gtk_text_buffer_set_text %s \"%s\" -1", gtk_container[widget], text)
    gtks.gtk("gtk_text_buffer_get_end_iter", gtk_container[widget], GTK_end_ITER[widget])
    mark = gtks.gtk("gtk_text_buffer_create_mark", gtk_container[widget], "mymark", GTK_end_ITER[widget], 0)
    gtks.gtk("gtk_text_view_scroll_to_mark", GTK_TEXT_VIEW[widget], mark, 0, 1, 0.0, 1.0)
    gtks.gtk("gtk_text_buffer_delete_mark", gtk_container[widget], mark)
  end if
  if gtk_type[widget] = "separator" then PRINT "WARNING: Cannot set text of " + gtk_type[widget] + " widget!\n"
  if gtk_type[widget] = "frame" then gtks.gtk("gtk_frame_set_label %s \"%s\"", widget, text)
  if gtk_type[widget] = "list" then
    gtks.gtk("gtk_list_store_clear", gtk_list_store[widget])
    if !ismap(text) then
      PRINT "WARNING: Pass an array to create a list!\n"
      gtk_list_array = 0
    else
      for i in text
        gtks.gtk("gtk_list_store_append %s %s", gtk_list_store[widget], gtk_list_iter[widget])
        gtks.gtk("gtk_list_store_set %s %s 0 \"%s\" -1", gtk_list_store[widget], gtk_list_iter[widget], text[i])
      next i
      gtk_list_array[widget] = text
    end if
  end if
end

rem
rem Find selection of chek/option button
rem
func g_get_value(widget)
  if gtk_type[widget] = "check" then
    get_value = VAL(gtks.gtk("gtk_toggle_button_get_active %s", widget))
  else if gtk_type[widget] = "radio" then
    get_value = VAL(gtks.gtk("gtk_toggle_button_get_active %s", widget))
  else if gtk_type[widget] = "droplist" then
    get_value = VAL(gtks.gtk("gtk_combo_box_get_active %s", widget))
  else if gtk_type[widget] = "text" then
    get_value = VAL(gtks.gtk("gtk_text_buffer_get_line_count %s", gtk_container[widget]))
  else if gtk_type[widget] = "list" then
    if VAL(gtks.gtk("gtk_tree_selection_get_selected %u %u %u", gtk_list_select[widget], "NULL", gtk_list_iter[widget])) then
      get_value = VAL(gtks.gtk("gtk_tree_model_get_string_from_iter %u %u", gtk_list_store[widget], gtk_list_iter[widget]))
    end if
  else
    PRINT "WARNING: Cannot get status of " + gtk_type[widget] + " widget!\n"
  end if
end

rem
rem Set selection of check/option button
rem
sub g_set_value(widget, n)
  local path, mark

  if gtk_type[widget] = "check" then
    gtks.gtk("gtk_toggle_button_set_active %s 1", widget)
  else if gtk_type[widget] = "radio" then
    gtks.gtk("gtk_toggle_button_set_active %s 1", widget)
  else if gtk_type[widget] = "droplist" then
    gtks.gtk("gtk_combo_box_set_active %s %u", widget, n)
  else if gtk_type[widget] = "text" then
    gtks.gtk("gtk_text_buffer_get_iter_at_line %s %s %u", gtk_container[widget], GTK_end_ITER[widget], n)
    mark = gtks.gtk("gtk_text_buffer_create_mark %u mkmark %u 0", gtk_container[widget], GTK_end_ITER[widget])
    gtks.gtk("gtk_text_view_scroll_to_mark %u %u 0 1 0.0 1.0", GTK_TEXT_VIEW[widget], mark)
    gtks.gtk("gtk_text_buffer_delete_mark %u %u", gtk_container[widget], mark)
  else if gtk_type[widget] = "list" then
    path = gtks.gtk("gtk_tree_path_new_from_string %u", n)
    gtks.gtk("gtk_tree_selection_select_path %s %s", gtk_list_select[widget], path)
    gtks.gtk("gtk_tree_path_free %s", path)
  else
    PRINT "WARNING: Cannot activate " + gtk_type[widget] + " widget!\n"
  end if
end

rem
rem Focus to widget
rem
sub g_focus(widget)
  gtks.gtk("gtk_widget_grab_focus %s", widget)
end

rem
rem Foreground colors
rem
sub g_fg_color(widget, r, g, b)
  local tmp, gtksettings
  gtksettings = gtks.gtk("gtk_settings_get_default")

  if gtk_type[widget] = "window" then PRINT "WARNING: Cannot set foreground color of window widget!\n"
  if gtk_type[widget] = "button" then
    gtks.gtk("gtk_rc_parse_string %s %s %s %s %s %s %s %s", "\"style \\\"" + widget + "\\\" [ fg[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ fg[PRELIGHT] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ fg[ACTIVE] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "*\\\" style \\\"" + widget + "\\\"\"")
  end if
  if gtk_type[widget] = "check" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ fg[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ fg[PRELIGHT] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ fg[ACTIVE] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "*\\\" style \\\"" + widget + "\\\"\"")
  end if
  if gtk_type[widget] = "radio" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ fg[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ fg[PRELIGHT] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ fg[ACTIVE] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "*\\\" style \\\"" + widget + "\\\"\"")
  end if
  if gtk_type[widget] = "entry" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ text[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ GtkWidget::cursor_color = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  if gtk_type[widget] = "password" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ text[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ GtkWidget::cursor_color = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  if gtk_type[widget] = "label" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ fg[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  if gtk_type[widget] = "droplist" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ text[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ text[PRELIGHT] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "*\\\" style \\\"" + widget + "\\\"\"")
  end if
  if gtk_type[widget] = "text" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ text[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ GtkWidget::cursor_color = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + GTK_TEXT_VIEW[widget] + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  if gtk_type[widget] = "separator" then PRINT "WARNING: Cannot set foreground color of separator widget!\n"
  if gtk_type[widget] = "frame" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ fg[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "*\\\" style \\\"" + widget + "\\\"\"")
  end if
  if gtk_type[widget] = "list" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ text[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + gtk_container[widget] + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  
  gtks.gtk("gtk_rc_reset_styles", gtksettings)
end

rem
rem Background colors
rem
sub g_bg_color(widget, r, g, b)
  local tmp, gtksettings
  
  gtksettings = gtks.gtk("gtk_settings_get_default")
  
  if gtk_type[widget] = "window" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ bg[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"" + widget + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  if gtk_type[widget] = "button" then
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
  if gtk_type[widget] = "check" then
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
  if gtk_type[widget] = "radio" then
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
  if gtk_type[widget] = "entry" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ base[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  if gtk_type[widget] = "password" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ base[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  if gtk_type[widget] = "text" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ base[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + GTK_TEXT_VIEW[widget] + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  if gtk_type[widget] = "droplist" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ bg[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    if r < 60415 then r = r + 5120 
    if g < 60415 then g = g + 5120
    if b < 60415 then b = b + 5120
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ bg[PRELIGHT] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "*\\\" style \\\"" + widget + "\\\"\"")
  end if
  
  if gtk_type[widget] = "label" then PRINT "WARNING: Cannot set background color of label widget!\n"
  if gtk_type[widget] = "separator" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ bg[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  if gtk_type[widget] = "frame" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ bg[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + widget + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  if gtk_type[widget] = "list" then
    gtks.gtk("gtk_rc_parse_string", "\"style \\\"" + widget + "\\\" [ base[NORMAL] = [" + r + ", " + g + ", " + b + "] ]\"")
    gtks.gtk("gtk_rc_parse_string \"widget \\\"*.*." + gtk_container[widget] + "\\\" style \\\"" + widget + "\\\"\"")
  end if
  
  gtks.gtk("gtk_rc_reset_styles", gtksettings)
end

rem
rem Disable widget
rem

sub g_disable(widget)
  if gtk_type[widget] = "window" then PRINT "WARNING: Cannot disable " + gtk_type[widget] + " widget!\n"
  if gtk_type[widget] = "button" then gtks.gtk("gtk_widget_set_sensitive %s 0", widget)
  if gtk_type[widget] = "check" then gtks.gtk("gtk_widget_set_sensitive %s 0", widget)
  if gtk_type[widget] = "radio" then gtks.gtk("gtk_widget_set_sensitive %s 0", widget)
  if gtk_type[widget] = "entry" then gtks.gtk("gtk_widget_set_sensitive %s 0", widget)
  if gtk_type[widget] = "password" then gtks.gtk("gtk_widget_set_sensitive %s", widget)
  if gtk_type[widget] = "label" then PRINT "WARNING: Cannot disable " + gtk_type[widget] + " widget!\n"
  if gtk_type[widget] = "droplist" then gtks.gtk("gtk_widget_set_sensitive %s", widget)
  if gtk_type[widget] = "text" then gtks.gtk("gtk_text_view_set_editable %s 0", GTK_TEXT_VIEW[widget])
  if gtk_type[widget] = "separator" then PRINT "WARNING: Cannot disable " + gtk_type[widget] + " widget!\n"
  if gtk_type[widget] = "frame" then PRINT "WARNING: Cannot disable " + gtk_type[widget] + " widget!\n"
  if gtk_type[widget] = "list" then gtks.gtk("gtk_widget_set_sensitive %s 0", widget)
end

rem
rem Enable widget
rem
sub g_enable(widget)
  if gtk_type[widget] = "window" then PRINT "WARNING: Cannot enable " + gtk_type[widget] + " widget!\n"
  if gtk_type[widget] = "button" then gtks.gtk("gtk_widget_set_sensitive %s 1", widget)
  if gtk_type[widget] = "check" then gtks.gtk("gtk_widget_set_sensitive %s 1", widget)
  if gtk_type[widget] = "radio" then gtks.gtk("gtk_widget_set_sensitive %s 1", widget)
  if gtk_type[widget] = "entry" then gtks.gtk("gtk_widget_set_sensitive %s 1", widget)
  if gtk_type[widget] = "password" then gtks.gtk("gtk_widget_set_sensitive %s 1", widget)
  if gtk_type[widget] = "label" then PRINT "WARNING: Cannot enable " + gtk_type[widget] + " widget!\n"
  if gtk_type[widget] = "droplist" then gtks.gtk("gtk_widget_set_sensitive %s 1", widget)
  if gtk_type[widget] = "text" then gtks.gtk("gtk_text_view_set_editable", GTK_TEXT_VIEW[widget], 1)
  if gtk_type[widget] = "separator" then PRINT "WARNING: Cannot enable " + gtk_type[widget] + " widget!\n"
  if gtk_type[widget] = "frame" then PRINT "WARNING: Cannot enable " + gtk_type[widget] + " widget!\n"
  if gtk_type[widget] = "list" then gtks.gtk("gtk_widget_set_sensitive %s 1", widget)
end

rem
rem Hide widget
rem
sub g_hide(widget)
  gtks.gtk("gtk_widget_hide %u", widget)
end

rem
rem Show widget
rem
sub g_show(widget)
  gtks.gtk("gtk_widget_show %u", widget)
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
sub g_logging()
  gtks.gtk("log")
end

