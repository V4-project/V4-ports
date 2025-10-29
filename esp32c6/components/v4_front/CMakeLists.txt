# V4-front Component for ESP-IDF Wraps V4-front (Forth compiler) for ESP-IDF build system

# Detect V4-front path
if(DEFINED ENV{V4_FRONT_PATH})
  set(V4_FRONT_DIR "$ENV{V4_FRONT_PATH}")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../V4-front")
  set(V4_FRONT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../V4-front")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../v4-front")
  set(V4_FRONT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../v4-front")
else()
  message(
    FATAL_ERROR
      "V4-front not found. Set V4_FRONT_PATH or place V4-front at ../../../v4-front")
endif()

# Detect V4 path (needed for V4 headers)
if(DEFINED ENV{V4_PATH})
  set(V4_DIR "$ENV{V4_PATH}")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../V4")
  set(V4_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../V4")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../v4")
  set(V4_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../v4")
else()
  message(FATAL_ERROR "V4 not found. Set V4_PATH or place V4 at ../../../v4")
endif()

message(STATUS "V4-front using V4-front directory: ${V4_FRONT_DIR}")
message(STATUS "V4-front using V4 directory: ${V4_DIR}")

# Register component with V4-front sources
idf_component_register(
  SRCS
  "${V4_FRONT_DIR}/src/compile.cpp"
  "${V4_FRONT_DIR}/src/bytecode_io.cpp"
  "${V4_FRONT_DIR}/src/disasm.cpp"
  INCLUDE_DIRS
  "${V4_FRONT_DIR}/include"
  "${V4_DIR}/include"
  REQUIRES
  v4_core)

# ESP32-specific memory optimization for embedded systems Reduces stack usage in
# compile_internal() from ~22KB to ~7KB
target_compile_definitions(
  ${COMPONENT_LIB}
  PRIVATE MAX_WORDS=64 # Reduce word dictionary: 19.5KB → 4.9KB
          MAX_CONTROL_DEPTH=16 # Reduce control stack: 2.2KB → 1.1KB
          MAX_TOKEN_LEN=128 # Reduce token buffer: 256B → 128B
)

# Compiler options for V4-front Note: Remove ESP-IDF's -Werror flags first, then add our
# own
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Os # Optimize for size
)

# Remove -Werror flags added by ESP-IDF (must be done AFTER setting standard flags)
get_target_property(COMPILE_OPTIONS ${COMPONENT_LIB} COMPILE_OPTIONS)
list(FILTER COMPILE_OPTIONS EXCLUDE REGEX "-Werror")
set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_OPTIONS "${COMPILE_OPTIONS}")

# Add permissive mode for C++ to allow typedef redefinition (v4front workaround)
target_compile_options(${COMPONENT_LIB} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fpermissive>)

# C++ standard and options
set_target_properties(
  ${COMPONENT_LIB}
  PROPERTIES CXX_STANDARD 17
             CXX_STANDARD_REQUIRED ON
             CXX_EXTENSIONS OFF)

# Disable C++ features that may cause initialization issues
target_compile_options(
  ${COMPONENT_LIB}
  PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions> $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
          $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
          $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>)

target_link_options(${COMPONENT_LIB} PRIVATE -fno-exceptions -fno-rtti)
