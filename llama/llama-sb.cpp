// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2026 Chris Warren-Smith

#include <chrono>
#include "llama.h"
#include "llama-sb.h"

constexpr int MAX_REPEAT = 5;

LlamaIter::LlamaIter() :
  _llama(nullptr),
  _tokens_sec(0),
  _repetition_count(0),
  _has_next(false) {
}

Llama::Llama() :
  _model(nullptr),
  _ctx(nullptr),
  _sampler(nullptr),
  _vocab(nullptr),
  _penalty_last_n(0),
  _penalty_repeat(0),
  _temperature(0),
  _top_k(0),
  _top_p(0),
  _min_p(0),
  _max_tokens(0),
  _log_level(GGML_LOG_LEVEL_CONT) {
  llama_log_set([](enum ggml_log_level level, const char * text, void *user_data) {
    Llama *llama = (Llama *)user_data;
    if (level > llama->_log_level) {
      fprintf(stderr, "LLAMA: %s", text);
    }
  }, this);
  reset();
}

Llama::~Llama() {
  if (_sampler) {
    llama_sampler_free(_sampler);
  }
  if (_ctx) {
    llama_free(_ctx);
  }
  if (_model) {
    llama_model_free(_model);
  }
}

void Llama::reset() {
  _stop_sequences.clear();
  _last_error = "";
  _penalty_last_n = 64;
  _penalty_repeat = 1.1f;
  _temperature = 0;
  _top_k = 0;
  _top_p = 1.0f;
  _min_p = 0.0f;
  _max_tokens = 150;
}

bool Llama::construct(string model_path, int n_ctx, int n_batch) {
  ggml_backend_load_all();

  llama_model_params mparams = llama_model_default_params();
  mparams.n_gpu_layers = 0;

  _model = llama_model_load_from_file(model_path.c_str(), mparams);
  if (!_model) {
    _last_error = "failed to load model";
  } else {
    llama_context_params cparams = llama_context_default_params();
    cparams.n_ctx   = n_ctx;
    cparams.n_batch = n_batch;
    cparams.no_perf = true;
    _ctx = llama_init_from_model(_model, cparams);
    if (!_ctx) {
      _last_error = "failed to create context";
    } else {
      _vocab = llama_model_get_vocab(_model);

      auto sparams = llama_sampler_chain_default_params();
      sparams.no_perf = false;
      _sampler = llama_sampler_chain_init(sparams);
    }
  }
  return _last_error.empty();
}

void Llama::configure_sampler() {
  llama_sampler_reset(_sampler);
  if (_penalty_last_n != 0 && _penalty_repeat != 1.0f) {
    auto penalties = llama_sampler_init_penalties(_penalty_last_n, _penalty_repeat, 0.0f, 0.0f);
    llama_sampler_chain_add(_sampler, penalties);
  }
  if (_temperature <= 0.0f) {
    llama_sampler_chain_add(_sampler, llama_sampler_init_greedy());
  } else {
    llama_sampler_chain_add(_sampler, llama_sampler_init_temp(_temperature));
    if (_top_k > 0) {
      llama_sampler_chain_add(_sampler, llama_sampler_init_top_k(_top_k));
    }
    if (_top_p < 1.0f) {
      llama_sampler_chain_add(_sampler, llama_sampler_init_top_p(_top_p, 1));
    }
    if (_min_p > 0.0f) {
      llama_sampler_chain_add(_sampler, llama_sampler_init_min_p(_min_p, 1));
    }
    llama_sampler_chain_add(_sampler, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));
  }
}

vector<llama_token> Llama::tokenize(const string &prompt) {
  vector<llama_token> result;

  int n_prompt = -llama_tokenize(_vocab, prompt.c_str(), prompt.size(), nullptr, 0, true, true);
  if (n_prompt <= 0) {
    _last_error = "failed to tokenize prompt";
  } else {
    result.reserve(n_prompt);
    result.resize(n_prompt);
    if (llama_tokenize(_vocab, prompt.c_str(), prompt.size(),
                       result.data(), n_prompt, true, true) < 0) {
      _last_error = "failed to tokenize prompt";
    }
  }
  return result;
}

bool Llama::generate(LlamaIter &iter, const string &prompt) {
  vector<llama_token> prompt_tokens = tokenize(prompt);
  if (prompt_tokens.size() == 0) {
    return false;
  }

  // decode prompt
  llama_batch batch = llama_batch_get_one(prompt_tokens.data(), prompt_tokens.size());
  if (llama_decode(_ctx, batch)) {
    _last_error = "failed to eval prompt";
    return false;
  }

  // handle encoder models
  if (llama_model_has_encoder(_model)) {
    llama_token decoder_start_token_id = llama_model_decoder_start_token(_model);
    if (decoder_start_token_id == LLAMA_TOKEN_NULL) {
      decoder_start_token_id = llama_vocab_bos(_vocab);
    }
    batch = llama_batch_get_one(&decoder_start_token_id, 1);
    if (llama_decode(_ctx, batch)) {
      _last_error = "failed to eval decoder start token";
      return false;
    }
  }

  configure_sampler();

  iter._llama = this;
  iter._has_next = true;
  iter._tokens_sec = 0;
  return true;
}

string Llama::next(LlamaIter &iter) {
  string out;

  vector<llama_token> decoded;
  decoded.reserve(_max_tokens);

  int generated = 0;
  auto t_start = std::chrono::high_resolution_clock::now();

  while (generated < _max_tokens) {
    // sample one token from the current logits
    llama_token tok = llama_sampler_sample(_sampler, _ctx, -1);

    // end-of-generation check
    if (llama_vocab_is_eog(_vocab, tok)) {
      iter._has_next = false;
      break;
    }

    // append token to decoded list
    decoded.push_back(tok);
    ++generated;

    // decode the token
    llama_batch batch = llama_batch_get_one(&tok, 1);
    if (llama_decode(_ctx, batch)) {
      _last_error = "failed to eval token during generation";
      break;
    }
  }

  // detokenize sequentially
  if (!decoded.empty()) {
    for (llama_token tok : decoded) {
      if (!llama_vocab_is_control(_vocab, tok)) {
        char buf[512];
        int n = llama_token_to_piece(_vocab, tok, buf, sizeof(buf), 0, false);
        if (n > 0) {
          if (iter._last_word == buf) {
            if (++iter._repetition_count == MAX_REPEAT) {
              iter._has_next = false;
              break;
            }
          } else {
            iter._repetition_count = 0;
            iter._last_word = buf;
          }
          out.append(buf, n);

          for (const auto &stop : _stop_sequences) {
            size_t pos = out.find(stop);
            if (pos != std::string::npos) {
              // found stop sequence - truncate and signal end
              out = out.substr(0, pos);
              iter._has_next = false;
              break;
            }
          }
        }
      }
    }
  }

  // timing
  auto t_end = std::chrono::high_resolution_clock::now();
  double secs = std::chrono::duration<double>(t_end - t_start).count();
  iter._tokens_sec = secs > 0 ? generated / secs : 0;

  return out;
}

