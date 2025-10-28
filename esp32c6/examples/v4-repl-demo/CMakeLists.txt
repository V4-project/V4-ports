# V4 REPL Demo for ESP32-C6
# Minimum required version for ESP-IDF v5.x
cmake_minimum_required(VERSION 3.16)

# Add component directories
# Include v4_hal_esp32c6, v4-core, and v4-front components
set(EXTRA_COMPONENT_DIRS
    "${CMAKE_CURRENT_LIST_DIR}/../../components"
    "${CMAKE_CURRENT_LIST_DIR}/../../../v4-core"
    "${CMAKE_CURRENT_LIST_DIR}/../../../v4-front"
)

# Include ESP-IDF project configuration
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Define project
project(v4-repl-demo)
