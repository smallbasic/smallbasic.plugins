/*
 * rag_index.cpp
 *
 * Reads chunks.jsonl produced by chunk_headers, embeds each chunk
 * using a GGUF embedding model via llama.h, saves a binary .db file.
 *
 * No fixed limits on chunk count or chunk length.
 *
 * Build:
 *   c++ -std=c++17 -o rag_index rag_index.cpp -lllama -lm
 *
 * Usage:
 *   ./rag_index \
 *     --model  nomic-embed-text-v1.5.Q4_K_M.gguf \
 *     --input  chunks.jsonl \
 *     --output notcurses.db
 */

#include "llama-sb.h"

#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/* ── tunables ──────────────────────────────────────────────── */
static constexpr int BATCH_SIZE = 512;
/* ─────────────────────────────────────────────────────────── */

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

struct Chunk {
  std::string         text;
  std::string         source;
  std::string         type;
  std::vector<float>  embedding;
};

/* ── tiny JSON string extractor ────────────────────────────── */
static bool json_get_string(const std::string &json,
                            const std::string &key,
                            std::string       &out) {
  std::string search = "\"" + key + "\":";
  size_t pos = json.find(search);
  if (pos == std::string::npos) return false;
  pos += search.size();
  while (pos < json.size() && json[pos] == ' ') ++pos;
  if (pos >= json.size() || json[pos] != '"') return false;
  ++pos; /* skip opening quote */
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

/* ── db save ───────────────────────────────────────────────── */
static bool save_db(const std::string        &path,
                    const std::vector<Chunk> &chunks,
                    int                       embed_dim) {
  std::ofstream f(path, std::ios::binary);
  if (!f) { std::cerr << "cannot open for write: " << path << "\n"; return false; }

  auto write32 = [&](uint32_t v) { f.write((char*)&v, 4); };
  auto write16 = [&](uint16_t v) { f.write((char*)&v, 2); };
  auto write8  = [&](uint8_t  v) { f.write((char*)&v, 1); };
  auto writestr = [&](const std::string &s, size_t max_len) {
    size_t len = std::min(s.size(), max_len);
    f.write(s.c_str(), (std::streamsize)len);
  };

  write32(0x52414744);              /* magic "RAGD" */
  write32(2);                       /* version      */
  write32((uint32_t)chunks.size()); /* n_chunks     */
  write32((uint32_t)embed_dim);     /* embed_dim    */

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

/* ── main ──────────────────────────────────────────────────── */
int main(int argc, char **argv) {
  std::string model_path;
  std::string input_path;
  std::string output_path = "corpus.db";

  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "--model")  && i+1 < argc) model_path  = argv[++i];
    if (!strcmp(argv[i], "--input")  && i+1 < argc) input_path  = argv[++i];
    if (!strcmp(argv[i], "--output") && i+1 < argc) output_path = argv[++i];
  }

  if (model_path.empty() || input_path.empty()) {
    std::cerr << "usage: rag_index --model <embed.gguf> "
      "--input <chunks.jsonl> [--output <db>]\n";
    return 1;
  }

  /* ── load embedding model ─────────────────────────────── */

  Llama llama;
  if (!llama.load_embedding_model(model_path)) {
    return 1;
  }

  int embed_dim = llama.get_embed_dim();
  std::cerr << "embedding dim: " << embed_dim << "\n";

  /* ── read and embed chunks ────────────────────────────── */
  std::vector<Chunk> chunks;
  std::ifstream fin(input_path);
  if (!fin) { std::cerr << "cannot open: " << input_path << "\n"; return 1; }

  std::string line;
  int skipped = 0;

  while (std::getline(fin, line)) {
    if (line.empty() || line[0] != '{') continue;

    Chunk c;
    if (!json_get_string(line, "text",   c.text)   ||
        !json_get_string(line, "source", c.source)) {
      ++skipped;
      continue;
    }
    json_get_string(line, "type", c.type);

    std::cerr << "\r[" << chunks.size() << "] embedding: "
              << c.text.substr(0, 40) << "...";

    if (!llama.embed_text(c.text, c.embedding, embed_dim)) {
      ++skipped;
      continue;
    }

    chunks.push_back(std::move(c));
  }
  std::cerr << "\n";
  std::cerr << "embedded " << chunks.size()
            << " chunks (" << skipped << " skipped)\n";

  /* ── save ─────────────────────────────────────────────── */
  if (!save_db(output_path, chunks, embed_dim)) {
    std::cerr << "failed to save db\n";
    return 1;
  }
  std::cerr << "saved → " << output_path << "\n";

  return 0;
}
