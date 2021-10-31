// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2020 Chris Warren-Smith

#include "config.h"

#include <sys/stat.h>
#include <cstring>
#include <cstdlib>

#include "include/var.h"
#include "include/module.h"
#include "include/param.h"

char *programSource = nullptr;
uint32_t modifiedTime;

uint32_t get_modified_time() {
  uint32_t result = 0;
  if (programSource != nullptr) {
    struct stat st_file;
    if (!stat(programSource, &st_file)) {
      result = st_file.st_mtime;
    }
  }
  return result;
}

static int cmd_textformat(int argc, slib_par_t *params, var_t *retval) {
  v_setstr(retval, format_text(argc, params, 0));
  return 1;
}

static int cmd_issourcemodified(int argc, slib_par_t *params, var_t *retval) {
  v_setint(retval, modifiedTime < get_modified_time());
  return 1;
}

FUNC_SIG lib_func[] = {
  {0, 0, "ISSOURCEMODIFIED", cmd_issourcemodified},
  {1, 20,"TEXTFORMAT", cmd_textformat},
};

int sblib_func_count() {
  return (sizeof(lib_func) / sizeof(lib_func[0]));
}

int sblib_func_getname(int index, char *proc_name) {
  int result;
  if (index < sblib_func_count()) {
    strcpy(proc_name, lib_func[index]._name);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

int sblib_func_exec(int index, int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (index < sblib_func_count()) {
    result = lib_func[index]._command(argc, params, retval);
  } else {
    result = 0;
  }
  return result;
}

int sblib_init(const char *sourceFile) {
  programSource = strdup(sourceFile);
  modifiedTime = get_modified_time();
  return 1;
}

void sblib_close(void) {
  free(programSource);
}
