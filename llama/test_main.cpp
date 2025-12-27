#include "llama-sb.h"
#include <cstdio>
#include <cstring>

static void print_usage(int, char ** argv) {
  printf("\nexample usage:\n");
  printf("\n    %s -m model.gguf [-n n_predict] [-ngl n_gpu_layers] [prompt]\n", argv[0]);
  printf("\n");
}

int main(int argc, char ** argv) {
  // path to the model gguf file
  std::string model_path;
  // prompt to generate text from
  std::string prompt = "Happy friday";
  // number of tokens to predict
  int n_predict = 32;

  // parse command line arguments
  int i = 1;
  for (; i < argc; i++) {
    if (strcmp(argv[i], "-m") == 0) {
      if (i + 1 < argc) {
        model_path = argv[++i];
      } else {
        print_usage(argc, argv);
        return 1;
      }
    } else if (strcmp(argv[i], "-n") == 0) {
      if (i + 1 < argc) {
        try {
          n_predict = std::stoi(argv[++i]);
        } catch (...) {
          print_usage(argc, argv);
          return 1;
        }
      } else {
        print_usage(argc, argv);
        return 1;
      }
    } else {
      // prompt starts here
      break;
    }
  }
  if (model_path.empty()) {
    print_usage(argc, argv);
    return 1;
  }
  if (i < argc) {
    prompt = argv[i++];
    for (; i < argc; i++) {
      prompt += " ";
      prompt += argv[i];
    }
  }

  Llama llama;
  if (llama.construct(model_path, 1024, 1024)) {
    LlamaIter iter;
    llama.generate(iter, prompt);
    while (iter._has_next) {
      auto out = llama.next(iter);
      printf("\033[33m");
      printf("%s\n", out.c_str());
      printf("\n\033[0m");
    }
  } else {
    fprintf(stderr, "ERR: %s\n", llama.last_error());
  }

  return 0;
}
