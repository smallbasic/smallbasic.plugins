// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2020 Chris Warren-Smith

#pragma once

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
bool is_param_int_byref(int argc, slib_par_t *params, int arg);
bool is_param_array(int argc, slib_par_t *params, int n);
bool is_param_num(int argc, slib_par_t *params, int n);
bool is_param_str(int argc, slib_par_t *params, int n);
bool is_param_map(int argc, slib_par_t *params, int n);
bool is_param_nil(int argc, slib_par_t *params, int n);
int get_param_int(int argc, slib_par_t *params, int n, int def);
int set_param_int(int argc, slib_par_t *params, int n, int value, var_t *retval);
var_num_t get_param_num(int argc, slib_par_t *params, int n, var_num_t def);
var_num_t get_param_num_field(int argc, slib_par_t *params, int n, const char *field);
var_num_t get_map_num(var_p_t map, const char *name);
var_num_t get_array_elem_num(var_p_t array, int index);
const char *get_param_str(int argc, slib_par_t *params, int n, const char *def);
const char *get_param_str_field(int argc, slib_par_t *params, int n, const char *field);
const char *format_text(int argc, slib_par_t *params, int n);
