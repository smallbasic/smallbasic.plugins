// nitro.cpp — Nitro Agent
// A standalone agentic LLM shell with notcurses TUI.
// Uses llama-sb.h as the sole llama.cpp integration layer.
//
// Build (example):
//   g++ -std=c++20 -O2 nitro.cpp llama-sb.cpp \
//       -I/path/to/llama.cpp/include \
//       -L/path/to/llama.cpp/build/src \
//       -lllama -lggml -lnotcurses-core -lnotcurses \
//       -o nitro
//
// Usage:
//   ./nitro [options] [project_dir]
//
// Options:
//   -m, --model  <path>       GGUF model to load on startup
//   -e, --embed  <path>       embedding model for RAG
//   -g, --gpu-layers <n>      layers to offload to GPU (default: 32)
//
// Slash commands:
//   /model  <path>            — load / hot-reload a GGUF model
//   /embed  <path>            — load an embedding model for RAG
//   /rag    <path>            — index a file or directory into RAG
//   /memory                   — show KV / VRAM / layer stats
//   /clear                    — reset conversation (keeps system prompt)
//   /help                     — list commands
//
// Tool protocol (LLM emits, Nitro executes):
//   TOOL:LIST   [dir]
//   TOOL:READ   <file>
//   TOOL:WRITE  <file> <content>
//   TOOL:EXISTS <file>
//   TOOL:RUN    <program> [args]
//   TOOL:DATE
//   TOOL:TIME
//   TOOL:RND
//   TOOL:PERMISSION
//
// Copyright (C) 2026 Chris Warren-Smith  —  GPLv2 or later

// ─── Standard library ────────────────────────────────────────────────────────
#include <algorithm>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

// ─── Integration layer (sole llama.cpp dependency for nitro) ─────────────────
#include "llama-sb.h"
#include "llama-sb-rag.h"

// ─── TUI ─────────────────────────────────────────────────────────────────────
#include <notcurses/notcurses.h>

namespace fs = std::filesystem;

// ═══════════════════════════════════════════════════════════════════════════
// Forward declarations
// ═══════════════════════════════════════════════════════════════════════════
struct NitroConfig;
struct TuiState;
struct AgentState;

static std::string  join_path(const std::string &a, const std::string &b);
static std::string  read_file(const std::string &path);
static bool         write_file(const std::string &path, const std::string &data);
static std::string  list_dir(const std::string &path);
static bool         path_in_sandbox(const std::string &sandbox, const std::string &path);
static std::string  strip_code_fences(const std::string &filename, const std::string &src);
static std::string  process_tool(const std::string &line, const std::string &sandbox,
                                  TuiState &tui);
static std::string  build_system_prompt(const std::vector<std::string> &knowledge_files,
                                         const std::string &sandbox);

// ═══════════════════════════════════════════════════════════════════════════
// Config  (mirrors the SB agent constants)
// ═══════════════════════════════════════════════════════════════════════════

struct NitroConfig {
  std::string model_path;   // empty = no model yet; set via -m/--model or /model
  std::string embed_path;
  std::string sandbox;
  int   n_ctx          = 65536;
  int   n_batch        = 512;
  int   n_gpu_layers   = 32;
  int   n_max_tokens   = 4096;
  int   log_level      = GGML_LOG_LEVEL_CONT;
  float temperature    = 0.6f;
  float top_p          = 0.95f;
  float min_p          = 0.0f;
  int   top_k          = 20;
  float penalty_repeat = 1.0f;
  int   penalty_last_n = 256;
  std::vector<std::string> knowledge_files;
  int   rag_top_k      = 5;
};

// ═══════════════════════════════════════════════════════════════════════════
// Notcurses TUI
// ═══════════════════════════════════════════════════════════════════════════
//
//  ┌──────────────────── header (1 row) ─────────────────────────────────┐
//  │ ✦ NITRO  model: …  tok/s: …  KV: …%  VRAM: …%                       │
//  ├─────────────────────────────────────────────────────────────────────┤
//  │                                                                     │
//  │  chat pane  (rows 1 … term_rows-3)                                  │
//  │                                                                     │
//  ├─────────────────────────────────────────────────────────────────────┤
//  │ ─────────────────────────────────────  (separator)                  │
//  │ ❯ input                                                             │
//  └─────────────────────────────────────────────────────────────────────┘

struct TuiState {
  // ── notcurses handles ──────────────────────────────────────────────
  struct notcurses *nc      = nullptr;
  struct ncplane   *stdpl   = nullptr;
  struct ncplane   *header  = nullptr;
  struct ncplane   *chatpl  = nullptr;
  struct ncplane   *inputpl = nullptr;

  // ── chat buffer ───────────────────────────────────────────────────
  std::vector<std::string> chat_lines;
  int scroll_offset = 0;       // lines scrolled up from bottom (0 = pinned)
  std::mutex lines_mutex;

  // ── streaming accumulator ─────────────────────────────────────────
  // Tokens arrive without newlines; we accumulate here and flush on \n.
  std::string token_acc;

  // ── input ─────────────────────────────────────────────────────────
  std::string input_buf;
  size_t      cursor_pos = 0;

  // ── status bar values (written by agent loop) ─────────────────────
  std::string current_model  = "none";
  float       tokens_per_sec = 0.0f;
  int         kv_used        = 0;
  int         kv_total       = 1;
  size_t      vram_used      = 0;
  size_t      vram_total     = 1;

  int term_rows = 0;
  int term_cols = 0;

  // ── thinking spinner ──────────────────────────────────────────────
  bool    thinking      = false;
  int     spinner_frame = 0;
  // Advance spinner by one frame and redraw the header.
  void tick_spinner();
  // Toggle thinking mode; redraws header immediately.
  void set_thinking(bool on);

  // ── lifecycle ─────────────────────────────────────────────────────
  void init();
  void destroy();
  void resize();

  // ── draw ──────────────────────────────────────────────────────────
  void redraw_header();
  void redraw_chat();
  void redraw_input();
  void redraw_all();

  // ── content helpers ───────────────────────────────────────────────
  // Append a complete line (wraps at terminal width, colour-coded by prefix).
  void append_line(const std::string &line);
  // Feed a streaming token fragment; flushes complete lines on \n.
  void append_token(const std::string &token);
  // Flush whatever is left in token_acc as a final line.
  void flush_token_acc();

  // ── interaction ───────────────────────────────────────────────────
  // Show a YES/NO confirm dialog in the input plane; writes "YES" or "NO".
  void confirm_dialog(const std::string &prompt, std::string &result);
  // Blocking readline with cursor, arrow-key scrolling, basic editing.
  std::string readline_blocking();
};

// ─── colour helpers ──────────────────────────────────────────────────────
// Our dark background colours (must match ncplane_set_base values in init).
static constexpr uint32_t BG_CHAT_R = 18,  BG_CHAT_G = 22,  BG_CHAT_B = 30;
static constexpr uint32_t BG_INP_R  = 22,  BG_INP_G  = 28,  BG_INP_B  = 38;
static constexpr uint32_t BG_HDR_R  = 30,  BG_HDR_G  = 40,  BG_HDR_B  = 55;

// fg only (use only where bg is already set via ncplane_set_base)
static inline uint64_t fg_rgb(uint32_t r, uint32_t g, uint32_t b) {
  return NCCHANNELS_INITIALIZER(r, g, b, 0, 0, 0);
}
// fg + explicit bg — use this for all ncplane_set_channels calls so the
// background behind each glyph matches the plane's base colour exactly.
static inline uint64_t chat_ch(uint32_t r, uint32_t g, uint32_t b) {
  return NCCHANNELS_INITIALIZER(r, g, b, BG_CHAT_R, BG_CHAT_G, BG_CHAT_B);
}
static inline uint64_t inp_ch(uint32_t r, uint32_t g, uint32_t b) {
  return NCCHANNELS_INITIALIZER(r, g, b, BG_INP_R, BG_INP_G, BG_INP_B);
}
static inline uint64_t hdr_ch(uint32_t r, uint32_t g, uint32_t b) {
  return NCCHANNELS_INITIALIZER(r, g, b, BG_HDR_R, BG_HDR_G, BG_HDR_B);
}

// ─── TuiState::init ──────────────────────────────────────────────────────

void TuiState::init() {
  notcurses_options opts{};
  opts.flags = NCOPTION_SUPPRESS_BANNERS;
  nc = notcurses_init(&opts, nullptr);
  if (!nc) { std::fputs("notcurses_init failed\n", stderr); std::exit(1); }

  stdpl = notcurses_stdplane(nc);
  notcurses_term_dim_yx(nc, (unsigned *)&term_rows, (unsigned *)&term_cols);

  // Fill the entire terminal with our dark background before creating
  // child planes — eliminates the "terminal colour showing through" artefact.
  uint64_t bg = NCCHANNELS_INITIALIZER(BG_CHAT_R, BG_CHAT_G, BG_CHAT_B,
                                        BG_CHAT_R, BG_CHAT_G, BG_CHAT_B);
  ncplane_set_base(stdpl, " ", 0, bg);
  ncplane_erase(stdpl);

  // Header: row 0
  ncplane_options hopt{};
  hopt.y = 0; hopt.x = 0;
  hopt.rows = 1; hopt.cols = (unsigned)term_cols;
  header = ncplane_create(stdpl, &hopt);

  // Chat pane: rows 1 … term_rows-3
  int chat_rows = std::max(1, term_rows - 3);
  ncplane_options copt{};
  copt.y = 1; copt.x = 0;
  copt.rows = (unsigned)chat_rows; copt.cols = (unsigned)term_cols;
  chatpl = ncplane_create(stdpl, &copt);
  ncplane_set_base(chatpl, " ", 0,
    NCCHANNELS_INITIALIZER(BG_CHAT_R, BG_CHAT_G, BG_CHAT_B,
                            BG_CHAT_R, BG_CHAT_G, BG_CHAT_B));

  // Input pane: last 2 rows
  ncplane_options iopt{};
  iopt.y = term_rows - 2; iopt.x = 0;
  iopt.rows = 2; iopt.cols = (unsigned)term_cols;
  inputpl = ncplane_create(stdpl, &iopt);
  ncplane_set_base(inputpl, " ", 0,
    NCCHANNELS_INITIALIZER(BG_INP_R, BG_INP_G, BG_INP_B,
                            BG_INP_R, BG_INP_G, BG_INP_B));

  redraw_all();
}

void TuiState::destroy() {
  if (nc) { notcurses_stop(nc); nc = nullptr; }
}

void TuiState::resize() {
  notcurses_term_dim_yx(nc, (unsigned *)&term_rows, (unsigned *)&term_cols);
  ncplane_resize_simple(header,  1,                       (unsigned)term_cols);
  int cr = std::max(1, term_rows - 3);
  ncplane_resize_simple(chatpl,  (unsigned)cr,            (unsigned)term_cols);
  ncplane_move_yx(inputpl, term_rows - 2, 0);
  ncplane_resize_simple(inputpl, 2,                       (unsigned)term_cols);
  redraw_all();
}

// ─── TuiState::redraw_* ──────────────────────────────────────────────────

void TuiState::redraw_header() {
  ncplane_erase(header);
  ncplane_set_base(header, " ", 0,
    NCCHANNELS_INITIALIZER(BG_HDR_R, BG_HDR_G, BG_HDR_B,
                            BG_HDR_R, BG_HDR_G, BG_HDR_B));

  float kv_pct   = kv_total   > 0 ? 100.f * (float)kv_used   / (float)kv_total   : 0.f;
  float vram_pct = vram_total  > 0 ? 100.f * (float)vram_used / (float)vram_total : 0.f;

  // Spinner: braille dots rotate smoothly, clearly visible on the header
  static const char *const SPIN[] = { "⣾","⣽","⣻","⢿","⡿","⣟","⣯","⣷" };
  const char *spin_str = thinking ? SPIN[spinner_frame % 8] : " ";

  char buf[512];
  int n = std::snprintf(buf, sizeof(buf),
    " ✦ NITRO  │ %-32s │ %5.1f tok/s │ KV %4.1f%%  VRAM %4.1f%%  %s",
    current_model.c_str(), (double)tokens_per_sec,
    (double)kv_pct, (double)vram_pct, spin_str);
  if (n > term_cols) buf[term_cols] = '\0';

  ncplane_set_channels(header, hdr_ch(130, 220, 200));
  ncplane_putstr_yx(header, 0, 0, buf);
}

void TuiState::redraw_chat() {
  ncplane_erase(chatpl);
  unsigned rows, cols;
  ncplane_dim_yx(chatpl, &rows, &cols);

  std::lock_guard<std::mutex> lk(lines_mutex);
  int total   = (int)chat_lines.size();
  int visible = (int)rows;
  int start   = std::max(0, total - visible - scroll_offset);
  int end     = std::min(total, start + visible);

  for (int i = start, row = 0; i < end; ++i, ++row) {
    const std::string &line = chat_lines[i];

    uint64_t ch;
    if      (line.rfind("You: ",   0) == 0) ch = chat_ch(100, 200, 255);
    else if (line.rfind("Nitro: ", 0) == 0) ch = chat_ch(180, 255, 180);
    else if (line.rfind("[tool]",  0) == 0) ch = chat_ch(255, 180,  80);
    else if (line.rfind("[err]",   0) == 0) ch = chat_ch(255,  80,  80);
    else if (line.rfind("[sys]",   0) == 0) ch = chat_ch(140, 140, 200);
    else                                     ch = chat_ch(210, 210, 210);

    ncplane_set_channels(chatpl, ch);
    std::string display = line.size() > cols ? line.substr(0, cols) : line;
    ncplane_putstr_yx(chatpl, row, 0, display.c_str());
  }
}

void TuiState::redraw_input() {
  ncplane_erase(inputpl);

  // Separator
  ncplane_set_channels(inputpl, inp_ch(80, 120, 160));
  std::string sep(term_cols, '─');
  ncplane_putstr_yx(inputpl, 0, 0, sep.c_str());

  // Prompt
  const std::string prompt = " ❯ ";
  const int prompt_cols = 4;
  ncplane_set_channels(inputpl, inp_ch(100, 210, 255));
  ncplane_putstr_yx(inputpl, 1, 0, prompt.c_str());

  // Buffer — split at cursor so we can render the cursor cell distinctly
  int max_w = std::max(0, term_cols - prompt_cols - 1);

  // Viewport: if buffer is wider than the available space, show the tail
  std::string visible = input_buf;
  int view_offset = 0;
  if ((int)visible.size() > max_w && max_w > 0) {
    view_offset = (int)visible.size() - max_w;
    visible = visible.substr(view_offset);
  }

  // Text before cursor
  int cur_in_view = std::max(0, (int)cursor_pos - view_offset);
  cur_in_view = std::min(cur_in_view, (int)visible.size());

  std::string before = visible.substr(0, cur_in_view);
  std::string after  = cur_in_view < (int)visible.size()
                         ? visible.substr(cur_in_view + 1) : "";
  char cursor_ch = cur_in_view < (int)visible.size()
                     ? visible[cur_in_view] : ' ';

  ncplane_set_channels(inputpl, inp_ch(230, 230, 230));
  ncplane_putstr_yx(inputpl, 1, prompt_cols, before.c_str());

  // Cursor cell: bright bg, dark text — stands out against the input bg
  int cx = prompt_cols + cur_in_view;
  ncplane_set_channels(inputpl,
    NCCHANNELS_INITIALIZER(BG_INP_R, BG_INP_G, BG_INP_B, 180, 230, 255));
  char cbuf[2] = { cursor_ch, '\0' };
  ncplane_putstr_yx(inputpl, 1, cx, cbuf);

  // Text after cursor
  ncplane_set_channels(inputpl, inp_ch(230, 230, 230));
  if (!after.empty())
    ncplane_putstr_yx(inputpl, 1, cx + 1, after.c_str());
}

void TuiState::redraw_all() {
  redraw_header();
  redraw_chat();
  redraw_input();
  notcurses_render(nc);
}

void TuiState::tick_spinner() {
  ++spinner_frame;
  redraw_header();
  notcurses_render(nc);
}

void TuiState::set_thinking(bool on) {
  thinking = on;
  if (!on) spinner_frame = 0;
  redraw_header();
  notcurses_render(nc);
}

// ─── TuiState content helpers ─────────────────────────────────────────────

void TuiState::append_line(const std::string &line) {
  std::lock_guard<std::mutex> lk(lines_mutex);
  int w = std::max(1, term_cols - 1);
  if ((int)line.size() <= w) {
    chat_lines.push_back(line);
  } else {
    for (int off = 0; off < (int)line.size(); off += w)
      chat_lines.push_back(line.substr(off, w));
  }
}

void TuiState::append_token(const std::string &token) {
  token_acc += token;
  for (;;) {
    auto pos = token_acc.find('\n');
    if (pos == std::string::npos) break;
    append_line(token_acc.substr(0, pos));
    token_acc = token_acc.substr(pos + 1);
  }
  redraw_chat();
  notcurses_render(nc);
}

void TuiState::flush_token_acc() {
  if (!token_acc.empty()) {
    append_line(token_acc);
    token_acc.clear();
    redraw_chat();
    notcurses_render(nc);
  }
}

// ─── TuiState::confirm_dialog ─────────────────────────────────────────────

void TuiState::confirm_dialog(const std::string &prompt, std::string &result) {
  ncplane_erase(inputpl);
  ncplane_set_channels(inputpl, inp_ch(255, 200, 80));
  std::string msg = " " + prompt + " [y/n] ❯ ";
  ncplane_putstr_yx(inputpl, 1, 0, msg.c_str());
  notcurses_render(nc);

  std::string answer;
  for (;;) {
    ncinput ni{};
    notcurses_get_blocking(nc, &ni);
    if (ni.id == NCKEY_ENTER || ni.id == '\r' || ni.id == '\n') break;
    if (ni.id == NCKEY_BACKSPACE && !answer.empty()) { answer.pop_back(); }
    else if (ni.id >= 32 && ni.id < 127) { answer += (char)ni.id; }

    ncplane_erase(inputpl);
    ncplane_set_channels(inputpl, inp_ch(255, 200, 80));
    ncplane_putstr_yx(inputpl, 1, 0, (msg + answer).c_str());
    notcurses_render(nc);
  }

  std::string lo = answer;
  std::transform(lo.begin(), lo.end(), lo.begin(), ::tolower);
  result = (lo == "y" || lo == "yes" || lo == "sure" || lo == "k") ? "YES" : "NO";
  redraw_input();
  notcurses_render(nc);
}

// ─── TuiState::readline_blocking ──────────────────────────────────────────

std::string TuiState::readline_blocking() {
  input_buf.clear();
  cursor_pos = 0;
  redraw_input();
  notcurses_render(nc);

  for (;;) {
    ncinput ni{};
    notcurses_get_blocking(nc, &ni);

    if (ni.id == NCKEY_ENTER || ni.id == '\r' || ni.id == '\n') {
      std::string result = input_buf;
      input_buf.clear(); cursor_pos = 0;
      redraw_input(); notcurses_render(nc);
      return result;
    }
    if (ni.id == NCKEY_BACKSPACE || ni.id == 127) {
      if (cursor_pos > 0) { input_buf.erase(cursor_pos - 1, 1); --cursor_pos; }
    } else if (ni.id == NCKEY_LEFT) {
      if (cursor_pos > 0) --cursor_pos;
    } else if (ni.id == NCKEY_RIGHT) {
      if (cursor_pos < input_buf.size()) ++cursor_pos;
    } else if (ni.id == NCKEY_HOME) {
      cursor_pos = 0;
    } else if (ni.id == NCKEY_END) {
      cursor_pos = input_buf.size();
    } else if (ni.id == NCKEY_UP) {
      ++scroll_offset; redraw_chat(); notcurses_render(nc); continue;
    } else if (ni.id == NCKEY_DOWN) {
      if (scroll_offset > 0) --scroll_offset;
      redraw_chat(); notcurses_render(nc); continue;
    } else if (ni.id >= 32 && ni.id < 0xD800) {
      input_buf.insert(cursor_pos, 1, (char)ni.id);
      ++cursor_pos;
    }
    redraw_input();
    notcurses_render(nc);
  }
}

// ═══════════════════════════════════════════════════════════════════════════
// AgentState  —  thin owner of Llama + LlamaIter + optional RAG objects.
//
// Design:
//   • Llama is created once; settings are applied before load_model().
//   • iter is an std::optional so we can construct it lazily (LlamaIter has
//     no default-construct-then-assign path once _has_next is false; we just
//     move a fresh one in via add_message()).
//   • reset_conversation() calls llama.reset() which clears the KV cache,
//     then re-injects the system prompt as the first message of the new turn.
//   • run_turn() mirrors the SB main() loop exactly:
//       while iter.has_next → next() → accumulate line → on TOOL: dispatch
// ═══════════════════════════════════════════════════════════════════════════

struct AgentState {
  Llama llama;

  // iter is valid (has_next may be false) after the first add_message call.
  // We use a pointer so it can be replaced by move.
  std::unique_ptr<LlamaIter> iter;

  // Separate Llama instance for embeddings (optional)
  std::unique_ptr<Llama> embed_llama;

  // RAG objects
  std::unique_ptr<RagDB>      rag_db;
  std::unique_ptr<RagSession> rag_session;

  bool model_loaded = false;
  std::string system_prompt;

  // ── setup ─────────────────────────────────────────────────────────
  bool setup_model(const NitroConfig &cfg, TuiState &tui);
  bool setup_embed(const std::string &path, TuiState &tui);
  void apply_generation_params(const NitroConfig &cfg);

  // ── conversation management ───────────────────────────────────────
  // Injects the system prompt as a fresh first turn.
  // Call after setup_model() or whenever /clear is issued.
  void reset_conversation(const std::string &sysprompt, TuiState &tui);

  // ── generation ────────────────────────────────────────────────────
  // Returns false on fatal error.
  bool run_turn(const std::string &user_message,
                const NitroConfig &cfg,
                TuiState          &tui);

  // ── RAG ───────────────────────────────────────────────────────────
  bool rag_index(const std::string &path, TuiState &tui);

  // ── status ────────────────────────────────────────────────────────
  std::string memory_info_text();

  // Compute tok/s from iter (matches SB iter.tokens_sec() idiom)
  float tokens_per_sec() const;
};

// ─── AgentState::setup_model ──────────────────────────────────────────────

void AgentState::apply_generation_params(const NitroConfig &cfg) {
  llama.add_stop("<|turn|>");
  llama.add_stop("<|im_end|>");
  llama.set_max_tokens(cfg.n_max_tokens);
  llama.set_temperature(cfg.temperature);
  llama.set_top_k(cfg.top_k);
  llama.set_top_p(cfg.top_p);
  llama.set_min_p(cfg.min_p);
  llama.set_penalty_repeat(cfg.penalty_repeat);
  llama.set_penalty_last_n(cfg.penalty_last_n);
  llama.set_log_level(cfg.log_level);
}

bool AgentState::setup_model(const NitroConfig &cfg, TuiState &tui) {
  if (cfg.model_path.empty()) {
    tui.append_line("[sys] No model loaded.  Use /model <path> to load a GGUF.");
    tui.redraw_all();
    return false;
  }

  // reset() clears any previous KV state cleanly
  llama.reset();
  apply_generation_params(cfg);

  if (!llama.load_model(cfg.model_path, cfg.n_ctx, cfg.n_batch,
                         cfg.n_gpu_layers, cfg.log_level)) {
    tui.append_line(std::string("[err] ") + llama.last_error());
    tui.redraw_all();
    return false;
  }

  model_loaded = true;
  tui.current_model = fs::path(cfg.model_path).filename().string();
  tui.append_line("[sys] Model ready: " + tui.current_model);

  // Show memory advice (mirrors SB: print GREEN + mem.advice)
  LlamaMemoryInfo mem = llama.memory_info();
  tui.append_line("[sys] " + mem.advice);
  tui.kv_used  = mem.kv_used;
  tui.kv_total = mem.kv_total;
  tui.vram_used  = mem.vram_used;
  tui.vram_total = mem.vram_total;
  tui.redraw_all();
  return true;
}

bool AgentState::setup_embed(const std::string &path, TuiState &tui) {
  tui.append_line("[sys] Loading embedding model: " + path);
  tui.redraw_all();
  embed_llama = std::make_unique<Llama>();
  if (!embed_llama->load_embedding_model(path)) {
    tui.append_line(std::string("[err] ") + embed_llama->last_error());
    tui.redraw_all();
    embed_llama.reset();
    return false;
  }
  rag_db      = std::make_unique<RagDB>();
  rag_session = std::make_unique<RagSession>();
  tui.append_line("[sys] Embedding model ready.");
  tui.redraw_all();
  return true;
}

// ─── AgentState::reset_conversation ──────────────────────────────────────
// Mirrors the SB pattern:
//   local iter = llama.add_message("system", initialize_agent())

void AgentState::reset_conversation(const std::string &sysprompt, TuiState &tui) {
  system_prompt = sysprompt;
  llama.reset();                    // clears KV cache + sampler state
  apply_generation_params(NitroConfig{}); // re-apply stops / params after reset

  iter = std::make_unique<LlamaIter>();
  if (!llama.add_message(*iter, "system", system_prompt)) {
    tui.append_line(std::string("[err] System prompt injection: ") + llama.last_error());
    tui.redraw_all();
  }
}

// ─── AgentState::tokens_per_sec ──────────────────────────────────────────
// LlamaIter stores _t_start and _tokens_generated; we replicate the SB
// iter.tokens_sec() calculation here since LlamaIter doesn't expose it
// as a method in the public header.

float AgentState::tokens_per_sec() const {
  if (!iter) return 0.0f;
  auto now = std::chrono::high_resolution_clock::now();
  double elapsed = std::chrono::duration<double>(now - iter->_t_start).count();
  if (elapsed <= 0.0 || iter->_tokens_generated <= 0) return 0.0f;
  return (float)(iter->_tokens_generated / elapsed);
}

// ─── AgentState::memory_info_text ────────────────────────────────────────

std::string AgentState::memory_info_text() {
  if (!model_loaded) return "No model loaded.";
  LlamaMemoryInfo m = llama.memory_info();
  std::ostringstream oss;
  oss << "KV cache  : " << m.kv_used << " / " << m.kv_total
      << "  (" << m.kv_percent << "%)\n";
  if (m.vram_total > 0) {
    oss << "VRAM      : " << (m.vram_used >> 20) << " MB / "
        << (m.vram_total >> 20) << " MB  (" << m.vram_percent << "%)\n";
  }
  oss << "GPU layers: " << m.n_layers_gpu << " / " << m.n_layers_total << "\n";
  oss << "CPU layers: " << m.n_layers_cpu << "\n";
  oss << "Advice    : " << m.advice << "\n";
  return oss.str();
}

// ─── AgentState::rag_index ───────────────────────────────────────────────

bool AgentState::rag_index(const std::string &path, TuiState &tui) {
  if (!embed_llama || !rag_db) {
    tui.append_line("[err] Load an embedding model first: /embed <path>");
    tui.redraw_all();
    return false;
  }

  auto index_one = [&](const std::string &filepath) {
    tui.append_line("[sys]   indexing: " + filepath);
    tui.redraw_all();
    if (!embed_llama->rag_load(*rag_db, filepath)) {
      tui.append_line(std::string("[err] rag_load: ") + embed_llama->last_error());
      tui.redraw_all();
    }
  };

  fs::path rp(path);
  std::error_code ec;
  if (fs::is_directory(rp, ec)) {
    for (const auto &entry : fs::recursive_directory_iterator(rp, ec)) {
      if (entry.is_regular_file()) index_one(entry.path().string());
    }
  } else {
    index_one(path);
  }
  return true;
}

// ═══════════════════════════════════════════════════════════════════════════
// Agent turn — mirrors SB main() loop
//
// SB pattern (condensed):
//   while iter.has_next()
//     buffer += iter.next()
//     if newline in buffer:
//       line = left side of buffer
//       if TOOL: → line += buffer + iter.all()
//                  iter = llama.add_message("tool", process_tool(line))
//       else print line
//   if remaining buffer is TOOL: → process it
//   else flush remaining buffer
// ═══════════════════════════════════════════════════════════════════════════

bool AgentState::run_turn(const std::string &user_message,
                           const NitroConfig &cfg,
                           TuiState          &tui) {
  if (!model_loaded) {
    tui.append_line("[err] No model loaded. Use /model <path>");
    tui.redraw_all();
    return false;
  }

  // ── optional RAG context injection ───────────────────────────────
  // If we have a RAG session, prepend retrieved context to the user message.
  std::string effective_message = user_message;
  if (embed_llama && rag_db && rag_session) {
    std::string context = llama.rag_retrieve(*rag_db, user_message,
                                              cfg.rag_top_k, *rag_session);
    if (!context.empty()) {
      effective_message = "Context:\n" + context + "\n\nUser: " + user_message;
    }
  }

  // ── inject user message ───────────────────────────────────────────
  // iter must already exist (reset_conversation initialises it with "system").
  // add_message("user", …) appends to the existing KV context.
  if (!iter) {
    tui.append_line("[err] Conversation not initialised (call /clear to reset)");
    tui.redraw_all();
    return false;
  }

  if (!llama.add_message(*iter, "user", effective_message)) {
    tui.append_line(std::string("[err] add_message: ") + llama.last_error());
    tui.redraw_all();
    return false;
  }

  // ── label the assistant response in the chat pane ────────────────
  tui.append_line("Nitro: ");
  tui.set_thinking(true);

  // ── generation loop ───────────────────────────────────────────────
  bool in_think = false;
  std::string buffer;      // accumulates tokens until we see a newline

  // Scan buffer for think open/close tags anywhere in the text.
  // Models use either <think> or <|think|> depending on the template.
  auto update_think_state = [&](const std::string &text) {
    // Opening tags
    if (text.find("<think>")    != std::string::npos ||
        text.find("<|think|>")  != std::string::npos)  in_think = true;
    // Closing tags — check after open so a single-line <think>…</think> ends correctly
    if (text.find("</think>")   != std::string::npos ||
        text.find("</|think|>") != std::string::npos)  in_think = false;
  };

  while (iter->_has_next) {
    std::string tok = llama.next(*iter);
    tui.tick_spinner();

    // Update think state on every token so tags that arrive mid-buffer
    // suppress display immediately rather than waiting for a newline.
    update_think_state(tok);

    buffer += tok;

    auto nl = buffer.find('\n');
    if (nl != std::string::npos) {
      std::string text_line = buffer.substr(0, nl);
      buffer = buffer.substr(nl + 1);

      // Trim leading whitespace to detect TOOL: reliably
      std::string trimmed = text_line;
      trimmed.erase(0, trimmed.find_first_not_of(" \t"));

      if (trimmed.substr(0, 5) == "TOOL:") {
        // Collect remainder: rest of buffer + everything iter still has.
        // For TOOL:WRITE the content may contain newlines, so we keep
        // the raw text and only strip newlines from the op+arg1 prefix.
        std::string tail = buffer + llama.all(*iter);

        // Parse op and arg1 from trimmed (first two space-separated tokens)
        // then treat everything after as the raw payload (preserving newlines).
        std::string op, arg1, payload;
        {
          auto s1 = trimmed.find(' ');
          if (s1 != std::string::npos) {
            op = trimmed.substr(0, s1);
            std::string rest = trimmed.substr(s1 + 1);
            rest.erase(0, rest.find_first_not_of(" \t"));
            auto s2 = rest.find(' ');
            if (s2 != std::string::npos) {
              arg1    = rest.substr(0, s2);
              payload = rest.substr(s2 + 1) + tail;
            } else {
              arg1    = rest;
              payload = tail;
            }
          } else {
            op = trimmed;
          }
        }

        // Reconstruct the tool line.  For ops that don't carry a file payload
        // (LIST, EXISTS, READ, DATE, TIME, RND, PERMISSION, RUN) we still
        // collapse newlines in payload so the single-line format is preserved.
        // For TOOL:WRITE we keep newlines in the payload intact.
        std::string tool_line;
        if (op == "TOOL:WRITE") {
          tool_line = op + " " + arg1 + " " + payload;
          // Trim only trailing whitespace
          while (!tool_line.empty() && tool_line.back() == '\n')
            tool_line.pop_back();
        } else {
          tool_line = op;
          if (!arg1.empty())    tool_line += " " + arg1;
          if (!payload.empty()) {
            std::string flat = payload;
            flat.erase(std::remove(flat.begin(), flat.end(), '\n'), flat.end());
            while (!flat.empty() && std::isspace((unsigned char)flat.back()))
              flat.pop_back();
            if (!flat.empty()) tool_line += " " + flat;
          }
        }

        tui.append_line("[tool] " + op + " " + arg1 +
                        (op == "TOOL:WRITE" ? " <content>" : ""));
        tui.redraw_all();

        std::string result = process_tool(tool_line, cfg.sandbox, tui);

        tui.append_line("[tool] → " +
          result.substr(0, 200) + (result.size() > 200 ? "…" : ""));
        tui.redraw_all();

        if (!llama.add_message(*iter, "tool", result)) {
          tui.append_line(std::string("[err] tool result inject: ") + llama.last_error());
          tui.redraw_all();
          break;
        }
        buffer.clear();

      } else {
        // Normal output line — suppress if inside think block
        if (!in_think) {
          tui.append_token(text_line + "\n");
        }
      }
    }
  }

  // ── flush remaining buffer ────────────────────────────────────────
  if (!buffer.empty()) {
    std::string trimmed = buffer;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));

    if (trimmed.substr(0, 5) == "TOOL:") {
      std::string result = process_tool(trimmed, cfg.sandbox, tui);
      tui.append_line("[tool] → " + result.substr(0, 200));
      tui.redraw_all();
      llama.add_message(*iter, "tool", result);
    } else {
      if (!in_think) tui.append_token(buffer);
    }
  }
  tui.flush_token_acc();
  tui.set_thinking(false);

  // ── update status bar ─────────────────────────────────────────────
  tui.tokens_per_sec = tokens_per_sec();
  LlamaMemoryInfo mem = llama.memory_info();
  tui.kv_used    = mem.kv_used;
  tui.kv_total   = mem.kv_total;
  tui.vram_used  = mem.vram_used;
  tui.vram_total = mem.vram_total;

  // ── stat line (mirrors SB: "Tokens/sec: …") ──────────────────────
  char stat[128];
  std::snprintf(stat, sizeof(stat), "[sys] %.1f tok/s  (%d tokens)  KV %.1f%%",
    (double)tui.tokens_per_sec,
    iter->_tokens_generated,
    (double)mem.kv_percent);
  tui.append_line(stat);
  tui.redraw_all();
  return true;
}

// ═══════════════════════════════════════════════════════════════════════════
// File-system helpers
// ═══════════════════════════════════════════════════════════════════════════

static std::string join_path(const std::string &a, const std::string &b) {
  if (b.empty()) return a;
  if (b[0] == '/') return b;
  std::string pa = a;
  if (!pa.empty() && pa.back() == '/') pa.pop_back();
  std::string pb = (b.front() == '/') ? b.substr(1) : b;
  return pa + "/" + pb;
}

static bool path_in_sandbox(const std::string &sandbox, const std::string &path) {
  std::error_code ec;
  auto base   = fs::canonical(sandbox, ec);  if (ec) return false;
  auto target = fs::weakly_canonical(path, ec);
  std::string bstr = base.string() + "/";
  std::string tstr = target.string();
  return tstr == base.string() || tstr.compare(0, bstr.size(), bstr) == 0;
}

static std::string read_file(const std::string &path) {
  std::ifstream f(path, std::ios::binary);
  if (!f) return "ERROR: cannot open " + path;
  std::ostringstream oss; oss << f.rdbuf();
  return oss.str();
}

static bool write_file(const std::string &path, const std::string &data) {
  fs::path p(path);
  if (p.has_parent_path()) {
    std::error_code ec;
    fs::create_directories(p.parent_path(), ec);
  }
  std::ofstream f(path, std::ios::binary | std::ios::trunc);
  if (!f) return false;
  f.write(data.data(), (std::streamsize)data.size());
  return f.good();
}

static std::string list_dir(const std::string &path) {
  std::ostringstream oss;
  std::error_code ec;
  for (const auto &e : fs::directory_iterator(path, ec)) {
    if (ec) break;
    std::string name = e.path().filename().string();
    if (name.empty() || name[0] == '.') continue;
    oss << (e.is_directory() ? "[" + name + "]" : name) << "\n";
  }
  return oss.str();
}

static const std::vector<std::string> CODE_EXTENSIONS = {
  ".py",".c",".cpp",".h",".bas",".java",".html",".js",".ts",
  ".json",".yaml",".toml",".sh",".go",".rs",".jsx",".tsx"
};

static std::string strip_code_fences(const std::string &filename,
                                      const std::string &src) {
  auto ext = fs::path(filename).extension().string();
  bool is_code = std::any_of(CODE_EXTENSIONS.begin(), CODE_EXTENSIONS.end(),
                              [&](const std::string &e){ return ext == e; });
  if (!is_code) return src;
  auto pos = src.find("```");
  if (pos == std::string::npos) return src;
  auto nl = src.find('\n', pos + 3);
  if (nl == std::string::npos) return src;
  std::string inner = src.substr(nl + 1);
  auto end = inner.rfind("```");
  if (end != std::string::npos) inner = inner.substr(0, end);
  return inner;
}

// ═══════════════════════════════════════════════════════════════════════════
// Tool dispatch  (mirrors SB process_tool)
// ═══════════════════════════════════════════════════════════════════════════

static std::string process_tool(const std::string &cmd,
                                  const std::string &sandbox,
                                  TuiState &tui) {
  // Parse:  OP [ARG1 [REST…]]
  std::string op, arg1, arg2;
  auto sp1 = cmd.find(' ');
  if (sp1 == std::string::npos) {
    op = cmd;
  } else {
    op = cmd.substr(0, sp1);
    std::string rest = cmd.substr(sp1 + 1);
    // ltrim
    rest.erase(0, rest.find_first_not_of(" \t"));
    auto sp2 = rest.find(' ');
    if (sp2 == std::string::npos) {
      arg1 = rest;
    } else {
      arg1 = rest.substr(0, sp2);
      arg2 = rest.substr(sp2 + 1);
    }
  }

  // Resolve arg1 into an absolute path inside the sandbox
  auto resolve = [&](const std::string &p) -> std::string {
    if (p.empty() || p == ".") return sandbox;
    if (p.substr(0, 2) == "./") return join_path(sandbox, p.substr(2));
    if (p[0] == '/') return p;
    return join_path(sandbox, p);
  };

  if (op == "TOOL:DATE") {
    char buf[32]; time_t t = time(nullptr);
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&t));
    return buf;
  }
  if (op == "TOOL:TIME") {
    char buf[32]; time_t t = time(nullptr);
    strftime(buf, sizeof(buf), "%H:%M:%S", localtime(&t));
    return buf;
  }
  if (op == "TOOL:RND") {
    return std::to_string((double)rand() / RAND_MAX);
  }
  if (op == "TOOL:LIST") {
    std::string dir = resolve(arg1);
    if (!path_in_sandbox(sandbox, dir)) return "ERROR: path outside sandbox";
    return list_dir(dir);
  }
  if (op == "TOOL:EXISTS") {
    std::string p = resolve(arg1);
    if (!path_in_sandbox(sandbox, p)) return "NO";
    return fs::exists(p) ? "YES" : "NO";
  }
  if (op == "TOOL:READ") {
    std::string p = resolve(arg1);
    if (!path_in_sandbox(sandbox, p)) return "ERROR: path outside sandbox";
    return read_file(p);
  }
  if (op == "TOOL:WRITE") {
    std::string p = resolve(arg1);
    if (!path_in_sandbox(sandbox, p)) return "ERROR: path outside sandbox";
    std::string content = strip_code_fences(arg1, arg2);
    return write_file(p, content) ? "OK: written to " + arg1
                                   : "ERROR: write failed for " + arg1;
  }
  if (op == "TOOL:PERMISSION") {
    std::string result;
    tui.confirm_dialog("Allow model to proceed?", result);
    return result;
  }
  if (op == "TOOL:RUN") {
    std::string prog = resolve(arg1);
    if (!path_in_sandbox(sandbox, prog)) return "ERROR: path outside sandbox";
    std::string command = prog + " " + arg2 + " 2>&1";
    FILE *fp = popen(command.c_str(), "r");
    if (!fp) return "ERROR: popen failed";
    std::string out;
    char buf[256];
    while (fgets(buf, sizeof(buf), fp)) out += buf;
    pclose(fp);
    if (out.size() > 4096) out = out.substr(0, 4096) + "\n…(truncated)";
    return out;
  }

  return "ERROR: unknown tool: " + op;
}

// ═══════════════════════════════════════════════════════════════════════════
// System prompt  (mirrors SB initialize_agent)
// ═══════════════════════════════════════════════════════════════════════════

static std::string build_system_prompt(const std::vector<std::string> &knowledge_files,
                                        const std::string &sandbox) {
  std::string p;
  p += "You are Nitro, an agentic AI assistant for software development.\n"
       "Your sandbox (project directory) is: " + sandbox + "\n\n"
       "## Tool protocol\n"
       "Emit tool calls on their own line. The host executes them and returns\n"
       "TOOL_RESULT: <value> on the next line.\n\n"
       "Available tools:\n"
       "  TOOL:LIST   [dir]          list files (default: sandbox root)\n"
       "  TOOL:READ   <file>         read file contents\n"
       "  TOOL:WRITE  <file> <text>  write text to file\n"
       "  TOOL:EXISTS <file>         YES or NO\n"
       "  TOOL:RUN    <prog> [args]  run program inside sandbox\n"
       "  TOOL:DATE                  current date\n"
       "  TOOL:TIME                  current time\n"
       "  TOOL:RND                   random float\n"
       "  TOOL:PERMISSION            ask user for explicit permission\n\n"
       "Rules:\n"
       "- Never access files outside the sandbox.\n"
       "- Use TOOL:PERMISSION before destructive or irreversible operations.\n"
       "- Reason step-by-step inside <|think|>…</|think|> (hidden from user).\n"
       "- After each tool call, explain what you did in plain English.\n\n";

  for (const auto &kf : knowledge_files) {
    std::ifstream f(kf);
    if (!f) continue;
    std::ostringstream oss; oss << f.rdbuf();
    p += "## Knowledge: " + kf + "\n" + oss.str() + "\n\n";
  }
  return p;
}

// ═══════════════════════════════════════════════════════════════════════════
// Slash command handler
// ═══════════════════════════════════════════════════════════════════════════

static void handle_slash(const std::string &input,
                          NitroConfig       &cfg,
                          AgentState        &agent,
                          TuiState          &tui) {
  auto sp = input.find(' ');
  std::string verb = (sp == std::string::npos) ? input : input.substr(0, sp);
  std::string rest;
  if (sp != std::string::npos) {
    rest = input.substr(sp + 1);
    rest.erase(0, rest.find_first_not_of(" \t"));
  }

  if (verb == "/help") {
    tui.append_line("[sys] Commands:");
    tui.append_line("[sys]   /model  <path>   load a GGUF model");
    tui.append_line("[sys]   /embed  <path>   load an embedding model for RAG");
    tui.append_line("[sys]   /rag    <path>   index file or directory");
    tui.append_line("[sys]   /memory          KV / VRAM / layer stats");
    tui.append_line("[sys]   /clear           reset conversation");
    tui.append_line("[sys]   /help            this message");
    tui.append_line("[sys]   exit / quit      exit Nitro");
    tui.redraw_all();
    return;
  }

  if (verb == "/model") {
    if (rest.empty()) {
      tui.append_line("[err] Usage: /model <path-to-gguf>");
      tui.redraw_all(); return;
    }
    cfg.model_path = rest;
    if (agent.setup_model(cfg, tui)) {
      std::string sysp = build_system_prompt(cfg.knowledge_files, cfg.sandbox);
      agent.reset_conversation(sysp, tui);
    }
    tui.redraw_all();
    return;
  }

  if (verb == "/embed") {
    if (rest.empty()) {
      tui.append_line("[err] Usage: /embed <path-to-gguf>");
      tui.redraw_all(); return;
    }
    cfg.embed_path = rest;
    agent.setup_embed(rest, tui);
    return;
  }

  if (verb == "/rag") {
    if (rest.empty()) {
      tui.append_line("[err] Usage: /rag <file-or-dir>");
      tui.redraw_all(); return;
    }
    agent.rag_index(rest, tui);
    return;
  }

  if (verb == "/memory") {
    std::istringstream iss(agent.memory_info_text());
    std::string line;
    while (std::getline(iss, line)) tui.append_line("[sys] " + line);
    tui.redraw_all();
    return;
  }

  if (verb == "/clear") {
    { std::lock_guard<std::mutex> lk(tui.lines_mutex);
      tui.chat_lines.clear(); }
    std::string sysp = build_system_prompt(cfg.knowledge_files, cfg.sandbox);
    agent.reset_conversation(sysp, tui);
    tui.append_line("[sys] Conversation cleared.");
    tui.redraw_all();
    return;
  }

  tui.append_line("[err] Unknown command: " + verb + "  (try /help)");
  tui.redraw_all();
}

// ═══════════════════════════════════════════════════════════════════════════
// Welcome banner
// ═══════════════════════════════════════════════════════════════════════════

static void welcome(TuiState &tui, const std::string &sandbox) {
  tui.append_line("[sys] ╔═══════════════════════════════════════════╗");
  tui.append_line("[sys] ║       N I T R O   A G E N T    v1.0       ║");
  tui.append_line("[sys] ╚═══════════════════════════════════════════╝");
  tui.append_line("[sys] Sandbox : " + sandbox);
  tui.append_line("[sys] /help for commands  ·  exit to quit");
  tui.append_line("");
  tui.redraw_all();
}

// ═══════════════════════════════════════════════════════════════════════════
// main()
// ═══════════════════════════════════════════════════════════════════════════

int main(int argc, char **argv) {
  NitroConfig cfg;

  // ── Parse arguments ───────────────────────────────────────────────
  // Accepted forms:
  //   ./nitro [options] [project_dir]
  //   -m / --model  <path>       GGUF to load
  //   -e / --embed  <path>       embedding model
  //   -g / --gpu-layers <n>      GPU layer count
  // The first non-option argument is treated as project_dir.

  auto resolve_path = [](const std::string &arg) -> std::string {
    std::error_code ec;
    if (arg.substr(0, 2) == "~/") {
      const char *home = getenv("HOME");
      return std::string(home ? home : ".") + "/" + arg.substr(2);
    }
    if (arg.substr(0, 2) == "./")
      return (fs::current_path(ec) / arg.substr(2)).string();
    return arg;
  };

  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];

    auto take_next = [&](const char *flag) -> std::string {
      if (i + 1 >= argc) {
        std::fprintf(stderr, "nitro: %s requires an argument\n", flag);
        std::exit(1);
      }
      return argv[++i];
    };

    if (a == "-m" || a == "--model") {
      cfg.model_path = resolve_path(take_next(a.c_str()));
    } else if (a == "-e" || a == "--embed") {
      cfg.embed_path = resolve_path(take_next(a.c_str()));
    } else if (a == "-g" || a == "--gpu-layers") {
      cfg.n_gpu_layers = std::stoi(take_next(a.c_str()));
    } else if (a == "-h" || a == "--help") {
      std::puts(
        "Usage: nitro [options] [project_dir]\n"
        "\n"
        "Options:\n"
        "  -m, --model  <path>      GGUF model to load on startup\n"
        "  -e, --embed  <path>      embedding model for RAG\n"
        "  -g, --gpu-layers <n>     GPU layers to offload (default: 32)\n"
        "  -h, --help               show this help\n"
        "\n"
        "project_dir defaults to the current working directory.\n"
        "\n"
        "Slash commands inside nitro:\n"
        "  /model  <path>           load / hot-reload a GGUF\n"
        "  /embed  <path>           load an embedding model\n"
        "  /rag    <path>           index file or directory\n"
        "  /memory                  KV / VRAM / layer stats\n"
        "  /clear                   reset conversation\n"
        "  /help                    list commands\n"
      );
      return 0;
    } else if (!a.empty() && a[0] == '-') {
      std::fprintf(stderr, "nitro: unknown option '%s'  (try --help)\n", a.c_str());
      std::exit(1);
    } else {
      // positional → project_dir
      cfg.sandbox = resolve_path(a);
    }
  }

  // ── Resolve sandbox ───────────────────────────────────────────────
  if (cfg.sandbox.empty()) {
    std::error_code ec;
    cfg.sandbox = fs::current_path(ec).string();
  }
  { std::error_code ec; fs::create_directories(cfg.sandbox, ec); }

  // ── Auto-discover knowledge files ─────────────────────────────────
  for (const char *kf : {"nitro.md", "AGENTS.md", "README.md"}) {
    if (fs::exists(kf)) cfg.knowledge_files.push_back(kf);
  }

  // ── Init TUI ──────────────────────────────────────────────────────
  TuiState tui;
  tui.init();
  welcome(tui, cfg.sandbox);

  // ── Init agent ────────────────────────────────────────────────────
  // AgentState owns a Llama whose constructor calls llama_backend_init();
  // its destructor calls llama_backend_free() — nitro never touches
  // the raw llama API directly.
  AgentState agent;

  if (!cfg.model_path.empty()) {
    // Model provided on the command line — load immediately.
    if (agent.setup_model(cfg, tui)) {
      std::string sysp = build_system_prompt(cfg.knowledge_files, cfg.sandbox);
      agent.reset_conversation(sysp, tui);
    }
    // Load embedding model if also provided up-front.
    if (!cfg.embed_path.empty())
      agent.setup_embed(cfg.embed_path, tui);
  } else {
    // No model yet — friendly prompt, not an error.
    tui.append_line("[sys] No model specified.  Use /model <path> to load one.");
    tui.append_line("[sys] Example: /model ~/models/qwen2.5-7b-q4_k_m.gguf");
    tui.redraw_all();
  }

  // ── Main loop ─────────────────────────────────────────────────────
  for (;;) {
    // Check for terminal resize
    {
      unsigned rows = 0, cols = 0;
      notcurses_stddim_yx(tui.nc, &rows, &cols);
      if ((int)rows != tui.term_rows || (int)cols != tui.term_cols)
        tui.resize();
    }

    std::string input = tui.readline_blocking();
    // trim
    input.erase(0, input.find_first_not_of(" \t"));
    if (!input.empty())
      input.erase(input.find_last_not_of(" \t\r\n") + 1);
    if (input.empty()) continue;

    tui.append_line("You: " + input);
    tui.redraw_all();

    if (input == "exit" || input == "quit") break;

    if (input[0] == '/') {
      handle_slash(input, cfg, agent, tui);
    } else {
      agent.run_turn(input, cfg, tui);
    }
  }

  tui.destroy();
  // agent destructor cleans up Llama (which calls llama_backend_free)
  return 0;
}
