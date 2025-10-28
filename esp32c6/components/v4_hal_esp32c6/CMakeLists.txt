# ESP32-C6 HAL Component for V4 VM
# This component implements V4 HAL API for ESP32-C6

idf_component_register(
    SRCS
        "src/hal_gpio.c"
        "src/hal_uart.c"
        "src/hal_timer.c"
    INCLUDE_DIRS
        "include"
    PRIV_REQUIRES
        driver
        esp_timer
)

# Add V4-core include path if available
# Users should set V4_CORE_PATH to point to V4-core directory
# Example: cmake -DV4_CORE_PATH=/path/to/v4-core ..
if(DEFINED ENV{V4_CORE_PATH})
    target_include_directories(${COMPONENT_LIB} PUBLIC
        "$ENV{V4_CORE_PATH}/include"
    )
    message(STATUS "V4-core path: $ENV{V4_CORE_PATH}")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../v4-core/include")
    target_include_directories(${COMPONENT_LIB} PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/../../../v4-core/include"
    )
    message(STATUS "V4-core found at: ${CMAKE_CURRENT_LIST_DIR}/../../../v4-core")
else()
    message(WARNING "V4-core not found. Set V4_CORE_PATH environment variable or place v4-core at ${CMAKE_CURRENT_LIST_DIR}/../../../v4-core")
endif()

# Compiler options
target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wall
    -Wextra
    -Werror=implicit-function-declaration
)

# Component version
set(V4_HAL_ESP32C6_VERSION "0.1.0")
target_compile_definitions(${COMPONENT_LIB} PRIVATE
    V4_HAL_ESP32C6_VERSION="${V4_HAL_ESP32C6_VERSION}"
)
