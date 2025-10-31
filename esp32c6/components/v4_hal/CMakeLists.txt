# V4 HAL Component for ESP-IDF This component wraps V4-hal library for use in ESP-IDF
# projects

# Find V4-hal directory
if(DEFINED ENV{V4_HAL_PATH})
  set(V4_HAL_DIR "$ENV{V4_HAL_PATH}")
  message(STATUS "V4-hal path (from V4_HAL_PATH): ${V4_HAL_DIR}")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../../V4-hal")
  set(V4_HAL_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../V4-hal")
  message(STATUS "V4-hal found at: ${V4_HAL_DIR}")
else()
  # Fetch from GitHub if not found locally
  include(FetchContent)
  message(STATUS "V4-hal not found locally, fetching from GitHub...")
  fetchcontent_declare(
    v4_hal_src
    GIT_REPOSITORY https://github.com/V4-project/V4-hal.git
    GIT_TAG main)
  fetchcontent_makeavailable(v4_hal_src)
  set(V4_HAL_DIR "${v4_hal_src_SOURCE_DIR}")
  message(STATUS "V4-hal fetched to: ${V4_HAL_DIR}")
endif()

# V4-hal source files
set(V4_HAL_SRCS
    "${V4_HAL_DIR}/src/common/hal_capabilities.cpp"
    "${V4_HAL_DIR}/src/common/hal_core.cpp"
    "${V4_HAL_DIR}/src/common/hal_error.cpp"
    "${V4_HAL_DIR}/src/bridge/hal_gpio_bridge.cpp"
    "${V4_HAL_DIR}/src/bridge/hal_uart_bridge.cpp"
    "${V4_HAL_DIR}/src/bridge/hal_timer_bridge.cpp"
    "${V4_HAL_DIR}/src/bridge/hal_console_bridge.cpp"
    "${V4_HAL_DIR}/ports/esp32/platform_esp32.cpp")

idf_component_register(
  SRCS
  ${V4_HAL_SRCS}
  INCLUDE_DIRS
  "${V4_HAL_DIR}/include"
  "${V4_HAL_DIR}/ports/esp32"
  PRIV_REQUIRES
  driver
  esp_driver_gpio
  esp_driver_uart
  esp_timer)

# Define HAL platform
target_compile_definitions(${COMPONENT_LIB} PRIVATE HAL_PLATFORM_ESP32)

# Set Platform type alias using Esp32Platform
target_compile_definitions(${COMPONENT_LIB}
                           PUBLIC HAL_PLATFORM_TYPE=v4::hal::Esp32Platform)

# Compiler options
target_compile_options(
  ${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror=implicit-function-declaration
                           -fno-exceptions -fno-rtti)

# Force-include config header to disable ESP-IDF macro self-tests
target_compile_options(${COMPONENT_LIB} PRIVATE -include
                                                ${CMAKE_CURRENT_LIST_DIR}/v4_hal_config.h)

# C++ standard
set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD_REQUIRED ON)

# Component version
set(V4_HAL_VERSION "0.2.0")
target_compile_definitions(${COMPONENT_LIB} PRIVATE V4_HAL_VERSION="${V4_HAL_VERSION}")
