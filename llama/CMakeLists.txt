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
set(GGML_NATIVE OFF CACHE BOOL "" FORCE)
set(GGML_METAL OFF CACHE BOOL "" FORCE) # Apple GPU API
set(GGML_OPENCL OFF CACHE BOOL "" FORCE) # Cross platform GPU API (AMD/Intel)
set(GGML_KOMPUTE OFF CACHE BOOL "" FORCE)  # Vulcan/more modern than OpenCL
set(GGML_SYCL OFF CACHE BOOL "" FORCE) # Intel
set(GGML_ACCELERATE OFF CACHE BOOL "" FORCE) # Apple

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
  llama-sb-rag.cpp
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
  # force dynamic libm
  -Wl,-Bdynamic,-lm
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
# nitro agent application
# (only built when notcurses is available)
# -----------------------------
find_package(PkgConfig QUIET)

# ── notcurses ─────────────────────────────────────────────────────────────
set(NC_FOUND FALSE)
set(NC_TARGET "")
if(DEFINED NOTCURSES_DIR)
  # Explicit path — create an imported target manually
  find_library(NC_LIB      NAMES notcurses      HINTS "${NOTCURSES_DIR}/lib" REQUIRED)
  find_library(NC_CORE_LIB NAMES notcurses-core HINTS "${NOTCURSES_DIR}/lib")
  add_library(notcurses_imported INTERFACE IMPORTED)
  target_include_directories(notcurses_imported INTERFACE "${NOTCURSES_DIR}/include")
  target_link_libraries(notcurses_imported INTERFACE ${NC_LIB})
  if(NC_CORE_LIB)
    target_link_libraries(notcurses_imported INTERFACE ${NC_CORE_LIB})
  endif()
  set(NC_TARGET notcurses_imported)
  set(NC_FOUND TRUE)
elseif(PkgConfig_FOUND)
  pkg_check_modules(NC QUIET IMPORTED_TARGET notcurses)
  if(NC_FOUND)
    set(NC_TARGET PkgConfig::NC)
  endif()
else()
  find_library(NC_LIB      NAMES notcurses)
  find_library(NC_CORE_LIB NAMES notcurses-core)
  if(NC_LIB AND NC_CORE_LIB)
    add_library(notcurses_imported INTERFACE IMPORTED)
    target_link_libraries(notcurses_imported INTERFACE ${NC_LIB} ${NC_CORE_LIB})
    set(NC_TARGET notcurses_imported)
    set(NC_FOUND TRUE)
  endif()
endif()

# ── libcurl ───────────────────────────────────────────────────────────────
# Try the modern CMake find-module first (ships with CMake ≥ 3.12).
# Fall back to pkg-config, then a raw library search.
set(CURL_FOUND_INTERNAL FALSE)
set(CURL_TARGET "")

if(DEFINED CURL_DIR)
  # Explicit path supplied by the user (-DCURL_DIR=...)
  find_library(CURL_LIB NAMES curl HINTS "${CURL_DIR}/lib" REQUIRED)
  find_path(CURL_INCLUDE NAMES curl/curl.h HINTS "${CURL_DIR}/include" REQUIRED)
  add_library(curl_imported INTERFACE IMPORTED)
  target_include_directories(curl_imported INTERFACE "${CURL_INCLUDE}")
  target_link_libraries(curl_imported INTERFACE ${CURL_LIB})
  set(CURL_TARGET curl_imported)
  set(CURL_FOUND_INTERNAL TRUE)
else()
  # CMake built-in (sets CURL::libcurl target when found)
  find_package(CURL QUIET)
  if(CURL_FOUND)
    set(CURL_TARGET CURL::libcurl)
    set(CURL_FOUND_INTERNAL TRUE)
  elseif(PkgConfig_FOUND)
    pkg_check_modules(CURL_PC QUIET IMPORTED_TARGET libcurl)
    if(CURL_PC_FOUND)
      set(CURL_TARGET PkgConfig::CURL_PC)
      set(CURL_FOUND_INTERNAL TRUE)
    endif()
  endif()
  if(NOT CURL_FOUND_INTERNAL)
    find_library(CURL_LIB NAMES curl)
    find_path(CURL_INCLUDE NAMES curl/curl.h)
    if(CURL_LIB AND CURL_INCLUDE)
      add_library(curl_imported INTERFACE IMPORTED)
      target_include_directories(curl_imported INTERFACE "${CURL_INCLUDE}")
      target_link_libraries(curl_imported INTERFACE ${CURL_LIB})
      set(CURL_TARGET curl_imported)
      set(CURL_FOUND_INTERNAL TRUE)
    endif()
  endif()
endif()

# ── nitro target ──────────────────────────────────────────────────────────
if(NC_FOUND)
  if(NOT CURL_FOUND_INTERNAL)
    message(WARNING "libcurl not found — TOOL:CURL will be unavailable. "
                    "Install libcurl-dev or set -DCURL_DIR=<prefix> to enable.")
  endif()

  message(STATUS "notcurses found — building nitro")
  add_executable(nitro
    nitro.cpp
  )
  target_include_directories(nitro PRIVATE
    ${LLAMA_DIR}/include
    ${LLAMA_DIR}/ggml/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
  )
  target_link_libraries(nitro PRIVATE
    llm
    llama
    ggml
    ${NC_TARGET}           # imported target carries include + lib paths
  )
  if(CURL_FOUND_INTERNAL)
    target_link_libraries(nitro PRIVATE ${CURL_TARGET})
    target_compile_definitions(nitro PRIVATE NITRO_HAVE_CURL=1)
  else()
    target_compile_definitions(nitro PRIVATE NITRO_HAVE_CURL=0)
  endif()
  set_target_properties(nitro PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
  )
else()
  message(STATUS "notcurses not found — skipping nitro (set -DNOTCURSES_DIR=... to enable)")
endif()

# -----------------------------
# Header preparation for RAG indexer
# -----------------------------
add_executable(chunk_headers
  chunk_headers.cpp
)

set_target_properties(chunk_headers PROPERTIES
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
