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
//   /model  <path>            — load / hot-reload a GGUF model (picker if no path)
//   /embed  <path>            — load an embedding model for RAG (picker if no path)
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
//   TOOL:CURL   <url>
//
// Copyright (C) 2026 Chris Warren-Smith  —  GPLv2 or later
//

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <ctime>
#include <curl/curl.h>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <memory>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "llama-sb.h"
#include "llama-sb-rag.h"

#include <notcurses/notcurses.h>

namespace fs = std::filesystem;

//
// NitroConfig
//
struct NitroConfig {
  std::string model_path;
  std::string embed_path;
  std::string sandbox;
  std::string agent_id;
  int   n_ctx          = 65536;
  int   n_batch        = 512;
  int   n_gpu_layers   = 32;
  int   log_level      = GGML_LOG_LEVEL_CONT;
  float temperature    = 0.6f;
  float top_p          = 0.95f;
  float min_p          = 0.0f;
  int   top_k          = 20;
  float penalty_repeat = 1.0f;
  int   penalty_last_n = 256;
  std::vector<std::string> knowledge_files;
  int   rag_top_k      = 5;
  bool  thinking       = true;
  bool  permission_prompt = false;
  // TOOL:RUN allowlist — if non-empty, only these program basenames may run.
  // Empty means "allow anything inside the sandbox" (original behaviour).
  std::vector<std::string> run_allowed;
};

//
// InputHistory — up/down arrow navigation through submitted inputs
//
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
      current_index = static_cast<int>(history_stack.size());
      return;
    }
    history_stack.push_back(input);
    current_index = static_cast<int>(history_stack.size());
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
    if (current_index >= static_cast<int>(history_stack.size())) {
      current_index = static_cast<int>(history_stack.size());
      out.clear();
      return false; // signal: restore blank input
    }
    out = history_stack[current_index];
    return true;
  }

  /** Reset navigation position without modifying the stack. */
  void reset_nav() {
    current_index = static_cast<int>(history_stack.size());
  }

  /**
   * @brief Load history from ~/.config/nitro/nitro.history (one entry per line).
   * Silently succeeds if the file doesn't exist.
   */
  void load(const std::string &path) {
    std::ifstream f(path);
    if (!f) return;
    std::string line;
    while (std::getline(f, line)) {
      if (!line.empty()) history_stack.push_back(line);
    }
    current_index = static_cast<int>(history_stack.size());
  }

  /**
   * @brief Persist history to disk (most-recent last, one entry per line).
   * Caps at MAX_PERSIST entries so the file never grows unbounded.
   */
  void save(const std::string &path) const {
    // Ensure parent directory exists.
    fs::path dir = fs::path(path).parent_path();
    std::error_code ec;
    fs::create_directories(dir, ec);

    std::ofstream f(path, std::ios::trunc);
    if (!f) return;

    static constexpr int MAX_PERSIST = 500;
    int start = std::max(0, static_cast<int>(history_stack.size()) - MAX_PERSIST);
    for (int i = start; i < static_cast<int>(history_stack.size()); ++i) {
      // Escape embedded newlines so each entry stays on one line.
      for (char c : history_stack[i]) {
        if (c == '\n') f << "\\n";
        else           f << c;
      }
      f << '\n';
    }
  }

  private:
  std::vector<std::string> history_stack;
  int current_index = 0;
};

//
// Notcurses TUI
//
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
  bool        mouse_mode = true;
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
  void redraw_header() const;
  void redraw_chat();
  void redraw_input() const;
  void redraw_all();
  // ── content helpers ───────────────────────────────────────────────
  void append_line(const std::string &line);
  void append_token(const std::string &token);
  void flush_token_acc();
  // ── interaction ───────────────────────────────────────────────────
  bool confirm_dialog(const std::string &prompt) const;
  // Blocking readline with history navigation, cursor, arrow-key scrolling.
  std::string readline_blocking();
  // Modal popup overlay while a long operation runs.
  // Call show_modal_popup to display; dismiss_modal_popup to remove.
  // The popup plane is stored in modal_plane; callers hold it as an opaque
  // handle — or just use the paired helpers below.
  struct ncplane *modal_plane = nullptr;
  void show_modal_popup(const std::string &message);
  void show_help();
  void dismiss_modal_popup();
  // ── folder picker popup ───────────────────────────────────────
  // Presents an interactive directory browser to let the user choose a
  // folder (or file) to index.  Returns the selected path, or empty string
  // if the user cancelled.
  // ── file browser popup ─────────────────────────────────────
  // Used by /rag, /model, and /embed to pick a path interactively.
  // Pass a hint string shown in the title bar (e.g. "RAG Folder",
  // "Model File", "Embedding Model").
  // Returns the selected path, or empty string if the user cancelled.
  std::string file_picker(const std::string &start_dir,
                          const std::string &title_hint = "File") const;
  // Legacy alias kept for callers that used the old name.
  std::string rag_folder_picker(const std::string &start_dir) const {
    return file_picker(start_dir, "RAG Folder");
  }
};

//
// AgentState
//
struct AgentState {
  std::unique_ptr<Llama> llama;
  std::unique_ptr<LlamaIter> iter;
  std::unique_ptr<Llama> embed_llama;
  std::unique_ptr<RagDB>      rag_db;
  std::unique_ptr<RagSession> rag_session;
  bool model_loaded = false;
  std::string system_prompt;

  bool rag_index(const std::string &path, const NitroConfig &cfg, TuiState &tui) const;
  bool rag_load_index(const std::string &path, TuiState &tui) const;
  bool run_turn(const std::string &user_message, const NitroConfig &cfg, TuiState &tui) const;
  bool setup_embed(const std::string &path, TuiState &tui);
  bool setup_model(const NitroConfig &cfg, TuiState &tui);
  void apply_generation_params(const NitroConfig &cfg) const;
  void reset_conversation(const std::string &sysprompt, TuiState &tui);
  std::string memory_info_text() const;
  std::string process_tool(const std::string &cmd, const NitroConfig &cfg, TuiState &tui) const;
  std::string rag_tool(const NitroConfig &cfg, const std::string &agent_query) const;
  float tokens_per_sec() const;
};

//
// Logging
//

// ─── Debug logging (file-backed, safe to call while notcurses is active) ──
static FILE *g_logfile = nullptr;

static void log_open() {
  const char *home = getenv("HOME");
  std::string path = std::string(home ? home : ".") + "/.config/nitro/nitro.log";
  g_logfile = fopen(path.c_str(), "a");
}

static void log_close() {
  if (g_logfile) { fclose(g_logfile); g_logfile = nullptr; }
}

static void log_write(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static void log_write(const char *fmt, ...) {
  if (!g_logfile) {
    return;
  }
  // timestamp
  time_t t = time(nullptr);
  char ts[32];
  strftime(ts, sizeof(ts), "%H:%M:%S", localtime(&t));
  fprintf(g_logfile, "[%s] ", ts);
  va_list ap;
  va_start(ap, fmt);
  vfprintf(g_logfile, fmt, ap);
  va_end(ap);
  fputc('\n', g_logfile);
  // flush immediately so tail -f works
  fflush(g_logfile);
}

//
// Agent uniqueId
//
inline std::string encode_base64(const std::vector<char>& data) {
  static const char base64_chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

  std::string encoded;
  encoded.reserve((data.size() + 2) / 3 * 4);

  size_t i = 0;
  while (i < data.size()) {
    uint32_t val = static_cast<uint32_t>(data[i] << 16) |
      (i + 1 < data.size() ? static_cast<uint32_t>(data[i+1]) << 8 : 0) |
      (i + 2 < data.size() ? static_cast<uint32_t>(data[i+2]) : 0);

    encoded.push_back(base64_chars[(val >> 18) & 0x3F]);
    encoded.push_back(base64_chars[(val >> 12) & 0x3F]);
    encoded.push_back((i + 1 < data.size()) ? base64_chars[(val >> 6) & 0x3F] : '=');
    encoded.push_back((i + 2 < data.size()) ? base64_chars[val & 0x3F] : '=');
    i += 3;
  }
  return encoded;
}

class AgentSessionId {
  public:
  // Static method: Generates ID once, then returns it
  static std::string uniqueId() {
    // Yoda condition: static variable initialized only once
    static std::string s_id;

    if (s_id.empty()) {
      // 1. Get high-resolution timestamp (nanoseconds since epoch)
      auto now = std::chrono::steady_clock::now();
      auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

      // 2. Generate 48 bits of randomness
      std::random_device rd;
      std::mt19937_64 rng(rd());
      std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

      // Fill with random bytes
      std::array<char, 6> random_bytes;
      for (auto& b : random_bytes) {
        b = static_cast<char>(dist(rng) & 0xFF);

      }

      // 3. Combine timestamp (48 bits) and random (48 bits) into a 96-bit integer
      std::vector<char> data;
      data.reserve(12); // 96 bits = 12 bytes

      // Pack timestamp (upper 48 bits)
      data.push_back(static_cast<char>((nanos >> 40) & 0xFF));
      data.push_back(static_cast<char>((nanos >> 32) & 0xFF));
      data.push_back(static_cast<char>((nanos >> 24) & 0xFF));
      data.push_back(static_cast<char>((nanos >> 16) & 0xFF));
      data.push_back(static_cast<char>((nanos >> 8) & 0xFF));
      data.push_back(static_cast<char>(nanos & 0xFF));

      // Pack random (lower 48 bits)
      data.push_back(static_cast<char>((dist(rng) >> 40) & 0xFF));
      data.push_back(static_cast<char>((dist(rng) >> 32) & 0xFF));
      data.push_back(static_cast<char>((dist(rng) >> 24) & 0xFF));
      data.push_back(static_cast<char>((dist(rng) >> 16) & 0xFF));
      data.push_back(static_cast<char>((dist(rng) >> 8) & 0xFF));
      data.push_back(static_cast<char>(dist(rng) & 0xFF));

      // 4. Encode to Base64
      s_id = encode_base64(data);
    }
    return s_id;
  }
};

//
// handling for strip_code_fences
//
static const std::vector<std::string> CODE_EXTENSIONS = {
  ".py",".c",".cpp",".h",".bas",".java",".html",".js",".ts",
  ".json",".yaml",".toml",".sh",".go",".rs",".jsx",".tsx"
};

//
// Settings persistence  (~/.config/nitro/nitro.settings.json)
// Returns the canonical settings path: ~/.config/nitro/settings.json
//
static std::string settings_path() {
  // Attempt to read settings from the current working directory first
  if (fs::exists("nitro.settings.json")) {
    return "nitro.settings.json";
  }
  const char *home = getenv("HOME");
  std::string base = home ? std::string(home) : ".";
  return base + "/.config/nitro/settings.json";
}

// Returns the history file path: ~/.config/nitro/history.txt
static std::string history_path() {
  const char *home = getenv("HOME");
  std::string base = home ? std::string(home) : ".";
  return base + "/.config/nitro/history.txt";
}

//
// A minimal hand-rolled JSON reader/writer for the flat key-value settings
// we care about.  We deliberately avoid a full JSON library dependency.
//
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
  if (pos == std::string::npos) {
    return false;
  }
  pos += search.size();
  while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) {
    ++pos;
  }
  if (pos >= json.size()) {
    return false;
  }
  size_t start = pos;
  if (json[pos] == '-') {
    ++pos;
  }
  while (pos < json.size() && (std::isdigit((unsigned char)json[pos]) || json[pos] == '.')) {
    ++pos;
  }
  if (pos == start) {
    return false;
  }
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

  cfg.thinking = true;
  cfg.agent_id = AgentSessionId::uniqueId();

  // String fields
  settings_get_str(json, "model_path",  cfg.model_path);
  settings_get_str(json, "embed_path",  cfg.embed_path);
  settings_get_str(json, "sandbox",     cfg.sandbox);

  // Integer fields
  settings_get_int(json, "n_ctx",          cfg.n_ctx);
  settings_get_int(json, "n_batch",        cfg.n_batch);
  settings_get_int(json, "n_gpu_layers",   cfg.n_gpu_layers);
  settings_get_int(json, "top_k",          cfg.top_k);
  settings_get_int(json, "penalty_last_n", cfg.penalty_last_n);
  settings_get_int(json, "rag_top_k",      cfg.rag_top_k);

  // Float fields
  settings_get_float(json, "temperature",    cfg.temperature);
  settings_get_float(json, "top_p",          cfg.top_p);
  settings_get_float(json, "min_p",          cfg.min_p);
  settings_get_float(json, "penalty_repeat", cfg.penalty_repeat);
}

//
// icons
//
static constexpr std::string ICON_ERR   = " ⚡ ▏";
static constexpr std::string ICON_THINK = " 🤔 ▏";
static constexpr std::string ICON_TOOL  = " 🔧 ▏";
static constexpr std::string ICON_SYS   = " ✨ ▏";

static std::string introspect(const NitroConfig &cfg) {
  static constexpr std::string_view tmpl =
    "{{\n"
    "  \"model_path\":     \"{}\",\n"
    "  \"embed_path\":     \"{}\",\n"
    "  \"sandbox\":        \"{}\",\n"
    "  \"n_ctx\":          {},\n"
    "  \"n_batch\":        {},\n"
    "  \"n_gpu_layers\":   {},\n"
    "  \"temperature\":    {},\n"
    "  \"top_p\":          {},\n"
    "  \"min_p\":          {},\n"
    "  \"top_k\":          {},\n"
    "  \"penalty_repeat\": {},\n"
    "  \"penalty_last_n\": {},\n"
    "  \"rag_top_k\":      {}\n"
    "}}\n";
  return std::format(tmpl,
                     cfg.model_path,
                     cfg.embed_path,
                     cfg.sandbox,
                     cfg.n_ctx,
                     cfg.n_batch,
                     cfg.n_gpu_layers,
                     cfg.temperature,
                     cfg.top_p,
                     cfg.min_p,
                     cfg.top_k,
                     cfg.penalty_repeat,
                     cfg.penalty_last_n,
                     cfg.rag_top_k);
}

// Persist the current cfg to ~/.config/nitro/settings.json.
static bool save_settings(const NitroConfig &cfg) {
  std::string path = settings_path();
  fs::path dir = fs::path(path).parent_path();
  std::error_code ec;
  fs::create_directories(dir, ec);

  std::ofstream f(path, std::ios::trunc);
  if (!f) {
    return false;
  }

  f << introspect(cfg);

  return f.good();
}

//
// Trims whitespace from both ends of a string
//
static std::string trim(std::string_view str) {
  constexpr std::string_view whitespace = " \t\n\r\f\v";

  // Find the first non-whitespace character
  const auto start = str.find_first_not_of(whitespace);
  if (start == std::string_view::npos) {
    return ""; // The string is entirely whitespace
  }

  // Find the last non-whitespace character
  const auto end = str.find_last_not_of(whitespace);

  // Return the substring between start and end
  return std::string(str.substr(start, end - start + 1));
}

/*
 * unwrap() - Remove a matching outer "wrapper" from a string.
 *
 * Trims leading/trailing whitespace first, then checks (in order):
 *
 *  1. Same-character pairs   "..."  '...'  |...|  `...`
 *  2. Mirror pairs           (...)  [...]  {...}
 *  3. HTML-like tags         <tag>...</tag>
 *  4. Plain angle brackets   <...>          (fallback if tags don't match)
 *
 * If none of the above apply, returns the whitespace-trimmed input unchanged.
 *
 * Examples:
 *   unwrap("\"hello\"")        -> "hello"
 *   unwrap("  [foo]  ")        -> "foo"
 *   unwrap("<b>bold</b>")      -> "bold"
 *   unwrap("<file>x</file>")   -> "x"
 *   unwrap("<hello>")          -> "hello"
 *   unwrap("plain")            -> "plain"
 *   unwrap("")                 -> ""
 */
std::string unwrap(const std::string &input) {
  if (input.empty()) {
    return input;
  }

  size_t left = 0;
  size_t right = input.length() - 1;

  while (left <= right && std::isspace(static_cast<unsigned char>(input[left]))) {
    left++;
  }
  while (left <= right && std::isspace(static_cast<unsigned char>(input[right]))) {
    right--;
  }

  if (left > right) {
    return "";
  }

  // Same-character pairs: "", '', ||, ``
  // Note: [], {} are NOT same-char pairs — they belong in mirror pairs only
  if (input[left] == input[right]) {
    if (input[left] == '"'  || input[left] == '\'' ||
        input[left] == '|'  || input[left] == '`') {
      return input.substr(left + 1, right - left - 1);
    }
  }

  // Mirror pairs: (), [], {}, but NOT <> (handled below as possible HTML tags)
  if (input[left] != input[right]) {
    if ((input[left] == '(' && input[right] == ')') ||
        (input[left] == '[' && input[right] == ']') ||
        (input[left] == '{' && input[right] == '}')) {
      return input.substr(left + 1, right - left - 1);
    }
  }

  // HTML-like tags: <tag>content</tag>
  // Also handles plain <...> as a fallback at the end
  if (input[left] == '<' && input[right] == '>') {
    // Find end of opening tag
    size_t openTagEnd = left + 1;
    while (openTagEnd <= right && input[openTagEnd] != '>') openTagEnd++;

    if (openTagEnd < right) {
      std::string openTagName = input.substr(left + 1, openTagEnd - left - 1);

      // Find start of closing tag (search backwards for '<')
      size_t closeTagStart = right;
      while (closeTagStart > openTagEnd && input[closeTagStart] != '<') closeTagStart--;

      if (closeTagStart > openTagEnd && input[closeTagStart + 1] == '/') {
        std::string closeTagName = input.substr(closeTagStart + 2, right - closeTagStart - 2);

        if (!openTagName.empty() && openTagName == closeTagName) {
          // Return content between the tags
          return input.substr(openTagEnd + 1, closeTagStart - openTagEnd - 1);
        }
      }
    }

    // Fallback: plain <...> with no matching HTML tags — unwrap the angle brackets
    return input.substr(left + 1, right - left - 1);
  }

  return input.substr(left, right - left + 1);
}

// ─── colour helpers ──────────────────────────────────────────────────────
static constexpr uint32_t BG_CHAT_R = 18,  BG_CHAT_G = 22,  BG_CHAT_B = 30;
static constexpr uint32_t BG_INP_R  = 22,  BG_INP_G  = 28,  BG_INP_B  = 38;
static constexpr uint32_t BG_HDR_R  = 30,  BG_HDR_G  = 40,  BG_HDR_B  = 55;

static inline uint64_t chat_ch(uint32_t r, uint32_t g, uint32_t b) {
  return NCCHANNELS_INITIALIZER(r, g, b, BG_CHAT_R, BG_CHAT_G, BG_CHAT_B);
}

static inline uint64_t inp_ch(uint32_t r, uint32_t g, uint32_t b) {
  return NCCHANNELS_INITIALIZER(r, g, b, BG_INP_R, BG_INP_G, BG_INP_B);
}

static inline uint64_t hdr_ch(uint32_t r, uint32_t g, uint32_t b) {
  return NCCHANNELS_INITIALIZER(r, g, b, BG_HDR_R, BG_HDR_G, BG_HDR_B);
}

//
// File-system helpers
//
static std::string join_path(const std::string &a, const std::string &b) {
  if (b.empty()) return a;
  if (b[0] == '/') return b;
  std::string pa = a;
  if (!pa.empty() && pa.back() == '/') pa.pop_back();
  std::string pb = (b.front() == '/') ? b.substr(1) : b;
  return pa + "/" + pb;
}

static std::string read_file(const std::string &path) {
  std::ifstream f(path, std::ios::binary);
  if (!f) {
    return "ERROR: cannot open [" + path + "]";
  }
  std::ostringstream oss; oss << f.rdbuf();
  return oss.str();
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

static bool path_in_sandbox(const std::string &sandbox, const std::string &path) {
  std::error_code ec;
  auto base   = fs::canonical(sandbox, ec);  if (ec) return false;
  auto target = fs::weakly_canonical(path, ec);
  std::string bstr = base.string() + "/";
  std::string tstr = target.string();
  return tstr == base.string() || tstr.compare(0, bstr.size(), bstr) == 0;
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

static bool make_dir(const std::string &path) {
  try {
    std::filesystem::path p(path);
    if (fs::exists(p)) {
      return true;
    }
    std::error_code ec;
    return fs::create_directories(p, ec);
  }
  catch (const std::filesystem::filesystem_error &e) {
    log_write("mkdir failed [%s]", e.what());
    return false;
  }
}

//
// System prompt
//
static std::string build_system_prompt(NitroConfig &cfg) {
  std::string p;
  p +=
    "You are Nitro, an agentic AI assistant for software development. "
    "Proceed with caution, guided by logic and the pursuit of knowledge.\n\n"

    "Your sandbox (project directory) is: " + cfg.sandbox + "\n\n"

    "## Core Principle\n"
    "Always follow this loop: THINK → DECIDE → ACT → RESPOND\n\n"

    "## Reasoning Protocol\n"
    "Use <|think|> to reason BEFORE acting. Keep it concise and structured.\n"
    "Format:\n"
    "<|think|>\n"
    "- What is the user asking?\n"
    "- Do I need external data (files, tools)?\n"
    "- What is the safest and most correct action?\n"
    "</|think|>\n\n"
    "Rules:\n"
    "- Do NOT call tools inside <|think|>\n"
    "- Do NOT include the final answer inside <|think|>\n"
    "- Always follow <|think|> with either a tool call OR a final answer\n"
    "- Skip <|think|> only for trivial or conversational responses\n\n"

    "## Tool Protocol\n"
    "Emit ONE tool call at a time, immediately followed by NITRO_END_TOOL.\n"
    "Do NOT add any commentary, explanation, or text between the tool call and NITRO_END_TOOL.\n"
    "The host executes the tool and returns NITRO_TOOL_RESULT: <value>.\n"
    "Wait for the result before continuing.\n"
    "After receiving NITRO_TOOL_RESULT you may explain what you did.\n\n"
    "Examples:\n\n"
    "TOOL:LIST\n"
    "NITRO_END_TOOL\n\n"
    "TOOL:READ readme.txt\n"
    "NITRO_END_TOOL\n\n"
    "TOOL:WRITE index.html <!DOCTYPE html><html>...</html>\n"
    "NITRO_END_TOOL\n\n"
    "TOOL:RUN ./build.sh\n"
    "NITRO_END_TOOL\n\n"

    "## Available Tools\n"
    "  TOOL:LIST   [dir]          list files (default: sandbox root)\n"
    "  TOOL:READ   <file>         read file contents\n"
    "  TOOL:WRITE  <file> <text>  write text to file\n"
    "  TOOL:MKDIR  <dir>          create a subfolder inside the sandbox\n"
    "  TOOL:EXISTS <file>         YES or NO\n"
    "  TOOL:RUN    <prog> [args]  run program inside sandbox\n"
    "  TOOL:DATE                  current date\n"
    "  TOOL:TIME                  current time\n"
    "  TOOL:RND                   random float 0..1\n"
    "  TOOL:RAG    <query>        query the RAG index for additional context\n"
    "  TOOL:ASK    <query>        ask the user for clarification or additional context\n"
    "  TOOL:INTROSPECT            show current model settings\n"
    "  TOOL:CURL   <url>          HTTP GET, returns response body (max 32 KB)\n"
    "  TOOL:PERMISSION            ask user for explicit permission\n\n"

    "## Tool Decision Rules\n"
    "Use tools ONLY if:\n"
    "- The user explicitly references files or the project, OR\n"
    "- The answer depends on local or project data, OR\n"
    "- The user asks for date, time, or a random number\n"
    "Otherwise answer directly using internal knowledge.\n\n"

    "## Tool Rules\n"
    "- NITRO_END_TOOL must immediately follow the tool call — no exceptions\n"
    "- Never add commentary before NITRO_END_TOOL\n"
    "- Only use one tool at a time, step by step\n"
    "- Never access files outside the sandbox\n"
    "- Use TOOL:PERMISSION before destructive or irreversible operations\n"
    "- Do NOT hallucinate file contents\n"
    "- Do NOT fabricate tool outputs\n"
    "- Do NOT assume files exist — use TOOL:EXISTS to check first\n\n"

    "## File Writing Rules\n"
    "Use TOOL:WRITE only if explicitly requested.\n"
    "- Write complete and valid content\n"
    "- Do not overwrite without clear intent\n"
    "- Use TOOL:PERMISSION before overwriting an existing file\n"
    "- Format: TOOL:WRITE <filename> <complete file content>\n\n"

    "## Interaction Guidelines\n"
    "- Be precise and efficient\n"
    "- Ask clarifying questions if the request is ambiguous or missing parameters\n"
    "- Prefer direct answers when no tools are needed\n"
    "- After each tool result, explain in plain English what was done\n"
    "- If no user request is provided, respond with a brief readiness message\n\n";

  for (const auto &kf : cfg.knowledge_files) {
    std::ifstream f(kf);
    if (!f) continue;
    std::ostringstream oss; oss << f.rdbuf();
    p += "## Knowledge: " + kf + "\n" + oss.str() + "\n\n";
  }
  return p;
}

static std::string strip_code_fences(const std::string &filename,
                                     const std::string &src) {
  auto ext = fs::path(filename).extension().string();
  bool is_code = ranges::any_of(CODE_EXTENSIONS, [&](const std::string &e){ return ext == e; });
  if (!is_code) {
    return unwrap(src);
  }
  auto pos = src.find("```");
  if (pos == std::string::npos) {
    return src;
  }
  auto nl = src.find('\n', pos + 3);
  if (nl == std::string::npos) {
    return src;
  }
  std::string inner = src.substr(nl + 1);
  auto end = inner.rfind("```");
  if (end != std::string::npos) {
    inner = inner.substr(0, end);
  }
  return inner;
}

//
// TOOL:CURL
//
static size_t curl_write_cb(void *contents, size_t size, size_t nmemb, void *userp) {
  auto *buf = static_cast<std::string *>(userp);
  auto total = size * nmemb;
  static constexpr size_t MAX_BODY = 32 * 1024;
  if (buf->size() < MAX_BODY) {
    size_t room = MAX_BODY - buf->size();
    buf->append(static_cast<char *>(contents), std::min(total, room));
  }
  return total;
}

//
// html_to_text — strip HTML for cleaner TOOL:CURL context
//
// Lightweight HTML→plain-text conversion:
//   • Drops <head>, <script>, <style> blocks entirely.
//   • Inserts newlines at block-level tags (p, div, br, li, h1-h6 …).
//   • Strips all remaining tags.
//   • Decodes common named & numeric HTML entities.
//   • Collapses whitespace runs; caps consecutive blank lines at 2.
//
static std::string html_to_text(const std::string &html) {
  std::string s = html;

  // 1. Remove <head>…</head>
  {
    std::string lo = s;
    ranges::transform(lo, lo.begin(), ::tolower);
    auto p0 = lo.find("<head");
    auto p1 = lo.find("</head>");
    if (p0 != std::string::npos && p1 != std::string::npos)
      s.erase(p0, p1 + 7 - p0);
  }

  // 2. Remove <script>…</script> and <style>…</style>
  for (const std::string &tag : {"script", "style"}) {
    std::string open  = "<" + tag;
    std::string close = "</" + tag + ">";
    std::string lo = s;
    ranges::transform(lo, lo.begin(), ::tolower);
    for (;;) {
      auto p0 = lo.find(open);
      if (p0 == std::string::npos) break;
      auto p1 = lo.find(close, p0);
      if (p1 == std::string::npos) { s.erase(p0); lo.erase(p0); break; }
      s.erase(p0, p1 + close.size() - p0);
      lo.erase(p0, p1 + close.size() - p0);
    }
  }

  // 3. Replace block-level tags with '\n' before stripping all tags.
  static const char *const BLOCK[] = {
    "p","div","br","li","tr","h1","h2","h3","h4","h5","h6",
    "article","section","header","footer","nav","main", nullptr
  };
  {
    std::string out;
    out.reserve(s.size());
    size_t i = 0;
    while (i < s.size()) {
      if (s[i] != '<') { out += s[i++]; continue; }
      auto ce = s.find('>', i);
      if (ce == std::string::npos) { out += s[i++]; continue; }
      std::string inner = s.substr(i + 1, ce - i - 1);
      size_t sp = inner.find_first_of(" \t/\r\n");
      std::string name = (sp != std::string::npos) ? inner.substr(0, sp) : inner;
      ranges::transform(name, name.begin(), ::tolower);
      for (int k = 0; BLOCK[k]; ++k) {
        if (name == BLOCK[k]) {
          out += '\n'; break;
        }
      }
      i = ce + 1;
    }
    s = out;
  }

  // 4. Strip all remaining tags.
  {
    std::string out; out.reserve(s.size());
    bool in_tag = false;
    for (char c : s) {
      if (c == '<')  { in_tag = true;  continue; }
      if (c == '>')  { in_tag = false; continue; }
      if (!in_tag)     out += c;
    }
    s = out;
  }

  // 5. Decode common HTML entities.
  static const std::pair<const char*, const char*> ENT[] = {
    {"&amp;","&"},{"&lt;","<"},{"&gt;",">"},{"&quot;","\""},
    {"&apos;","'"},{"&nbsp;"," "},{"&mdash;","—"},{"&ndash;","–"},
    {"&hellip;","…"},{"&#39;","'"},{"&#34;","\""},
    {nullptr,nullptr}
  };
  for (int k = 0; ENT[k].first; ++k) {
    std::string e = ENT[k].first, r = ENT[k].second;
    size_t pos = 0;
    while ((pos = s.find(e, pos)) != std::string::npos)
      { s.replace(pos, e.size(), r); pos += r.size(); }
  }
  // Numeric entities &#NNN; and &#xHHH;
  {
    std::string out; out.reserve(s.size());
    size_t i = 0;
    while (i < s.size()) {
      if (s[i]=='&' && i+2<s.size() && s[i+1]=='#') {
        size_t semi = s.find(';', i+2);
        if (semi != std::string::npos && semi-i < 10) {
          std::string num = s.substr(i+2, semi-i-2);
          try {
            uint32_t cp = (num[0]=='x'||num[0]=='X')
              ? (uint32_t)std::stoul(num.substr(1),nullptr,16)
              : (uint32_t)std::stoul(num);
            if      (cp < 0x80)  { out += (char)cp; }
            else if (cp < 0x800) { out += (char)(0xC0|(cp>>6)); out += (char)(0x80|(cp&0x3F)); }
            else                 { out += (char)(0xE0|(cp>>12)); out += (char)(0x80|((cp>>6)&0x3F)); out += (char)(0x80|(cp&0x3F)); }
            i = semi+1; continue;
          } catch (...) {}
        }
      }
      out += s[i++];
    }
    s = out;
  }

  // 6. Collapse whitespace; cap blank lines at 2.
  {
    std::string out; out.reserve(s.size());
    int nl_run = 0; bool last_sp = false;
    for (char c : s) {
      if (c == '\r') continue;
      if (c == '\t') c = ' ';
      if (c == '\n') { ++nl_run; last_sp=false; if (nl_run<=2) out+='\n'; continue; }
      nl_run = 0;
      if (c == ' ') { if (!last_sp) { out+=' '; last_sp=true; } continue; }
      last_sp = false; out += c;
    }
    size_t f = out.find_first_not_of(" \n");
    size_t l = out.find_last_not_of(" \n");
    s = (f == std::string::npos) ? "" : out.substr(f, l-f+1);
  }
  return s;
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
  curl_easy_setopt(curl, CURLOPT_TIMEOUT,        15L);
  curl_easy_setopt(curl, CURLOPT_USERAGENT,      "nitro/1.0");
  // Accept compressed responses; curl will decompress automatically.
  curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");

  CURLcode res = curl_easy_perform(curl);
  long http_code = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

  // Query content-type before cleanup (pointer is only valid while handle lives).
  char *ct_raw = nullptr;
  curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct_raw);
  std::string content_type = ct_raw ? ct_raw : "";
  ranges::transform(content_type,
                    content_type.begin(), ::tolower);
  curl_easy_cleanup(curl);
  if (res != CURLE_OK) {
    return std::string("ERROR: curl: ") + curl_easy_strerror(res);
  }
  if (http_code >= 400) {
    return "ERROR: HTTP " + std::to_string(http_code) + " from " + url;
  }
  if (body.empty()) {
    return "(empty response)";
  }

  // Strip HTML tags so the model receives clean plain text.
  bool is_html = (content_type.find("text/html") != std::string::npos)
    || (body.size() > 5 && body.substr(0,5) == "<!DOC")
    || (body.size() > 6 && body.substr(0,6) == "<html>");
  if (is_html) {
    body = html_to_text(body);
  }

  return body;
}

//
// TuiState::init
//
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
  notcurses_mice_enable(nc, NCMICE_BUTTON_EVENT);
  redraw_all();
}

void TuiState::destroy() {
  if (nc) {
    notcurses_stop(nc);
    nc = nullptr;
  }
}

void TuiState::resize() {
  notcurses_term_dim_yx(nc, (unsigned *)&term_rows, (unsigned *)&term_cols);
  ncplane_resize_simple(header, 1, (unsigned)term_cols);
  int cr = std::max(1, term_rows - 3);
  ncplane_resize_simple(chatpl, (unsigned)cr, (unsigned)term_cols);
  ncplane_move_yx(inputpl, term_rows - 2, 0);
  ncplane_resize_simple(inputpl, 2, (unsigned)term_cols);
  redraw_all();
}

//
// TuiState::redraw
//
void TuiState::redraw_header() const {
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

  // Pre-compute wrapped lines so we know total visual rows.
  struct VisualLine {
    std::string text;
    uint64_t    ch;
  };
  std::vector<VisualLine> visual;
  visual.reserve(chat_lines.size());

  for (const std::string &line : chat_lines) {
    uint64_t ch;
    if (line.rfind("[logo_", 0) == 0 && line.size() > 7 && line[7] == ']') {
      int logo_row = line[6] - '0';
      static const uint32_t GRAD_R[] = {  0,  20,  60, 120, 180, 210, 220 };
      static const uint32_t GRAD_G[] = { 230, 255, 255, 255, 200, 130,  80 };
      static const uint32_t GRAD_B[] = { 255, 200, 140,  80, 100, 200, 255 };
      int gi = std::max(0, std::min(logo_row, 6));
      ch = chat_ch(GRAD_R[gi], GRAD_G[gi], GRAD_B[gi]);
    }
    else if (line.rfind("You: ",    0) == 0) ch = chat_ch(100, 200, 255);
    else if (line.rfind("Nitro: ",  0) == 0) ch = chat_ch(180, 255, 180);
    else if (line.rfind(ICON_SYS,   0) == 0) ch = chat_ch(160,  82,  45);
    else if (line.rfind(ICON_TOOL,  0) == 0) ch = chat_ch(255, 180,  80);
    else if (line.rfind(ICON_ERR,   0) == 0) ch = chat_ch(255,  80,  80);
    else if (line.rfind(ICON_THINK, 0) == 0) ch = chat_ch(140, 140, 200);
    else                                     ch = chat_ch(210, 210, 210);

    std::string display = (line.rfind("[logo_", 0) == 0 && line.size() > 8)
      ? line.substr(8) : line;

    // Split into cols-wide chunks, each carrying the same channel.
    if (display.empty()) {
      visual.push_back({"", ch});
    } else {
      for (size_t off = 0; off < display.size(); off += cols) {
        visual.push_back({display.substr(off, cols), ch});
      }
    }
  }

  int total   = static_cast<int>(visual.size());
  int visible = static_cast<int>(rows);
  int start   = std::max(0, total - visible - scroll_offset);
  int end     = std::min(total, start + visible);

  for (int i = start, row = 0; i < end; ++i, ++row) {
    ncplane_set_channels(chatpl, visual[i].ch);
    ncplane_putstr_yx(chatpl, row, 0, visual[i].text.c_str());
  }
}

void TuiState::redraw_input() const {
  ncplane_erase(inputpl);

  if (thinking) {
    static constexpr const char *BLOCKS[] = { "-", "~", "≈", "~", "-" };
    static constexpr int    N_BLOCKS = 5;
    static constexpr double FREQ     = 0.25;  // gentler wave
    static constexpr double SPEED    = 0.15;  // slower scroll
    static constexpr int    DELAY    = 12;    // frames before animation starts

    if (spinner_frame < DELAY) {
      // still just a plain separator during the pause
      ncplane_set_channels(inputpl, inp_ch(80, 120, 160));
      std::string sep(term_cols, '-');
      ncplane_putstr_yx(inputpl, 0, 0, sep.c_str());
    } else {
      int frame = spinner_frame - DELAY;  // animation frame relative to start
      for (int col = 0; col < term_cols; ++col) {
        double phase = (col * FREQ) - (frame * SPEED);
        int idx = static_cast<int>(((std::sin(phase) + 1.0) * 0.5 * (N_BLOCKS - 1)));
        idx = std::max(0, std::min(idx, N_BLOCKS - 1));
        // subtle brightness shift — blue-grey, not full glow
        int brightness = 80 + idx * 20;
        ncplane_set_channels(inputpl, NCCHANNELS_INITIALIZER(brightness, brightness + 20, brightness + 40,
                                                             BG_INP_R, BG_INP_G, BG_INP_B));
        ncplane_putstr_yx(inputpl, 0, col, BLOCKS[idx]);
      }
    }
    ncplane_set_channels(inputpl, inp_ch(140, 140, 180));
    ncplane_putstr_yx(inputpl, 1, 2, "thinking…");
  } else {
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
    if (visible.size() > max_w && max_w > 0) {
      view_offset = static_cast<int>(visible.size() - max_w);
      visible = visible.substr(view_offset);
    }
    int cur_in_view = std::max(0, static_cast<int>(cursor_pos - view_offset));
    cur_in_view = std::min(cur_in_view, (int)visible.size());
    std::string before = visible.substr(0, cur_in_view);
    std::string after  = cur_in_view < (int)visible.size()
      ? visible.substr(cur_in_view + 1) : "";
    char cursor_ch_val = cur_in_view < (int)visible.size()
      ? visible[cur_in_view] : ' ';
    ncplane_set_channels(inputpl, inp_ch(230, 230, 230));
    ncplane_putstr_yx(inputpl, 1, prompt_cols, before.c_str());
    int cx = prompt_cols + cur_in_view;
    ncplane_set_channels(inputpl, NCCHANNELS_INITIALIZER(BG_INP_R, BG_INP_G, BG_INP_B, 180, 230, 255));
    char cbuf[2] = { cursor_ch_val, '\0' };
    ncplane_putstr_yx(inputpl, 1, cx, cbuf);
    ncplane_set_channels(inputpl, inp_ch(230, 230, 230));
    if (!after.empty()) {
      ncplane_putstr_yx(inputpl, 1, cx + 1, after.c_str());
    }
  }
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
  redraw_input();
  notcurses_render(nc);
}

void TuiState::set_thinking(bool on) {
  thinking = on;
  if (!on) spinner_frame = 0;
  redraw_header();
  redraw_input();
  notcurses_render(nc);
}

//
// TuiState content helpers
//
void TuiState::append_line(const std::string &line) {
  std::lock_guard<std::mutex> lk(lines_mutex);
  int w = std::max(1, term_cols - 1);
  if ((int)line.size() <= w) {
    chat_lines.push_back(line);
  } else {
    for (int off = 0; off < (int)line.size(); off += w) {
      chat_lines.push_back(line.substr(off, w));
    }
  }
}

void TuiState::append_token(const std::string &token) {
  token_acc += token;
  for (;;) {
    auto pos = token_acc.find('\n');
    if (pos == std::string::npos) {
      break;
    }
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

//
// Creates a centred floating plane with a border and a status message.
// The popup sits above all other planes and blocks until explicitly dismissed.
//
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

void TuiState::show_help() {
  append_line(ICON_SYS + "Commands:");
  append_line(ICON_SYS + "  /model  [path]           load a GGUF model (picker if no path)");
  append_line(ICON_SYS + "  /embed  [path]           load an embedding model (picker if no path)");
  append_line(ICON_SYS + "  /rag    [path]           index file or directory (picker if no path)");
  append_line(ICON_SYS + "  /memory                  KV / VRAM / layer stats");
  append_line(ICON_SYS + "  /clear                   reset conversation");
  append_line(ICON_SYS + "  /settings                show current settings");
  append_line(ICON_SYS + "  /set    <key> <value>    change a setting live");
  append_line(ICON_SYS + "  /help                    this message");
  append_line(ICON_SYS + "  exit / quit              exit Nitro");
  append_line(ICON_SYS + "Settable keys (via /set):");
  append_line(ICON_SYS + "  temperature  top_p  top_k  min_p  penalty_repeat");
  append_line(ICON_SYS + "  penalty_last_n  rag_top_k  n_gpu_layers");
  append_line(ICON_SYS + "  run_allowed  (comma-separated list, e.g. python3,make)");
  redraw_all();
}

void TuiState::dismiss_modal_popup() {
  if (modal_plane) {
    ncplane_destroy(modal_plane);
    modal_plane = nullptr;
    notcurses_render(nc);
  }
}

//
// ─── TuiState::file_picker ────────────────────────────────────────────────
// Interactive directory/file browser popup.
// Keyboard:  ↑/↓ navigate,  Enter select/descend,  Backspace go up,
//            's' select current dir for indexing,   Esc cancel.
// Returns the chosen path or "" on cancel.
// ─── TuiState::file_picker ────────────────────────────────────────────────
// Unified interactive directory/file browser used by /rag, /model, /embed.
// title_hint appears in the popup header (e.g. "RAG Folder", "Model File").
//
// Keyboard:
//   ↑/↓        navigate list
//   Enter      descend into directory, or select a file
//   Backspace  go up one directory
//   s          select the current directory itself (useful for /rag)
//   Esc        cancel → returns ""
//
// Returns the chosen path, or "" on cancel.
//
std::string TuiState::file_picker(const std::string &start_dir,
                                  const std::string &title_hint) const {
  std::string current_dir = start_dir;
  {
    std::error_code ec;
    auto canon = fs::canonical(start_dir, ec);
    if (!ec) current_dir = canon.string();
  }
  auto load_entries = [](const std::string &dir,
                         std::vector<std::string> &entries)
  {
    entries.clear();
    std::error_code ec;
    if (fs::path(dir).has_parent_path() &&
        fs::path(dir) != fs::path(dir).root_path())
      entries.emplace_back("..");
    std::vector<std::string> dirs, files;
    for (const auto &e : fs::directory_iterator(dir, ec)) {
      if (ec) break;
      std::string name = e.path().filename().string();
      if (name.empty() || name[0] == '.') continue;
      if (e.is_directory()) dirs.push_back(name);
      else                  files.push_back(name);
    }
    ranges::sort(dirs);
    ranges::sort(files);
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
  ncplane_set_base(picker, " ", 0, NCCHANNELS_INITIALIZER(PBG_R, PBG_G, PBG_B, PBG_R, PBG_G, PBG_B));
  // Build a compact hint line appropriate to the operation.
  // /rag adds 's=select dir'; /model and /embed only need file selection.
  std::string hint_line = "↑↓ navigate  Enter open/select  Esc cancel";
  if (title_hint.find("RAG") != std::string::npos ||
      title_hint.find("Folder") != std::string::npos) {
    hint_line = "↑↓ navigate  Enter open  s=select dir  Esc cancel";
  }
  auto draw_picker = [&]() {
    ncplane_erase(picker);
    uint64_t border_ch = NCCHANNELS_INITIALIZER(100, 180, 255, PBG_R, PBG_G, PBG_B);
    ncplane_set_channels(picker, border_ch);
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
    ncplane_set_channels(picker, NCCHANNELS_INITIALIZER(255, 220, 80, PBG_R, PBG_G, PBG_B));
    std::string title_str = " 📂 " + title_hint + " Picker ";
    if ((int)title_str.size() > PW - 4) title_str = title_str.substr(0, PW - 4);
    ncplane_putstr_yx(picker, 0, 2, title_str.c_str());
    // Current path (truncated).
    std::string path_display = current_dir;
    if ((int)path_display.size() > PW - 4)
      path_display = "…" + path_display.substr(path_display.size() - (PW - 5));
    ncplane_set_channels(picker, NCCHANNELS_INITIALIZER(160, 200, 240, PBG_R, PBG_G, PBG_B));
    ncplane_putstr_yx(picker, 1, 2, path_display.c_str());
    // Hint line (bottom interior row).
    ncplane_set_channels(picker, NCCHANNELS_INITIALIZER(120, 120, 160, PBG_R, PBG_G, PBG_B));
    std::string hint_trunc = hint_line;
    if ((int)hint_trunc.size() > PW - 4) hint_trunc = hint_trunc.substr(0, PW - 4);
    ncplane_putstr_yx(picker, PH - 2, 2, hint_trunc.c_str());
    // Entry list.
    int list_rows = PH - 5;
    if (selected < scroll) scroll = selected;
    if (selected >= scroll + list_rows) scroll = selected - list_rows + 1;
    for (int i = 0; i < list_rows; ++i) {
      int idx = scroll + i;
      if (idx >= (int)entries.size()) break;
      bool is_selected = (idx == selected);
      bool is_dir = !entries[idx].empty() && entries[idx].back() == '/';
      uint32_t fr, fg, fb;
      if (is_selected)  { fr = 20;  fg = 20;  fb = 20;  }
      else if (is_dir)   { fr = 120; fg = 200; fb = 255; }
      else               { fr = 200; fg = 200; fb = 200; }
      uint32_t br = is_selected ? 100 : PBG_R;
      uint32_t bg = is_selected ? 180 : PBG_G;
      uint32_t bb = is_selected ? 255 : PBG_B;
      ncplane_set_channels(picker, NCCHANNELS_INITIALIZER(fr, fg, fb, br, bg, bb));
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
    // 's' — select the current directory (useful for /rag, ignored for file pickers).
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
        // Select the highlighted file.
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

//
// ─── TuiState::confirm_dialog ─────────────────────────────────────────────
//
bool TuiState::confirm_dialog(const std::string &prompt) const {
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
  ranges::transform(lo, lo.begin(), ::tolower);
  redraw_input();
  notcurses_render(nc);
  return (lo == "y" || lo == "yes" || lo == "sure" || lo == "k");
}

//
// Integrates InputHistory:  Up/Down arrows navigate the history stack.
// On submit the entry is pushed to history, and nav is reset.
//
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
        if (!input_buf.empty() && hist_entry != input_buf) {
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
      if (history.down(hist_entry)) {
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
    if (ni.id == NCKEY_SCROLL_UP && scroll_offset < term_rows + 10) {
      scroll_offset += 1;
      redraw_chat();
      notcurses_render(nc);
      continue;
    }
    if (ni.id == NCKEY_SCROLL_DOWN && scroll_offset > 0) {
      scroll_offset -= 1;
      redraw_chat();
      notcurses_render(nc);
      continue;
    }
    if (ni.id == NCKEY_F01) {
      show_help();
      continue;
    }
    if (ni.id == NCKEY_F02) {
      mouse_mode = !mouse_mode;
      if (mouse_mode) {
        notcurses_mice_enable(nc, NCMICE_BUTTON_EVENT);
      } else {
        notcurses_mice_disable(nc);
      }
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

void AgentState::apply_generation_params(const NitroConfig &cfg) const {
  llama->add_stop("<|turn|>");
  llama->add_stop("<|im_end|>");
  llama->set_max_tokens(512000);
  llama->set_temperature(cfg.temperature);
  llama->set_top_k(cfg.top_k);
  llama->set_top_p(cfg.top_p);
  llama->set_min_p(cfg.min_p);
  llama->set_penalty_repeat(cfg.penalty_repeat);
  llama->set_penalty_last_n(cfg.penalty_last_n);
  llama->set_log_level(cfg.log_level);
}

//
// Shows a modal loading popup while the model loads.
//
bool AgentState::setup_model(const NitroConfig &cfg, TuiState &tui) {
  if (cfg.model_path.empty()) {
    tui.append_line(ICON_SYS + "No model loaded.  Use /model <path> to load a GGUF.");
    tui.redraw_all();
    return false;
  }
  // Show a modal popup so the user knows loading is in progress.
  std::string model_name = fs::path(cfg.model_path).filename().string();
  tui.show_modal_popup("Loading " + model_name);
  // Destroy the iterator first — it holds references into the llama context.
  // Freeing llama while iter is still alive causes use-after-free / load failure.
  iter.reset();
  model_loaded = false;
  llama = std::make_unique<Llama>();

  apply_generation_params(cfg);
  if (!llama->load_model(cfg.model_path, cfg.n_ctx, cfg.n_batch,
                         cfg.n_gpu_layers, cfg.log_level)) {
    tui.dismiss_modal_popup();
    tui.append_line(ICON_ERR + llama->last_error());
    tui.redraw_all();
    return false;
  }
  tui.dismiss_modal_popup();
  model_loaded = true;
  tui.current_model = model_name;
  tui.append_line(ICON_SYS + "Model ready: " + tui.current_model);
  LlamaMemoryInfo mem = llama->memory_info();
  tui.append_line(ICON_SYS + "" + mem.advice);
  tui.kv_used  = mem.kv_used;
  tui.kv_total = mem.kv_total;
  tui.vram_used  = mem.vram_used;
  tui.vram_total = mem.vram_total;
  tui.append_line(ICON_SYS + "Thinking mode: " + (cfg.thinking ? "enabled" : "disabled"));
  tui.redraw_all();
  return true;
}

bool AgentState::setup_embed(const std::string &path, TuiState &tui) {
  tui.show_modal_popup("Loading embedding model: " + fs::path(path).filename().string());
  tui.redraw_all();
  embed_llama = std::make_unique<Llama>();
  if (!embed_llama->load_embedding_model(path)) {
    tui.dismiss_modal_popup();
    tui.append_line(ICON_ERR + embed_llama->last_error());
    tui.redraw_all();
    embed_llama.reset();
    return false;
  }
  tui.dismiss_modal_popup();
  rag_db      = std::make_unique<RagDB>();
  rag_session = std::make_unique<RagSession>();
  tui.append_line(ICON_SYS + "Embedding model ready.");
  tui.redraw_all();
  return true;
}

void AgentState::reset_conversation(const std::string &sysprompt, TuiState &tui) {
  system_prompt = sysprompt;
  llama->reset();
  apply_generation_params(NitroConfig{});
  iter = std::make_unique<LlamaIter>();
  if (!llama->add_message(*iter, "system", system_prompt)) {
    tui.append_line(ICON_ERR + "System prompt injection: " + llama->last_error());
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

std::string AgentState::memory_info_text() const {
  if (!model_loaded) return "No model loaded.";
  LlamaMemoryInfo m = llama->memory_info();
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

std::string AgentState::rag_tool(const NitroConfig &cfg, const std::string &agent_query) const {
  std::string result;
  if (embed_llama && rag_db && rag_session) {
    result = embed_llama->rag_retrieve(*rag_db, agent_query, cfg.rag_top_k, *rag_session);
    if (result.empty()) {
      result = "RAG: no context found";
    }
  } else {
    result = "RAG: not enabled";
  }
  return result;
}

bool AgentState::rag_load_index(const std::string &path, TuiState &tui) const {
  if (!embed_llama || !rag_db) {
    tui.append_line(ICON_ERR + "Load an embedding model first: /embed <path>");
    tui.redraw_all();
    return false;
  }

  if (!rag_db->load(path)) {
    tui.append_line(ICON_SYS + "failed to load");
    tui.redraw_all();
  }

  return true;
}

bool AgentState::rag_index(const std::string &path, const NitroConfig &cfg, TuiState &tui) const {
  if (!embed_llama || !rag_db) {
    tui.append_line(ICON_ERR + "Load an embedding model first: /embed <path>");
    tui.redraw_all();
    return false;
  }

  auto index_one = [&](const std::string &filepath) {
    tui.append_line(ICON_SYS + "  indexing: " + filepath);
    tui.redraw_all();
    if (!embed_llama->rag_index(*rag_db, filepath)) {
      tui.append_line(ICON_ERR + "rag_load: " + embed_llama->last_error());
      tui.redraw_all();
    }
  };

  // must be set before indexing
  rag_db->embed_dim = embed_llama->get_embed_dim();

  fs::path rp(path);
  std::error_code ec;
  if (fs::is_directory(rp, ec)) {
    for (const auto &entry : fs::recursive_directory_iterator(rp, ec)) {
      if (entry.is_regular_file()) {
        index_one(entry.path().string());
      }
    }
  } else {
    index_one(path);
  }

  std::string save_path = join_path(cfg.sandbox, "rag-index.bin");
  tui.append_line(ICON_SYS + "saving index: " + save_path);
  tui.redraw_all();
  rag_db->save(save_path);

  return true;
}

//
// Tool dispatch
//
std::string AgentState::process_tool(const std::string &cmd, const NitroConfig &cfg, TuiState &tui) const {
  const std::string &sandbox = cfg.sandbox;
  const std::vector<std::string> &run_allowed = cfg.run_allowed;

  std::string op, arg1, arg2;
  auto sp1 = cmd.find_first_of(" \n");
  if (sp1 == std::string::npos) {
    op = trim(cmd);
  } else {
    op = trim(cmd.substr(0, sp1));
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
    if (p.empty() || p == ".") {
      return sandbox;
    }
    if (p.substr(0, 2) == "./") {
      return join_path(sandbox, p.substr(2));
    }
    if (p[0] == '/') {
      return p;
    }
    return join_path(sandbox, unwrap(p));
  };

  auto show_tool = [&](const std::string &tool) -> void {
    tui.append_line(ICON_TOOL + "→ " + tool);
    tui.redraw_all();
  };

  if (op == "TOOL:DATE") {
    show_tool(op);
    char buf[32]; time_t t = time(nullptr);
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&t));
    return buf;
  }
  if (op == "TOOL:TIME") {
    show_tool(op);
    char buf[32]; time_t t = time(nullptr);
    strftime(buf, sizeof(buf), "%H:%M:%S", localtime(&t));
    return buf;
  }
  if (op == "TOOL:RND") {
    show_tool(op);
    return std::to_string((double)rand() / RAND_MAX);
  }
  if (op == "TOOL:RAG") {
    show_tool(op);
    return rag_tool(cfg, arg1);
  }
  if (op == "TOOL:LIST") {
    std::string dir = resolve(arg1);
    show_tool("listing: " + dir);
    return list_dir(dir);
  }
  if (op == "TOOL:EXISTS") {
    std::string p = resolve(arg1);
    show_tool("checking: " + p);
    return fs::exists(p) ? "YES" : "NO";
  }
  if (op == "TOOL:READ") {
    show_tool("reading: " + arg1);
    std::string p = resolve(arg1);
    return read_file(p);
  }
  if (op == "TOOL:WRITE") {
    show_tool("writing: " + arg1);
    std::string p = resolve(arg1);
    if (!path_in_sandbox(sandbox, p)) {
      return "ERROR: path outside sandbox";
    }
    if (cfg.permission_prompt && !tui.confirm_dialog(std::format("Allow model to write {}?", p))) {
      return "ERROR: action prevented by user";
    }
    std::string content = strip_code_fences(arg1, arg2);
    return write_file(p, content) ? "OK: written to " + arg1 : "ERROR: write failed for " + arg1;
  }
  if (op == "TOOL:MKDIR") {
    std::string p = resolve(arg1);
    show_tool("mkdir: " + arg1);
    if (!path_in_sandbox(sandbox, p)) {
      return "ERROR: path outside sandbox";
    }
    return make_dir(p) ? "OK: created " + arg1 : "ERROR: mkdir failed for " + arg1;
  }
  if (op == "TOOL:CURL") {
    show_tool("curl: " + arg1);
    return tool_curl(arg1);
  }
  if (op == "TOOL:INTROSPECT") {
    show_tool("introspecting: " + arg1);
    return introspect(cfg);
  }
  if (op == "TOOL:ASK") {
    tui.set_thinking(false);
    show_tool("asking: " + arg1 + " " + arg2);
    return tui.readline_blocking();
  }
  if (op == "TOOL:PERMISSION") {
    tui.set_thinking(false);
    show_tool("asking permission: " + arg1 + " " + arg2);
    return tui.confirm_dialog(arg1 + " " + arg2) ? "YES" : "NO";
  }
  if (op == "TOOL:RUN") {
    if (!run_allowed.empty()) {
      bool permitted = ranges::any_of(run_allowed, [&](const std::string &a) {return a == arg1;});
      if (!permitted) {
        return "ERROR: '" + arg1 + "' is not in the TOOL:RUN allowlist. "
          "Use /set run_allowed <name> to permit it.";
      }
    } else if (cfg.permission_prompt && !tui.confirm_dialog(std::format("Allow {} {} to run?", arg1, arg2))) {
      return "ERROR: prevented by user";
    }
    std::string command = arg1 + " " + arg2 + " 2>&1";
    show_tool("running: " + command);
    FILE *fp = popen(command.c_str(), "r");
    if (!fp) {
      return "ERROR: popen failed";
    }
    std::string out;
    char buf[256];
    while (fgets(buf, sizeof(buf), fp)) {
      out += buf;
    }
    pclose(fp);
    if (out.size() > 4096) {
      out = out.substr(0, 4096) + "\n…(truncated)";
    }
    return out;
  }
  return "ERROR: unknown tool: [" + op + "]";
}

//
// Agent turn
//
bool AgentState::run_turn(const std::string &user_message, const NitroConfig &cfg, TuiState &tui) const {
  if (!model_loaded) {
    tui.append_line(ICON_ERR + "No model loaded. Use /model <path>");
    tui.redraw_all();
    return false;
  }
  std::string effective_message = user_message;
  if (embed_llama && rag_db && rag_session) {
    std::string context = embed_llama->rag_retrieve(*rag_db, user_message, cfg.rag_top_k, *rag_session);
    if (!context.empty()) {
      log_write("RAG: %s", context.c_str());
      effective_message = "Context:\n" + context + "\n\nUser: " + user_message;
    } else {
      log_write("RAG: no context found [%s]", embed_llama->last_error());
    }
  }
  if (!iter) {
    tui.append_line(ICON_ERR + "Conversation not initialised (call /clear to reset)");
    tui.redraw_all();
    return false;
  }
  if (!llama->add_message(*iter, "user", effective_message)) {
    tui.append_line(ICON_ERR + "add_message: " + llama->last_error());
    tui.redraw_all();
    return false;
  }
  tui.append_line("Nitro: ");

  // in_think starts false — models that don't use <think> blocks emit
  // visible text immediately.  The spinner activates only while thinking.
  enum {t_init, t_think, t_thunk} think_mode = (cfg.thinking ? t_init : t_thunk);

  tui.set_thinking(true);
  std::string buffer;

  auto invoke_tool = [&](const std::string &buffer, const std::string_view template_str) -> void {
    static constexpr std::string_view END_TOOL = "\nNITRO_END_TOOL";
    static const std::string TOOL_RESULT = "NITRO_TOOL_RESULT: ";

    std::string tool;
    const auto pos = buffer.rfind(END_TOOL);
    if (pos != std::string::npos) {
      tool = buffer.substr(0, pos);
      auto endTool = buffer.substr(pos);
      if (endTool.length() > END_TOOL.length()) {
        log_write("ERROR: trailing delimiter: [%s]", endTool.c_str());
      }
    } else {
      tool = buffer;
    }

    log_write("tool request: mode:[%d] [%s]", think_mode, tool.c_str());
    std::string result = process_tool(tool, cfg, tui);
    std::string content = TOOL_RESULT + std::vformat(template_str, std::make_format_args(result));
    log_write("tool: [%s] result: [%s]", tool.c_str(), result.c_str());

    if (content.size() > llama->max_tool_result_size()) {
      // Index the content into RAG and tell the model where to find it
      if (embed_llama && rag_db && rag_session) {
        content = std::format("Tool result too large ({} bytes). The content has been indexed. "
                              "Use TOOL:RAG with a relevant query to retrieve the information you need.",
                              content.size());
      } else {
        content = std::format("Tool result too large ({} bytes).",  content.size());
      }
      tui.append_line(ICON_ERR + content);
    }
    if (!llama->add_message(*iter, "tool_result", content)) {
      tui.append_line(ICON_ERR + "tool result inject: " + llama->last_error());
    }
    if (!iter->_has_next) {
      tui.append_line(ICON_ERR + "failed to evoke tool response: " + llama->last_error());
    }
    tui.redraw_all();
  };

  auto start_think = [&](const std::string &tag) -> void {
    if (think_mode == t_init) {
      auto pos = buffer.find(tag);
      if (pos != std::string::npos) {
        think_mode = t_think;
        // display prededing text
        buffer = buffer.substr(0, pos);
      }
    }
  };

  auto end_think = [&](const std::string &tag) -> void {
    if (think_mode == t_think) {
      auto pos = buffer.find(tag);
      if (pos != std::string::npos) {
        think_mode = t_thunk;
        // display remaining text
        buffer = buffer.substr(pos + tag.length());
      }
    }
  };

  auto is_escape = [&]() -> bool {
    ncinput ni{};
    notcurses_get_nblock(tui.nc, &ni);
    if (ni.id == NCKEY_ESC) {
      tui.set_thinking(false);
      tui.append_line(ICON_ERR + "Generation cancelled by user (Escape)");
      tui.redraw_all();
    }
    return ni.id == NCKEY_ESC;
  };

  auto fetch_all = [&]() -> void {
    while (iter->_has_next && !is_escape()) {
      std::string tok = llama->next(*iter);
      buffer += tok;
      tui.tick_spinner();
    }
  };

  while (iter->_has_next && !is_escape()) {
    std::string tok = llama->next(*iter);
    if (tok == "<") {
      // fetch the complete tag
      std::string tag = tok;
      while (iter->_has_next && tag.find(">") == std::string::npos) {
        tag += llama->next(*iter);
      }
      if (tag == "<|think|>") {
        think_mode = t_think;
        continue;
      } else {
        buffer += tag;
      }
    } else {
      buffer += tok;
    }
    if (think_mode == t_init) {
      start_think("<think>");
      start_think("<|think|>");
      start_think("<think|>");
      start_think("<|channel>thought");
    }
    if (think_mode == t_think) {
      tui.tick_spinner();
      end_think("</think>");
      end_think("</|think|>");
      end_think("</|think>");
      end_think("<think|>");
      end_think("<channel|>");
    }
    if (think_mode == t_thunk) {
      auto tool_start = buffer.find("TOOL:");
      if (tool_start == 0) {
        fetch_all();
        invoke_tool(trim(buffer), "TOOL_RESULT: {}");
        buffer.clear();
        think_mode = t_init;
        continue;
      }
      tool_start = buffer.find("<|tool_call>call:");
      if (tool_start != std::string::npos) {
        // see https://ai.google.dev/gemma/docs/core/prompt-formatting-gemma4
        fetch_all();
        auto pos = buffer.find_last_not_of("}<tool_call|>");
        if (pos != std::string::npos) {
          buffer = buffer.substr(0, pos);
        }
        pos = buffer.find_first_not_of('{');
        if (pos != std::string::npos) {
          buffer = buffer.substr(0, pos) + buffer.substr(pos + 1);
        }
        invoke_tool(trim(buffer), "<|tool_response>{}<tool_response|>");
        buffer.clear();
        think_mode = t_init;
        continue;
      }
      auto pos = buffer.find('\n');
      if (pos != std::string::npos) {
        tui.append_token(buffer.substr(0, pos + 1));
        buffer = buffer.substr(pos + 1);
      }
    } else {
      auto pos = buffer.find('\n');
      if (pos != std::string::npos) {
        auto thought = buffer.substr(0, pos + 1);
        if (thought.length() > 1) {
          tui.append_token(ICON_THINK + thought);
        }
        buffer = buffer.substr(pos + 1);
      }
    }
  }

  if (!buffer.empty()) {
    tui.append_token(buffer + "\n");
  }

  tui.flush_token_acc();
  tui.set_thinking(false);
  tui.tokens_per_sec = tokens_per_sec();
  LlamaMemoryInfo mem = llama->memory_info();
  tui.kv_used    = mem.kv_used;
  tui.kv_total   = mem.kv_total;
  tui.vram_used  = mem.vram_used;
  tui.vram_total = mem.vram_total;
  char stat[128];
  auto patterm = ICON_SYS + "%.1f tok/s  (%d tokens)  KV %.1f%%";
  std::snprintf(stat, sizeof(stat), patterm.c_str(),
                (double)tui.tokens_per_sec,
                iter->_tokens_generated,
                (double)mem.kv_percent);
  tui.append_line(stat);
  tui.redraw_all();
  return true;
}

//
// Slash command handler
//
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
    tui.show_help();
    return;
  }
  // ── /model ──────────────────────────────────────────────────────────────
  // If no path is given, open the file picker so the user can browse to a
  // GGUF.  The picker starts in the current sandbox directory.
  if (verb == "/model") {
    if (rest.empty()) {
      tui.append_line(ICON_SYS + "Opening model picker…");
      tui.redraw_all();
      rest = tui.file_picker(cfg.sandbox, "Model File");
      if (rest.empty()) {
        tui.append_line(ICON_SYS + "/model cancelled.");
        tui.redraw_all();
        return;
      }
    }
    cfg.model_path = rest;
    if (agent.setup_model(cfg, tui)) {
      std::string sysp = build_system_prompt(cfg);
      agent.reset_conversation(sysp, tui);
      save_settings(cfg);
    }
    tui.redraw_all();
    return;
  }

  // ── /embed ──────────────────────────────────────────────────────────────
  // If no path is given, open the file picker so the user can browse to an
  // embedding GGUF.
  if (verb == "/embed") {
    if (rest.empty()) {
      tui.append_line(ICON_SYS + "Opening embedding model picker…");
      tui.redraw_all();
      rest = tui.file_picker(cfg.sandbox, "Embedding Model");
      if (rest.empty()) {
        tui.append_line(ICON_SYS + "/embed cancelled.");
        tui.redraw_all();
        return;
      }
    }
    cfg.embed_path = rest;
    if (agent.setup_embed(rest, tui)) {
      save_settings(cfg);
    }
    return;
  }

  // ── /rag ────────────────────────────────────────────────────────────────
  if (verb == "/rag") {
    std::string path = rest;
    if (path.empty()) {
      // Launch the interactive folder picker starting from the sandbox.
      path = tui.rag_folder_picker(cfg.sandbox);
      if (path.empty()) {
        tui.append_line(ICON_SYS + "RAG indexing cancelled.");
        tui.redraw_all();
        return;
      }
    }
    if (path.find_last_not_of(".bin") != std::string::npos) {
      tui.append_line(ICON_SYS + "Loading index: " + path);
      tui.redraw_all();
      agent.rag_load_index(path, tui);
    } else {
      tui.append_line(ICON_SYS + "Indexing: " + path);
      tui.redraw_all();
      agent.rag_index(path, cfg, tui);
    }
    tui.append_line(ICON_SYS + "done");
    tui.redraw_all();
    return;
  }

  if (verb == "/memory") {
    std::istringstream iss(agent.memory_info_text());
    std::string line;
    while (std::getline(iss, line)) tui.append_line(ICON_SYS + "" + line);
    tui.redraw_all();
    return;
  }

  if (verb == "/clear") {
    { std::lock_guard<std::mutex> lk(tui.lines_mutex);
      tui.chat_lines.clear(); }
    std::string sysp = build_system_prompt(cfg);
    agent.reset_conversation(sysp, tui);
    tui.append_line(ICON_SYS + "Conversation cleared.");
    tui.redraw_all();
    return;
  }

  if (verb == "/settings") {
    tui.append_line(ICON_SYS + "Current settings:");
    tui.append_line(ICON_SYS + "  model_path    : " + cfg.model_path);
    tui.append_line(ICON_SYS + "  embed_path    : " + cfg.embed_path);
    tui.append_line(ICON_SYS + "  sandbox       : " + cfg.sandbox);
    tui.append_line(ICON_SYS + "  n_ctx         : " + std::to_string(cfg.n_ctx));
    tui.append_line(ICON_SYS + "  n_gpu_layers  : " + std::to_string(cfg.n_gpu_layers));
    tui.append_line(ICON_SYS + "  temperature   : " + std::to_string(cfg.temperature));
    tui.append_line(ICON_SYS + "  top_p         : " + std::to_string(cfg.top_p));
    tui.append_line(ICON_SYS + "  top_k         : " + std::to_string(cfg.top_k));
    tui.append_line(ICON_SYS + "  penalty_repeat: " + std::to_string(cfg.penalty_repeat));
    tui.append_line(ICON_SYS + "  rag_top_k     : " + std::to_string(cfg.rag_top_k));
    tui.append_line(ICON_SYS + "  saved to      : " + settings_path());
    tui.redraw_all();
    return;
  }

  if (verb == "/set") {
    // Usage: /set <key> <value>
    // Parses the key and value, updates cfg in place, re-applies generation
    // params if needed, and saves settings to disk.
    auto sp2 = rest.find(' ');
    std::string key = (sp2 == std::string::npos) ? rest : rest.substr(0, sp2);
    std::string val = (sp2 == std::string::npos) ? "" : rest.substr(sp2 + 1);
    val.erase(0, val.find_first_not_of(" \t"));

    if (key.empty() || val.empty()) {
      tui.append_line(ICON_ERR + "Usage: /set <key> <value>");
      tui.redraw_all(); return;
    }

    bool ok = true;
    bool needs_reparam = false;
    try {
      if (key == "temperature")    { cfg.temperature    = std::stof(val); needs_reparam = true; }
      else if (key == "top_p")     { cfg.top_p          = std::stof(val); needs_reparam = true; }
      else if (key == "min_p")     { cfg.min_p          = std::stof(val); needs_reparam = true; }
      else if (key == "top_k")     { cfg.top_k          = std::stoi(val); needs_reparam = true; }
      else if (key == "penalty_repeat") { cfg.penalty_repeat = std::stof(val); needs_reparam = true; }
      else if (key == "penalty_last_n") { cfg.penalty_last_n = std::stoi(val); needs_reparam = true; }
      else if (key == "rag_top_k")      { cfg.rag_top_k      = std::stoi(val); }
      else if (key == "n_gpu_layers")   {
        cfg.n_gpu_layers = std::stoi(val);
        tui.append_line(ICON_SYS + "n_gpu_layers will take effect on next /model load.");
      } else if (key == "run_allowed") {
        // Accept a comma-separated list of basenames, or "none" to clear.
        cfg.run_allowed.clear();
        if (val != "none") {
          std::istringstream iss(val);
          std::string tok;
          while (std::getline(iss, tok, ',')) {
            tok.erase(0, tok.find_first_not_of(" \t"));
            tok.erase(tok.find_last_not_of(" \t") + 1);
            if (!tok.empty()) cfg.run_allowed.push_back(tok);
          }
        }
        if (cfg.run_allowed.empty()) {
          tui.append_line(ICON_SYS + "run_allowed cleared — all sandbox programs permitted.");
        } else {
          std::string list;
          for (const auto &e : cfg.run_allowed) list += e + " ";
          tui.append_line(ICON_SYS + "run_allowed: " + list);
        }
      } else {
        tui.append_line(ICON_ERR + "Unknown key '" + key + "'.  Try /help for list.");
        ok = false;
      }
    } catch (const std::exception &ex) {
      tui.append_line(ICON_ERR + "/set: " + ex.what());
      ok = false;
    }

    if (ok) {
      if (needs_reparam && agent.model_loaded) {
        agent.apply_generation_params(cfg);
      }
      save_settings(cfg);
      tui.append_line(ICON_SYS + "" + key + " = " + val);
    }
    tui.redraw_all();
    return;
  }

  tui.append_line(ICON_ERR + "Unknown command: " + verb + "  (try /help)");
  tui.redraw_all();
}

//
// Welcome banner  — colourful multi-line ASCII logo
//
static void welcome(TuiState &tui, const std::string &sandbox) {
  tui.append_line("");
  tui.append_line("[logo_0]  ███╗   ██╗██╗████████╗██████╗  ██████╗ ");
  tui.append_line("[logo_1]  ████╗  ██║██║╚══██╔══╝██╔══██╗██╔═══██╗");
  tui.append_line("[logo_2]  ██╔██╗ ██║██║   ██║   ██████╔╝██║   ██║");
  tui.append_line("[logo_3]  ██║╚██╗██║██║   ██║   ██╔══██╗██║   ██║");
  tui.append_line("[logo_4]  ██║ ╚████║██║   ██║   ██║  ██║╚██████╔╝");
  tui.append_line("[logo_5]  ╚═╝  ╚═══╝╚═╝   ╚═╝   ╚═╝  ╚═╝ ╚═════╝ ");
  tui.append_line("[logo_6]  ─────────── agentic LLM shell v1.0 ──────────────");
  tui.append_line("");
  tui.append_line(ICON_SYS + " Sandbox : " + sandbox);
  tui.append_line(ICON_SYS + " /help for commands  ·  exit to quit");
  tui.append_line("");
  tui.redraw_all();
}

//
// main()
//
int main(int argc, char **argv) {
  // ── Load persisted settings first (provides defaults) ────────────
  NitroConfig cfg;
  // ── Parse arguments (command-line overrides saved settings) ──────
  load_settings(cfg);
  auto resolve_path = [](const std::string &arg) -> std::string {
    if (arg.substr(0, 2) == "~/") {
      const char *home = getenv("HOME");
      return std::string(home ? home : ".") + "/" + arg.substr(2);
    }
    if (arg.substr(0, 2) == "./")
      {
        std::error_code ec;
        return (fs::current_path(ec) / arg.substr(2)).string();
      }
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
    } else if (a == "-l" || a == "--log") {
      log_open();
    } else if (a == "-t" || a == "--think") {
      cfg.thinking = false;
    } else if (a == "-p" || a == "--prompt-permission") {
      cfg.permission_prompt = true;
    } else if (a == "-h" || a == "--help") {
      std::puts("Usage: nitro [options] [project_dir]\n"
                "\n"
                "Options:\n"
                "  -m, --model  <path>      GGUF model to load on startup\n"
                "  -e, --embed  <path>      embedding model for RAG\n"
                "  -g, --gpu-layers <n>     GPU layers to offload (default: 32)\n"
                "  -l, --log                enabled logging\n"
                "  -h, --help               show this help\n"
                "\n"
                "project_dir defaults to the current working directory.\n"
                "Settings are persisted to ~/.config/nitro/settings.json.\n"
                "\n"
                "Slash commands inside nitro:\n"
                "  /model  [path]           load / hot-reload a GGUF (picker if no path)\n"
                "  /embed  [path]           load an embedding model  (picker if no path)\n"
                "  /rag    [path]           index file or directory  (picker if no path)\n"
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
  {
    std::error_code ec;
    fs::create_directories(cfg.sandbox, ec);
  }

  // ── Auto-discover knowledge files ─────────────────────────────────
  for (const char *kf : {"nitro.md", "AGENTS.md", "README.md"}) {
    if (fs::exists(kf)) cfg.knowledge_files.emplace_back(kf);
  }

  // ── Init curl globally ────────────────────────────────────────────
  curl_global_init(CURL_GLOBAL_DEFAULT);

  // ── Init TUI ──────────────────────────────────────────────────────
  TuiState tui;
  tui.init();
  // Load persisted input history so up-arrow works across sessions.
  tui.history.load(history_path());
  welcome(tui, cfg.sandbox);

  log_write("nitro starting");

  // ── Init agent ────────────────────────────────────────────────────
  AgentState agent;
  if (!cfg.model_path.empty()) {
    if (agent.setup_model(cfg, tui)) {
      std::string sysp = build_system_prompt(cfg);
      agent.reset_conversation(sysp, tui);
    }
    if (!cfg.embed_path.empty()) {
      agent.setup_embed(cfg.embed_path, tui);
    }
  } else {
    tui.append_line(ICON_SYS + "No model specified.  Use /model to open the file picker,");
    tui.append_line(ICON_SYS + "or /model <path> to load directly.");
    tui.append_line(ICON_SYS + "Example: /model ~/models/qwen2.5-7b-q4_k_m.gguf");
    tui.redraw_all();
  }

  // ── Main loop ─────────────────────────────────────────────────────
  for (;;) {
    {
      unsigned rows = 0, cols = 0;
      notcurses_stddim_yx(tui.nc, &rows, &cols);
      if ((int)rows != tui.term_rows || (int)cols != tui.term_cols) {
        tui.resize();
      }
    }
    std::string input = tui.readline_blocking();
    input.erase(0, input.find_first_not_of(" \t"));
    if (!input.empty()) {
      input.erase(input.find_last_not_of(" \t\r\n") + 1);
    }
    if (input.empty()) {
      continue;
    }
    tui.append_line("You: " + input);
    tui.redraw_all();
    if (input == "exit" || input == "quit") {
      break;
    }
    if (input[0] == '/') {
      handle_slash(input, cfg, agent, tui);
    } else {
      agent.run_turn(input, cfg, tui);
    }
  }

  log_write("nitro exiting");
  log_close();
  tui.destroy();
  // Persist input history for the next session.
  tui.history.save(history_path());
  curl_global_cleanup();
  return 0;
}
