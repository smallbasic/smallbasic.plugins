cmake_minimum_required(VERSION 3.15)
project(llm C CXX)

# clang-check ../*.cpp
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

# -----------------------------
# Path to llama.cpp
# -----------------------------
set(LLAMA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp)

set(LLAMA_BACKEND "AUTO" CACHE STRING "llama.cpp backend: AUTO, CPU, GPU, CUDA")
set_property(CACHE LLAMA_BACKEND PROPERTY STRINGS AUTO CPU GPU CUDA)

# -----------------------------
# FORCE static builds
# -----------------------------
# Disable all shared libraries globally
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# llama.cpp specific static settings
set(LLAMA_STATIC ON CACHE BOOL "" FORCE)
set(LLAMA_SHARED OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_LLAMA_SHARED OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_GGML_SHARED OFF CACHE BOOL "" FORCE)
set(LLAMA_SERVER_BUILD OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# ggml specific static settings
set(GGML_STATIC ON CACHE BOOL "" FORCE)
set(GGML_SHARED OFF CACHE BOOL "" FORCE)
set(GGML_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(GGML_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GGML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# -------------------------------
# Define backend options
# -------------------------------
set(LLAMA_BACKEND "AUTO" CACHE STRING "Select llama.cpp backend: AUTO, CPU, GPU, CUDA")
set_property(CACHE LLAMA_BACKEND PROPERTY STRINGS AUTO CPU GPU CUDA)

#
# sudo apt install nvidia-open cuda-toolkit
#

# -------------------------------
# Disable all accelerators by default
# -------------------------------
set(GGML_OPENMP OFF CACHE BOOL "" FORCE)
set(GGML_CUDA OFF CACHE BOOL "" FORCE)
set(GGML_METAL OFF CACHE BOOL "" FORCE)
set(GGML_OPENCL OFF CACHE BOOL "" FORCE)
set(GGML_KOMPUTE OFF CACHE BOOL "" FORCE)
set(GGML_SYCL OFF CACHE BOOL "" FORCE)
set(GGML_ACCELERATE OFF CACHE BOOL "" FORCE)
set(GGML_NATIVE OFF CACHE BOOL "" FORCE)  # default off

# -------------------------------
# Configure backends based on LLAMA_BACKEND
# -------------------------------
include(CheckLanguage)

if(LLAMA_BACKEND STREQUAL "CPU")
  message(STATUS "llama.cpp backend: CPU-only")
  set(GGML_NATIVE ON CACHE BOOL "" FORCE)  # enable CPU SIMD optimizations

elseif(LLAMA_BACKEND STREQUAL "GPU")
  message(STATUS "llama.cpp backend: GPU (non-CUDA)")
  set(GGML_OPENMP ON CACHE BOOL "" FORCE)  # parallel CPU fallback
  # GPU non-CUDA options can be added here in the future

elseif(LLAMA_BACKEND STREQUAL "CUDA")
  message(STATUS "llama.cpp backend: CUDA")

  check_language(CUDA)
  if(CMAKE_CUDA_COMPILER)
    enable_language(CUDA)
    set(GGML_CUDA ON CACHE BOOL "" FORCE)
  else()
    message(FATAL_ERROR "CUDA backend requested but nvcc not found")
  endif()

elseif(LLAMA_BACKEND STREQUAL "AUTO")
  message(STATUS "llama.cpp backend: AUTO")

  check_language(CUDA)
  if(CMAKE_CUDA_COMPILER)
    enable_language(CUDA)
    set(GGML_CUDA ON CACHE BOOL "" FORCE)
    message(STATUS "CUDA detected – enabling GGML_CUDA")
  else()
    set(GGML_OPENMP ON CACHE BOOL "" FORCE)
    set(GGML_NATIVE ON CACHE BOOL "" FORCE)
    message(STATUS "CUDA not found – using CPU/OpenMP")
  endif()

else()
  message(FATAL_ERROR "Invalid LLAMA_BACKEND value: ${LLAMA_BACKEND}")
endif()

# -----------------------------
# Add llama.cpp subdirectories
# -----------------------------
add_subdirectory(${LLAMA_DIR}/ggml)
add_subdirectory(${LLAMA_DIR})

# -----------------------------
# Build plugin as a shared library (.so)
# -----------------------------
set(PLUGIN_SOURCES
  main.cpp
  llama-sb.cpp
  ../include/param.cpp
  ../include/hashmap.cpp
  ../include/apiexec.cpp
)

add_library(llm SHARED ${PLUGIN_SOURCES})

target_include_directories(llm PRIVATE
  ${LLAMA_DIR}/include
  ${LLAMA_DIR}/ggml/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
  ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(llm PRIVATE
  llama
  ggml
)

# Include all static code into plugin
target_link_options(llm PRIVATE
  -Wl,--whole-archive
  $<TARGET_FILE:llama>
  $<TARGET_FILE:ggml>
  -Wl,--no-whole-archive
)

# Ensure position-independent code for .so
set_target_properties(llm PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# -----------------------------
# Optional test application
# -----------------------------
add_executable(llm_test
  test_main.cpp
)

target_include_directories(llm_test PRIVATE
  ${LLAMA_DIR}/include
  ${LLAMA_DIR}/ggml/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(llm_test PRIVATE
  llm
  llama
  ggml
)

set_target_properties(llm_test PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# ------------------------------------------------------------------
# Android native library
# ------------------------------------------------------------------
if (ANDROID)
  set(GGML_LLAMAFILE OFF CACHE BOOL "" FORCE)
  set(GGML_BLAS OFF CACHE BOOL "" FORCE)

  # CMake sets ANDROID when using the Android toolchain
  # Re‑use the same source files for the Android .so
  add_library(llm_android SHARED
    main.cpp
    llama-sb.cpp
    ../include/param.cpp
    ../include/hashmap.cpp
    ../include/apiexec.cpp
  )

  # Optional: set the SONAME / versioning if you need it
  set_target_properties(llm_android PROPERTIES
    OUTPUT_NAME "libllm"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${ANDROID_ABI}")

  target_link_libraries(llm_test PRIVATE
    log
    llm
    llama
    ggml
  )

  # Export the location so Gradle can copy it later
  set(MY_NATIVE_LIB_PATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${ANDROID_ABI}/libllm.so")
endif()
