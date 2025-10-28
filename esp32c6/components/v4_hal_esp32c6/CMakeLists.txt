# ESP32-C6 HAL Component for V4 VM This component implements V4 HAL API for ESP32-C6

idf_component_register(
  SRCS
  "src/hal_gpio.c"
  "src/hal_uart.c"
  "src/hal_timer.c"
  "src/hal_system.c"
  INCLUDE_DIRS
  "include"
  PRIV_REQUIRES
  driver
  esp_timer)

# Add V4 include path if available Users should set V4_PATH to point to V4 directory
# Example: export V4_PATH=/path/to/v4 (then run idf.py build)
if(DEFINED ENV{V4_PATH})
  target_include_directories(${COMPONENT_LIB} PUBLIC "$ENV{V4_PATH}/include")
  message(STATUS "V4 path (from V4_PATH): $ENV{V4_PATH}")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../../V4/include")
  # Local development (uppercase V4)
  target_include_directories(${COMPONENT_LIB}
                             PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../../../../V4/include")
  message(STATUS "V4 found at: ${CMAKE_CURRENT_LIST_DIR}/../../../../V4")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../../v4/include")
  # CI environment (lowercase v4)
  target_include_directories(${COMPONENT_LIB}
                             PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../../../../v4/include")
  message(STATUS "V4 found at: ${CMAKE_CURRENT_LIST_DIR}/../../../../v4")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../V4/include")
  # Legacy path (uppercase V4, 3 levels up)
  target_include_directories(${COMPONENT_LIB}
                             PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../../../V4/include")
  message(STATUS "V4 found at: ${CMAKE_CURRENT_LIST_DIR}/../../../V4")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../v4/include")
  # Legacy path (lowercase v4, 3 levels up)
  target_include_directories(${COMPONENT_LIB}
                             PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../../../v4/include")
  message(STATUS "V4 found at: ${CMAKE_CURRENT_LIST_DIR}/../../../v4")
else()
  message(
    WARNING
      "V4 not found. Set V4_PATH environment variable or place V4/v4 at ${CMAKE_CURRENT_LIST_DIR}/../../../../{V4,v4}"
  )
endif()

# Compiler options
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra
                                                -Werror=implicit-function-declaration)

# Component version
set(V4_HAL_ESP32C6_VERSION "0.1.0")
target_compile_definitions(${COMPONENT_LIB}
                           PRIVATE V4_HAL_ESP32C6_VERSION="${V4_HAL_ESP32C6_VERSION}")
