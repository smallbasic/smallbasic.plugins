// This file is part of SmallBASIC
//
// Support for dictionary/associative array variables
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2007-2016 Chris Warren-Smith.

#include "config.h"

#include <cstdlib>
#include <cstring>
#include <cassert>
#include <ctype.h>
#include <cstdio>

#include "include/var.h"
#include "include/hashmap.h"

#define MAP_SIZE 32

/**
 * Our internal tree element node
 */
typedef struct Node {
  var_p_t key;
  var_p_t value;
  struct Node *left, *right;
} Node;

/**
 * Returns a new tree node
 */
Node *tree_create_node(var_p_t key) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->key = key;
  node->value = nullptr;
  node->left = nullptr;
  node->right = nullptr;
  return node;
}

int str_compare(const char *s1, int s1n, const char *s2, int s2n) {
  int result = 0;
  for (int i = 0;; i++) {
    if (i == s1n || i == s2n) {
      result = s1n < s2n ? -1 : s1n > s2n ? 1 : 0;
      break;
    }
    char c1 = s1[i];
    char c2 = s2[i];
    if (c1 != c2) {
      c1 = tolower(c1);
      c2 = tolower(c2);
      if (c1 != c2) {
        result = c1 < c2 ? -1 : 1;
        break;
      }
    }
  }
  return result;
}

static inline int tree_compare(const char *key, int length, var_p_t vkey) {
  int len1 = length;
  if (len1 && key[len1 - 1] == '\0') {
    len1--;
  }
  int len2 = vkey->v.p.length;
  if (len2 && vkey->v.p.ptr[len2 - 1] == '\0') {
    len2--;
  }
  return str_compare(key, len1, vkey->v.p.ptr, len2);
}

Node *tree_search(Node **rootp, const char *key, int length) {
  while (*rootp != nullptr) {
    int r = tree_compare(key, length, (*rootp)->key);
    if (r == 0) {
      return *rootp;
    }
    rootp = (r < 0) ? &(*rootp)->left : &(*rootp)->right;
  }
  Node *result = tree_create_node(nullptr);
  *rootp = result;
  return result;
}

Node *tree_find(Node **rootp, const char *key, int length) {
  while (*rootp != nullptr) {
    int r = tree_compare(key, length, (*rootp)->key);
    if (r == 0) {
      return *rootp;
    }
    rootp = (r < 0) ? &(*rootp)->left : &(*rootp)->right;
  }
  return nullptr;
}

int hashmap_get_hash(const char *key, int length) {
  int hash = 1, i;
  for (i = 0; i < length && key[i] != '\0'; i++) {
    hash += tolower(key[i]);
    hash <<= 3;
    hash ^= (hash >> 3);
  }
  return hash;
}

static inline Node *hashmap_search(var_p_t map, const char *key, int length) {
  int index = hashmap_get_hash(key, length) % map->v.m.size;
  Node **table = (Node **)map->v.m.map;
  Node *result = table[index];
  if (result == nullptr) {
    // new entry
    result = table[index] = tree_create_node(nullptr);
  } else {
    int r = tree_compare(key, length, result->key);
    if (r < 0) {
      result = tree_search(&result->left, key, length);
    } else if (r > 0) {
      result = tree_search(&result->right, key, length);
    }
  }
  return result;
}

static inline Node *hashmap_find(var_p_t map, const char *key) {
  int length = strlen(key);
  int index = hashmap_get_hash(key, length) % map->v.m.size;
  Node **table = (Node **)map->v.m.map;
  Node *result = table[index];
  if (result != nullptr) {
    int r = tree_compare(key, length, result->key);
    if (r < 0 && result->left != nullptr) {
      result = tree_find(&result->left, key, length);
    } else if (r > 0 && result->right != nullptr) {
      result = tree_find(&result->right, key, length);
    } else if (r != 0) {
      result = nullptr;
    }
  }
  return result;
}

void hashmap_create(var_p_t map, int size) {
  map->type = V_MAP;
  map->v.m.count = 0;
  map->v.m.id = -1;
  if (size == 0) {
    map->v.m.size = MAP_SIZE;
  } else {
    map->v.m.size = (size * 100) / 75;
  }
  map->v.m.map = calloc(map->v.m.size, sizeof(Node *));
}

var_p_t hashmap_putv(var_p_t map, const var_p_t key) {
  assert(key->type == V_STR);
  Node *node = hashmap_search(map, key->v.p.ptr, key->v.p.length);
  assert(node->key == nullptr);
  node->key = key;
  node->value = v_new();
  map->v.m.count++;
  return node->value;
}

var_p_t hashmap_get(var_p_t map, const char *key) {
  var_p_t result;
  Node *node = hashmap_find(map, key);
  if (node != nullptr) {
    result = node->value;
  } else {
    result = nullptr;
  }
  return result;
}

