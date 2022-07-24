//
// Nulear keyboard handling
//

// file navigation:
// ctx => struct nk_context {
// ctx->current => struct nk_window {
// ctx->current->layout => struct nk_panel {
// nk_combo_begin(

#if !defined(NK_IMPLEMENTATION)
#error "NK_IMPLEMENTATION not defined"
#endif

// internal state
struct Nkbd {
  Nkbd() : seq(0), focus(0), tab_pressed(false), enter_pressed(false) {}
  virtual ~Nkbd() {}

  int seq;
  int focus;
  bool tab_pressed;
  bool enter_pressed;
  struct nk_rect focus_rect;
  struct nk_rect widget_rect;
};

NK_LIB void nkbd_create(nk_context *ctx) {
  nk_handle handle;
  handle.ptr = new Nkbd();
  nk_set_user_data(ctx, handle);
}

NK_LIB void nkbd_destroy(nk_context *ctx) {
  Nkbd *nkbd = (Nkbd *)ctx->userdata.ptr;
  delete nkbd;
}

//
// prepare for the next frame
//
NK_LIB void nkbd_begin(nk_context *ctx) {
  Nkbd *nkbd = (Nkbd *)ctx->userdata.ptr;
  if (nk_input_is_key_released(&ctx->input, NK_KEY_ENTER)) {
    nkbd->enter_pressed = true;
  } else if (nk_input_is_key_released(&ctx->input, NK_KEY_ENTER)) {
    nkbd->enter_pressed = false;
  }
  if (!nkbd->tab_pressed && nk_input_is_key_pressed(&ctx->input, NK_KEY_TAB)) {
    if (nk_input_is_key_pressed(&ctx->input, NK_KEY_SHIFT)) {
      if (nkbd->focus > 0) {
        nkbd->focus--;
      }
    } else {
      nkbd->focus++;
    }
    nkbd->tab_pressed = true;
  } else if (nk_input_is_key_released(&ctx->input, NK_KEY_TAB)) {
    nkbd->tab_pressed = false;
  }
  nkbd->seq = 0;
}

//
// end of frame - draw the focus rectangle
//
NK_LIB void nkbd_end(nk_context *ctx, nk_color color) {
  Nkbd *nkbd = (Nkbd *)ctx->userdata.ptr;
  if (nkbd->focus >= nkbd->seq) {
    nkbd->focus = 0;
  }
  nkbd->seq = 0;
  nk_stroke_rect(&ctx->current->buffer, nkbd->focus_rect, 4.0f, 1.0f, color);
}

//
// remember the potentical focus rectange
//
NK_LIB void nkbd_widget_begin(nk_context *ctx) {
  Nkbd *nkbd = (Nkbd *)ctx->userdata.ptr;
  nk_layout_peek(&nkbd->widget_rect, ctx);
}

//
// sets the focus rectangle
//
NK_LIB bool nkbd_widget_end(nk_context *ctx, bool active) {
  Nkbd *nkbd = (Nkbd *)ctx->userdata.ptr;
  bool result = active;

  if ((nkbd->focus == nkbd->seq) || active) {
    result = nkbd->enter_pressed;
    nkbd->enter_pressed = false;
    nkbd->focus_rect = nkbd->widget_rect;
  }

  if (active) {
    // focus on selected item
    nkbd->focus = nkbd->seq;
    nk_edit_unfocus(ctx);
  }

  // set ID for next widget
  nkbd->seq++;
  return result;
}

//
// nkbd_widget_end handler for edit controls
//
NK_LIB bool nkbd_edit_end(nk_context *ctx, bool active) {
  Nkbd *nkbd = (Nkbd *)ctx->userdata.ptr;
  bool result = active;
  struct nk_window *win = ctx->current;

  // sequence is advanced in nk_edit_buffer, so one less is current
  if (win->edit.active && win->edit.name == win->edit.seq - 1) {
    // edit widget is active
    if (nkbd->focus != nkbd->seq && nkbd->tab_pressed) {
      // tabbed away
      win->edit.active = nk_false;
    } else {
      // tabbed into
      active = 1;
    }
  }

  nkbd_widget_end(ctx, active);
  if (nkbd->focus == nkbd->seq - 1 && !win->edit.active) {
    win->edit.active = nk_true;
    win->edit.name = win->edit.seq - 1;
  }

  return result;
}

//
// open or close the combo box
//
NK_LIB void nkbd_combo_begin(nk_context *ctx) {
  nkbd_widget_begin(ctx);
  Nkbd *nkbd = (Nkbd *)ctx->userdata.ptr;
  struct nk_window *win = ctx->current;

  // relies on mod to nuklear.h
  if (nkbd->focus == nkbd->seq && nkbd->enter_pressed) {
    if (win->layout->flags & NK_WINDOW_ROM) {
      // close
      win->layout->flags |= NK_WINDOW_REMOVE_ROM;
    } else {
      // open
      win->layout->flags |= NK_WINDOW_ROM;
    }
  } else if (nkbd->focus != nkbd->seq && win->layout->flags & NK_WINDOW_ROM) {
    // close when navigating away
    win->layout->flags |= NK_WINDOW_REMOVE_ROM;
  }
}

/*
diff --git a/nuklear.h b/nuklear.h
index aef8f7f..7392972 100644
--- a/nuklear.h
+++ b/nuklear.h
@@ -20860,6 +20860,10 @@ nk_nonblock_begin(struct nk_context *ctx,
         in_header = nk_input_is_mouse_hovering_rect(&ctx->input, header);
         if (pressed && (!in_body || in_header))
             is_active = nk_false;
+        // SmallBASIC
+        if (win->layout->flags & NK_WINDOW_REMOVE_ROM) {
+          is_active = nk_false;
+        }
     }
     win->popup.header = header;
 
@@ -28682,6 +28686,10 @@ nk_combo_begin(struct nk_context *ctx, struct nk_window *win,
     hash = win->popup.combo_count++;
     is_open = (popup) ? nk_true:nk_false;
     is_active = (popup && (win->popup.name == hash) && win->popup.type == NK_PANEL_COMBO);
+    // SmallBASIC
+    if (!is_open && win->layout->flags & NK_WINDOW_ROM) {
+      is_clicked = nk_true;
+    }
     if ((is_clicked && is_open && !is_active) || (is_open && !is_active) ||
         (!is_open && !is_active && !is_clicked)) return 0;
     if (!nk_nonblock_begin(ctx, 0, body,
*/
