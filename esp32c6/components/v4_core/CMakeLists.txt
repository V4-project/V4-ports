# V4 Core Component for ESP-IDF
# Wraps V4 VM core for ESP-IDF build system

# Detect V4 path
if(DEFINED ENV{V4_PATH})
  set(V4_DIR "$ENV{V4_PATH}")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../V4")
  set(V4_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../V4")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../v4")
  set(V4_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../v4")
else()
  message(FATAL_ERROR "V4 not found. Set V4_PATH or place V4 at ../../../v4")
endif()

message(STATUS "V4 core using V4 directory: ${V4_DIR}")

# Register component with V4 sources
idf_component_register(
  SRCS
  "${V4_DIR}/src/core.cpp"
  "${V4_DIR}/src/memory.cpp"
  "${V4_DIR}/src/arena.cpp"
  INCLUDE_DIRS
  "${V4_DIR}/include"
  REQUIRES
  v4_hal_esp32c6)

# Compiler options for V4
target_compile_options(${COMPONENT_LIB} PRIVATE
  -Wall
  -Wextra
  -Os  # Optimize for size
)

# C++ standard and options
set_target_properties(${COMPONENT_LIB} PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

# Disable C++ features that may cause initialization issues
target_compile_options(${COMPONENT_LIB} PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
)

target_link_options(${COMPONENT_LIB} PRIVATE
  -fno-exceptions
  -fno-rtti
)
