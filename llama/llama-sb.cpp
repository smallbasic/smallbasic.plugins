// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2026 Chris Warren-Smith

#include <chrono>
#include <vector>
#include "llama.h"
#include "llama-sb.h"

Llama::Llama() :
  _model(nullptr),
  _ctx(nullptr),
  _sampler(nullptr),
  _vocab(nullptr),
  _penalty_last_n(64),
  _penalty_repeat(1.1f),
  _temperature(0),
  _top_k(0),
  _top_p(1.0f),
  _min_p(0.0f),
  _max_tokens(150),
  _log_level(GGML_LOG_LEVEL_CONT) {
  llama_log_set([](enum ggml_log_level level, const char * text, void *user_data) {
    Llama *llama = (Llama *)user_data;
    if (level > llama->_log_level) {
      fprintf(stderr, "LLAMA: %s", text);
    }
  }, this);
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

void Llama::reset() {
  // llama_kv_cache_clear(it->second->ctx);
  _chat_prompt.clear();
}

string Llama::generate(const string &prompt) {
  string out = prompt;

  // ---- tokenize prompt ----
  int n_prompt = -llama_tokenize(_vocab, prompt.c_str(), prompt.size(),
                                 nullptr, 0, true, true);

  if (n_prompt <= 0) {
    _last_error = "failed to tokenize prompt";
    return out;
  }

  std::vector<llama_token> prompt_tokens(n_prompt);
  if (llama_tokenize(_vocab, prompt.c_str(), prompt.size(),
                     prompt_tokens.data(), n_prompt, true, true) < 0) {
    _last_error = "failed to tokenize prompt";
    return out;
  }

  // ---- sampler ----
  configure_sampler();

  // ---- decode prompt ----
  llama_batch batch = llama_batch_get_one(prompt_tokens.data(), n_prompt);
  if (llama_decode(_ctx, batch)) {
    _last_error = "failed to eval prompt";
    return out;
  }

  // ---- handle encoder models ----
  if (llama_model_has_encoder(_model)) {
    llama_token decoder_start_token_id = llama_model_decoder_start_token(_model);
    if (decoder_start_token_id == LLAMA_TOKEN_NULL) {
      decoder_start_token_id = llama_vocab_bos(_vocab);
    }
    batch = llama_batch_get_one(&decoder_start_token_id, 1);
    if (llama_decode(_ctx, batch)) {
      _last_error = "failed to eval decoder start token";
      return out;
    }
  }

  // ---- generation loop ----
  std::vector<llama_token> decoded;
  decoded.reserve(_max_tokens);

  int generated = 0;
  auto t_start = std::chrono::high_resolution_clock::now();

  while (generated < _max_tokens) {
    // sample one token from the current logits
    llama_token tok = llama_sampler_sample(_sampler, _ctx, -1);

    // end-of-generation check
    if (llama_vocab_is_eog(_vocab, tok)) {
      break;
    }

    // append token to decoded list
    decoded.push_back(tok);
    ++generated;

    // ---- decode the token immediately ----
    llama_batch batch = llama_batch_get_one(&tok, 1);
    if (llama_decode(_ctx, batch)) {
      _last_error = "failed to eval token during generation";
      break;
    }
  }

  // ---- detokenize sequentially ----
  if (!decoded.empty()) {
    char buf[512];
    for (llama_token tok : decoded) {
      if (llama_vocab_is_control(_vocab, tok)) {
        continue;
      }
      int n = llama_token_to_piece(_vocab, tok, buf, sizeof(buf), 0, false);
      if (n > 0) {
        out.append(buf, n);
      }
    }
  }

  // ---- timing ----
  auto t_end = std::chrono::high_resolution_clock::now();
  double secs = std::chrono::duration<double>(t_end - t_start).count();
  double tokps = secs > 0 ? generated / secs : 0;

  fprintf(stderr,
          "[tok/s=%.2f] generated=%d time=%.3fs\n",
          tokps, generated, secs);

  return out;
}

