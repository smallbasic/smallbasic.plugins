// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2023 Chris Warren-Smith

#include "config.h"

#include <sys/stat.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "include/var.h"
#include "include/module.h"
#include "include/param.h"

extern "C" char *gtk(char *arg);

#define MAX_TEXT_BUFFER_LENGTH 1024

static char buffer[MAX_TEXT_BUFFER_LENGTH];

static int cmd_gtk(int argc, slib_par_t *params, var_t *retval) {
  strcpy(buffer, format_text(argc, params, 0));
  char *result = gtk(buffer);
  v_setstrn(retval, result, strlen(result) - 1);
  return 1;
}

static int cmd_gtk_proc(int argc, slib_par_t *params, var_t *retval) {
  strcpy(buffer, format_text(argc, params, 0));
  gtk(buffer);
  return 1;
}

FUNC_SIG lib_func[] = {
  {1, 20, "GTK", cmd_gtk},
};

FUNC_SIG lib_proc[] = {
  {1, 20, "GTK", cmd_gtk_proc},
};

SBLIB_API int sblib_proc_count() {
  return (sizeof(lib_proc) / sizeof(lib_proc[0]));
}

SBLIB_API int sblib_func_count() {
  return (sizeof(lib_func) / sizeof(lib_func[0]));
}
