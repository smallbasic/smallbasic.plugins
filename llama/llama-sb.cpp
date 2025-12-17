// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2026 Chris Warren-Smith

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

bool Llama::construct(string model_path, int n_ctx, bool disable_log) {
  if (disable_log) {
    // only print errors
    llama_log_set([](enum ggml_log_level level, const char * text, void * /* user_data */) {
      if (level >= GGML_LOG_LEVEL_ERROR && text[0] != '.' && text[0] != '\n') {
        fprintf(stderr, "%s", text);
      }
    }, nullptr);
  }

  ggml_backend_load_all();

  llama_model_params mparams = llama_model_default_params();
  mparams.n_gpu_layers = 99;

  _model = llama_model_load_from_file(model_path.c_str(), mparams);
  if (!_model) {
    _last_error = "failed to load model";
  } else {
    llama_context_params cparams = llama_context_default_params();
    cparams.n_ctx   = n_ctx;
    cparams.n_batch = n_ctx;
    cparams.no_perf = true;

    _ctx = llama_init_from_model(_model, cparams);
    if (!_ctx) {
      _last_error = "failed to create context";
    } else {
      _vocab = llama_model_get_vocab(_model);
    }
  }
  return _last_error.empty();
}

void Llama::configure_sampler(float temperature) {
  if (temperature != _temperature || _sampler == nullptr) {
    if (_sampler != nullptr) {
      llama_sampler_free(_sampler);
    }
    auto sparams = llama_sampler_chain_default_params();
    sparams.no_perf = false;
    _sampler = llama_sampler_chain_init(sparams);
    _temperature = temperature;

    //  llama_sampler_chain_reset(sampler);
    if (temperature <= 0.0f) {
      llama_sampler_chain_add(_sampler, llama_sampler_init_greedy());
    } else {
      llama_sampler_chain_add(_sampler, llama_sampler_init_min_p(0.05f, 1));
      llama_sampler_chain_add(_sampler, llama_sampler_init_temp(temperature));
      llama_sampler_chain_add(_sampler, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));
    }
  }
}

string Llama::generate(const string &prompt, int max_tokens, float temperature) {
  string out;

  // find the number of tokens in the prompt
  int n_prompt = -llama_tokenize(_vocab, prompt.c_str(), prompt.size(), nullptr, 0, true, true);

  // allocate space for the tokens and tokenize the prompt
  std::vector<llama_token> prompt_tokens(n_prompt);
  if (llama_tokenize(_vocab, prompt.c_str(), prompt.size(), prompt_tokens.data(), prompt_tokens.size(), true, true) < 0) {
    _last_error = "failed tokenize the prompt";
    return out;
  }

  // initialize the sampler
  configure_sampler(temperature);

  // prepare a batch for the prompt
  llama_batch batch = llama_batch_get_one(prompt_tokens.data(), prompt_tokens.size());
  if (llama_model_has_encoder(_model)) {
    if (llama_encode(_ctx, batch)) {
      _last_error = "failed to eval";
      return out;
    }

    llama_token decoder_start_token_id = llama_model_decoder_start_token(_model);
    if (decoder_start_token_id == LLAMA_TOKEN_NULL) {
      decoder_start_token_id = llama_vocab_bos(_vocab);
    }

    batch = llama_batch_get_one(&decoder_start_token_id, 1);
  }

  for (int n_pos = 0; n_pos + batch.n_tokens < n_prompt + max_tokens;) {
    // evaluate the current batch with the transformer model
    if (llama_decode(_ctx, batch)) {
      _last_error = "failed to eval";
      break;
    }

    n_pos += batch.n_tokens;

    // sample the next token
    llama_token new_token_id = llama_sampler_sample(_sampler, _ctx, -1);

    // is it an end of generation?
    if (llama_vocab_is_eog(_vocab, new_token_id)) {
      break;
    }

    char buf[128];
    int n = llama_token_to_piece(_vocab, new_token_id, buf, sizeof(buf), 0, true);
    if (n < 0) {
      _last_error = "failed to convert token to piece";
      break;
    } else if (n > 0) {
      out.append(buf, n);
    }

    // prepare the next batch with the sampled token
    batch = llama_batch_get_one(&new_token_id, 1);
  }

  return out;
}

void Llama::reset() {
  // llama_kv_cache_clear(it->second->ctx);
  _chat_prompt.clear();
}
