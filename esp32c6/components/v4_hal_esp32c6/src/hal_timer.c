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
 * @brief Get milliseconds since system startup
 *
 * @return Milliseconds since startup
 */
uint32_t v4_hal_millis(void)
{
  // esp_timer_get_time() returns microseconds
  int64_t us = esp_timer_get_time();
  return (uint32_t)(us / 1000);
}

/**
 * @brief Get microseconds since system startup
 *
 * @return Microseconds since startup
 */
uint64_t v4_hal_micros(void)
{
  return (uint64_t)esp_timer_get_time();
}

/**
 * @brief Blocking delay in milliseconds
 *
 * @param ms Milliseconds to delay
 */
void v4_hal_delay_ms(uint32_t ms)
{
  // Use FreeRTOS vTaskDelay for accurate timing
  TickType_t ticks = pdMS_TO_TICKS(ms);
  if (ticks == 0 && ms > 0)
  {
    ticks = 1;  // Minimum 1 tick delay
  }
  vTaskDelay(ticks);
}

/**
 * @brief Blocking delay in microseconds
 *
 * @param us Microseconds to delay
 */
void v4_hal_delay_us(uint32_t us)
{
  if (us == 0)
  {
    return;
  }

  // For delays > 1ms, use vTaskDelay for better CPU efficiency
  if (us >= 1000)
  {
    v4_hal_delay_ms(us / 1000);
    return;
  }

  // Busy-wait for short delays
  int64_t start = esp_timer_get_time();
  while ((esp_timer_get_time() - start) < us)
  {
    // Busy wait
  }
}
