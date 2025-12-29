// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2026 Chris Warren-Smith

#pragma once

#include <chrono>
#include <string>
#include <vector>
#include "llama.h"

using namespace std;

struct Llama;

struct LlamaIter {
  explicit LlamaIter();
  ~LlamaIter() {}

  Llama *_llama;
  string _last_word;
  chrono::high_resolution_clock::time_point _t_start;
  int _repetition_count;
  int _tokens_generated;
  bool _has_next;
};

struct Llama {
  explicit Llama();
  ~Llama();

  // init
  bool construct(string model_path, int n_ctx, int n_batch, int n_gpu_layers);

  // generation
  bool generate(LlamaIter &iter, const string &prompt);
  string next(LlamaIter &iter);

  // generation parameters
  void add_stop(const char *stop) { _stop_sequences.push_back(stop); }
  void clear_stops() { _stop_sequences.clear(); }
  void set_penalty_last_n(int32_t penalty_last_n) { _penalty_last_n = penalty_last_n; }
  void set_penalty_repeat(float penalty_repeat) { _penalty_repeat = penalty_repeat; }
  void set_max_tokens(int max_tokens) { _max_tokens = max_tokens; }
  void set_min_p(float min_p) { _min_p = min_p; }
  void set_temperature(float temperature) { _temperature = temperature; }
  void set_top_k(int top_k) { _top_k = top_k; }
  void set_top_p(float top_p) { _top_p = top_p; }

  // error handling
  const char *last_error() { return _last_error.c_str(); }
  void set_log_level(int level) { _log_level = level; }
  void reset();

  private:
  bool ends_with_sentence_boundary(const string &out);
  void configure_sampler();
  bool make_space_for_tokens(int n_tokens, int keep_min);
  vector<llama_token> tokenize(const string &prompt);

  llama_model *_model;
  llama_context *_ctx;
  llama_sampler *_sampler;
  const llama_vocab *_vocab;
  vector<string> _stop_sequences;
  string _last_error;
  int32_t _penalty_last_n;
  float _penalty_repeat;
  float _temperature;
  float _top_p;
  float _min_p;
  int _top_k;
  int _max_tokens;
  int _log_level;
};
