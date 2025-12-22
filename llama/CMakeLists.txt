cmake_minimum_required(VERSION 3.15)
project(llm C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

# -----------------------------
# Path to llama.cpp
# -----------------------------
set(LLAMA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp)

# -----------------------------
# FORCE CPU-only static builds
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

# CPU-only flags
set(GGML_OPENMP OFF CACHE BOOL "" FORCE)
set(GGML_CUDA OFF CACHE BOOL "" FORCE)
set(GGML_METAL OFF CACHE BOOL "" FORCE)
set(GGML_OPENCL OFF CACHE BOOL "" FORCE)
set(GGML_KOMPUTE OFF CACHE BOOL "" FORCE)
set(GGML_SYCL OFF CACHE BOOL "" FORCE)
set(GGML_ACCELERATE OFF CACHE BOOL "" FORCE)
set(GGML_NATIVE ON CACHE BOOL "" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

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
