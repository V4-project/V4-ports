# V4-repl Component for ESP-IDF Wraps V4-repl (Forth REPL) for ESP-IDF build system

# Detect V4-repl path
if(DEFINED ENV{V4_REPL_PATH})
  set(V4_REPL_DIR "$ENV{V4_REPL_PATH}")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../V4-repl")
  set(V4_REPL_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../V4-repl")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../v4-repl")
  set(V4_REPL_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../v4-repl")
else()
  # Fetch from GitHub if not found locally
  include(FetchContent)
  message(STATUS "V4-repl not found locally, fetching from GitHub...")
  FetchContent_Declare(
    v4_repl_src
    GIT_REPOSITORY https://github.com/V4-project/V4-repl.git
    GIT_TAG main)
  FetchContent_MakeAvailable(v4_repl_src)
  set(V4_REPL_DIR "${v4_repl_src_SOURCE_DIR}")
  message(STATUS "V4-repl fetched to: ${V4_REPL_DIR}")
endif()

# Detect V4 path (needed for V4 headers)
if(DEFINED ENV{V4_PATH})
  set(V4_DIR "$ENV{V4_PATH}")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../V4")
  set(V4_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../V4")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../v4")
  set(V4_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../v4")
else()
  # Fetch from GitHub if not found locally
  include(FetchContent)
  message(STATUS "V4 not found locally, fetching from GitHub...")
  FetchContent_Declare(
    v4_src
    GIT_REPOSITORY https://github.com/V4-project/V4.git
    GIT_TAG main)
  FetchContent_MakeAvailable(v4_src)
  set(V4_DIR "${v4_src_SOURCE_DIR}")
  message(STATUS "V4 fetched to: ${V4_DIR}")
endif()

# Detect V4-front path (needed for V4-front headers)
if(DEFINED ENV{V4_FRONT_PATH})
  set(V4_FRONT_DIR "$ENV{V4_FRONT_PATH}")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../V4-front")
  set(V4_FRONT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../V4-front")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../v4-front")
  set(V4_FRONT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../v4-front")
else()
  # Fetch from GitHub if not found locally
  include(FetchContent)
  message(STATUS "V4-front not found locally, fetching from GitHub...")
  FetchContent_Declare(
    v4_front_src
    GIT_REPOSITORY https://github.com/V4-project/V4-front.git
    GIT_TAG main)
  FetchContent_MakeAvailable(v4_front_src)
  set(V4_FRONT_DIR "${v4_front_src_SOURCE_DIR}")
  message(STATUS "V4-front fetched to: ${V4_FRONT_DIR}")
endif()

message(STATUS "V4-repl using V4-repl directory: ${V4_REPL_DIR}")
message(STATUS "V4-repl using V4 directory: ${V4_DIR}")
message(STATUS "V4-repl using V4-front directory: ${V4_FRONT_DIR}")

# Register component with V4-repl sources Using minimal stub for debugging
idf_component_register(
  SRCS
  "repl_local_stub.c"
  INCLUDE_DIRS
  "${V4_REPL_DIR}/include"
  REQUIRES
  v4_core
  v4_front)

# Compiler options for V4-repl Temporarily disabled all custom flags for debugging
# target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Os  # Optimize for size
# -Wno-error=format  # ESP32-C6 uses long int for int32_t )
