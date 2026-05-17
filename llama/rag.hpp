/*
 * rag.hpp
 *
 * Single-header C++ RAG runtime library.
 * No fixed limits on chunk count, chunk length, or embedding dimension.
 * Includes RagSession for deduplication across turns.
 *
 * Usage (in ONE .cpp file):
 *   #define RAG_IMPLEMENTATION
 *   #include "rag.hpp"
 *
 * All other files:
 *   #include "rag.hpp"
 *
 * Db format version: 2  (written by rag_index.cpp)
 */

#pragma once

#include "llama.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

/* ═══════════════════════════════════════════════════════════
 * Data structures
 * ═══════════════════════════════════════════════════════════ */

struct RagChunk {
  std::string        text;
  std::string        source;
  std::string        type;
  std::vector<float> embedding;
};

struct RagDB {
  std::vector<RagChunk> chunks;
  int                   embed_dim = 0;

  int  size()  const { return (int)chunks.size(); }
  bool empty() const { return chunks.empty(); }
};

/* ── per-session deduplication + token budget ──────────────── */
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

/* ═══════════════════════════════════════════════════════════
 * API declarations
 * ═══════════════════════════════════════════════════════════ */

/* Load .db file produced by rag_index.cpp (version 2).
 * Returns true on success. */
bool rag_load(RagDB &db, const std::string &path);

/*
 * Embed query, score all chunks, inject top_k unseen results into out.
 * Skips chunks already in session.seen and below session.score_threshold.
 * Updates session.seen and session.tokens_used.
 * Returns the context string (empty if nothing retrieved).
 */
std::string rag_retrieve(const RagDB      &db,
                         llama_context    *embed_ctx,
                         llama_model      *embed_model,
                         const std::string &query,
                         int               top_k,
                         RagSession       &session);

/*
 * Stateless overload — no deduplication, no budget tracking.
 * Useful for one-shot queries or testing retrieval quality.
 */
std::string rag_retrieve(const RagDB      &db,
                         llama_context    *embed_ctx,
                         llama_model      *embed_model,
                         const std::string &query,
                         int               top_k);

/* ═══════════════════════════════════════════════════════════
 * Implementation
 * ═══════════════════════════════════════════════════════════ */
#ifdef RAG_IMPLEMENTATION

/* ── db load ───────────────────────────────────────────────── */
bool rag_load(RagDB &db, const std::string &path) {
  std::ifstream f(path, std::ios::binary);
  if (!f) { std::cerr << "rag_load: cannot open " << path << "\n"; return false; }

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
    std::cerr << "rag_load: bad magic\n"; return false;
  }
  if (version != 2) {
    std::cerr << "rag_load: unsupported version " << version
              << " (expected 2)\n"; return false;
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
    f.read((char*)c.embedding.data(),
           (std::streamsize)(edim * sizeof(float)));
  }

  if (!f) { std::cerr << "rag_load: read error\n"; return false; }

  std::cerr << "rag: loaded " << db.chunks.size()
            << " chunks (dim=" << db.embed_dim
            << ") from " << path << "\n";
  return true;
}

/* ── embed query ───────────────────────────────────────────── */
static bool rag_embed_query(llama_context     *ctx,
                            llama_model       *model,
                            const std::string &query,
                            std::vector<float> &out,
                            int                embed_dim) {
  std::string prefixed = "search_query: " + query;

  int n = -llama_tokenize(model,
                          prefixed.c_str(), (int)prefixed.size(),
                          nullptr, 0, true, true);
  if (n <= 0) return false;

  std::vector<llama_token> tokens(n);
  llama_tokenize(model,
                 prefixed.c_str(), (int)prefixed.size(),
                 tokens.data(), n, true, true);

  int n_ctx = llama_n_ctx(ctx);
  if (n > n_ctx) n = n_ctx;

  llama_kv_cache_clear(ctx);

  llama_batch batch = llama_batch_init(n, 0, 1);
  for (int i = 0; i < n; i++) {
    llama_seq_id seq = 0;
    llama_batch_add(batch, tokens[i], i, &seq, 1, i == n - 1);
  }

  if (llama_decode(ctx, batch) != 0) {
    llama_batch_free(batch);
    return false;
  }

  float *emb = llama_get_embeddings_seq(ctx, 0);
  if (!emb) emb = llama_get_embeddings_ith(ctx, n - 1);
  if (!emb) { llama_batch_free(batch); return false; }

  out.assign(emb, emb + embed_dim);

  float norm = 0.0f;
  for (float v : out) norm += v * v;
  norm = std::sqrt(norm);
  if (norm > 1e-9f)
    for (float &v : out) v /= norm;

  llama_batch_free(batch);
  return true;
}

/* ── cosine similarity (vectors already L2-normalized) ─────── */
static float rag_cosine(const std::vector<float> &a,
                        const std::vector<float> &b) {
  float dot = 0.0f;
  size_t n = std::min(a.size(), b.size());
  for (size_t i = 0; i < n; i++) dot += a[i] * b[i];
  return dot;
}

/* ── build context string from ranked results ──────────────── */
static std::string rag_build_context(
    const RagDB              &db,
    const std::vector<int>   &indices,
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

/* ── retrieve with session ─────────────────────────────────── */
std::string rag_retrieve(const RagDB      &db,
                         llama_context    *embed_ctx,
                         llama_model      *embed_model,
                         const std::string &query,
                         int               top_k,
                         RagSession       &session) {
  if (db.empty()) return {};

  std::vector<float> qvec;
  if (!rag_embed_query(embed_ctx, embed_model, query, qvec, db.embed_dim))
    return {};

  /* score all chunks */
  std::vector<int>   order(db.size());
  std::iota(order.begin(), order.end(), 0);
  std::vector<float> scores(db.size());
  for (int i = 0; i < db.size(); i++)
    scores[i] = rag_cosine(qvec, db.chunks[i].embedding);

  std::sort(order.begin(), order.end(),
            [&](int a, int b){ return scores[a] > scores[b]; });

  /* collect top_k unseen, within budget, above threshold */
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

/* ── stateless retrieve ────────────────────────────────────── */
std::string rag_retrieve(const RagDB      &db,
                         llama_context    *embed_ctx,
                         llama_model      *embed_model,
                         const std::string &query,
                         int               top_k) {
  if (db.empty()) return {};

  std::vector<float> qvec;
  if (!rag_embed_query(embed_ctx, embed_model, query, qvec, db.embed_dim))
    return {};

  std::vector<int>   order(db.size());
  std::iota(order.begin(), order.end(), 0);
  std::vector<float> scores(db.size());
  for (int i = 0; i < db.size(); i++)
    scores[i] = rag_cosine(qvec, db.chunks[i].embedding);

  std::sort(order.begin(), order.end(),
            [&](int a, int b){ return scores[a] > scores[b]; });

  std::vector<int>   result_idx;
  std::vector<float> result_scores;
  for (int i = 0; i < std::min(top_k, db.size()); i++) {
    result_idx.push_back(order[i]);
    result_scores.push_back(scores[order[i]]);
  }

  return rag_build_context(db, result_idx, result_scores);
}

#endif /* RAG_IMPLEMENTATION */
