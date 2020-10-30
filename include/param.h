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

int is_param_array(int argc, slib_par_t *params, int n);
int is_param_num(int argc, slib_par_t *params, int n);
int is_param_str(int argc, slib_par_t *params, int n);
int is_param_map(int argc, slib_par_t *params, int n);
int is_param_nil(int argc, slib_par_t *params, int n);
int get_param_int(int argc, slib_par_t *params, int n, int def);
var_num_t get_param_num(int argc, slib_par_t *params, int n, var_num_t def);
var_num_t get_param_num_field(int argc, slib_par_t *params, int n, const char *field);
const char *get_param_str(int argc, slib_par_t *params, int n, const char *def);
const char *get_param_str_field(int argc, slib_par_t *params, int n, const char *field);
float get_array_elem_num(var_p_t array, int index);
