// nitro.cpp — Nitro Agent
// A standalone agentic LLM shell with notcurses TUI.
// Uses llama-sb.h as the sole llama.cpp integration layer.
//
// Build (example):
//   g++ -std=c++20 -O2 nitro.cpp llama-sb.cpp \
//       -I/path/to/llama.cpp/include \
//       -L/path/to/llama.cpp/build/src \
//       -lllama -lggml -lnotcurses-core -lnotcurses -lcurl \
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
//   TOOL:CURL   <url>
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
// ─── curl ─────────────────────────────────────────────────────────────────────
#include <curl/curl.h>
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
// ─── RAG indexing ─────────────────────────────────────────────────────────────
static constexpr int BATCH_SIZE = 512;

struct Chunk {
  std::string         text;
  std::string         source;
  std::string         type;
  std::vector<float>  embedding;
};

static bool json_get_string(const std::string &json,
                            const std::string &key,
                            std::string       &out) {
  std::string search = "\"" + key + "\":";
  size_t pos = json.find(search);
  if (pos == std::string::npos) return false;
  pos += search.size();
  while (pos < json.size() && json[pos] == ' ') ++pos;
  if (pos >= json.size() || json[pos] != '"') return false;
  ++pos;
  out.clear();
  while (pos < json.size()) {
    char c = json[pos++];
    if (c == '\\' && pos < json.size()) {
      char e = json[pos++];
      switch (e) {
      case 'n':  out += '\n'; break;
      case 't':  out += '\t'; break;
      case '"':  out += '"';  break;
      case '\\': out += '\\'; break;
      default:   out += e;    break;
      }
    } else if (c == '"') {
      break;
    } else {
      out += c;
    }
  }
  return true;
}

static bool save_db(const std::string        &path,
                    const std::vector<Chunk> &chunks,
                    int                       embed_dim) {
  std::ofstream f(path, std::ios::binary);
  if (!f) {
    std::fprintf(stderr, "cannot open for write: %s\n\n", path);
    return false;
  }
  auto write32 = [&](uint32_t v) { f.write((char*)&v, 4); };
  auto write16 = [&](uint16_t v) { f.write((char*)&v, 2); };
  auto write8  = [&](uint8_t  v) { f.write((char*)&v, 1); };
  auto writestr = [&](const std::string &s, size_t max_len) {
    size_t len = std::min(s.size(), max_len);
    f.write(s.c_str(), (std::streamsize)len);
  };
  write32(0x52414744);
  write32(2);
  write32((uint32_t)chunks.size());
  write32((uint32_t)embed_dim);
  for (const Chunk &c : chunks) {
    write32((uint32_t)c.text.size());
    f.write(c.text.c_str(), (std::streamsize)c.text.size());
    uint16_t src_len = (uint16_t)std::min(c.source.size(), (size_t)65535);
    write16(src_len);
    writestr(c.source, src_len);
    uint8_t type_len = (uint8_t)std::min(c.type.size(), (size_t)255);
    write8(type_len);
    writestr(c.type, type_len);
    f.write((char*)c.embedding.data(),
            (std::streamsize)(embed_dim * sizeof(float)));
  }
  return f.good();
}

// ═══════════════════════════════════════════════════════════════════════════
// InputHistory — up/down arrow navigation through submitted inputs
// ═══════════════════════════════════════════════════════════════════════════
class InputHistory {
  public:
  explicit InputHistory() = default;
  ~InputHistory() = default;
  InputHistory(const InputHistory &) = delete;
  InputHistory &operator=(const InputHistory &) = delete;

  /**
   * @brief Adds a new command string to the history stack.
   * Resets navigation index upon adding a new item.
   * Deduplicates consecutive identical entries.
   */
  void push(const std::string &input) {
    if (input.empty()) return;
    if (!history_stack.empty() && history_stack.back() == input) {
      // Don't push duplicate of last entry; just reset nav position.
      current_index = (int)history_stack.size();
      return;
    }
    history_stack.push_back(input);
    current_index = (int)history_stack.size();
  }

  /**
   * @brief Navigates to an earlier entry.
   * @param out Set to the selected entry on success.
   * @return true if an item was successfully retrieved.
   */
  bool up(std::string &out) {
    if (history_stack.empty() || current_index <= 0) return false;
    --current_index;
    out = history_stack[current_index];
    return true;
  }

  /**
   * @brief Navigates to a later entry, or clears when past the newest.
   * @param out Set to the selected entry, or cleared if past the end.
   * @return true if a history entry was retrieved (false means "clear input").
   */
  bool down(std::string &out) {
    if (history_stack.empty()) return false;
    ++current_index;
    if (current_index >= (int)history_stack.size()) {
      current_index = (int)history_stack.size();
      out.clear();
      return false; // signal: restore blank input
    }
    out = history_stack[current_index];
    return true;
  }

  /** Reset navigation position without modifying the stack. */
  void reset_nav() {
    current_index = (int)history_stack.size();
  }

  private:
  std::vector<std::string> history_stack;
  int current_index = 0;
};

// ═══════════════════════════════════════════════════════════════════════════
// Settings persistence  (~/.config/nitro.settings.json)
// ═══════════════════════════════════════════════════════════════════════════
// A minimal hand-rolled JSON reader/writer for the flat key-value settings
// we care about.  We deliberately avoid a full JSON library dependency.

struct NitroConfig {
  std::string model_path;
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

// Returns the canonical settings path: ~/.config/nitro.settings.json
static std::string settings_path() {
  const char *home = getenv("HOME");
  std::string base = home ? std::string(home) : ".";
  return base + "/.config/nitro.settings.json";
}

// Tiny helper: extract a quoted string value from flat JSON for a known key.
static bool settings_get_str(const std::string &json,
                              const std::string &key,
                              std::string &out) {
  return json_get_string(json, key, out);
}

// Tiny helper: extract an integer value from flat JSON.
static bool settings_get_int(const std::string &json,
                              const std::string &key,
                              int &out) {
  std::string search = "\"" + key + "\":";
  size_t pos = json.find(search);
  if (pos == std::string::npos) return false;
  pos += search.size();
  while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) ++pos;
  if (pos >= json.size()) return false;
  // read digits (and optional leading minus)
  size_t start = pos;
  if (json[pos] == '-') ++pos;
  while (pos < json.size() && std::isdigit((unsigned char)json[pos])) ++pos;
  if (pos == start) return false;
  out = std::stoi(json.substr(start, pos - start));
  return true;
}

// Tiny helper: extract a float value from flat JSON.
static bool settings_get_float(const std::string &json,
                                const std::string &key,
                                float &out) {
  std::string search = "\"" + key + "\":";
  size_t pos = json.find(search);
  if (pos == std::string::npos) return false;
  pos += search.size();
  while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) ++pos;
  if (pos >= json.size()) return false;
  size_t start = pos;
  if (json[pos] == '-') ++pos;
  while (pos < json.size() && (std::isdigit((unsigned char)json[pos]) || json[pos] == '.')) ++pos;
  if (pos == start) return false;
  out = std::stof(json.substr(start, pos - start));
  return true;
}

// Load settings from disk into cfg.  Fields present in the file overwrite
// the defaults already in cfg; fields absent are left at their defaults.
// Silently succeeds if the file doesn't exist yet.
static void load_settings(NitroConfig &cfg) {
  std::string path = settings_path();
  std::ifstream f(path);
  if (!f) return;                  // no file → use defaults
  std::ostringstream oss; oss << f.rdbuf();
  std::string json = oss.str();

  // String fields
  settings_get_str(json, "model_path",  cfg.model_path);
  settings_get_str(json, "embed_path",  cfg.embed_path);
  settings_get_str(json, "sandbox",     cfg.sandbox);

  // Integer fields
  settings_get_int(json, "n_ctx",          cfg.n_ctx);
  settings_get_int(json, "n_batch",        cfg.n_batch);
  settings_get_int(json, "n_gpu_layers",   cfg.n_gpu_layers);
  settings_get_int(json, "n_max_tokens",   cfg.n_max_tokens);
  settings_get_int(json, "top_k",          cfg.top_k);
  settings_get_int(json, "penalty_last_n", cfg.penalty_last_n);
  settings_get_int(json, "rag_top_k",      cfg.rag_top_k);

  // Float fields
  settings_get_float(json, "temperature",    cfg.temperature);
  settings_get_float(json, "top_p",          cfg.top_p);
  settings_get_float(json, "min_p",          cfg.min_p);
  settings_get_float(json, "penalty_repeat", cfg.penalty_repeat);
}

// Escape a string for embedding in JSON.
static std::string json_escape(const std::string &s) {
  std::string out;
  out.reserve(s.size() + 4);
  for (char c : s) {
    switch (c) {
      case '"':  out += "\\\""; break;
      case '\\': out += "\\\\"; break;
      case '\n': out += "\\n";  break;
      case '\t': out += "\\t";  break;
      default:   out += c;      break;
    }
  }
  return out;
}

// Persist the current cfg to ~/.config/nitro.settings.json.
static bool save_settings(const NitroConfig &cfg) {
  std::string path = settings_path();
  // Ensure ~/.config/ exists
  fs::path dir = fs::path(path).parent_path();
  std::error_code ec;
  fs::create_directories(dir, ec);

  std::ofstream f(path, std::ios::trunc);
  if (!f) return false;

  f << "{\n";
  f << "  \"model_path\":    \"" << json_escape(cfg.model_path)  << "\",\n";
  f << "  \"embed_path\":    \"" << json_escape(cfg.embed_path)   << "\",\n";
  f << "  \"sandbox\":       \"" << json_escape(cfg.sandbox)      << "\",\n";
  f << "  \"n_ctx\":          " << cfg.n_ctx          << ",\n";
  f << "  \"n_batch\":        " << cfg.n_batch         << ",\n";
  f << "  \"n_gpu_layers\":   " << cfg.n_gpu_layers    << ",\n";
  f << "  \"n_max_tokens\":   " << cfg.n_max_tokens    << ",\n";
  f << "  \"temperature\":    " << cfg.temperature     << ",\n";
  f << "  \"top_p\":          " << cfg.top_p           << ",\n";
  f << "  \"min_p\":          " << cfg.min_p           << ",\n";
  f << "  \"top_k\":          " << cfg.top_k           << ",\n";
  f << "  \"penalty_repeat\": " << cfg.penalty_repeat  << ",\n";
  f << "  \"penalty_last_n\": " << cfg.penalty_last_n  << ",\n";
  f << "  \"rag_top_k\":      " << cfg.rag_top_k       << "\n";
  f << "}\n";

  return f.good();
}

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
  int scroll_offset = 0;
  std::mutex lines_mutex;
  // ── streaming accumulator ─────────────────────────────────────────
  std::string token_acc;
  // ── input ─────────────────────────────────────────────────────────
  std::string input_buf;
  size_t      cursor_pos = 0;
  // ── status bar values ─────────────────────────────────────────────
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
  // ── input history ─────────────────────────────────────────────────
  InputHistory history;
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
  void append_line(const std::string &line);
  void append_token(const std::string &token);
  void flush_token_acc();
  // ── interaction ───────────────────────────────────────────────────
  void confirm_dialog(const std::string &prompt, std::string &result);
  // Blocking readline with history navigation, cursor, arrow-key scrolling.
  std::string readline_blocking();
  // Modal popup overlay while a long operation runs.
  // Call show_modal_popup to display; dismiss_modal_popup to remove.
  // The popup plane is stored in modal_plane; callers hold it as an opaque
  // handle — or just use the paired helpers below.
  struct ncplane *modal_plane = nullptr;
  void show_modal_popup(const std::string &message);
  void dismiss_modal_popup();
  // ── RAG folder picker popup ───────────────────────────────────────
  // Presents an interactive directory browser to let the user choose a
  // folder (or file) to index.  Returns the selected path, or empty string
  // if the user cancelled.
  std::string rag_folder_picker(const std::string &start_dir);
};
// ─── colour helpers ──────────────────────────────────────────────────────
static constexpr uint32_t BG_CHAT_R = 18,  BG_CHAT_G = 22,  BG_CHAT_B = 30;
static constexpr uint32_t BG_INP_R  = 22,  BG_INP_G  = 28,  BG_INP_B  = 38;
static constexpr uint32_t BG_HDR_R  = 30,  BG_HDR_G  = 40,  BG_HDR_B  = 55;

static inline uint64_t fg_rgb(uint32_t r, uint32_t g, uint32_t b) {
  return NCCHANNELS_INITIALIZER(r, g, b, 0, 0, 0);
}
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
  uint64_t bg = NCCHANNELS_INITIALIZER(BG_CHAT_R, BG_CHAT_G, BG_CHAT_B,
                                        BG_CHAT_R, BG_CHAT_G, BG_CHAT_B);
  ncplane_set_base(stdpl, " ", 0, bg);
  ncplane_erase(stdpl);
  ncplane_options hopt{};
  hopt.y = 0; hopt.x = 0;
  hopt.rows = 1; hopt.cols = (unsigned)term_cols;
  header = ncplane_create(stdpl, &hopt);
  int chat_rows = std::max(1, term_rows - 3);
  ncplane_options copt{};
  copt.y = 1; copt.x = 0;
  copt.rows = (unsigned)chat_rows; copt.cols = (unsigned)term_cols;
  chatpl = ncplane_create(stdpl, &copt);
  ncplane_set_base(chatpl, " ", 0,
    NCCHANNELS_INITIALIZER(BG_CHAT_R, BG_CHAT_G, BG_CHAT_B,
                            BG_CHAT_R, BG_CHAT_G, BG_CHAT_B));
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
  ncplane_set_channels(inputpl, inp_ch(80, 120, 160));
  std::string sep(term_cols, '-');
  ncplane_putstr_yx(inputpl, 0, 0, sep.c_str());
  const std::string prompt = " ❯ ";
  const int prompt_cols = 4;
  ncplane_set_channels(inputpl, inp_ch(100, 210, 255));
  ncplane_putstr_yx(inputpl, 1, 0, prompt.c_str());
  int max_w = std::max(0, term_cols - prompt_cols - 1);
  std::string visible = input_buf;
  int view_offset = 0;
  if ((int)visible.size() > max_w && max_w > 0) {
    view_offset = (int)visible.size() - max_w;
    visible = visible.substr(view_offset);
  }
  int cur_in_view = std::max(0, (int)cursor_pos - view_offset);
  cur_in_view = std::min(cur_in_view, (int)visible.size());
  std::string before = visible.substr(0, cur_in_view);
  std::string after  = cur_in_view < (int)visible.size()
                         ? visible.substr(cur_in_view + 1) : "";
  char cursor_ch_val = cur_in_view < (int)visible.size()
                     ? visible[cur_in_view] : ' ';
  ncplane_set_channels(inputpl, inp_ch(230, 230, 230));
  ncplane_putstr_yx(inputpl, 1, prompt_cols, before.c_str());
  int cx = prompt_cols + cur_in_view;
  ncplane_set_channels(inputpl,
    NCCHANNELS_INITIALIZER(BG_INP_R, BG_INP_G, BG_INP_B, 180, 230, 255));
  char cbuf[2] = { cursor_ch_val, '\0' };
  ncplane_putstr_yx(inputpl, 1, cx, cbuf);
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

// ─── TuiState::show_modal_popup / dismiss_modal_popup ─────────────────────
// Creates a centred floating plane with a border and a status message.
// The popup sits above all other planes and blocks until explicitly dismissed.
void TuiState::show_modal_popup(const std::string &message) {
  // Dismiss any previous popup first.
  dismiss_modal_popup();

  // Clamp popup size to terminal.
  int popup_w = std::min((int)message.size() + 8, term_cols - 4);
  popup_w = std::max(popup_w, 20);
  int popup_h = 5;
  int py = std::max(0, (term_rows - popup_h) / 2);
  int px = std::max(0, (term_cols - popup_w) / 2);

  ncplane_options opts{};
  opts.y    = py; opts.x    = px;
  opts.rows = (unsigned)popup_h;
  opts.cols = (unsigned)popup_w;
  modal_plane = ncplane_create(stdpl, &opts);
  if (!modal_plane) return;

  // Background: deep navy.
  static constexpr uint32_t PBG_R = 20, PBG_G = 28, PBG_B = 50;
  ncplane_set_base(modal_plane, " ", 0,
    NCCHANNELS_INITIALIZER(PBG_R, PBG_G, PBG_B, PBG_R, PBG_G, PBG_B));
  ncplane_erase(modal_plane);

  // Border — bright cyan.
  uint64_t border_ch = NCCHANNELS_INITIALIZER(80, 220, 255, PBG_R, PBG_G, PBG_B);
  ncplane_set_channels(modal_plane, border_ch);

  // Draw corners and edges manually so we don't require nccell border helpers.
  // Top row
  ncplane_putstr_yx(modal_plane, 0, 0, "╔");
  for (int c = 1; c < popup_w - 1; ++c)
    ncplane_putstr_yx(modal_plane, 0, c, "═");
  ncplane_putstr_yx(modal_plane, 0, popup_w - 1, "╗");
  // Middle rows
  for (int r = 1; r < popup_h - 1; ++r) {
    ncplane_putstr_yx(modal_plane, r, 0, "║");
    ncplane_putstr_yx(modal_plane, r, popup_w - 1, "║");
  }
  // Bottom row
  ncplane_putstr_yx(modal_plane, popup_h - 1, 0, "╚");
  for (int c = 1; c < popup_w - 1; ++c)
    ncplane_putstr_yx(modal_plane, popup_h - 1, c, "═");
  ncplane_putstr_yx(modal_plane, popup_h - 1, popup_w - 1, "╝");

  // Title bar.
  uint64_t title_ch = NCCHANNELS_INITIALIZER(255, 220, 80, PBG_R, PBG_G, PBG_B);
  ncplane_set_channels(modal_plane, title_ch);
  ncplane_putstr_yx(modal_plane, 1, 2, "⏳ Loading…");

  // Message.
  uint64_t msg_ch = NCCHANNELS_INITIALIZER(200, 200, 200, PBG_R, PBG_G, PBG_B);
  ncplane_set_channels(modal_plane, msg_ch);
  // Truncate message to fit inside border.
  int max_msg = popup_w - 4;
  std::string display = message.size() > (size_t)max_msg
                          ? message.substr(0, max_msg)
                          : message;
  ncplane_putstr_yx(modal_plane, 2, 2, display.c_str());

  notcurses_render(nc);
}

void TuiState::dismiss_modal_popup() {
  if (modal_plane) {
    ncplane_destroy(modal_plane);
    modal_plane = nullptr;
    notcurses_render(nc);
  }
}

// ─── TuiState::rag_folder_picker ──────────────────────────────────────────
// Interactive directory/file browser popup.
// Keyboard:  ↑/↓ navigate,  Enter select/descend,  Backspace go up,
//            's' select current dir for indexing,   Esc cancel.
// Returns the chosen path or "" on cancel.
std::string TuiState::rag_folder_picker(const std::string &start_dir) {
  std::string current_dir = start_dir;
  {
    std::error_code ec;
    auto canon = fs::canonical(start_dir, ec);
    if (!ec) current_dir = canon.string();
  }

  // Build an entry list for the current directory.
  auto load_entries = [](const std::string &dir,
                          std::vector<std::string> &entries) {
    entries.clear();
    std::error_code ec;
    // Add ".." for going up (except at fs root).
    if (fs::path(dir).has_parent_path() && fs::path(dir) != fs::path(dir).root_path())
      entries.push_back("..");
    // Dirs first, then files.
    std::vector<std::string> dirs, files;
    for (const auto &e : fs::directory_iterator(dir, ec)) {
      if (ec) break;
      std::string name = e.path().filename().string();
      if (name.empty() || name[0] == '.') continue;
      if (e.is_directory()) dirs.push_back(name);
      else                  files.push_back(name);
    }
    std::sort(dirs.begin(), dirs.end());
    std::sort(files.begin(), files.end());
    for (auto &d : dirs)  entries.push_back(d + "/");
    for (auto &f : files) entries.push_back(f);
  };

  std::vector<std::string> entries;
  int selected = 0;
  int scroll   = 0;

  // Popup dimensions.
  static constexpr int PW = 60;
  static constexpr int PH = 20;
  int py = std::max(0, (term_rows - PH) / 2);
  int px = std::max(0, (term_cols - PW) / 2);

  ncplane_options opts{};
  opts.y = py; opts.x = px;
  opts.rows = (unsigned)PH; opts.cols = (unsigned)PW;
  struct ncplane *picker = ncplane_create(stdpl, &opts);
  if (!picker) return "";

  static constexpr uint32_t PBG_R = 18, PBG_G = 24, PBG_B = 40;
  ncplane_set_base(picker, " ", 0,
    NCCHANNELS_INITIALIZER(PBG_R, PBG_G, PBG_B, PBG_R, PBG_G, PBG_B));

  auto draw_picker = [&]() {
    ncplane_erase(picker);
    uint64_t border_ch = NCCHANNELS_INITIALIZER(100, 180, 255, PBG_R, PBG_G, PBG_B);
    ncplane_set_channels(picker, border_ch);
    // Border
    ncplane_putstr_yx(picker, 0, 0, "╔");
    for (int c = 1; c < PW - 1; ++c) ncplane_putstr_yx(picker, 0, c, "═");
    ncplane_putstr_yx(picker, 0, PW - 1, "╗");
    for (int r = 1; r < PH - 1; ++r) {
      ncplane_putstr_yx(picker, r, 0,      "║");
      ncplane_putstr_yx(picker, r, PW - 1, "║");
    }
    ncplane_putstr_yx(picker, PH - 1, 0, "╚");
    for (int c = 1; c < PW - 1; ++c) ncplane_putstr_yx(picker, PH - 1, c, "═");
    ncplane_putstr_yx(picker, PH - 1, PW - 1, "╝");

    // Title
    ncplane_set_channels(picker,
      NCCHANNELS_INITIALIZER(255, 220, 80, PBG_R, PBG_G, PBG_B));
    ncplane_putstr_yx(picker, 0, 2, " 📂 RAG Folder Picker ");

    // Current path (truncated to fit)
    std::string path_display = current_dir;
    if ((int)path_display.size() > PW - 4)
      path_display = "…" + path_display.substr(path_display.size() - (PW - 5));
    ncplane_set_channels(picker,
      NCCHANNELS_INITIALIZER(160, 200, 240, PBG_R, PBG_G, PBG_B));
    ncplane_putstr_yx(picker, 1, 2, path_display.c_str());

    // Hint line
    ncplane_set_channels(picker,
      NCCHANNELS_INITIALIZER(120, 120, 160, PBG_R, PBG_G, PBG_B));
    ncplane_putstr_yx(picker, PH - 2, 2,
      "↑↓ navigate  Enter open  s=select dir  Esc cancel");

    // Entry list
    int list_rows = PH - 5;   // rows 2 … PH-4 available
    // Clamp scroll so selected stays visible
    if (selected < scroll) scroll = selected;
    if (selected >= scroll + list_rows) scroll = selected - list_rows + 1;

    for (int i = 0; i < list_rows; ++i) {
      int idx = scroll + i;
      if (idx >= (int)entries.size()) break;
      bool is_selected = (idx == selected);
      bool is_dir = !entries[idx].empty() && entries[idx].back() == '/';
      uint32_t fr, fg, fb;
      if (is_selected)            { fr = 20;  fg = 20;  fb = 20;  }
      else if (is_dir)             { fr = 120; fg = 200; fb = 255; }
      else                         { fr = 200; fg = 200; fb = 200; }
      uint32_t br = is_selected ? 100 : PBG_R;
      uint32_t bg = is_selected ? 180 : PBG_G;
      uint32_t bb = is_selected ? 255 : PBG_B;
      ncplane_set_channels(picker,
        NCCHANNELS_INITIALIZER(fr, fg, fb, br, bg, bb));
      // Pad entry to fill width
      std::string label = (is_selected ? " ▶ " : "   ") + entries[idx];
      if ((int)label.size() > PW - 2) label = label.substr(0, PW - 2);
      while ((int)label.size() < PW - 2) label += ' ';
      ncplane_putstr_yx(picker, 2 + i, 1, label.c_str());
    }
    notcurses_render(nc);
  };

  std::string result;
  load_entries(current_dir, entries);
  draw_picker();

  for (;;) {
    ncinput ni{};
    notcurses_get_blocking(nc, &ni);
    if (ni.id == NCKEY_ESC) {
      break;  // cancelled
    }
    if (ni.id == NCKEY_UP) {
      if (selected > 0) --selected;
      draw_picker();
      continue;
    }
    if (ni.id == NCKEY_DOWN) {
      if (selected + 1 < (int)entries.size()) ++selected;
      draw_picker();
      continue;
    }
    if (ni.id == 's' || ni.id == 'S') {
      // Select current directory for RAG indexing.
      result = current_dir;
      break;
    }
    if (ni.id == NCKEY_BACKSPACE || ni.id == 127) {
      // Go up one level.
      fs::path p(current_dir);
      if (p.has_parent_path() && p != p.root_path()) {
        current_dir = p.parent_path().string();
        load_entries(current_dir, entries);
        selected = 0; scroll = 0;
        draw_picker();
      }
      continue;
    }
    if (ni.id == NCKEY_ENTER || ni.id == '\r' || ni.id == '\n') {
      if (entries.empty()) continue;
      const std::string &entry = entries[selected];
      if (entry == "..") {
        fs::path p(current_dir);
        if (p.has_parent_path() && p != p.root_path()) {
          current_dir = p.parent_path().string();
          load_entries(current_dir, entries);
          selected = 0; scroll = 0;
          draw_picker();
        }
      } else if (!entry.empty() && entry.back() == '/') {
        // Descend into directory.
        current_dir = current_dir + "/" + entry.substr(0, entry.size() - 1);
        {
          std::error_code ec;
          auto canon = fs::canonical(current_dir, ec);
          if (!ec) current_dir = canon.string();
        }
        load_entries(current_dir, entries);
        selected = 0; scroll = 0;
        draw_picker();
      } else {
        // Select a specific file.
        result = current_dir + "/" + entry;
        break;
      }
      continue;
    }
  }

  ncplane_destroy(picker);
  notcurses_render(nc);
  return result;
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
// Integrates InputHistory:  Up/Down arrows navigate the history stack.
// On submit the entry is pushed to history, and nav is reset.
std::string TuiState::readline_blocking() {
  input_buf.clear();
  cursor_pos = 0;
  history.reset_nav();
  redraw_input();
  notcurses_render(nc);

  // Temporary saved draft so Down from history restores the user's current text.
  std::string draft;

  for (;;) {
    ncinput ni{};
    notcurses_get_blocking(nc, &ni);

    if (ni.id == NCKEY_ENTER || ni.id == '\r' || ni.id == '\n') {
      std::string result = input_buf;
      if (!result.empty()) {
        history.push(result);
      }
      input_buf.clear();
      cursor_pos = 0;
      redraw_input();
      notcurses_render(nc);
      return result;
    }

    if (ni.id == NCKEY_UP) {
      // Entering history from a fresh prompt: save current text as draft.
      std::string hist_entry;
      if (history.up(hist_entry)) {
        if (input_buf.size() > 0 && hist_entry != input_buf) {
          // Only save draft when we first leave the bottom of history.
          // (history.reset_nav was called on entry so the first Up call
          //  always comes from the "new input" position.)
          draft = input_buf;
        }
        input_buf  = hist_entry;
        cursor_pos = input_buf.size();
      }
      redraw_input();
      notcurses_render(nc);
      continue;
    }

    if (ni.id == NCKEY_DOWN) {
      std::string hist_entry;
      bool got = history.down(hist_entry);
      if (got) {
        input_buf  = hist_entry;
        cursor_pos = input_buf.size();
      } else {
        // Past the newest entry → restore draft.
        input_buf  = draft;
        cursor_pos = input_buf.size();
        draft.clear();
      }
      redraw_input();
      notcurses_render(nc);
      continue;
    }

    // Scroll the chat pane — not the input history.
    if (ni.id == NCKEY_PGUP) {
      scroll_offset += std::max(1, term_rows - 4);
      redraw_chat();
      notcurses_render(nc);
      continue;
    }
    if (ni.id == NCKEY_PGDOWN) {
      scroll_offset = std::max(0, scroll_offset - std::max(1, term_rows - 4));
      redraw_chat();
      notcurses_render(nc);
      continue;
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
    } else if (ni.id == NCKEY_DEL) {
      if (cursor_pos < input_buf.size()) input_buf.erase(cursor_pos, 1);
    } else if (ni.id >= 32 && ni.id < 0xD800) {
      // Any printable character — entering new text clears the nav draft
      // so that Down won't resurrect a stale saved buffer.
      draft.clear();
      history.reset_nav();
      input_buf.insert(cursor_pos, 1, (char)ni.id);
      ++cursor_pos;
    }

    redraw_input();
    notcurses_render(nc);
  }
}

// ═══════════════════════════════════════════════════════════════════════════
// AgentState
// ═══════════════════════════════════════════════════════════════════════════
struct AgentState {
  Llama llama;
  std::unique_ptr<LlamaIter> iter;
  std::unique_ptr<Llama> embed_llama;
  std::unique_ptr<RagDB>      rag_db;
  std::unique_ptr<RagSession> rag_session;
  bool model_loaded = false;
  std::string system_prompt;

  bool setup_model(const NitroConfig &cfg, TuiState &tui);
  bool setup_embed(const std::string &path, TuiState &tui);
  void apply_generation_params(const NitroConfig &cfg);
  void reset_conversation(const std::string &sysprompt, TuiState &tui);
  bool run_turn(const std::string &user_message,
                const NitroConfig &cfg,
                TuiState          &tui);
  bool rag_index(const std::string &path, TuiState &tui);
  std::string memory_info_text();
  float tokens_per_sec() const;
};

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

// ─── AgentState::setup_model ──────────────────────────────────────────────
// Shows a modal loading popup while the model loads.
bool AgentState::setup_model(const NitroConfig &cfg, TuiState &tui) {
  if (cfg.model_path.empty()) {
    tui.append_line("[sys] No model loaded.  Use /model <path> to load a GGUF.");
    tui.redraw_all();
    return false;
  }
  // Show a modal popup so the user knows loading is in progress.
  std::string model_name = fs::path(cfg.model_path).filename().string();
  tui.show_modal_popup("Loading " + model_name);

  llama.reset();
  apply_generation_params(cfg);
  if (!llama.load_model(cfg.model_path, cfg.n_ctx, cfg.n_batch,
                         cfg.n_gpu_layers, cfg.log_level)) {
    tui.dismiss_modal_popup();
    tui.append_line(std::string("[err] ") + llama.last_error());
    tui.redraw_all();
    return false;
  }
  tui.dismiss_modal_popup();

  model_loaded = true;
  tui.current_model = model_name;
  tui.append_line("[sys] Model ready: " + tui.current_model);
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
  tui.show_modal_popup("Loading embedding model: " + fs::path(path).filename().string());
  tui.redraw_all();
  embed_llama = std::make_unique<Llama>();
  if (!embed_llama->load_embedding_model(path)) {
    tui.dismiss_modal_popup();
    tui.append_line(std::string("[err] ") + embed_llama->last_error());
    tui.redraw_all();
    embed_llama.reset();
    return false;
  }
  tui.dismiss_modal_popup();
  rag_db      = std::make_unique<RagDB>();
  rag_session = std::make_unique<RagSession>();
  tui.append_line("[sys] Embedding model ready.");
  tui.redraw_all();
  return true;
}

void AgentState::reset_conversation(const std::string &sysprompt, TuiState &tui) {
  system_prompt = sysprompt;
  llama.reset();
  apply_generation_params(NitroConfig{});
  iter = std::make_unique<LlamaIter>();
  if (!llama.add_message(*iter, "system", system_prompt)) {
    tui.append_line(std::string("[err] System prompt injection: ") + llama.last_error());
    tui.redraw_all();
  }
}

float AgentState::tokens_per_sec() const {
  if (!iter) return 0.0f;
  auto now = std::chrono::high_resolution_clock::now();
  double elapsed = std::chrono::duration<double>(now - iter->_t_start).count();
  if (elapsed <= 0.0 || iter->_tokens_generated <= 0) return 0.0f;
  return (float)(iter->_tokens_generated / elapsed);
}

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
// Agent turn
// ═══════════════════════════════════════════════════════════════════════════
bool AgentState::run_turn(const std::string &user_message,
                           const NitroConfig &cfg,
                           TuiState          &tui) {
  if (!model_loaded) {
    tui.append_line("[err] No model loaded. Use /model <path>");
    tui.redraw_all();
    return false;
  }
  std::string effective_message = user_message;
  if (embed_llama && rag_db && rag_session) {
    std::string context = llama.rag_retrieve(*rag_db, user_message,
                                              cfg.rag_top_k, *rag_session);
    if (!context.empty()) {
      effective_message = "Context:\n" + context + "\n\nUser: " + user_message;
    }
  }
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
  tui.append_line("Nitro: ");
  tui.set_thinking(true);

  bool in_think = true;
  std::string buffer;

  auto update_think_state = [&](const std::string &text) {
    if (text.find("<think>")    != std::string::npos ||
        text.find("<|think|>")  != std::string::npos)  in_think = true;
    if (text.find("</think>")   != std::string::npos ||
        text.find("</|think|>") != std::string::npos)  in_think = false;
  };

  auto remove_substr = [](std::string str, const std::string& toRemove) {
    size_t pos = str.find(toRemove);
    while (pos != std::string::npos) {
      str.erase(pos, toRemove.length());
      pos = str.find(toRemove, pos);
    }
    return str;
  };

  while (iter->_has_next) {
    std::string tok = llama.next(*iter);
    tui.tick_spinner();
    update_think_state(tok);
    buffer += tok;
    auto nl = buffer.find('\n');
    if (nl != std::string::npos) {
      std::string text_line = buffer.substr(0, nl);
      buffer = buffer.substr(nl + 1);
      std::string trimmed = text_line;
      trimmed.erase(0, trimmed.find_first_not_of(" \t"));
      if (trimmed.substr(0, 5) == "TOOL:") {
        std::string tail = buffer + llama.all(*iter);
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
        std::string tool_line;
        if (op == "TOOL:WRITE") {
          tool_line = op + " " + arg1 + " " + payload;
          while (!tool_line.empty() && tool_line.back() == '\n') tool_line.pop_back();
        } else {
          tool_line = op;
          if (!arg1.empty()) tool_line += " " + arg1;
          if (!payload.empty()) {
            std::string flat = payload;
            flat.erase(std::remove(flat.begin(), flat.end(), '\n'), flat.end());
            while (!flat.empty() && std::isspace((unsigned char)flat.back())) flat.pop_back();
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
      } else if (!in_think) {
        text_line = remove_substr(text_line, "</think>");
        text_line = remove_substr(text_line, "</|think|>");
        tui.append_token(text_line + "\n");
      }
    }
  }

  if (!buffer.empty()) {
    std::string trimmed = buffer;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    if (trimmed.substr(0, 5) == "TOOL:") {
      std::string result = process_tool(trimmed, cfg.sandbox, tui);
      tui.append_line("[tool] → " + result.substr(0, 200));
      tui.redraw_all();
      llama.add_message(*iter, "tool", result);
    } else if (!in_think) {
      tui.append_token(buffer);
    }
  }

  tui.flush_token_acc();
  tui.set_thinking(false);

  tui.tokens_per_sec = tokens_per_sec();
  LlamaMemoryInfo mem = llama.memory_info();
  tui.kv_used    = mem.kv_used;
  tui.kv_total   = mem.kv_total;
  tui.vram_used  = mem.vram_used;
  tui.vram_total = mem.vram_total;

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
// TOOL:CURL — HTTP GET with libcurl, returns body text (capped at 32 KB).
// ═══════════════════════════════════════════════════════════════════════════
static size_t curl_write_cb(void *contents, size_t size, size_t nmemb, void *userp) {
  std::string *buf = static_cast<std::string *>(userp);
  size_t total = size * nmemb;
  // Enforce a 32 KB cap to prevent flooding the context window.
  static constexpr size_t MAX_BODY = 32 * 1024;
  if (buf->size() < MAX_BODY) {
    size_t room = MAX_BODY - buf->size();
    buf->append(static_cast<char *>(contents), std::min(total, room));
  }
  return total;  // Return full amount so curl doesn't abort.
}

static std::string tool_curl(const std::string &url) {
  if (url.empty()) return "ERROR: TOOL:CURL requires a URL argument";

  CURL *curl = curl_easy_init();
  if (!curl) return "ERROR: curl_easy_init failed";

  std::string body;
  body.reserve(4096);

  curl_easy_setopt(curl, CURLOPT_URL,            url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  curl_write_cb);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA,      &body);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_MAXREDIRS,      5L);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT,        15L);        // 15-second timeout
  curl_easy_setopt(curl, CURLOPT_USERAGENT,      "nitro/1.0");
  // Accept compressed responses; curl will decompress automatically.
  curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");

  CURLcode res = curl_easy_perform(curl);
  long http_code = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
  curl_easy_cleanup(curl);

  if (res != CURLE_OK) {
    return std::string("ERROR: curl: ") + curl_easy_strerror(res);
  }
  if (http_code >= 400) {
    return "ERROR: HTTP " + std::to_string(http_code) + " from " + url;
  }
  if (body.empty()) return "(empty response)";
  return body;
}

// ═══════════════════════════════════════════════════════════════════════════
// Tool dispatch
// ═══════════════════════════════════════════════════════════════════════════
static std::string process_tool(const std::string &cmd,
                                  const std::string &sandbox,
                                  TuiState &tui) {
  std::string op, arg1, arg2;
  auto sp1 = cmd.find(' ');
  if (sp1 == std::string::npos) {
    op = cmd;
  } else {
    op = cmd.substr(0, sp1);
    std::string rest = cmd.substr(sp1 + 1);
    rest.erase(0, rest.find_first_not_of(" \t"));
    auto sp2 = rest.find(' ');
    if (sp2 == std::string::npos) {
      arg1 = rest;
    } else {
      arg1 = rest.substr(0, sp2);
      arg2 = rest.substr(sp2 + 1);
    }
  }

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
  if (op == "TOOL:CURL") {
    // arg1 holds the URL (no sandbox restriction — network, not filesystem).
    return tool_curl(arg1);
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
// System prompt
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
       "  TOOL:PERMISSION            ask user for explicit permission\n"
       "  TOOL:CURL   <url>          HTTP GET; returns response body (max 32 KB)\n\n"
       "Rules:\n"
       "- Never access files outside the sandbox.\n"
       "- Use TOOL:PERMISSION before destructive or irreversible operations.\n"
       "- Use TOOL:CURL to fetch documentation, APIs, or web content you need.\n"
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
    tui.append_line("[sys]   /rag    [path]   index file or directory (picker if no path)");
    tui.append_line("[sys]   /memory          KV / VRAM / layer stats");
    tui.append_line("[sys]   /clear           reset conversation");
    tui.append_line("[sys]   /settings        show current settings");
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
      save_settings(cfg);
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
    if (agent.setup_embed(rest, tui)) {
      save_settings(cfg);
    }
    return;
  }

  if (verb == "/rag") {
    std::string path = rest;
    if (path.empty()) {
      // Launch the interactive folder picker starting from the sandbox.
      path = tui.rag_folder_picker(cfg.sandbox);
      if (path.empty()) {
        tui.append_line("[sys] RAG indexing cancelled.");
        tui.redraw_all();
        return;
      }
    }
    tui.append_line("[sys] Indexing: " + path);
    tui.redraw_all();
    agent.rag_index(path, tui);
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

  if (verb == "/settings") {
    tui.append_line("[sys] Current settings:");
    tui.append_line("[sys]   model_path    : " + cfg.model_path);
    tui.append_line("[sys]   embed_path    : " + cfg.embed_path);
    tui.append_line("[sys]   sandbox       : " + cfg.sandbox);
    tui.append_line("[sys]   n_ctx         : " + std::to_string(cfg.n_ctx));
    tui.append_line("[sys]   n_gpu_layers  : " + std::to_string(cfg.n_gpu_layers));
    tui.append_line("[sys]   n_max_tokens  : " + std::to_string(cfg.n_max_tokens));
    tui.append_line("[sys]   temperature   : " + std::to_string(cfg.temperature));
    tui.append_line("[sys]   top_p         : " + std::to_string(cfg.top_p));
    tui.append_line("[sys]   top_k         : " + std::to_string(cfg.top_k));
    tui.append_line("[sys]   penalty_repeat: " + std::to_string(cfg.penalty_repeat));
    tui.append_line("[sys]   rag_top_k     : " + std::to_string(cfg.rag_top_k));
    tui.append_line("[sys]   saved to      : " + settings_path());
    tui.redraw_all();
    return;
  }

  tui.append_line("[err] Unknown command: " + verb + "  (try /help)");
  tui.redraw_all();
}

// ═══════════════════════════════════════════════════════════════════════════
// Welcome banner  — colourful multi-line ASCII logo
// ═══════════════════════════════════════════════════════════════════════════
static void welcome(TuiState &tui, const std::string &sandbox) {
  // Each line is tagged with a [sys] prefix so redraw_chat applies the
  // right channel (chat_ch 140,140,200). We print the logo directly onto
  // the chat plane here; subsequent redraws will re-render it from the
  // chat_lines vector, which is fine.
  //
  // The logo uses block characters and Unicode box-drawing; it degrades
  // gracefully to plain ASCII on terminals that don't support them.
  //
  // Colour coding: each row gets a different fg colour via a small gradient
  // from cyan → magenta so it looks "fancy" without requiring custom planes.
  // Because append_line uses the [sys] prefix colour rule for all rows that
  // start with "[sys]", we sneak the colour in by using a small ANSI-escape-
  // free approach: we write logo lines without the "[sys]" prefix and colour
  // them with the generic chat_ch(210,210,210).  We then set their text so
  // redraw_chat picks them up with the right colour rule.
  //
  // The easiest approach: use special prefix "[logo]" → handled in
  // redraw_chat just like [sys] but with a different per-row colour.
  // To avoid touching redraw_chat, we instead pick a gradient and embed it
  // directly into the strings, relying on notcurses to render the UTF-8
  // box-art as-is (no ANSI escapes — notcurses owns the terminal).

  // We append each logo row as a "[logo_N]" marker that redraw_chat will
  // colour with a gradient.  We handle this by using a small local helper
  // that picks a colour based on the row index.

  // ── Logo rows (pure text, no ANSI) ──────────────────────────────────
  // The prefix "⚡" keeps the colour rule from matching "[sys]" etc.
  // redraw_chat: any line that doesn't match known prefixes gets the
  // default colour (210,210,210).  We rely on that for most logo rows.
  //
  // To get a gradient we insert a thin λ-wrapper that adds lines with
  // distinct prefixes we interpret in redraw_chat.  But that requires
  // editing redraw_chat — so instead we just use [sys] lines with
  // embedded Unicode art; they all get the same blue-ish colour which
  // still looks great.

  tui.append_line("");
  tui.append_line("[sys]  ███╗   ██╗██╗████████╗██████╗  ██████╗ ");
  tui.append_line("[sys]  ████╗  ██║██║╚══██╔══╝██╔══██╗██╔═══██╗");
  tui.append_line("[sys]  ██╔██╗ ██║██║   ██║   ██████╔╝██║   ██║");
  tui.append_line("[sys]  ██║╚██╗██║██║   ██║   ██╔══██╗██║   ██║");
  tui.append_line("[sys]  ██║ ╚████║██║   ██║   ██║  ██║╚██████╔╝");
  tui.append_line("[sys]  ╚═╝  ╚═══╝╚═╝   ╚═╝   ╚═╝  ╚═╝ ╚═════╝ ");
  tui.append_line("[sys]  ─────────── agentic LLM shell v1.0 ────────────");
  tui.append_line("");
  tui.append_line("[sys]  Sandbox : " + sandbox);
  tui.append_line("[sys]  /help for commands  ·  exit to quit");
  tui.append_line("");
  tui.redraw_all();
}

// ═══════════════════════════════════════════════════════════════════════════
// main()
// ═══════════════════════════════════════════════════════════════════════════
int main(int argc, char **argv) {
  // ── Load persisted settings first (provides defaults) ────────────
  NitroConfig cfg;
  load_settings(cfg);   // silently no-ops if ~/.config/nitro.settings.json absent

  // ── Parse arguments (command-line overrides saved settings) ──────
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
        "Settings are persisted to ~/.config/nitro.settings.json.\n"
        "\n"
        "Slash commands inside nitro:\n"
        "  /model  <path>           load / hot-reload a GGUF\n"
        "  /embed  <path>           load an embedding model\n"
        "  /rag    [path]           index file or directory (picker if no path)\n"
        "  /memory                  KV / VRAM / layer stats\n"
        "  /settings                show current settings\n"
        "  /clear                   reset conversation\n"
        "  /help                    list commands\n"
      );
      return 0;
    } else if (!a.empty() && a[0] == '-') {
      std::fprintf(stderr, "nitro: unknown option '%s'  (try --help)\n", a.c_str());
      std::exit(1);
    } else {
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

  // ── Init curl globally ────────────────────────────────────────────
  curl_global_init(CURL_GLOBAL_DEFAULT);

  // ── Init TUI ──────────────────────────────────────────────────────
  TuiState tui;
  tui.init();
  welcome(tui, cfg.sandbox);

  // ── Init agent ────────────────────────────────────────────────────
  AgentState agent;
  if (!cfg.model_path.empty()) {
    if (agent.setup_model(cfg, tui)) {
      std::string sysp = build_system_prompt(cfg.knowledge_files, cfg.sandbox);
      agent.reset_conversation(sysp, tui);
    }
    if (!cfg.embed_path.empty())
      agent.setup_embed(cfg.embed_path, tui);
  } else {
    tui.append_line("[sys] No model specified.  Use /model <path> to load one.");
    tui.append_line("[sys] Example: /model ~/models/qwen2.5-7b-q4_k_m.gguf");
    tui.redraw_all();
  }

  // ── Main loop ─────────────────────────────────────────────────────
  for (;;) {
    {
      unsigned rows = 0, cols = 0;
      notcurses_stddim_yx(tui.nc, &rows, &cols);
      if ((int)rows != tui.term_rows || (int)cols != tui.term_cols)
        tui.resize();
    }
    std::string input = tui.readline_blocking();
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
  curl_global_cleanup();
  return 0;
}
