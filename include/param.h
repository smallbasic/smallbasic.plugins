// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2020 Chris Warren-Smith

#pragma once

#include "config.h"
#include "var.h"
#include "module.h"

typedef struct API {
  const char *name;
  int (*command)(int, slib_par_t *, var_t *retval);
} API;

typedef struct FUNC_SIG {
  int _min;
  int _max;
  const char *_name;
  int (*_command)(int, slib_par_t *, var_t *retval);
} FUNC_SIG;

void error(var_p_t var, const char *field, int nMin, int nMax);
void error(var_p_t var, const char *field, int n);
void error(var_p_t var, const char *text);
void map_init(var_p_t map);
void map_init_id(var_p_t map, int id);
void map_set_int(var_p_t base, const char *name, var_int_t n);
void v_free(var_t *var);
bool get_bool(var_p_t var);
bool is_array(var_p_t var, uint32_t size);
bool is_map(var_p_t var);
bool is_param_int_byref(int argc, slib_par_t *params, int arg);
bool is_param_array(int argc, slib_par_t *params, int n);
bool is_param_num(int argc, slib_par_t *params, int n);
bool is_param_str(int argc, slib_par_t *params, int n);
bool is_param_map(int argc, slib_par_t *params, int n);
bool is_param_nil(int argc, slib_par_t *params, int n);
int map_get_bool(var_p_t base, const char *name);
int get_param_int(int argc, slib_par_t *params, int n, int def);
int set_param_int(int argc, slib_par_t *params, int n, int value, var_t *retval);
var_num_t get_num(var_p_t var);
var_num_t get_param_num(int argc, slib_par_t *params, int n, var_num_t def);
var_num_t get_param_num_field(int argc, slib_par_t *params, int n, const char *field);
var_num_t get_map_num(var_p_t map, const char *name);
var_num_t get_array_elem_num(var_p_t array, int index);
var_p_t map_add_var(var_p_t base, const char *name, int value);
var_p_t map_get(var_p_t base, const char *name);
const char *get_param_str(int argc, slib_par_t *params, int n, const char *def);
const char *get_param_str_field(int argc, slib_par_t *params, int n, const char *field);
const char *format_text(int argc, slib_par_t *params, int n);

#if !defined(SBLIB_API)
 #if defined(_WIN32)
   #define SBLIB_API __declspec(dllexport)
 #else
   #define SBLIB_API
 #endif
#endif
