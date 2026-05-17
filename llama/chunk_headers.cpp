/*
 * chunk_headers.cpp
 *
 * Smart chunker for C/C++ headers — keeps semantic units together:
 *   - block comment + following declaration/function
 *   - struct/enum/typedef blocks
 *   - grouped #define macros
 *   - standalone inline-commented declarations
 *
 * Output: one chunk per line in a .jsonl file:
 *   {"source":"notcurses.h","type":"function","text":"..."}
 *
 * Build: c++ -std=c++17 -o chunk_headers chunk_headers.cpp
 * Usage: ./chunk_headers notcurses/include/notcurses/notcurses.h > chunks.jsonl
 *        ./chunk_headers dir/                                    > chunks.jsonl
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

/* ── tunables ──────────────────────────────────────────────── */
static constexpr size_t MIN_CHUNK = 40;   /* ignore tiny fragments  */
/* ─────────────────────────────────────────────────────────── */

enum class ChunkType {
  Function, Struct, Enum, Typedef, Defines, Other
};

static std::string type_name(ChunkType t) {
  switch (t) {
    case ChunkType::Function: return "function";
    case ChunkType::Struct:   return "struct";
    case ChunkType::Enum:     return "enum";
    case ChunkType::Typedef:  return "typedef";
    case ChunkType::Defines:  return "defines";
    default:                  return "other";
  }
}

/* ── helpers ───────────────────────────────────────────────── */

static bool starts_with(const std::string &s, const std::string &prefix) {
  return s.size() >= prefix.size() &&
         s.compare(0, prefix.size(), prefix) == 0;
}

static bool is_blank(const std::string &s) {
  for (char c : s) if (!isspace((unsigned char)c)) return false;
  return true;
}

static std::string json_escape(const std::string &in) {
  std::string out;
  out.reserve(in.size() + 32);
  for (unsigned char c : in) {
    if      (c == '"')  { out += "\\\""; }
    else if (c == '\\') { out += "\\\\"; }
    else if (c == '\n') { out += "\\n";  }
    else if (c == '\r') { /* skip */     }
    else if (c == '\t') { out += "\\t";  }
    else if (c < 0x20)  { /* skip */     }
    else                { out += c;      }
  }
  return out;
}

static void emit_chunk(const std::string &source, ChunkType type,
                       const std::string &text) {
  if (text.size() < MIN_CHUNK) return;

  /* trim trailing newlines */
  size_t end = text.size();
  while (end > 0 && (text[end-1] == '\n' || text[end-1] == '\r')) --end;
  if (end < MIN_CHUNK) return;

  std::cout << "{\"source\":\"" << json_escape(source)
            << "\",\"type\":\""  << type_name(type)
            << "\",\"text\":\""  << json_escape(text.substr(0, end))
            << "\"}\n";
}

/* ── state machine ─────────────────────────────────────────── */

enum class State {
  Idle, BlockComment, LineComment, Declaration, Struct, Defines
};

static void process_file(const fs::path &path) {
  std::ifstream f(path);
  if (!f) { std::cerr << "cannot open: " << path << "\n"; return; }

  const std::string source = path.filename().string();

  State     state       = State::Idle;
  std::string chunk;
  ChunkType chunk_type  = ChunkType::Other;
  int       brace_depth = 0;
  int       paren_depth = 0;
  int       define_count = 0;

  auto flush = [&](ChunkType t) {
    emit_chunk(source, t, chunk);
    chunk.clear();
    state       = State::Idle;
    brace_depth = 0;
    paren_depth = 0;
  };

  std::string line;
  while (std::getline(f, line)) {
    /* trim trailing CR */
    if (!line.empty() && line.back() == '\r') line.pop_back();

    /* find first non-whitespace for prefix checks */
    size_t trim_pos = 0;
    while (trim_pos < line.size() &&
           (line[trim_pos] == ' ' || line[trim_pos] == '\t')) ++trim_pos;
    const std::string trimmed = line.substr(trim_pos);

    /* ── #define handling ─────────────────────────────────── */
    if (starts_with(trimmed, "#define ")) {
      if (state == State::BlockComment || state == State::LineComment) {
        chunk += line + "\n";
        state = State::Defines;
        define_count = 1;
      } else if (state == State::Defines) {
        chunk += line + "\n";
        define_count++;
      } else {
        if (chunk.size() >= MIN_CHUNK) emit_chunk(source, chunk_type, chunk);
        chunk.clear();
        chunk += line + "\n";
        state = State::Defines;
        define_count = 1;
      }
      continue;
    }

    /* non-define while in define group */
    if (state == State::Defines) {
      flush(ChunkType::Defines);
      define_count = 0;
      /* fall through to process this line normally */
    }

    /* ── block comment start ──────────────────────────────── */
    if ((starts_with(trimmed, "/*") || starts_with(trimmed, "/**")) &&
        state == State::Idle) {
      if (chunk.size() >= MIN_CHUNK) emit_chunk(source, chunk_type, chunk);
      chunk.clear();
      chunk_type = ChunkType::Other;
      chunk += line + "\n";
      state = (trimmed.find("*/", 2) != std::string::npos)
              ? State::LineComment
              : State::BlockComment;
      continue;
    }

    /* ── inside block comment ─────────────────────────────── */
    if (state == State::BlockComment) {
      chunk += line + "\n";
      if (trimmed.find("*/") != std::string::npos)
        state = State::LineComment;
      continue;
    }

    /* ── // line comment ──────────────────────────────────── */
    if (starts_with(trimmed, "//")) {
      if (state == State::Idle) {
        if (chunk.size() >= MIN_CHUNK) emit_chunk(source, chunk_type, chunk);
        chunk.clear();
        chunk += line + "\n";
        state = State::LineComment;
      } else if (state == State::LineComment) {
        chunk += line + "\n";
      }
      continue;
    }

    /* ── blank line ───────────────────────────────────────── */
    if (is_blank(trimmed)) {
      if (state == State::LineComment)
        flush(ChunkType::Other);
      else if (state == State::Idle && chunk.size() >= MIN_CHUNK)
        flush(chunk_type);
      continue;
    }

    /* ── skip preprocessor noise ──────────────────────────── */
    if (starts_with(trimmed, "#ifndef") || starts_with(trimmed, "#ifdef")  ||
        starts_with(trimmed, "#endif")  || starts_with(trimmed, "#pragma") ||
        starts_with(trimmed, "#include")) {
      if (state == State::LineComment || state == State::BlockComment) {
        chunk.clear();
        state = State::Idle;
      }
      continue;
    }

    /* ── typedef struct / enum start ─────────────────────── */
    if ((starts_with(trimmed, "typedef struct") ||
         starts_with(trimmed, "typedef enum")   ||
         starts_with(trimmed, "struct ")         ||
         starts_with(trimmed, "enum "))          &&
        (state == State::Idle || state == State::LineComment)) {

      if (state == State::Idle && chunk.size() >= MIN_CHUNK)
        emit_chunk(source, chunk_type, chunk);

      /* preserve any comment already in chunk */
      if (state == State::Idle) chunk.clear();

      chunk += line + "\n";
      chunk_type = starts_with(trimmed, "typedef") ? ChunkType::Typedef
                 : starts_with(trimmed, "enum ")   ? ChunkType::Enum
                                                    : ChunkType::Struct;
      state = State::Struct;
      for (char c : line) {
        if (c == '{') ++brace_depth;
        if (c == '}') --brace_depth;
      }
      if (brace_depth <= 0 && line.find(';') != std::string::npos)
        flush(chunk_type);
      continue;
    }

    /* ── inside struct/enum body ──────────────────────────── */
    if (state == State::Struct) {
      chunk += line + "\n";
      for (char c : line) {
        if (c == '{') ++brace_depth;
        if (c == '}') --brace_depth;
      }
      if (brace_depth <= 0 && line.find(';') != std::string::npos)
        flush(chunk_type);
      continue;
    }

    /* ── function / other declaration ────────────────────── */
    if (state == State::LineComment || state == State::Idle) {
      if (state == State::Idle && chunk.size() >= MIN_CHUNK) {
        emit_chunk(source, chunk_type, chunk);
        chunk.clear();
      }
      chunk += line + "\n";
      chunk_type = ChunkType::Function;
      state = State::Declaration;
      for (char c : line) {
        if (c == '(') ++paren_depth;
        if (c == ')') --paren_depth;
      }
      if (paren_depth <= 0 && line.find(';') != std::string::npos)
        flush(ChunkType::Function);
      continue;
    }

    /* ── multi-line declaration ───────────────────────────── */
    if (state == State::Declaration) {
      chunk += line + "\n";
      for (char c : line) {
        if (c == '(') ++paren_depth;
        if (c == ')') --paren_depth;
      }
      if (paren_depth <= 0 && line.find(';') != std::string::npos)
        flush(ChunkType::Function);
      continue;
    }
  }

  /* flush remainder */
  if (chunk.size() >= MIN_CHUNK) emit_chunk(source, chunk_type, chunk);
}

/* ── directory walker ──────────────────────────────────────── */

static void process_path(const fs::path &path) {
  if (fs::is_directory(path)) {
    /* sorted for deterministic output */
    std::vector<fs::path> entries;
    for (auto &e : fs::recursive_directory_iterator(path))
      entries.push_back(e.path());
    std::sort(entries.begin(), entries.end());
    for (auto &e : entries) {
      if (!fs::is_regular_file(e)) continue;
      auto ext = e.extension().string();
      if (ext == ".h" || ext == ".hpp" || ext == ".c" || ext == ".cpp")
        process_file(e);
    }
  } else if (fs::is_regular_file(path)) {
    process_file(path);
  }
}

/* ── main ──────────────────────────────────────────────────── */

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "usage: " << argv[0]
              << " <header.h|dir> [header2.h ...]\n";
    return 1;
  }
  for (int i = 1; i < argc; i++)
    process_path(fs::path(argv[i]));
  return 0;
}
