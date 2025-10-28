/**
 * @file hal_system.c
 * @brief ESP32-C6 System HAL implementation for V4 VM
 */

#include <stdio.h>

#include "esp_system.h"
#include "v4/v4_hal.h"
#include "v4_hal_esp32c6.h"

/**
 * @brief Perform system reset
 *
 * Resets the ESP32-C6 microcontroller.
 * This function does not return.
 */
void v4_hal_system_reset(void)
{
  printf("System reset requested...\n");
  esp_restart();
  // Never returns
}

/**
 * @brief Get system information
 *
 * Returns platform identification string.
 *
 * @return Pointer to static string
 */
const char *v4_hal_system_info(void)
{
  return "ESP32-C6 (" V4_HAL_ESP32C6_VERSION ")";
}
