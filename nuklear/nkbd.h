//
// Nulear keyboard handling
//

#if !defined(NK_IMPLEMENTATION)
#error "NK_IMPLEMENTATION not defined"
#endif

// internal state
static struct {
  int seq;
  int focus;
  bool tab_pressed;
  bool enter_pressed;
  struct nk_rect focus_rect;
  struct nk_rect widget_rect;
} _nkbd;

//
// prepare for the next frame
//
NK_LIB void nkbd_begin(nk_context *ctx) {
  if (nk_input_is_key_released(&ctx->input, NK_KEY_ENTER)) {
    _nkbd.enter_pressed = true;
  } else if (nk_input_is_key_released(&ctx->input, NK_KEY_ENTER)) {
    _nkbd.enter_pressed = false;
  }
  if (!_nkbd.tab_pressed && nk_input_is_key_pressed(&ctx->input, NK_KEY_TAB)) {
    if (nk_input_is_key_pressed(&ctx->input, NK_KEY_SHIFT)) {
      if (_nkbd.focus > 0) {
        _nkbd.focus--;
      }
    } else {
      _nkbd.focus++;
    }
    _nkbd.tab_pressed = true;
  } else if (nk_input_is_key_released(&ctx->input, NK_KEY_TAB)) {
    _nkbd.tab_pressed = false;
  }
  _nkbd.seq = 0;
}

//
// end of frame - draw the focus rectangle
//
NK_LIB void nkbd_end(nk_context *ctx, nk_color color) {
  if (_nkbd.focus >= _nkbd.seq) {
    _nkbd.focus = 0;
  }
  _nkbd.seq = 0;
  nk_stroke_rect(&ctx->current->buffer, _nkbd.focus_rect, 4.0f, 1.0f, color);
}

//
// remember the potentical focus rectange
//
NK_LIB void nkbd_widget_begin(nk_context *ctx) {
  nk_layout_peek(&_nkbd.widget_rect, ctx);
}

//
// sets the focus rectangle
//
NK_LIB bool nkbd_widget_end(nk_context *ctx, bool edit, bool clicked) {
  bool result = clicked;
  struct nk_window *win = ctx->current;
  if (edit && win->edit.active) {
    // edit widget is active
    if (_nkbd.focus != _nkbd.seq && _nkbd.tab_pressed) {
      // tabbed away
      win->edit.active = nk_false;
    } else {
      // tabbed into
      clicked = 1;
    }
  }
  if ((_nkbd.focus == _nkbd.seq) || clicked) {
    result = _nkbd.enter_pressed;
    _nkbd.enter_pressed = false;
    _nkbd.focus_rect = _nkbd.widget_rect;
    if (edit && !win->edit.active) {
      win->edit.active = nk_true;
      // sequence is advanced in nk_edit_buffer, so one less is current
      win->edit.name = win->edit.seq - 1;
    }
  }
  if (clicked) {
    // focus on selected item
    _nkbd.focus = _nkbd.seq;
  }
  // set ID for next widget
  _nkbd.seq++;
  return result;
}
