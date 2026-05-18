# notcurses RAG — C++ Library Expert via llama.cpp

A self-contained RAG (Retrieval-Augmented Generation) pipeline in C++17
that turns a GGUF inference model into a focused expert on any C/C++ library.
Demonstrated here with [notcurses](https://github.com/dankamongmen/notcurses)
but works with any header-based library.

No fixed limits on chunk count, chunk length, or embedding dimension.
No Python, no vector database daemon, no external dependencies beyond llama.cpp.

---

## How it works

```
INDEXING (one-time offline)
────────────────────────────────────────────────────────────────
notcurses headers
      │
      ▼
chunk_headers        ← semantic chunker, outputs chunks.jsonl
      │
      ▼
rag_index            ← embeds each chunk via qwen3-embedding-0.6b-q4_k_m.gguf
      │
      ▼
notcurses.db         ← binary vector store (embeddings + text)


RUNTIME (each query)
────────────────────────────────────────────────────────────────
user query
      │
      ▼
rag_retrieve()       ← embeds query, cosine similarity against db
      │              ← skips chunks already seen this session
      ▼
new top-k chunks     ← most relevant unseen API fragments
      │
      ▼
prompt assembly      ← system + prior history + new context + query
      │
      ▼
Qwen3 inference      ← <|think|> reasoning + final answer
      │
      ▼
history              ← appended for next turn (KV cache intact)
```

---

## Files

| File | Purpose |
|---|---|
| `chunk_headers.cpp` | Parses C/C++ headers into semantic chunks, outputs `.jsonl` |
| `rag_index.cpp` | Reads `.jsonl`, embeds each chunk, saves binary `.db` |
| `rag.hpp` | Single-header C++17 runtime — load db, session, retrieve |
| `example.cpp` | Full pipeline wired together, multi-turn query loop |

---

## Dependencies

- [llama.cpp](https://github.com/ggerganov/llama.cpp) — `libllama` + `llama.h`
- A GGUF **inference model** — tested with `Qwen3.5-9B-Q4_K_M.gguf`
- A GGUF **embedding model** — `qwen3-embedding-0.6b-q4_k_m.gguf`
- C++17 compiler (gcc 8+, clang 7+, MSVC 2019+)

---

## Build

```bash
c++ -std=c++17 -o chunk_headers chunk_headers.cpp
c++ -std=c++17 -o rag_index     rag_index.cpp     -lllama -lm
c++ -std=c++17 -o example       example.cpp       -lllama -lm
```

If llama.cpp is not on your system library path:

```bash
c++ -std=c++17 -o rag_index rag_index.cpp \
    -I/path/to/llama.cpp/include \
    -L/path/to/llama.cpp/build -lllama -lm
```

---

## Usage

### Step 1 — Chunk the headers (one-time)

```bash
./chunk_headers notcurses/include/notcurses/ > chunks.jsonl
```

Accepts a single file or a directory (walked recursively).
Multiple paths can be given:

```bash
./chunk_headers include/foo.h include/bar.h src/examples/ > chunks.jsonl
```

Handles `.h`, `.hpp`, `.c`, `.cpp`. Inspect before indexing:

```bash
head -5 chunks.jsonl | python3 -m json.tool
```

### Step 2 — Embed and index (one-time)

```bash
./rag_index \
  --model qwen3-embedding-0.6b-q4_k_m.gguf \
  --input chunks.jsonl \
  --output notcurses.db
```

Takes a few minutes for a large corpus. The `.db` is reusable
until the library changes.

### Step 3 — Run

```bash
./example \
  --model Qwen3.5-9B-Q4_K_M.gguf \
  --embed qwen3-embedding-0.6b-q4_k_m.gguf \
  --db    notcurses.db
```

```
notcurses expert ready. ctrl+d to quit.

you: how do I create a plane and render text into it?
assistant: ...

you: what options does it take?       ← follow-up; no repeated context
assistant: ...
```

---

## Using rag.hpp in your own project

Single-header, stb-style. In **one** `.cpp` file:

```cpp
#define RAG_IMPLEMENTATION
#include "rag.hpp"
```

All other files that need the types:

```cpp
#include "rag.hpp"
```

### Minimal integration

```cpp
// startup
RagDB db;
rag_load(db, "notcurses.db");

RagSession session;
session.init(db.size(), 8192);   // n_chunks, your n_ctx
session.score_threshold = 0.60f;

// each turn
std::string context = rag_retrieve(db, embed_ctx, embed_model,
                                   user_query, 5, session);
// context is empty string if nothing new/relevant was found
// build prompt with context and hand to your inference context
```

### Stateless retrieval (no deduplication)

```cpp
std::string context = rag_retrieve(db, embed_ctx, embed_model,
                                   user_query, 5);
```

### API

```cpp
// Load .db file (version 2). Returns true on success.
bool rag_load(RagDB &db, const std::string &path);

// Retrieve with session deduplication + token budget.
// Returns context string ready to inject into prompt.
// Empty string if nothing new or relevant was found.
std::string rag_retrieve(const RagDB       &db,
                         llama_context     *embed_ctx,
                         llama_model       *embed_model,
                         const std::string &query,
                         int                top_k,
                         RagSession        &session);

// Stateless overload — no deduplication.
std::string rag_retrieve(const RagDB       &db,
                         llama_context     *embed_ctx,
                         llama_model       *embed_model,
                         const std::string &query,
                         int                top_k);
```

### RagSession fields

```cpp
struct RagSession {
  std::vector<bool> seen;         // one bit per chunk, sized to db
  int  tokens_used  = 0;          // running token estimate
  int  tokens_max   = 0;          // your n_ctx ceiling
  float score_threshold = 0.60f;  // skip weak matches

  void init(int n_chunks, int ctx_size);
  void reset();                   // start a fresh conversation
};
```

---

## Chunking strategy

`chunk_headers` uses a state machine that keeps each **semantic unit**
together as one chunk:

- Block comment (`/* ... */`) + following declaration
- `//` line comments + following declaration
- `typedef struct` / `typedef enum` entire body
- Consecutive `#define` macro groups
- Multi-line function signatures

Example — this stays as one chunk:

```c
// ncplane_create() - create a new plane as a child of 'n'.
// 'nopts' may be NULL for defaults. Returns NULL on error.
struct ncplane* ncplane_create(struct ncplane *n,
                               const struct ncplane_options *nopts);
```

---

## Session deduplication

The KV cache is not cleared between turns, so the model already has
earlier chunks in memory. `RagSession` tracks which chunks have been
injected and skips them on subsequent turns:

```
Turn 1: retrieved chunks [42, 17, 83]  → all new → inject all
Turn 2: retrieved chunks [42, 55, 17]  → 42,17 seen → inject only [55]
Turn 3: retrieved chunks [7, 14, 55]   → 55 seen → inject [7, 14]
```

Context window grows efficiently — no repeated API reference, and the
model remembers everything already seen via the intact KV cache.

---

## Adapting to other libraries

Change only the input to `chunk_headers`:

| Library | Input |
|---|---|
| stb (stb_image, stb_truetype ...) | single `.h` file |
| SDL2 / OpenGL / Vulkan | `include/` directory |
| Your own engine | any `.h` / `.hpp` mix |
| Spring / Java | extend chunker for Javadoc + `.java` |

Re-run steps 1 and 2 to produce a new `.db`. Runtime code unchanged.
Multiple `.db` files can be loaded and queried independently.

---

## .db file format (version 2)

Variable-length fields — no wasted padding.

```
Header (16 bytes):
  uint32  magic      = 0x52414744  ("RAGD")
  uint32  version    = 2
  uint32  n_chunks
  uint32  embed_dim

Per chunk:
  uint32  text_len
  char[]  text              (text_len bytes, no null)
  uint16  source_len
  char[]  source            (source_len bytes, no null)
  uint8   type_len
  char[]  type              (type_len bytes, no null)
  float[] embedding         (embed_dim × 4 bytes)
```

---

## GPU memory

On an 8 GB GPU with `Qwen3.5-9B-Q4_K_M`:

| Component | VRAM |
|---|---|
| Inference model (Q4_K_M 9B) | ~5.5 GB |
| Embedding model (nomic Q4) | ~0.3 GB |
| KV cache (8k ctx, Q4_0 K/V) | ~0.5 GB |
| **Total** | **~6.3 GB** |

---

## Qwen3 thinking mode

The model emits `<|think|>...<|/think|>` before its answer.
`example.cpp` strips this with `strip_think()` before printing.
The think block improves RAG quality — the model explicitly reasons
over injected context chunks before answering.

To expose reasoning (useful for debugging retrieval quality), remove
the `strip_think()` call and print `raw` directly.
