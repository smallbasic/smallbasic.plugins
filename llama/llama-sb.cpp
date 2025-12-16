// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2026 Chris Warren-Smith

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "llama.h"
#include "llama-sb.h"

Llama::Llama() :
  _model(nullptr),
  _ctx(nullptr),
  _sampler(nullptr),
  _vocab(nullptr),
  _temperature(0),
  _n_ctx(0) {
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

void Llama::append_response(const string &response) {
  _chat_prompt += response;
  _chat_prompt += "\n";
}

const string Llama::build_chat_prompt(const string &user_msg) {
  _chat_prompt += "User: ";
  _chat_prompt += user_msg;
  _chat_prompt += "\nAssistant: ";
  return _chat_prompt;
}

bool Llama::create(string model_path, int n_ctx, bool disable_log) {
  if (disable_log) {
    // only print errors
    llama_log_set([](enum ggml_log_level level, const char * text, void * /* user_data */) {
      if (level >= GGML_LOG_LEVEL_ERROR) {
        fprintf(stderr, "%s", text);
      }
    }, nullptr);
  }

  ggml_backend_load_all();

  llama_model_params mparams = llama_model_default_params();
  mparams.n_gpu_layers = 0;

  _model = llama_model_load_from_file(model_path.c_str(), mparams);
  if (!_model) {
    _last_error = "failed to load model";
  } else {
    llama_context_params cparams = llama_context_default_params();
    cparams.n_ctx   = n_ctx;
    cparams.n_batch = n_ctx;

    _ctx = llama_init_from_model(_model, cparams);
    if (!_ctx) {
      _last_error = "failed to create context";
    } else {
      _vocab = llama_model_get_vocab(_model);
      configure_sampler(0);
    }
  }
  return _last_error.empty();
}

void Llama::configure_sampler(float temperature) {
  if (temperature != _temperature || _sampler == nullptr) {
    if (_sampler) {
      llama_sampler_free(_sampler);
    }
    auto sparams = llama_sampler_chain_default_params();
    _sampler = llama_sampler_chain_init(sparams);
    _temperature = temperature;

    //  llama_sampler_chain_reset(sampler);
    if (temperature <= 0.0f) {
      llama_sampler_chain_add(_sampler, llama_sampler_init_greedy());
    } else {
      llama_sampler_chain_add(_sampler, llama_sampler_init_temp(temperature));
    }
  }
}

static std::vector<llama_token> tokenize(const llama_vocab *vocab, const string &text) {
  int n = -llama_tokenize(vocab, text.c_str(), text.size(), nullptr, 0, true, true);
  std::vector<llama_token> tokens(n);
  llama_tokenize(vocab, text.c_str(), text.size(), tokens.data(), tokens.size(), true, true);
  return tokens;
}

string Llama::generate(const string &prompt, int max_tokens, float temperature, bool echo, bool clear_cache) {
  string out;

  if (clear_cache) {
    // llama_kv_cache_clear(_ctx);
  }

  auto prompt_tokens = tokenize(_vocab, prompt);
  configure_sampler(temperature);

  llama_batch batch = llama_batch_get_one(prompt_tokens.data(), prompt_tokens.size());

  if (llama_decode(_ctx, batch)) {
    _last_error = "decode failed";
    return out;
  }

  if (echo) {
    out += prompt;
  }

  for (int i = 0; i < max_tokens; ++i) {
    llama_token tok = llama_sampler_sample(_sampler, _ctx, -1);

    if (llama_vocab_is_eog(_vocab, tok)) {
      break;
    }

    char buf[128];
    int n = llama_token_to_piece(_vocab, tok, buf, sizeof(buf), 0, true);

    if (n > 0) {
      out.append(buf, n);
    }
    batch = llama_batch_get_one(&tok, 1);
    if (llama_decode(_ctx, batch)) {
      break;
    }
  }

  return out;
}

void Llama::reset() {
  // llama_kv_cache_clear(it->second->ctx);
  _chat_prompt.clear();
}
