// This file is part of SmallBASIC
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2026 Chris Warren-Smith

#pragma once

struct RagChunk {
  std::string        text;
  std::string        source;
  std::string        type;
  std::vector<float> embedding;
};

/* ── on-disk chunk (variable-length text) ──────────────────── */
/*
 * db header  (16 bytes):
 *   uint32  magic      = 0x52414744  "RAGD"
 *   uint32  version    = 2
 *   uint32  n_chunks
 *   uint32  embed_dim
 *
 * per chunk:
 *   uint32  text_len
 *   char[]  text          (text_len bytes, no null)
 *   uint16  source_len
 *   char[]  source        (source_len bytes, no null)
 *   uint8   type_len
 *   char[]  type          (type_len bytes, no null)
 *   float[] embedding     (embed_dim floats)
 */
struct RagDB {
  std::vector<RagChunk> chunks;
  int embed_dim = 0;

  bool load(const std::string &path);
  bool save(const std::string &path);

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

