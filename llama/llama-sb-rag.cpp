// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2026 Chris Warren-Smith

#include "llama-sb.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

struct RagChunk {
  std::string        text;
  std::string        source;
  std::string        type;
  std::vector<float> embedding;
};

struct RagDB {
  std::vector<RagChunk> chunks;
  int embed_dim = 0;

  int  size()  const { return (int)chunks.size(); }
  bool empty() const { return chunks.empty(); }
};

//
// per-session deduplication + token budget
//
struct RagSession {
  std::vector<bool> seen;        /* sized to db.size() on init  */
  int  tokens_used  = 0;
  int  tokens_max   = 0;         /* set to your n_ctx           */
  float score_threshold = 0.60f; /* skip weak matches           */

  void init(int n_chunks, int ctx_size) {
    seen.assign(n_chunks, false);
    tokens_used = 0;
    tokens_max  = ctx_size;
  }

  void reset() {
    std::fill(seen.begin(), seen.end(), false);
    tokens_used = 0;
  }

  bool is_seen(int idx)  const { return idx < (int)seen.size() && seen[idx]; }
  void mark(int idx)           { if (idx < (int)seen.size()) seen[idx] = true; }

  /* rough token estimate: 1 token ≈ 4 chars */
  bool budget_ok(const std::string &text) const {
    return tokens_max == 0 ||
           (tokens_used + (int)text.size() / 4) < (int)(tokens_max * 0.85f);
  }

  void charge(const std::string &text) {
    tokens_used += (int)text.size() / 4;
  }
};

bool Llama::embed_text(const std::string &text, std::vector<float> &out, int embed_dim) {
  vector<llama_token> tokens = tokenize(text);
  if (tokens.size() == 0) {
    return false;
  }

  // truncate to context window
  int n_ctx = llama_n_ctx(_ctx);
  int n = tokens.size();
  if (n > n_ctx) {
    _last_error = std::format("warning: chunk truncated {} -> {} tokens ", n, n_ctx);
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
    _last_error = "no embedding returned\n";
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

bool Llama::rag_load(RagDB &db, const std::string &path) {
  std::ifstream f(path, std::ios::binary);
  if (!f) {
    _last_error = std::format("rag_load: cannot open {}", path);
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

  if (magic != 0x52414744) {
    _last_error = "rag_load: bad magic";
    return false;
  }
  if (version != 2) {
    _last_error = std::format("rag_load: unsupported version {} (expected 2)", version);
    return false;
  }

  db.embed_dim = (int)edim;
  db.chunks.resize(n);

  for (uint32_t i = 0; i < n; i++) {
    RagChunk &c = db.chunks[i];

    uint32_t text_len = read32();
    c.text = readstr(text_len);

    uint16_t src_len = read16();
    c.source = readstr(src_len);

    uint8_t type_len = read8();
    c.type = readstr(type_len);

    c.embedding.resize(edim);
    f.read((char*)c.embedding.data(), (std::streamsize)(edim * sizeof(float)));
  }

  if (!f) {
    _last_error = "rag_load: read error";
    return false;
  }

  std::cerr << "rag: loaded " << db.chunks.size()
            << " chunks (dim=" << db.embed_dim
            << ") from " << path << "\n";
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
// retrieve with session
//
std::string Llama::rag_retrieve(const RagDB &db,
                                const std::string &query,
                                int top_k,
                                RagSession &session) {
  if (db.empty()) {
    return {};
  }

  std::vector<float> qvec;
  std::string text = "Instruct: Given a programming question, retrieve relevant API documentation\nQuery: " + query;
  if (!embed_text(text, qvec, db.embed_dim)) {
    return {};
  }

  // score all chunks
  std::vector<int>   order(db.size());
  std::iota(order.begin(), order.end(), 0);
  std::vector<float> scores(db.size());
  for (int i = 0; i < db.size(); i++)
    scores[i] = rag_cosine(qvec, db.chunks[i].embedding);

  std::sort(order.begin(), order.end(),
            [&](int a, int b){ return scores[a] > scores[b]; });

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
