// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2026 Chris Warren-Smith

#pragma once

#include <string>
#include "llama.h"

using namespace std;

struct Llama {
  explicit Llama();
  ~Llama();

  bool create(string model_path, int n_ctx, bool disable_log);
  string generate(const string &prompt,
                  int max_tokens = 128,
                  float temperature = 0.8f,
                  bool echo = true,
                  bool clear_cache = true);
  const char *last_error() { return _last_error.c_str(); }

  private:
  string build_chat_prompt(const string &user_msg);
  void configure_sampler(float temperature);

  llama_model *_model;
  llama_context *_ctx;
  llama_sampler *_sampler;
  const llama_vocab *_vocab;
  string _chat_prompt;
  string _last_error;
  float _temperature;
  int _n_ctx;
};
