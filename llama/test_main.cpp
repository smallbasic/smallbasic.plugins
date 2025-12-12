#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#define LLAMA_BUILD_EXAMPLES
#include "llama.h"

int main() {
  const char *model_path = "model.gguf";

  // --- Model load ---
  llama_model_params model_params = llama_model_default_params();
  llama_model *model = llama_model_load_from_file(model_path, model_params);
  if (!model) {
    fprintf(stderr, "failed to load model\n");
    return 1;
  }

  const llama_vocab *vocab = llama_model_get_vocab(model);

  // --- Context creation ---
  llama_context_params ctx_params = llama_context_default_params();
  llama_context *ctx = llama_init_from_model(model, ctx_params);
  if (!ctx) {
    fprintf(stderr, "failed to create context\n");
    llama_model_free(model);
    return 1;
  }

  // --- Tokenize ---
  std::string prompt = "Hello, Llama!";

  int n_tokens = llama_tokenize(
                                vocab,
                                prompt.c_str(),
                                prompt.size(),
                                nullptr,
                                0,
                                true,   // add BOS
                                false
                                );

  if (n_tokens <= 0) return 1;

  std::vector<llama_token> tokens(n_tokens);

  llama_tokenize(
                 vocab,
                 prompt.c_str(),
                 prompt.size(),
                 tokens.data(),
                 n_tokens,
                 true,
                 false
                 );

  // --- Build batch for prompt ---
  llama_batch batch = llama_batch_init(tokens.size(), 0, 1);

  for (size_t i = 0; i < tokens.size(); i++) {
    batch.token[i]      = tokens[i];
    batch.pos[i]        = i;
    batch.seq_id[i]     = 0;
    batch.logits[i]     = false;
    batch.n_tokens++;
  }

  if (llama_decode(ctx, batch) != 0) {
    fprintf(stderr, "decode failed\n");
    return 1;
  }

  // --- Sampler ---
  llama_sampler *sampler = llama_sampler_init_greedy();

  // --- Generation loop ---
  for (int i = 0; i < 100; i++) {
    llama_token tok = llama_sampler_sample(sampler, ctx, 0);

    //std::string piece = llama_vocab_token_to_piece(vocab, tok);
    char piece_buf[512];
    int32_t piece_len = llama_token_to_piece(
                                             vocab,
                                             tok,
                                             piece_buf,
                                             sizeof(piece_buf),
                                             0,      // lstrip
                                             true    // print special tokens
                                             );

    if (piece_len > 0) {
      printf("%s", piece_buf);
    }

    // feed token back
    llama_batch next = llama_batch_init(1, 0, 1);
    next.token[0] = tok;
    next.pos[0]   = 0;
    next.seq_id[0]= 0;
    next.logits[0]= false;
    next.n_tokens = 1;

    if (llama_decode(ctx, next) != 0) break;
  }

  printf("\n");

  llama_sampler_free(sampler);
  llama_free(ctx);
  llama_model_free(model);
}
