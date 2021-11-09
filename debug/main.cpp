// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2021 Chris Warren-Smith

#include "config.h"

#include <sys/stat.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>

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

static void bload_read(int bitSize, size_t size, FILE *file, var_t *retval) {
  size_t count;
  uint8_t *data8;
  uint16_t *data16;
  uint32_t *data32;
  switch (bitSize) {
  case 8:
    data8 = (uint8_t *)malloc(size * sizeof(uint8_t));
    count = fread(data8, sizeof(uint8_t), size, file);
    v_toarray1(retval, count);
    for (unsigned i = 0; i < count; i++) {
      v_setint(v_elem(retval, i), data8[i]);
    }
    free(data8);
    break;
  case 16:
    data16 = (uint16_t *)malloc(size * sizeof(uint16_t));
    count = fread(data16, sizeof(uint16_t), size, file);
    v_toarray1(retval, count);
    for (unsigned i = 0; i < count; i++) {
      v_setint(v_elem(retval, i), data16[i]);
    }
    free(data16);
    break;
  case 32:
    data32 = (uint32_t *)malloc(size * sizeof(uint32_t));
    count = fread(data32, sizeof(uint32_t), size, file);
    v_toarray1(retval, count);
    for (unsigned i = 0; i < count; i++) {
      v_setint(v_elem(retval, i), data32[i]);
    }
    free(data32);
    break;
  }
}

static int cmd_bload(int argc, slib_par_t *params, var_t *retval) {
  auto fileName = get_param_str(argc, params, 0, nullptr);
  auto offset = get_param_int(argc, params, 1, 0);
  auto length = get_param_int(argc, params, 2, 0);
  auto bitSize = get_param_int(argc, params, 3, 8);
  char message[256] = {0};

  if (fileName != nullptr && offset >= 0 && length >= 0 && (bitSize == 8 || bitSize == 16 || bitSize == 32)) {
    auto file = fopen(fileName, "rb");
    if (file != nullptr) {
      fseek(file, 0, SEEK_END);
      auto size = ftell(file);
      if (size > 0) {
        if (offset < size) {
          if (offset + length < size) {
            // [0 | 1 | 2 | 3 | 4]
            //    --^     --^
            if (length) {
              size = length;
            }
          } else {
            // [0 | 1 | 2 | 3 | 4]
            //    --^             --^
            size -= offset;
          }
          fseek(file, offset, SEEK_SET);
          bload_read(bitSize, size, file, retval);
        } else {
          // [0 | 1 | 2 | 3 | 4]
          //                   --^  --^
          v_setint(retval, 0);
        }
      } else {
        snprintf(message, sizeof(message), "BLOAD: [%s] Failed to read file", fileName);
      }
      fclose(file);
    } else {
      snprintf(message, sizeof(message), "BLOAD: [%s] Failed to open file", fileName);
    }
  } else {
    snprintf(message, sizeof(message), "BLOAD: fileName [offset [length [bitSize 8|16|32]]]");
  }
  if (message[0] != '\0') {
    v_setstr(retval, message);
  }
  return message[0] == '\0';
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
  {1, 4, "BLOAD", cmd_bload},
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

SBLIB_API int sblib_func_exec(int index, int argc, slib_par_t *params, var_t *retval) {
  int result;
  if (index >= 0 && index < sblib_func_count()) {
    if (argc < lib_func[index]._min || argc > lib_func[index]._max) {
      if (lib_func[index]._min == lib_func[index]._max) {
        error(retval, lib_func[index]._name, lib_func[index]._min);
      } else {
        error(retval, lib_func[index]._name, lib_func[index]._min, lib_func[index]._max);
      }
      result = 0;
    } else {
      result = lib_func[index]._command(argc, params, retval);
    }
  } else {
    fprintf(stderr, "Debug: FUNC index error [%d]\n", index);
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
