# V4-link Component for ESP-IDF Wraps V4-link bytecode transfer layer for ESP-IDF projects

# Detect V4-link path
if(DEFINED ENV{V4_LINK_PATH})
  set(V4_LINK_DIR "$ENV{V4_LINK_PATH}")
  message(STATUS "V4-link path (from V4_LINK_PATH): ${V4_LINK_DIR}")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../../V4-link")
  set(V4_LINK_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../V4-link")
  message(STATUS "V4-link found at: ${V4_LINK_DIR}")
else()
  # Fetch from GitHub if not found locally
  include(FetchContent)
  message(STATUS "V4-link not found locally, fetching from GitHub...")
  fetchcontent_declare(
    v4_link_src
    GIT_REPOSITORY https://github.com/V4-project/V4-link.git
    GIT_TAG main)
  fetchcontent_makeavailable(v4_link_src)
  set(V4_LINK_DIR "${v4_link_src_SOURCE_DIR}")
  message(STATUS "V4-link fetched to: ${V4_LINK_DIR}")
endif()

# V4-link source files
set(V4_LINK_SRCS "${V4_LINK_DIR}/src/link.cpp" "${V4_LINK_DIR}/src/link_c_api.cpp"
                 "${V4_LINK_DIR}/src/frame.cpp" "${V4_LINK_DIR}/src/crc8.cpp")

# Component port implementation
set(V4_LINK_PORT_SRCS "v4_link_port.cpp")

idf_component_register(
  SRCS
  ${V4_LINK_SRCS}
  ${V4_LINK_PORT_SRCS}
  INCLUDE_DIRS
  "${V4_LINK_DIR}/include"
  "."
  REQUIRES
  v4_core
  v4_hal
  driver
  esp_driver_uart)

# Compiler options
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Os)

# Disable ESP-IDF macro self-tests (incompatible with some GCC versions)
target_compile_definitions(${COMPONENT_LIB} PRIVATE "ESP_STATIC_ASSERT(condition,message)=")

# C++ standard (C++14 to avoid ESP-IDF macro test failures with C++17)
set_target_properties(
  ${COMPONENT_LIB}
  PROPERTIES CXX_STANDARD 14
             CXX_STANDARD_REQUIRED ON
             CXX_EXTENSIONS OFF)

# Disable C++ features for embedded (no exceptions, no RTTI)
target_compile_options(
  ${COMPONENT_LIB}
  PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions> $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
          $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
          $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>)

target_link_options(${COMPONENT_LIB} PRIVATE -fno-exceptions -fno-rtti)

# Component version
set(V4_LINK_VERSION "0.1.0")
target_compile_definitions(${COMPONENT_LIB} PRIVATE V4_LINK_VERSION="${V4_LINK_VERSION}")
