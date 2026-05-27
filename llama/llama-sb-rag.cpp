// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2026 Chris Warren-Smith

#include "llama-sb.h"
#include "llama-sb-rag.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

static constexpr uint32_t MAGIC = 0x52414744;
static constexpr size_t MIN_CHUNK = 40;
static constexpr const char *INSTRUCT_EMBED = "Instruct: Represent this API documentation for code retrieval\nQuery: ";
static constexpr const char *INSTRUCT_QUERY = "Instruct: Given a programming question, retrieve relevant API documentation\nQuery: ";

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

/* ── state machine ─────────────────────────────────────────── */

enum class State {
  Idle, BlockComment, LineComment, Declaration, Struct, Defines
};

template<typename EmitChunk>

static bool chunk_file(const fs::path &path, EmitChunk emit_chunk) {
  std::ifstream f(path);
  if (!f) {
    return false;
  }

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

  return true;
}

//
// cosine similarity (vectors already L2-normalized)
//
static float rag_cosine(const std::vector<float> &a,
                        const std::vector<float> &b) {
  float dot = 0.0f;
  size_t n = std::min(a.size(), b.size());
  for (size_t i = 0; i < n; i++) {
    dot += a[i] * b[i];
  }
  return dot;
}

//
// build context string from ranked results
//
static std::string rag_build_context(const RagDB &db,
                                     const std::vector<int> &indices,
                                     const std::vector<float> &scores) {
  std::ostringstream out;
  for (size_t i = 0; i < indices.size(); i++) {
    const RagChunk &c = db.chunks[indices[i]];
    out << "// source: " << c.source
        << " [" << c.type << "]"
        << " (score: " << scores[i] << ")\n"
        << c.text << "\n---\n";
  }
  return out.str();
}

//
// index the file
//
bool Llama::rag_index(RagDB &db, const std::string &filepath) {
  bool embed_fail = false;
  auto emit_chunk = [&](const std::string &source, ChunkType type,
                        const std::string &text) {
    if (text.size() > MIN_CHUNK) {
      RagChunk chunk;
      chunk.text = text;
      chunk.source = source;
      chunk.type = type_name(type);
      if (!embed_text(INSTRUCT_EMBED + text, chunk.embedding, db.embed_dim)) {
        embed_fail = true;
      } else {
        db.chunks.push_back(std::move(chunk));
      }
    }
  };

  return !embed_fail && chunk_file(filepath, emit_chunk);
}

//
// retrieve with session
//
std::string Llama::rag_retrieve(const RagDB &db,
                                const std::string &query,
                                int top_k,
                                RagSession &session) {
  if (db.empty()) {
    _last_error = "no input";
    return {};
  }

  std::vector<float> qvec;
  std::string text = INSTRUCT_QUERY + query;
  if (!embed_text(text, qvec, db.embed_dim)) {
    _last_error = "failed to embed text";
    return {};
  }

  // score all chunks
  std::vector<int>   order(db.size());
  std::iota(order.begin(), order.end(), 0);
  std::vector<float> scores(db.size());
  for (int i = 0; i < db.size(); i++) {
    scores[i] = rag_cosine(qvec, db.chunks[i].embedding);
  }
  std::sort(order.begin(), order.end(), [&](int a, int b){ return scores[a] > scores[b]; });

  // collect top_k unseen, within budget, above threshold
  std::vector<int>   result_idx;
  std::vector<float> result_scores;

  for (int idx : order) {
    if ((int)result_idx.size() >= top_k) break;
    if (session.is_seen(idx))            continue;
    if (scores[idx] < session.score_threshold) break; /* sorted, so stop */
    if (!session.budget_ok(db.chunks[idx].text)) break;

    result_idx.push_back(idx);
    result_scores.push_back(scores[idx]);
    session.mark(idx);
    session.charge(db.chunks[idx].text);
  }

  return rag_build_context(db, result_idx, result_scores);
}

bool RagDB::save(const std::string &path) {
  std::ofstream f(path, std::ios::binary);
  if (!f) {
    return false;
  }

  auto write32 = [&](uint32_t v) { f.write((char*)&v, 4); };
  auto write16 = [&](uint16_t v) { f.write((char*)&v, 2); };
  auto write8  = [&](uint8_t  v) { f.write((char*)&v, 1); };
  auto writestr = [&](const std::string &s, size_t max_len) {
    size_t len = std::min(s.size(), max_len);
    f.write(s.c_str(), (std::streamsize)len);
  };

  write32(MAGIC);              /* magic "RAGD" */
  write32(2);                       /* version      */
  write32((uint32_t)chunks.size()); /* n_chunks     */
  write32((uint32_t)embed_dim);     /* embed_dim    */

  for (const RagChunk &c : chunks) {
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

bool RagDB::load(const std::string &path) {
  std::ifstream f(path, std::ios::binary);
  if (!f) {
    return false;
  }

  auto read32 = [&]() -> uint32_t {
    uint32_t v = 0; f.read((char*)&v, 4); return v;
  };
  auto read16 = [&]() -> uint16_t {
    uint16_t v = 0; f.read((char*)&v, 2); return v;
  };
  auto read8 = [&]() -> uint8_t {
    uint8_t v = 0; f.read((char*)&v, 1); return v;
  };
  auto readstr = [&](size_t len) -> std::string {
    std::string s(len, '\0');
    f.read(&s[0], (std::streamsize)len);
    return s;
  };

  uint32_t magic   = read32();
  uint32_t version = read32();
  uint32_t n       = read32();
  uint32_t edim    = read32();

  if (magic != MAGIC) {
    return false;
  }
  if (version != 2) {
    return false;
  }

  embed_dim = (int)edim;
  chunks.resize(n);

  for (uint32_t i = 0; i < n; i++) {
    RagChunk &c = chunks[i];

    uint32_t text_len = read32();
    c.text = readstr(text_len);

    uint16_t src_len = read16();
    c.source = readstr(src_len);

    uint8_t type_len = read8();
    c.type = readstr(type_len);

    c.embedding.resize(edim);
    f.read((char*)c.embedding.data(), (std::streamsize)(edim * sizeof(float)));
  }

  return true;
}
