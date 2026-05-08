--- # SmallBASIC Llama Module

A comprehensive SmallBASIC library module that bridges the scripting capabilities of SmallBASIC with the power of Llama.cpp Large Language Models. This project allows developers to create, configure, and interact with LLM instances directly within a SmallBASIC environment.

## Table of Contents
1. [System Requirements & CUDA Setup](#system-requirements--cuda-setup)
2. [Obtaining Models from Hugging Face](#obtaining-models-from-hugging-face)
3. [Architecture](#architecture)
4. [Features](#features)
5. [Usage Examples](#usage-examples)
6. [API Reference](#api-reference)
7. [Configuration Presets](#configuration-presets)

---

## System Requirements & CUDA Setup

For optimal performance, especially on NVIDIA hardware, the CUDA toolkit must be correctly configured.

### 1. Check NVIDIA Drivers
Ensure the NVIDIA open driver is installed and working:
```bash
nvidia-smi
```
If this command works, the proprietary driver is not strictly necessary for CUDA toolkit installation.

### 2. Add NVIDIA CUDA Repository
For Debian 12:
```bash
wget https://developer.download.nvidia.com/compute/cuda/repos/debian12/x86_64/cuda-keyring_1.1-1_all.deb
sudo dpkg -i cuda-keyring_1.1-1_all.deb
sudo apt update
```

### 3. Install CUDA Toolkit
Install only the toolkit (no driver replacement):
```bash
sudo apt install -y cuda-toolkit
```
This installs `nvcc`, headers, and runtime libraries.

### 4. Environment Variables
Add the following to your environment:
```bash
export PATH=/usr/local/cuda/bin:$PATH
export CUDAToolkit_ROOT=/usr/local/cuda
```
To make this permanent, add to `~/.bashrc` and source it.

### 5. Verify Installation
```bash
nvcc --version
```
Output should indicate the release version (e.g., release 12.4).

### 6. Build Configuration
When building the module, ensure the build directory is clean and configured for the CUDA backend:
```bash
rm -rf build
mkdir build
cd build
cmake -DLLAMA_BACKEND=CUDA ..
make -j$(nproc)
```
*Note: Fully static builds are not possible for CUDA; some `.so` libraries will remain dynamically linked.*

---

## Obtaining Models from Hugging Face

The `LLAMA` function expects a path to a model file (e.g., `gguf` format). Models can be obtained from the Hugging Face Hub.

### Method 1: Using `huggingface-cli` (Recommended)

1.  **Setup Environment**
    Create a virtual environment (optional but recommended) and install the CLI tool:
    ```bash
    pyenv virtualenv 3.10.13 hf-tools
    pyenv activate hf-tools
    pip install -U pip
    pip install huggingface_hub
    ```

2.  **Login**
    Authenticate with your Hugging Face account:
    ```bash
    huggingface-cli login
    ```
    (Follow the prompts to enter your token).

3.  **Download Model**
    Use the `huggingface-cli download` command to fetch the model directly to your desired directory.
    ```bash
    # Example: Download Llama-3-8B-Instruct
    huggingface-cli download meta-llama/Meta-Llama-3-8B-Instruct --include "*.gguf" --local-dir models/llama3-8b
    ```

    *Note: This command downloads all `.gguf` files associated with the repository into the `models/llama3-8b` folder.*

### Method 2: Using Python (`huggingface_hub`)

If you prefer a scriptable approach:
```python
from huggingface_hub import hf_hub_download

model_path = hf_hub_download(
    repo_id="meta-llama/Meta-Llama-3-8B-Instruct",
    filename="llama-3-8b-instruct.Q4_K_M.gguf", # Specify exact file if needed
    local_dir="models",
    local_dir_use_symlinks=False
)
```

Once the model file is in your `models` directory (or wherever specified), you can reference it in SmallBASIC:
```basic
llama = LLAMA("models/llama3-8b/llama-3-8b-instruct.Q4_K_M.gguf", 2048, 1024, -1, 0)
```

---

## Architecture

The module operates as a compiled library (`SBLIB`) exposing C++ functionality to SmallBASIC scripts.

### Core Components
1.  **Llama Instance Manager (`g_llama`)**:
    *   Stores active Llama models in a hash map keyed by ID.
    *   Supports initialization with custom context sizes, batch sizes, and GPU acceleration.
    *   Handles memory cleanup to prevent leaks.

2.  **Response Iterator (`g_llama_iter`)**:
    *   Manages the streaming response of an LLM.
    *   Provides token-by-token access to generated text.
    *   Tracks generation speed (`tokens/sec`) and remaining tokens.

3.  **Command Interface**:
    *   Exposes a set of SmallBASIC functions (callbacks) for configuration and interaction.

---

## Features

### Initialization
The `LLAMA` function creates a new model instance.
```basic
' Syntax: LLAMA(model_path, n_ctx, n_batch, n_gpu_layers, n_log_level)
' Example:
' llama = LLAMA("models/llama-7b.gguf", 2048, 1024, -1, 0)
```

### Configuration
Once an instance is created, various parameters can be adjusted dynamically:

*   **Temperature**: Controls randomness in generation.
*   **Top-K / Top-P**: Nucleus sampling parameters.
*   **Max Tokens**: Limits the length of the response.
*   **Penalties**: Frequency, presence, and repeat penalties to avoid repetition.
*   **Grammar**: Constrains output to specific patterns.

```basic
' Examples:
llama.set_temperature(0.8)
llama.set_max_tokens(50)
llama.set_penalty_repeat(0.8)
llama.set_seed(123)
```

### Interaction
The primary method of interaction is `add_message`, which sends a prompt to the model.

```basic
' Syntax: llama.add_message(role, content)
' Returns: An iterator object for the response.
response = llama.add_message("user", "Please describe a sunset in poetry.")
```

### Streaming Responses
The returned iterator allows real-time processing of the model's output:

*   `response.all()`: Returns the complete generated text.
*   `response.next()`: Retrieves the next token.
*   `response.has_next()`: Checks if more tokens are available.
*   `response.tokens_sec`: Calculates current generation speed.

```basic
' Example loop:
while response.has_next()
  print response.next()
  sleep 100
end while
```

---

## Usage Examples

### Factual Answers & Tool Use
*Best for: Summaries, code generation, technical queries.*
```basic
llama.set_max_tokens(150)
llama.set_temperature(0.0)
llama.set_top_k(1)
llama.set_top_p(0.0)
llama.set_min_p(0.0)
```

### Assistant / Q&A / Chat
*Best for: Conversational agents, explanations.*
```basic
llama.set_max_tokens(150)
llama.set_temperature(0.8)
llama.set_top_k(40)
llama.set_top_p(0.0)
llama.set_min_p(0.05)
```

### Creative Writing & Storytelling
*Best for: Fiction, poetry, imaginative tasks.*
```basic
llama.set_max_tokens(200)
llama.set_temperature(1.0)
llama.set_top_k(80)
llama.set_top_p(0.0)
llama.set_min_p(0.1)
```

### Technical & Conservative
*Best for: Documentation, logic, precise tasks.*
```basic
llama.set_max_tokens(150)
llama.set_temperature(0.6)
llama.set_top_k(30)
llama.set_top_p(0.0)
llama.set_min_p(0.02)
```

### Speed Optimized (CPU)
*Best for: Rapid iteration or low-resource environments.*
```basic
' llama.set_max_tokens(10)
' llama.set_temperature(0.7)
' llama.set_top_k(20)
' llama.set_top_p(0.0)
' llama.set_min_p(0.05)
```

---

## API Reference

### Class: Llama
| Method | Description |
| :--- | :--- |
| `add_stop(text)` | Adds a stop sequence to the generation. |
| `set_penalty_repeat(value)` | Sets repeat penalty (default 1.1). |
| `set_penalty_freq(value)` | Sets frequency penalty. |
| `set_penalty_present(value)` | Sets presence penalty. |
| `set_penalty_last_n(value)` | Sets penalty context size. |
| `set_max_tokens(value)` | Sets maximum output tokens. |
| `set_min_p(value)` | Sets minimum probability threshold. |
| `set_temperature(value)` | Sets generation temperature. |
| `set_top_k(value)` | Sets top-k sampling. |
| `set_top_p(value)` | Sets top-p sampling. |
| `set_grammar(text)` | Sets output grammar constraint. |
| `set_seed(value)` | Sets random seed for reproducibility. |
| `reset()` | Clears the current conversation context. |
| `add_message(role, content)` | Sends a message and returns an iterator. |

### Class: LlamaIter
| Method | Description |
| :--- | :--- |
| `all()` | Returns the full string of the response. |
| `has_next()` | Returns true if more tokens are available. |
| `next()` | Returns the next token string. |
| `tokens_sec` | Returns current tokens per second. |

---

## Repetition Control Strategies

### Conservative (Minimal Control)
*Use when occasional repetition is acceptable.*
```basic
llama.set_penalty_last_n(64)
llama.set_penalty_repeat(1.05)
```

### Balanced (Default)
*Recommended for general usage.*
```basic
llama.set_penalty_last_n(64)
llama.set_penalty_repeat(1.1)
```

### Aggressive (Strong Anti-Repetition)
*Use for long-form generation where repetition must be avoided.*
```basic
llama.set_penalty_last_n(128)
llama.set_penalty_repeat(1.2)
```

### Disabled
*Use when repetition is desired or irrelevant.*
```basic
llama.set_penalty_last_n(0)
llama.set_penalty_repeat(1.0)
```

---

## Conclusion
This module empowers SmallBASIC users to build sophisticated AI applications, from chatbots to creative writing tools, leveraging the efficiency of Llama.cpp within a familiar scripting paradigm. Proper configuration of CUDA and generation parameters ensures optimal performance and output quality. Models can be easily acquired via the Hugging Face Hub using standard CLI tools or Python scripts.
---
