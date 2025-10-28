#ifndef V4_HAL_ESP32C6_H
#define V4_HAL_ESP32C6_H

/**
 * @file v4_hal_esp32c6.h
 * @brief ESP32-C6 Hardware Abstraction Layer for V4 VM
 *
 * This header provides ESP32-C6 specific HAL implementation.
 * Implements the V4 HAL API defined in v4/v4_hal.h
 */

#ifdef __cplusplus
extern "C"
{
#endif

// HAL implementation version
#define V4_HAL_ESP32C6_VERSION "0.1.0"

// Platform identification
#define V4_HAL_PLATFORM_NAME "ESP32-C6"
#define V4_HAL_PLATFORM_ESP32C6 1

#ifdef __cplusplus
}
#endif

#endif  // V4_HAL_ESP32C6_H
