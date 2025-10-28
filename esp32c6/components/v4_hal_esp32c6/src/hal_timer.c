/**
 * @file hal_timer.c
 * @brief ESP32-C6 Timer HAL implementation for V4 VM
 */

#include <stdio.h>

#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "v4/v4_hal.h"
#include "v4_hal_esp32c6.h"

/**
 * @brief Delay execution for specified milliseconds
 *
 * @param ms Milliseconds to delay
 * @return V4_ERR_OK on success
 */
v4_err v4_hal_delay_ms(uint32_t ms)
{
  // Use FreeRTOS vTaskDelay for accurate timing
  TickType_t ticks = pdMS_TO_TICKS(ms);
  if (ticks == 0 && ms > 0)
  {
    ticks = 1;  // Minimum 1 tick delay
  }
  vTaskDelay(ticks);
  return V4_ERR_OK;
}

/**
 * @brief Delay execution for specified microseconds
 *
 * @param us Microseconds to delay
 * @return V4_ERR_OK on success
 */
v4_err v4_hal_delay_us(uint32_t us)
{
  // Use esp_rom_delay_us for short delays
  if (us == 0)
  {
    return V4_ERR_OK;
  }

  // For delays > 1ms, use vTaskDelay for better CPU efficiency
  if (us >= 1000)
  {
    return v4_hal_delay_ms(us / 1000);
  }

  // Busy-wait for short delays
  int64_t start = esp_timer_get_time();
  while ((esp_timer_get_time() - start) < us)
  {
    // Busy wait
  }

  return V4_ERR_OK;
}

/**
 * @brief Get current time in milliseconds
 *
 * @param ms Pointer to store milliseconds since boot
 * @return V4_ERR_OK on success, error code otherwise
 */
v4_err v4_hal_millis(uint32_t *ms)
{
  if (!ms)
  {
    return V4_ERR_INVALID_ARG;
  }

  // esp_timer_get_time() returns microseconds
  int64_t us = esp_timer_get_time();
  *ms = (uint32_t)(us / 1000);

  return V4_ERR_OK;
}

/**
 * @brief Get current time in microseconds
 *
 * @param us Pointer to store microseconds since boot
 * @return V4_ERR_OK on success, error code otherwise
 */
v4_err v4_hal_micros(uint64_t *us)
{
  if (!us)
  {
    return V4_ERR_INVALID_ARG;
  }

  *us = esp_timer_get_time();
  return V4_ERR_OK;
}

/**
 * @brief Get system tick count
 *
 * @param ticks Pointer to store tick count
 * @return V4_ERR_OK on success, error code otherwise
 */
v4_err v4_hal_get_ticks(uint32_t *ticks)
{
  if (!ticks)
  {
    return V4_ERR_INVALID_ARG;
  }

  *ticks = (uint32_t)xTaskGetTickCount();
  return V4_ERR_OK;
}

/**
 * @brief Get tick frequency in Hz
 *
 * @param freq_hz Pointer to store tick frequency
 * @return V4_ERR_OK on success, error code otherwise
 */
v4_err v4_hal_get_tick_freq(uint32_t *freq_hz)
{
  if (!freq_hz)
  {
    return V4_ERR_INVALID_ARG;
  }

  *freq_hz = (uint32_t)configTICK_RATE_HZ;
  return V4_ERR_OK;
}
