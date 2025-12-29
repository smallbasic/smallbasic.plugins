1️⃣ Ensure nvidia-open driver is installed and working

Check:

``
nvidia-smi
``

If it works, your driver is fine — no need to install the proprietary driver.

2️⃣ Add NVIDIA CUDA repository

```
wget https://developer.download.nvidia.com/compute/cuda/repos/debian12/x86_64/cuda-keyring_1.1-1_all.deb
sudo dpkg -i cuda-keyring_1.1-1_all.deb
sudo apt update
```

This repo contains the latest CUDA toolkit for Debian 12.

3️⃣ Install CUDA Toolkit only (no driver replacement)
sudo apt install -y cuda-toolkit


This installs:

- nvcc compiler
- CUDA headers
- Runtime libraries (libcudart.so, etc.)

4️⃣ Add CUDA to your environment

```
export PATH=/usr/local/cuda/bin:$PATH
export CUDAToolkit_ROOT=/usr/local/cuda
```

Optional: add to ~/.bashrc to make it permanent:

```
echo 'export PATH=/usr/local/cuda/bin:$PATH' >> ~/.bashrc
echo 'export CUDAToolkit_ROOT=/usr/local/cuda' >> ~/.bashrc
source ~/.bashrc
```

Verify:

nvcc --version

Should show something like:

```
nvcc: NVIDIA (R) Cuda compiler driver
Cuda compilation tools, release 12.4, V12.4.105
```

5️⃣ Clean llama.cpp build directory

```
rm -rf build
mkdir build
cd build
```

6️⃣ Configure CMake for CUDA backend

```
cmake -DLLAMA_BACKEND=CUDA ..
```

You should now see:

-- CUDA detected – enabling GGML_CUDA

7️⃣ Build 

```
make -j$(nproc)
```

The binary will use CUDA acceleration

Note: fully static builds are not possible for CUDA; some .so libraries will remain dynamically linked (normal).

#  Generator settings

## factual answers, tools, summaries

```
llama.set_max_tokens(150)
llama.set_temperature(0.0)
llama.set_top_k(1)
llama.set_top_p(0.0)
llama.set_min_p(0.0)
```

## assistant, Q+A, explanations, chat

```
llama.set_max_tokens(150)
llama.set_temperature(0.8)
llama.set_top_k(40)
llama.set_top_p(0.0)
llama.set_min_p(0.05)
```

## creative, storytelling

```
llama.set_max_tokens(20)
llama.set_temperature(1.0)
llama.set_top_k(80)
llama.set_top_p(0.0)
llama.set_min_p(0.1)
```

## surprises

```
llama.set_max_tokens(200)
llama.set_temperature(1.2)
llama.set_top_k(120)
llama.set_top_p(0.0)
llama.set_min_p(0.15)
```

## technical, conservative

```
llama.set_max_tokens(150)
llama.set_temperature(0.6)
llama.set_top_k(30)
llama.set_top_p(0.0)
llama.set_min_p(0.02)
```

## speed optimised on CPU

```
' llama.set_max_tokens(10)
' llama.set_temperature(0.7)
' llama.set_top_k(20)
' llama.set_top_p(0.0)
' llama.set_min_p(0.05)
```

# Avoiding repetition

## Conservative - minimal repetition control

```
llama.set_penalty_last_n(64)
llama.set_penalty_repeat(1.05)
```

## Balanced - good default

```
set_penalty_last_n(64)
set_penalty_repeat(1.1)
```

## Aggressive - strong anti-repetition

```
set_penalty_last_n(128)
set_penalty_repeat(1.2)
```

## Disabled

```
llama.set_penalty_last_n(0)
llama.set_penalty_repeat(1.0)
```
