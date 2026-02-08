// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2026 Chris Warren-Smith

#include <format>
#include <span>
#include "llama.h"
#include "llama-sb.h"

constexpr int MAX_REPEAT = 5;

LlamaIter::LlamaIter() :
  _llama(nullptr),
  _repetition_count(0),
  _tokens_generated(0),
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
  _top_p(0),
  _min_p(0),
  _top_k(0),
  _max_tokens(0),
  _log_level(GGML_LOG_LEVEL_CONT),
  _seed(LLAMA_DEFAULT_SEED) {
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
  _grammar_src.clear();
  _grammar_root.clear();
  _seed = LLAMA_DEFAULT_SEED;
  if (_ctx) {
    llama_memory_clear(llama_get_memory(_ctx), true);
  }
}

bool Llama::construct(string model_path, int n_ctx, int n_batch, int n_gpu_layers) {
  ggml_backend_load_all();

  llama_model_params mparams = llama_model_default_params();
  if (n_gpu_layers >= 0) {
     mparams.n_gpu_layers = n_gpu_layers;
  }

  _model = llama_model_load_from_file(model_path.c_str(), mparams);
  if (!_model) {
    _last_error = "Failed to load model";
  } else {
    llama_context_params cparams = llama_context_default_params();
    cparams.n_ctx   = n_ctx;
    cparams.n_batch = n_batch;
    cparams.n_ubatch = n_batch;
    cparams.no_perf = true;
    cparams.attention_type = LLAMA_ATTENTION_TYPE_UNSPECIFIED;
    cparams.flash_attn_type = LLAMA_FLASH_ATTN_TYPE_AUTO;

    _ctx = llama_init_from_model(_model, cparams);
    if (!_ctx) {
      _last_error = "Failed to create context";
    } else {
      _vocab = llama_model_get_vocab(_model);
    }
  }
  return _last_error.empty();
}

void Llama::set_grammar(const string &src, const string &root) {
  _grammar_src = src;
  _grammar_root = root;
}

bool Llama::configure_sampler() {
  auto sparams = llama_sampler_chain_default_params();
  sparams.no_perf = false;
  llama_sampler *chain = llama_sampler_chain_init(sparams);

  if (!_grammar_src.empty()) {
    llama_sampler *grammar = llama_sampler_init_grammar(_vocab, _grammar_src.c_str(), _grammar_root.c_str());
    if (!grammar) {
      _last_error = "failed to initialize grammar sampler";
      return false;
    }
    llama_sampler_chain_add(chain, grammar);
  }
  if (_penalty_last_n != 0 && _penalty_repeat != 1.0f) {
    auto penalties = llama_sampler_init_penalties(_penalty_last_n, _penalty_repeat, 0.0f, 0.0f);
    llama_sampler_chain_add(chain, penalties);
  }
  if (_temperature <= 0.0f) {
    llama_sampler_chain_add(chain, llama_sampler_init_greedy());
  } else {
    if (_top_k > 0) {
      llama_sampler_chain_add(chain, llama_sampler_init_top_k(_top_k));
    }
    if (_top_p < 1.0f || _min_p > 0.0f) {
      llama_sampler_chain_add(chain, llama_sampler_init_top_p(_top_p, 1));
    }
    if (_min_p > 0.0f) {
      llama_sampler_chain_add(chain, llama_sampler_init_min_p(_min_p, 1));
    }
    llama_sampler_chain_add(chain, llama_sampler_init_temp(_temperature));
    llama_sampler_chain_add(chain, llama_sampler_init_dist(_seed));
  }
  if (_sampler) {
    llama_sampler_free(_sampler);
  }
  _sampler = chain;
  return true;
}

vector<llama_token> Llama::tokenize(const string &prompt) {
  vector<llama_token> result;

  int n_prompt = -llama_tokenize(_vocab, prompt.c_str(), prompt.size(), nullptr, 0, true, true);
  if (n_prompt <= 0) {
    _last_error = "Failed to tokenize prompt";
  } else {
    result.reserve(n_prompt);
    result.resize(n_prompt);
    if (llama_tokenize(_vocab, prompt.c_str(), prompt.size(),
                       result.data(), n_prompt, true, true) < 0) {
      _last_error = "Failed to tokenize prompt";
    }
  }
  return result;
}

// Makes space in the context for n_tokens by removing old tokens if necessary
// Returns true if successful, false if impossible to make space
//
// Strategies:
// - If enough space exists, does nothing
// - If n_tokens > n_ctx, fails (impossible to fit)
// - Otherwise, removes oldest tokens to make room
//
// Parameters:
//   n_tokens  - Number of tokens we need space for
//   keep_min  - Minimum tokens to keep (e.g., system prompt), default 0
//
bool Llama::make_space_for_tokens(int n_tokens, int keep_min) {
  int n_ctx = llama_n_ctx(_ctx);
  if (n_tokens > n_ctx) {
    _last_error = "Too many tokens, increase context size (n_ctx)";
    return false;
  }

  llama_memory_t mem = llama_get_memory(_ctx);

  // Get current position range
  llama_pos pos_min = llama_memory_seq_pos_min(mem, 0);
  llama_pos pos_max = llama_memory_seq_pos_max(mem, 0);

  // Empty memory - nothing to do
  if (pos_max < 0) {
    return true;
  }

  int current_used = pos_max - pos_min + 1;
  int space_needed = n_tokens;
  int space_available = n_ctx - current_used;

  // Already have enough space
  if (space_available >= space_needed) {
    return true;
  }

  // Calculate how many tokens to remove
  int tokens_to_remove = space_needed - space_available;

  // Can't remove more than we have (minus keep_min)
  int removable = current_used - keep_min;
  if (tokens_to_remove > removable) {
    _last_error = "Can't make enough space while keeping keep_min tokens";
    return false;
  }

  // Remove oldest tokens (from pos_min to pos_min + tokens_to_remove)
  llama_memory_seq_rm(mem, 0, pos_min, pos_min + tokens_to_remove);

  // Shift remaining tokens down
  llama_memory_seq_add(mem, 0, pos_min + tokens_to_remove, -1, -tokens_to_remove);

  return true;
}

bool Llama::generate(LlamaIter &iter, const string &prompt) {
  if (!configure_sampler()) {
    return false;
  }

  vector<llama_token> prompt_tokens = tokenize(prompt);
  if (prompt_tokens.size() == 0) {
    return false;
  }

  if (!make_space_for_tokens(prompt_tokens.size(), 0)) {
    return false;
  }

  // batch decode tokens
  uint32_t n_batch = llama_n_batch(_ctx);
  for (size_t i = 0; i < prompt_tokens.size(); i += n_batch) {
    size_t batch_size = std::min((size_t)n_batch, prompt_tokens.size() - i);
    llama_batch batch = llama_batch_get_one(prompt_tokens.data() + i, batch_size);
    int result = llama_decode(_ctx, batch);
    if (result != 0) {
      _last_error = std::format("Failed to decode batch. position:{} error:{}", i, result);
      return false;
    }
  }

  // handle encoder models
  if (llama_model_has_encoder(_model)) {
    // for example: T5, BART, and mBART.
    // Used for translation, summarization, text-to-text, paraphrasing, question answering
    llama_token decoder_start_token_id = llama_model_decoder_start_token(_model);
    if (decoder_start_token_id == LLAMA_TOKEN_NULL) {
      decoder_start_token_id = llama_vocab_bos(_vocab);
    }

    llama_batch decoder_batch = llama_batch_get_one(&decoder_start_token_id, 1);
    if (llama_decode(_ctx, decoder_batch)) {
      _last_error = "Failed to evaluate decoder start token";
      return false;
    }
  }

  iter._t_start = std::chrono::high_resolution_clock::now();
  iter._llama = this;
  iter._has_next = true;
  return true;
}

bool Llama::ends_with_sentence_boundary(const string &text) {
  if (text.empty()) {
    return false;
  }

  // Get last few characters (in case of whitespace after punctuation)
  size_t check_len = std::min(text.length(), (size_t)5);
  std::string ending = text.substr(text.length() - check_len);

  // Check for various sentence endings
  // Period followed by space or end
  if (ending.find(". ") != std::string::npos ||
      ending.back() == '.') {
    return true;
  }

  // Exclamation mark
  if (ending.find("! ") != std::string::npos ||
      ending.back() == '!') {
    return true;
  }

  // Question mark
  if (ending.find("? ") != std::string::npos ||
      ending.back() == '?') {
    return true;
  }

  // Newline (paragraph break)
  if (ending.find('\n') != std::string::npos) {
    return true;
  }

  // Quote followed by period: "something."
  if (ending.find(".\"") != std::string::npos ||
      ending.find("!\"") != std::string::npos ||
      ending.find("?\"") != std::string::npos) {
    return true;
  }

  return false;
}

string Llama::token_to_string(LlamaIter &iter, llama_token tok) {
  string result;
  char buf[512];
  int n = llama_token_to_piece(_vocab, tok, buf, sizeof(buf), 0, false);
  if (n > 0) {
    // detect repetition
    if (iter._last_word == buf) {
      if (++iter._repetition_count == MAX_REPEAT) {
        iter._has_next = false;
      }
    } else {
      iter._repetition_count = 0;
      iter._last_word = buf;
    }

    result.append(buf, n);

    // detect end of max-tokens
    if (++iter._tokens_generated > _max_tokens && ends_with_sentence_boundary(result)) {
      iter._has_next = false;
    }

    // detect stop words
    if (iter._has_next) {
      for (const auto &stop : _stop_sequences) {
        size_t pos = result.find(stop);
        if (pos != std::string::npos) {
          // found stop sequence - truncate and signal end
          result = result.substr(0, pos);
          iter._has_next = false;
          break;
        }
      }
    }
  }
  return result;
}

string Llama::next(LlamaIter &iter) {
  if (!iter._has_next) {
    _last_error = "Iteration beyond end of stream";
    return "";
  }

  // sample the next token from the current logits
  llama_token tok = llama_sampler_sample(_sampler, _ctx, -1);

  // end-of-generation check
  if (llama_vocab_is_eog(_vocab, tok)) {
    iter._has_next = false;
    return "";
  }

  string result = token_to_string(iter, tok);

  // prepare the next batch with the sampled token
  llama_batch batch = llama_batch_get_one(&tok, 1);
  if (llama_decode(_ctx, batch)) {
    _last_error = "Failed to evaluate token during generation";
    return "";
  }

  return result;
}

string Llama::all(LlamaIter &iter) {
  string out;

  vector<llama_token> decoded;
  decoded.reserve(_max_tokens);

  int generated = 0;

  while (generated < _max_tokens) {
    // sample the next token from the current logits
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
      _last_error = "Failed to evaluate token during generation";
      break;
    }
  }

  // detokenize sequentially
  if (!decoded.empty()) {
    for (llama_token tok : decoded) {
      out.append(token_to_string(iter, tok));
    }
  }

  return out;
}
