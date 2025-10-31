# V4 Blink Example for ESP32-C6 Minimum required version for ESP-IDF v5.x
cmake_minimum_required(VERSION 3.16)

# Add component directories
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../components")

# Include ESP-IDF project configuration
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Define project
project(v4-blink)

# Force-include project config header globally to disable ESP-IDF macro tests
idf_build_set_property(COMPILE_OPTIONS
                       "-include;${CMAKE_CURRENT_LIST_DIR}/project_config.h" APPEND)
