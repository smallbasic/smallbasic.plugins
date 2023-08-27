rem
rem Demo program using the GTK context - based on gtk-server-2.4.6/scriptbasic/demo.sb
rem

import gtk

rem Window prototype: window(title, xsize, ysize) -> returns windowID
win = gtk.g_window("Hello world!", 400, 250)

rem Button prototype: button(label, xsize, ysize -> returns buttonID
exit_but = gtk.g_button("Exit", 100, 50)

rem Foreground/background color prototype: fg_color(widget, r, g, b) -> no returnvalue
gtk.g_fg_color(exit_but, 0, 65535, 0)
gtk.g_bg_color(exit_but, 10240, 32768, 10240)

rem Attach widget to parent prototype: attach(widget, parent, xpos, ypos) -> no returnvalue
gtk.g_attach(exit_but, win, 290, 190)

print_but = gtk.g_button("Print text", 100, 50)
gtk.g_attach(print_but, win, 10, 190)

rem Frame prototype: frame(text, xsize, ysize) -> returns frameID
fr = gtk.g_frame(" Frame ", 120, 50)
gtk.g_attach(fr, win, 150, 190)
gtk.g_fg_color(fr, 65535, 0, 0)

rem Separator widget prototype: separator(length) -> returns separatorID
sep = gtk.g_separator(400)
gtk.g_attach(sep, win, 0, 180)
gtk.g_bg_color(sep, 65535, 65535, 0)

rem Entry/password prototype: entry(xsize, ysize)  - or -  password(xsize, ysize) -> returns entryID
entry = gtk.g_entry(200, 25)
gtk.g_attach(entry, win, 10, 10)
gtk.g_bg_color(entry, 65535, 65535, 0)

rem Label prototype: label(text, xsize, ysize) -> returns labelID
lab = gtk.g_label("This is a demo", 90, 20)
gtk.g_attach(lab, win, 10, 50)
gtk.g_fg_color(lab, 49152, 0, 49152)

rem Check button prototype: check(text, xsize, ysize) -> returns checkbuttonID
c_but = gtk.g_check("Option here", 100, 20)
gtk.g_attach(c_but, win, 10, 80)
gtk.g_fg_color(c_but, 0, 0, 65535)

rem Set value of check/option button: select(button) -> no returnvalue
gtk.g_set_value(c_but, 0)

rem Radio button prototype: radio(text, xsize, ysize, group) -> returns radiobuttonID
rem (First radiobutton should be attached to group "NULL")
r1_but = gtk.g_radio("Selection 1", 110, 20, "NULL")
gtk.g_attach(r1_but, win, 120, 50)
gtk.g_bg_color(r1_but, 65535, 0, 0)
r2_but = gtk.g_radio("Selection 2", 110, 20, r1_but)
gtk.g_attach(r2_but, win, 120, 70)
r3_but = gtk.g_radio("Selection 3", 110, 20, r1_but)
gtk.g_attach(r3_but, win, 120, 90)
gtk.g_disable(r3_but)

rem Droplist prototype: droplist(array, xsize, ysize) -> returns droplistID
a={}
a[0] = "value one"
a[1] = "value two"
a[2] = "value three"
drop = gtk.g_droplist(a, 200, 30)
gtk.g_attach(drop, win, 10, 130)

b={}
b[0] = "peter"
b[1] = "van"
b[2] = "eerten surname"

gtk.g_set_text(drop, b)
gtk.g_set_value(drop, 2)
gtk.g_bg_color(drop, 0,0,65535)

rem Multiline textedit prototype: text(xsize, ysize) -> returns textID
txt = gtk.g_text(150, 150)
gtk.g_attach(txt, win, 240, 10)
gtk.g_fg_color(txt, 65535, 0, 0)
gtk.g_disable(txt)
list = gtk.g_list(b, 150, 150)
gtk.g_attach(list, win, 240, 10)
gtk.g_bg_color(list, 0,65535,0)
gtk.g_fg_color(list, 0,0,65535)
gtk.g_set_value(list, 0)
gtk.g_focus(entry)

repeat
  rem Event prototype: event() -> returns widgetID on which event occured
  action = gtk.g_event()
  if action == print_but then
    print gtk.g_get_text(list)
  endif

  rem Selected prototype: get_value(widget) -> returns 0 or 1 depending on state
  if gtk.g_get_value(r2_but) then
    print "Button 2 selected!"
  endif

until action = exit_but OR action = win
