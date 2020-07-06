// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2020 Chris Warren-Smith

#include "config.h"
#include <string.h>

#include "libclipboard/include/libclipboard.h"
#include "var.h"
#include "param.h"

clipboard_c *clipboard;

int sblib_init(void) {
  clipboard = clipboard_new(nullptr);
  return clipboard != nullptr;
}

void sblib_close(void) {
  clipboard_free(clipboard);
}

// clipboard.paste(text)
int sblib_proc_count() {
  return 1;
}

int sblib_proc_getname(int index, char *proc_name) {
  int result;
  if (index < sblib_proc_count()) {
    strcpy(proc_name, "PASTE");
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int sblib_proc_exec(int index, int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (index == 0 && argc == 1 && is_param_str(argc, params, 0)) {
    const char *text = get_param_str(0, params, 0, "");
    result = clipboard_set_text(clipboard, text);
  } else {
    result = 0;
  }
  return result;
}

// let text = clipboard.copy()
int sblib_func_count() {
  return 1;
}

int sblib_func_getname(int index, char *proc_name) {
  int result;
  if (index < sblib_func_count()) {
    strcpy(proc_name, "COPY");
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int sblib_func_exec(int index, int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (index == 0 && argc == 0) {
    char *text = clipboard_text(clipboard);
    if (text != nullptr) {
      v_setstr(retval, text);
      free(text);
    }
    result = 1;
  } else {
    result = 0;
  }
  return result;
}
