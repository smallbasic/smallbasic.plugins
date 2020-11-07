// This file is part of SmallBASIC
//
// Support for dictionary/associative array variables
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2007-2020 Chris Warren-Smith.

#ifndef _HASHMAP_H_
#define _HASHMAP_H_

#include "include/var.h"

void hashmap_create(var_p_t map, int size);
var_p_t hashmap_putv(var_p_t map, const var_p_t key);
var_p_t hashmap_get(var_p_t map, const char *key);

#endif /* !_HASHMAP_H_ */

