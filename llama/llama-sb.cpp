// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2026 Chris Warren-Smith

#include <format>
#include <span>
#include <cmath>
#include <utility>
#include "ggml-cuda.h"

#include "llama.h"
#include "llama-sb.h"

constexpr int MAX_REPEAT = 50;

static bool read_vram(size_t &used, size_t &total) {
  size_t free = 0;
  total = 0;
#ifdef GGML_USE_CUDA
  ggml_backend_cuda_get_device_memory(0, &free, &total);
  if (total > 0) {
    used = total - free;
    return true;
  }
#endif
  return false;
}

LlamaIter::LlamaIter() :
  _llama(nullptr),
  _repetition_count(0),
  _tokens_generated(0),
  _has_next(false) {
}

LlamaIter::LlamaIter(LlamaIter &&other) noexcept
  : _llama(std::exchange(other._llama, nullptr))
  , _last_word(std::move(other._last_word))
  , _t_start(std::move(other._t_start))
  , _repetition_count(other._repetition_count)
  , _tokens_generated(other._tokens_generated)
  , _has_next(other._has_next) {
}

Llama::Llama() :
  _model(nullptr),
  _ctx(nullptr),
  _sampler(nullptr),
  _vocab(nullptr),
  _penalty_last_n(0),
  _penalty_repeat(0),
  _penalty_freq(0.0f),
  _penalty_present(0.0f),
  _temperature(0),
  _top_p(0),
  _min_p(0),
  _top_k(0),
  _max_tokens(0),
  _log_level(GGML_LOG_LEVEL_CONT),
  _n_gpu_layers(0),
  _n_system_tokens(0),
  _is_gemma4(false),
  _sampler_dirty(false),
  _can_shift(false),
  _seed(LLAMA_DEFAULT_SEED) {
  llama_log_set([](enum ggml_log_level level, const char *text, void *user_data) {
    Llama *llama = (Llama *)user_data;
    if (level == GGML_LOG_LEVEL_ERROR && llama->_last_error.empty()) {
      // remember the first error message
      llama->_last_error = text;
    }
    if (level > llama->_log_level) {
      fprintf(stderr, "LLAMA: %s", text);
    }
  }, this);
  reset();
  llama_backend_init();
}

Llama::Llama(Llama &&other) noexcept
  : _model(std::exchange(other._model, nullptr))
  , _ctx(std::exchange(other._ctx, nullptr))
  , _sampler(std::exchange(other._sampler, nullptr))
  , _vocab(std::exchange(other._vocab, nullptr))
  , _stop_sequences(std::move(other._stop_sequences))
  , _grammar_src(std::move(other._grammar_src))
  , _grammar_root(std::move(other._grammar_root))
  , _last_error(std::move(other._last_error))
  , _template(std::move(other._template))
  , _penalty_last_n(other._penalty_last_n)
  , _penalty_repeat(other._penalty_repeat)
  , _penalty_freq(other._penalty_freq)
  , _penalty_present(other._penalty_present)
  , _temperature(other._temperature)
  , _top_p(other._top_p)
  , _min_p(other._min_p)
  , _top_k(other._top_k)
  , _max_tokens(other._max_tokens)
  , _log_level(other._log_level)
  , _n_gpu_layers(other._n_gpu_layers)
  , _n_system_tokens(other._n_system_tokens)
  , _is_gemma4(other._is_gemma4)
  , _sampler_dirty(other._sampler_dirty)
  , _can_shift(other._can_shift)
  , _seed(other._seed) {
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
  llama_backend_free();
}

void Llama::reset() {
  _stop_sequences.clear();
  _last_error.clear();
  _penalty_last_n = 64;
  _penalty_repeat = 1.1f;
  _penalty_freq = 0.0f;
  _penalty_present = 0.0f;
  _temperature = 0;
  _top_k = 0;
  _top_p = 1.0f;
  _min_p = 0.0f;
  _max_tokens = 150;
  _n_system_tokens = 0;
  _seed = LLAMA_DEFAULT_SEED;
  _sampler_dirty = true;
  if (_ctx) {
    llama_memory_clear(llama_get_memory(_ctx), true);
  }
}

int Llama::max_tool_result_size() {
  // ~3 bytes/tok, 25% of ctx
  return (llama_n_ctx(_ctx) / 4) * 3;
}

bool Llama::load_model(string model_path, int n_ctx, int n_batch, int n_gpu_layers, int log_level) {
  ggml_backend_load_all();

  llama_model_params mparams = llama_model_default_params();
  if (n_gpu_layers >= 0) {
    mparams.n_gpu_layers = n_gpu_layers;
  }

  _last_error.clear();
  _log_level = log_level;
  _n_gpu_layers = n_gpu_layers;
  _model = llama_model_load_from_file(model_path.c_str(), mparams);
  if (!_model) {
    set_last_error("Load model");
  } else {
    llama_context_params cparams = llama_context_default_params();
    cparams.n_ctx   = n_ctx;
    cparams.n_batch = n_batch;
    cparams.n_ubatch = n_batch;
    cparams.no_perf = true;
    cparams.attention_type = LLAMA_ATTENTION_TYPE_UNSPECIFIED;
    cparams.flash_attn_type = LLAMA_FLASH_ATTN_TYPE_ENABLED;

    // or Q4_0 for more aggressive saving
    cparams.type_k = GGML_TYPE_Q4_0;
    cparams.type_v = GGML_TYPE_Q4_0;

    // keep KV cache on GPU
    cparams.offload_kqv = true;

    _ctx = llama_init_from_model(_model, cparams);
    if (!_ctx) {
      set_last_error("Create context");
    } else {
      _vocab = llama_model_get_vocab(_model);
      _template = llama_model_chat_template(_model, nullptr);
      _is_gemma4 = (_template.find("<|turn>model") != string::npos);
      _can_shift = llama_memory_can_shift(llama_get_memory(_ctx));
    }
  }

  return _last_error.empty();
}

bool Llama::load_embedding_model(string model_path) {
  ggml_backend_load_all();

  llama_model_params mparams = llama_model_default_params();
  mparams.n_gpu_layers = 99;

  _last_error.clear();
  _model = llama_model_load_from_file(model_path.c_str(), mparams);
  if (!_model) {
    set_last_error("Load model");
  } else {
    llama_context_params cparams = llama_context_default_params();
    cparams.n_ctx        = 512;
    cparams.n_batch      = 512;
    cparams.embeddings   = true;
    cparams.pooling_type = LLAMA_POOLING_TYPE_MEAN;

    _ctx = llama_init_from_model(_model, cparams);
    if (!_ctx) {
      set_last_error("Create context");
    } else {
      _vocab = llama_model_get_vocab(_model);
    }
  }

  return _last_error.empty();
}

void Llama::set_grammar(const string &src, const string &root) {
  _grammar_src = src;
  _grammar_root = root;
  dirty();
}

bool Llama::add_message(LlamaIter &iter, const string &role, const string &content) {
  llama_chat_message message = {role.c_str(), content.c_str()};
  int buf_size = 2 * (int)(role.size() + content.size() + 64);
  vector<char> buf(buf_size);
  int32_t n = 0;

  if (_template.empty()) {
    set_last_error("No chat template available");
    return false;
  }

  if (_is_gemma4) {
    // see: https://ai.google.dev/gemma/docs/core/prompt-formatting-gemma4
    string str;
    if (role == "system") {
      str = "<|turn>system\n<|think|>" + content + "<turn|>\n";
    } else {
      str = "<|turn>" + role + "\n" + content + "<turn|>\n";
    }
    n = str.size();
    buf.assign(str.begin(), str.end());
    buf.push_back('\0');
  } else {
    bool add_ass = (role == "user" || role == "tool" || role == "tool_result");
    n = llama_chat_apply_template(_template.c_str(), &message, 1, add_ass, buf.data(), buf_size);
    if (n < 0) {
      set_last_error("No chat template no supported");
      return false;
    } else if (n > (int32_t)buf.size()) {
      buf.resize(n);
      llama_chat_apply_template(_template.c_str(), &message, 1, add_ass, buf.data(), buf.size());
    }
  }
  string prompt(buf.data(), n);

  if (_sampler_dirty) {
    // avoid wasteful rebuild
    if (!configure_sampler()) {
      return false;
    }
    _sampler_dirty = false;
  }

  vector<llama_token> prompt_tokens = tokenize(prompt);
  if (prompt_tokens.size() == 0) {
    return false;
  }

  if (role == "system") {
    // always retain system tokens
    _n_system_tokens = prompt_tokens.size();
  }

  if (!make_space_for_tokens((prompt_tokens.size() * 3) / 2)) {
    return false;
  }

  // batch decode tokens
  if (!batch_decode_tokens(prompt_tokens)) {
    return false;
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
      set_last_error("Failed to evaluate decoder start token");
      return false;
    }
  }

  iter._t_start = std::chrono::high_resolution_clock::now();
  iter._llama = this;
  iter._has_next = true;
  return true;
}

string Llama::next(LlamaIter &iter) {
  if (!iter._has_next) {
    set_last_error("Iteration beyond end of stream");
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
    set_last_error("Failed to evaluate token during generation");
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
      break;
    }

    // append token to decoded list
    decoded.push_back(tok);
    ++generated;

    // decode the token
    llama_batch batch = llama_batch_get_one(&tok, 1);
    if (llama_decode(_ctx, batch)) {
      set_last_error("Failed to evaluate token during generation");
      break;
    }
  }

  // tokens exhausted - call add_message to continue
  iter._has_next = false;

  // detokenize sequentially
  if (!decoded.empty()) {
    for (llama_token tok : decoded) {
      out.append(token_to_string(iter, tok));
    }
  }

  return out;
}

LlamaMemoryInfo Llama::memory_info() {
  LlamaMemoryInfo info = {};

  // KV cache usage
  llama_memory_t mem = llama_get_memory(_ctx);
  llama_pos pos_max  = llama_memory_seq_pos_max(mem, 0);
  int n_ctx          = llama_n_ctx(_ctx);
  info.kv_total      = n_ctx;
  info.kv_used       = (pos_max < 0) ? 0 : (int)pos_max + 1;
  info.kv_percent    = 100.0f * info.kv_used / info.kv_total;

  // Model layers
  auto n_gpu_layers = std::max(0, _n_gpu_layers);
  info.n_layers_total = llama_model_n_layer(_model);
  info.n_layers_gpu   = std::min(info.n_layers_total, n_gpu_layers);
  info.n_layers_cpu   = info.n_layers_total - info.n_layers_gpu;

  // ram
  if (read_vram(info.vram_used, info.vram_total)) {
    info.vram_percent = 100.0f * info.vram_used / info.vram_total;
  }

  info.model_native_max_ctx = llama_model_n_ctx_train(_model);

  // Advice
  ostringstream advice;

  // Check structural limits & model configuration quirks
  if (info.kv_total > info.model_native_max_ctx) {
    advice << "WARNING: Configured context size (" << info.kv_total
           << ") exceeds model native training length (" << info.model_native_max_ctx
           << "). Logic flaws or repetition bugs will occur unless RoPE scaling options are enabled. ";
  }

  if (n_gpu_layers < info.n_layers_total) {
    advice << "Only " << n_gpu_layers << "/" << info.n_layers_total
           << " layers on GPU - increase n_gpu_layers if VRAM allows. ";
  } else {
    advice << "All " << info.n_layers_total << " layers on GPU. ";
  }
  if (info.n_layers_cpu > 0) {
    advice << "CPU offload active (" << info.n_layers_cpu
           << " layers on CPU) - increase n_gpu_layers if VRAM allows. ";
  }
  if (info.vram_percent > 90.0f) {
    advice << "VRAM >90% - reduce n_ctx or use Q4_0 KV cache. ";
  } else if (info.vram_percent < 60.0f && info.n_layers_cpu > 0) {
    advice << "VRAM headroom available - try adding more GPU layers. ";
  }
  if (info.kv_percent > 80.0f) {
    advice << "Context >80% full - consider calling clear_history(). ";
  }
  info.advice = advice.str();

  return info;
}

bool Llama::embed_text(const std::string &text, std::vector<float> &out, int embed_dim) {
  vector<llama_token> tokens = tokenize(text);
  if (tokens.size() == 0) {
    return false;
  }

  // truncate to context window
  int n_ctx = llama_n_ctx(_ctx);
  int n = tokens.size();
  if (n > n_ctx) {
    set_last_error(std::format("warning: chunk truncated {} -> {} tokens ", n, n_ctx));
    n = n_ctx;
    tokens.resize(n);
  }

  llama_memory_clear(llama_get_memory(_ctx), true);

  if (!batch_decode_tokens(tokens)) {
    return false;
  }

  float *emb = llama_get_embeddings_seq(_ctx, 0);
  if (!emb) {
    emb = llama_get_embeddings_ith(_ctx, n - 1);
  }

  if (!emb) {
    set_last_error("no embedding returned");
    return false;
  }

  out.assign(emb, emb + embed_dim);

  /* L2 normalize */
  float norm = 0.0f;
  for (float v : out) {
    norm += v * v;
  }
  norm = std::sqrt(norm);
  if (norm > 1e-9f) {
    for (float &v : out) {
      v /= norm;
    }
  }

  return true;
}

bool Llama::batch_decode_tokens(vector<llama_token> &tokens) {
  uint32_t n_batch = llama_n_batch(_ctx);
  for (size_t i = 0; i < tokens.size(); i += n_batch) {
    size_t batch_size = std::min((size_t)n_batch, tokens.size() - i);
    llama_batch batch = llama_batch_get_one(tokens.data() + i, batch_size);
    int result = llama_decode(_ctx, batch);
    if (result == 1) {
      // KV full or fragmented mid-batch - evict oldest tokens and retry
      if (!make_space_for_tokens(n_batch)) {
        set_decode_error(result, i, tokens.size());
        return false;
      }
      result = llama_decode(_ctx, batch);
      if (result == 1) {
        // Eviction reported enough logical space but decode still failed -
        // this is fragmentation, not a real space shortage. No defrag API
        // is available, so fall back to a full non-system flush, which
        // guarantees one contiguous block.
        if (!full_flush_except_system()) {
          set_decode_error(result, i, tokens.size());
          return false;
        }
        result = llama_decode(_ctx, batch);
      }
    }
    if (result != 0) {
      set_decode_error(result, i, tokens.size());
      return false;
    }
  }
  return true;
}

bool Llama::configure_sampler() {
  auto sparams = llama_sampler_chain_default_params();
  sparams.no_perf = false;
  llama_sampler *chain = llama_sampler_chain_init(sparams);

  if (!_grammar_src.empty()) {
    llama_sampler *grammar = llama_sampler_init_grammar(_vocab, _grammar_src.c_str(), _grammar_root.c_str());
    if (!grammar) {
      set_last_error("failed to initialize grammar sampler");
      return false;
    }
    llama_sampler_chain_add(chain, grammar);
  }
  if (_penalty_last_n != 0 && _penalty_repeat != 1.0f) {
    auto penalties = llama_sampler_init_penalties(_penalty_last_n, _penalty_repeat, _penalty_freq, _penalty_present);
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

bool Llama::full_flush_except_system() {
  llama_memory_t mem = llama_get_memory(_ctx);
  llama_pos pos_min = llama_memory_seq_pos_min(mem, 0);
  if (pos_min < 0) {
    return true; // already empty
  }
  llama_pos flush_start = pos_min + _n_system_tokens;
  bool ok = llama_memory_seq_rm(mem, 0, flush_start, -1);
  if (!ok) {
    set_last_error("Failed to flush memory past system tokens");
    return false;
  }
  return true;
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
//
bool Llama::make_space_for_tokens(int n_tokens) {
  int n_ctx = llama_n_ctx(_ctx);
  if (n_tokens > n_ctx) {
    set_last_error("Too many tokens, increase context size (n_ctx)");
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

  // Can't remove more than we have (minus _n_system_tokens)
  int removable = current_used - _n_system_tokens;
  if (tokens_to_remove > removable) {
    set_last_error("Can't make enough space while keeping num_system_tokens tokens");
    return false;
  }
  if (!_can_shift) {
    set_last_error("Memory type doesn't support shifting, can't evict mid-sequence");
    return false;
  }

  llama_pos remove_start = pos_min + _n_system_tokens;

  // Remove oldest tokens (from pos_min to pos_min + tokens_to_remove)
  llama_memory_seq_rm(mem, 0, remove_start, remove_start + tokens_to_remove);

  // Shift remaining tokens down
  llama_memory_seq_add(mem, 0, remove_start + tokens_to_remove, -1, -tokens_to_remove);

  set_last_error(std::format("made space for {} tokens", n_tokens));
  return true;
}

vector<llama_token> Llama::tokenize(const string &prompt) {
  vector<llama_token> result;

  int n_prompt = -llama_tokenize(_vocab, prompt.c_str(), prompt.size(), nullptr, 0, true, true);
  if (n_prompt <= 0) {
    set_last_error("Failed to tokenize prompt");
  } else {
    result.reserve(n_prompt);
    result.resize(n_prompt);
    if (llama_tokenize(_vocab, prompt.c_str(), prompt.size(),
                       result.data(), n_prompt, true, true) < 0) {
      set_last_error("Failed to tokenize prompt");
    }
  }
  return result;
}

string Llama::token_to_string(LlamaIter &iter, llama_token tok) {
  string result;
  char buf[512];
  int n = llama_token_to_piece(_vocab, tok, buf, sizeof(buf), 0, false);
  if (n > 0) {
    // detect repetition - only on non-whitespace tokens, otherwise
    // spaces/newlines trigger false positives almost immediately.
    string piece(buf, n);
    bool is_trivial = piece.find_first_not_of(" \t\n\r") == string::npos;
    if (!is_trivial) {
      if (iter._last_word == piece) {
        if (++iter._repetition_count >= MAX_REPEAT) {
          iter._has_next = false;
        }
      } else {
        iter._repetition_count = 0;
        iter._last_word = piece;
      }
    }

    result.append(buf, n);

    // detect end of max-tokens
    if (++iter._tokens_generated > _max_tokens) {
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

void Llama::set_last_error(const string &message) {
  if (!_last_error.empty()) {
    if (_last_error.back() == '\n') {
      _last_error.pop_back();
    }
    _last_error = std::format("{}: {}", message, _last_error);
  } else {
    _last_error = std::format("{} failed", message);
  }
}

void Llama::set_decode_error(int32_t error, int index, int num_tokens) {
  if (error == 1) {
    llama_memory_t mem = llama_get_memory(_ctx);
    llama_pos pos_min = llama_memory_seq_pos_min(mem, 0);
    llama_pos pos_max = llama_memory_seq_pos_max(mem, 0);
    int n_ctx = llama_n_ctx(_ctx);
    int current_used = pos_max - pos_min + 1;
    int space_needed = num_tokens;
    int space_available = n_ctx - current_used;
    _n_system_tokens;
    set_last_error(std::format("KV exhausted. Reduce batch or context sizes. batchNo:{} requested:{} available:{}",
                               index, space_needed, space_available));
  } else {
    auto message = error == 2 ? "abort" : error == -1 ? "invalid" : "fatal";
    set_last_error(std::format("Failed to decode batch. batchNo:{} error:'{}'", index, message));
  }
}
