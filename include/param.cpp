// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2020 Chris Warren-Smith

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ctype.h>

#include "config.h"
#include "param.h"
#include "hashmap.h"
#include "var.h"

#define MAX_TEXT_BUFFER_LENGTH 1024

void error(var_p_t var, const char *field, int nMin, int nMax) {
  char message[256];
  snprintf(message, sizeof(message), "Invalid Input: [%s] - expected [%d - %d] arguments", field, nMin, nMax);
  v_setstr(var, message);
}

void error(var_p_t var, const char *field, int n) {
  char message[256];
  snprintf(message, sizeof(message), "Invalid Input: [%s] - expected [%d] arguments", field, n);
  v_setstr(var, message);
}

void error(var_p_t var, const char *text) {
  char message[256];
  snprintf(message, sizeof(message), "Error: [%s]", text);
  v_setstr(var, message);
}

var_num_t get_num(var_p_t var) {
  var_num_t result;
  switch (var->type) {
  case V_INT:
    result = var->v.i;
    break;
  case V_NUM:
    result = var->v.n;
    break;
  default:
    result = 0.0;
    break;
  }
  return result;
}

bool get_bool(var_p_t var) {
  bool result;
  switch (var->type) {
  case V_INT:
    result = (var->v.i != 0);
    break;
  case V_NUM:
    result = (var->v.n != 0);
    break;
  case V_STR:
    result = (strncasecmp(var->v.p.ptr, "true", 4));
    break;
  default:
    result = false;
    break;
  }
  return result;
}

int get_int(var_t *v) {
  int result;
  switch (v ? v->type : -1) {
  case V_INT:
    result = v->v.i;
    break;
  case V_NUM:
    result = v->v.n;
    break;
  default:
    result = 0;
    break;
  }
  return result;
}

void v_init(var_t *v) {
  v->type = V_INT;
  v->const_flag = 0;
  v->v.i = 0;
}

uint32_t v_get_capacity(uint32_t size) {
  return size + (size / 2) + 1;
}

void v_alloc_capacity(var_t *var, uint32_t size) {
  uint32_t capacity = v_get_capacity(size);
  v_capacity(var) = capacity;
  v_asize(var) = size;
  v_data(var) = (var_t *)malloc(sizeof(var_t) * capacity);
  if (v_data(var)) {
    for (uint32_t i = 0; i < capacity; i++) {
      var_t *e = v_elem(var, i);
      e->pooled = 0;
      v_init(e);
    }
  }
}

var_t *v_new() {
  var_t *result = (var_t *)malloc(sizeof(var_t));
  result->pooled = 0;
  v_init(result);
  return result;
}

void v_array_free(var_t *var) {
  uint32_t v_size = v_capacity(var);
  if (v_size && v_data(var)) {
    for (uint32_t i = 0; i < v_size; i++) {
      v_free(v_elem(var, i));
    }
    free(var->v.a.data);
  }
}

void v_free(var_t *var) {
  switch (var->type) {
  case V_STR:
    if (var->v.p.owner) {
      free(var->v.p.ptr);
    }
    break;
  case V_ARRAY:
    v_array_free(var);
    break;
  case V_MAP:
    assert("cannot free map");
    break;
  }
}

int set_param_int(int argc, slib_par_t *params, int param, int value, var_t *retval) {
  int result;
  if (argc < param || !params[param].byref || params[param].var_p->type != V_INT) {
    v_setstr(retval, "Cannot assign argument reference");
    result = 0;
  } else {
    v_setint(params[param].var_p, value);
    result = 1;
  }
  return result;
}

void v_setint(var_t *var, var_int_t i) {
  v_free(var);
  var->type = V_INT;
  var->v.i = i;
}

void v_setreal(var_t *var, var_num_t n) {
  v_free(var);
  var->type = V_NUM;
  var->v.n = n;
}

void v_setstr(var_t *var, const char *str) {
  assert(var->type != V_ARRAY && var->type != V_MAP);

  bool isSet = false;
  if (var->type == V_STR) {
    if (strcmp(str, var->v.p.ptr) == 0) {
      // already set
      isSet = true;
    } else if (var->v.p.owner) {
      free(var->v.p.ptr);
    }
  }
  if (!isSet) {
    int length = strlen(str == nullptr ? 0 : str);
    var->type = V_STR;
    var->v.p.ptr = (char *)malloc(length + 1);
    var->v.p.ptr[0] = '\0';
    var->v.p.length = length + 1;
    var->v.p.owner = 1;
    strcpy(var->v.p.ptr, str);
  }
}

int v_strlen(const var_t *v) {
  int result;
  if (v->type == V_STR) {
    result = v->v.p.length;
    if (result && v->v.p.ptr[result - 1] == '\0') {
      result--;
    }
  } else {
    result = 0;
  }
  return result;
}

void v_new_array(var_t *var, uint32_t size) {
  assert(var->type == V_INT);
  var->type = V_ARRAY;
  v_alloc_capacity(var, size);
}

void v_toarray1(var_t *v, uint32_t r) {
  v_new_array(v, r);
  v_maxdim(v) = 1;
  v_lbound(v, 0) = 0;
  v_ubound(v, 0) = r - 1;
}

void v_tomatrix(var_t *v, int r, int c) {
  v_new_array(v, r * c);
  v_maxdim(v) = 2;
  v_lbound(v, 0) = v_lbound(v, 1) = 0;
  v_ubound(v, 0) = r - 1;
  v_ubound(v, 1) = c - 1;
}

var_p_t map_add_var(var_p_t base, const char *name, int value) {
  var_p_t key = v_new();
  v_setstr(key, name);
  var_p_t var = hashmap_putv(base, key);
  v_setint(var, value);
  return var;
}

var_p_t map_get(var_p_t base, const char *name) {
  var_p_t result;
  if (base != nullptr && base->type == V_MAP) {
    result = hashmap_get(base, name);
  } else {
    result = nullptr;
  }
  return result;
}

void map_init(var_p_t map) {
  assert(map->type == V_INT);
  v_init(map);
  hashmap_create(map, 0);
}

void map_init_id(var_p_t map, int id) {
  map_init(map);
  map->v.m.id = id;
}

int map_get_bool(var_p_t base, const char *name) {
  var_p_t var = map_get(base, name);
  return var != nullptr ? get_bool(var) : 0;
}

void map_set_int(var_p_t base, const char *name, var_int_t n) {
  var_p_t var = map_get(base, name);
  if (var != nullptr) {
    v_setint(var, n);
  }
}

int map_get_int(var_p_t base, const char *name, int def) {
  var_p_t var = map_get(base, name);
  return var != nullptr ? get_int(var) : def;
}

bool is_array(var_p_t var, uint32_t size) {
  return var != nullptr && v_is_type(var, V_ARRAY) && v_asize(var) == size;
}

bool is_map(var_p_t var) {
  return var != nullptr && v_is_type(var, V_MAP);
}

bool is_param_int_byref(int argc, slib_par_t *params, int arg) {
  return (arg < argc && params[arg].byref &&
          (v_is_type(params[arg].var_p, V_INT) || v_is_type(params[arg].var_p, V_NUM)));
}

bool is_param_array(int argc, slib_par_t *params, int n) {
  bool result;
  if (n >= 0 && n < argc) {
    result = (params[n].var_p->type == V_ARRAY);
  } else {
    result = false;
  }
  return result;
}

bool is_param_num(int argc, slib_par_t *params, int n) {
  int result;
  if (n >= 0 && n < argc) {
    result = (params[n].var_p->type == V_NUM ||
              params[n].var_p->type == V_INT);
  } else {
    result = false;
  }
  return result;
}

bool is_param_str(int argc, slib_par_t *params, int n) {
  int result;
  if (n >= 0 && n < argc) {
    result = (params[n].var_p->type == V_STR);
  } else {
    result = false;
  }
  return result;
}

bool is_param_map(int argc, slib_par_t *params, int n) {
  int result;
  if (n >= 0 && n < argc) {
    result = (params[n].var_p->type == V_MAP);
  } else {
    result = false;
  }
  return result;
}

bool is_param_nil(int argc, slib_par_t *params, int n) {
  int result;
  if (n >= 0 && n < argc) {
    result = (params[n].var_p->type == V_NIL);
  } else {
    result = false;
  }
  return result;
}

int get_param_int(int argc, slib_par_t *params, int n, int def) {
  int result;
  if (n >= 0 && n < argc) {
    switch (params[n].var_p->type) {
    case V_INT:
      result = params[n].var_p->v.i;
      break;
    case V_NUM:
      result = params[n].var_p->v.n;
      break;
    default:
      result = def;
    }
  } else {
    result = def;
  }
  return result;
}

var_num_t get_param_num(int argc, slib_par_t *params, int n, var_num_t def) {
  var_num_t result;
  if (n >= 0 && n < argc) {
    switch (params[n].var_p->type) {
    case V_INT:
      result = params[n].var_p->v.i;
      break;
    case V_NUM:
      result = params[n].var_p->v.n;
      break;
    default:
      result = def;
      break;
    }
  } else {
    result = def;
  }
  return result;
}

var_num_t get_param_num_field(int argc, slib_par_t *params, int n, const char *field) {
  var_num_t result;
  if (is_param_map(argc, params, n)) {
    var_p_t v_value = map_get(params[n].var_p, field);
    if (v_is_type(v_value, V_INT)) {
      result = v_value->v.i;
    } else if (v_is_type(v_value, V_NUM)) {
      result = v_value->v.n;
    } else {
      result = 0;
    }
  } else {
    result = 0;
  }
  return result;
}

const char *get_param_str(int argc, slib_par_t *params, int n, const char *def) {
  const char *result;
  static char buf[256];
  if (n >= 0 && n < argc) {
    switch (params[n].var_p->type) {
    case V_STR:
      result = params[n].var_p->v.p.ptr;
      break;
    case V_INT:
      sprintf(buf, "%lld", params[n].var_p->v.i);
      result = buf;
      break;
    case V_NUM:
      sprintf(buf, "%f", params[n].var_p->v.n);
      result = buf;
      break;
    default:
      result = "";
      break;
    }
  } else {
    result = def == nullptr ? "" : def;
  }
  return result;
}

const char *get_param_str_field(int argc, slib_par_t *params, int n, const char *field) {
  const char *result;
  if (is_param_map(argc, params, n)) {
    var_p_t v_value = map_get(params[n].var_p, field);
    if (v_is_type(v_value, V_STR)) {
      result = v_value->v.p.ptr;
    } else {
      result = nullptr;
    }
  } else {
    result = nullptr;
  }
  return result;
}

var_num_t get_map_num(var_p_t map, const char *name) {
  var_p_t var = map_get(map, name);
  return var != nullptr ? get_num(var) : 0;
}

var_num_t get_array_elem_num(var_p_t array, int index) {
  float result;
  int size = v_asize(array);
  if (index >= 0 && index < size) {
    result = get_num(v_elem(array, index));
  } else {
    result = 0.0;
  }
  return result;
}

int is_format_char(char c) {
  static char specifiers[] = {'d', 'i', 'u', 'o', 'x', 'X', 'f', 'F', 'e', 'E', 'g', 'G', 'a', 'A', 'c', 's', 'n'};
  static int len = sizeof(specifiers) / sizeof(char);
  int result = 0;
  for (int i = 0; i < len && !result; i++) {
    result = (c == specifiers[i]);
  }
  return result;
}

const char *format_text(int argc, slib_par_t *params, int param) {
  // supported for inline use only
  static char buffer[MAX_TEXT_BUFFER_LENGTH];
  const char *format = get_param_str(argc, params, param++, "");
  const char *start = format;
  const int padding = 10;

  char *end = (char *)format;
  int length = 0;
  bool error = false;

  memset(buffer, '\0', MAX_TEXT_BUFFER_LENGTH);

  while (*end != '\0' && !error) {
    if (*end != '%' || param == argc) {
      end++;
    } else {
      while (*end != '\0') {
        // skip next format symbol
        end++;
        if (is_format_char(*end)) {
          // skip terminating format symbol
          char formatChar = *end;
          end++;
          int segLength = end - start;
          if (segLength + length + padding < MAX_TEXT_BUFFER_LENGTH) {
            char cNull = *end;
            int maxChars = MAX_TEXT_BUFFER_LENGTH - length;
            int count;
            *end = '\0';
            // append to buffer, process the next single var-arg
            switch (params[param].var_p->type) {
            case V_INT:
              if (formatChar == 'f') {
                double value =  (double)params[param].var_p->v.i;
                count = snprintf(buffer + length, maxChars, start, value);
              } else if (formatChar != 's') {
                count = snprintf(buffer + length, maxChars, start, params[param].var_p->v.i);
              } else {
                count = -1;
              }
              break;
            case V_NUM:
              if (formatChar != 's') {
                count = snprintf(buffer + length, maxChars, start, params[param].var_p->v.n);
              } else {
                count = -1;
              }
              break;
            case V_STR:
              if (formatChar == 's') {
                count = snprintf(buffer + length, maxChars, start, params[param].var_p->v.p.ptr);
              } else {
                count = -1;
              }
              break;
            default:
              count = -1;
              break;
            }
            param++;
            length += count;
            *end = cNull;
            start = end;
            if (count < 0 || count >= maxChars) {
              error = true;
              break;
            }
          }
          break;
        } else if (*end != '.' && !isdigit(*end)) {
          // non-formatting symbol
          buffer[length++] = *end;
          buffer[length] = '\0';
          end++;
          start = end;
          break;
        }
      }
    }
  }

  if (error) {
    memset(buffer, '\0', MAX_TEXT_BUFFER_LENGTH);
    strcpy(buffer, "ERROR: Invalid text format");
  } else {
    int segLength = end - start;
    if (segLength && (segLength + length + padding < MAX_TEXT_BUFFER_LENGTH)) {
      strncpy(buffer + length, start, segLength);
      buffer[length + segLength + 1] = '\0';
    }
  }

  return buffer;
}

void v_create_func(var_p_t map, const char *name, method cb) {
  var_p_t v_func = map_add_var(map, name, 0);
  v_func->type = V_FUNC;
  v_func->v.fn.cb = cb;
  v_func->v.fn.id = map->v.m.id;
}

