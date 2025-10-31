/**
 * @file main.c
 * @brief V4 Blink Example for ESP32-C6
 *
 * Simple LED blink example to verify GPIO and Timer HAL implementation.
 * Toggles LED at 1Hz and prints status to serial console.
 */

#include <inttypes.h>
#include <stdio.h>

#include "v4/hal.h"

// LED pin configuration for ESP32-C6
// GPIO7: Simple on-board LED (can be controlled with standard GPIO)
// GPIO8: WS2812 RGB LED (requires special driver)
#define LED_PIN 7

// Blink interval in milliseconds
#define BLINK_INTERVAL_MS 500

void app_main(void)
{
  printf("\n");
  printf("========================================\n");
  printf("V4 Blink Example - ESP32-C6\n");
  printf("========================================\n");
  printf("LED Pin: GPIO%d\n", LED_PIN);
  printf("Blink Interval: %d ms\n", BLINK_INTERVAL_MS);
  printf("========================================\n\n");

  // Initialize GPIO pin for LED output
  int err = hal_gpio_mode(LED_PIN, HAL_GPIO_OUTPUT);
  if (err != 0)
  {
    printf("ERROR: Failed to initialize GPIO%d (error code: %d)\n", LED_PIN, err);
    return;
  }
  printf("GPIO%d initialized as OUTPUT\n\n", LED_PIN);

  // LED state
  int state = 0;
  uint32_t loop_count = 0;

  // Main blink loop
  while (1)
  {
    // Toggle LED state
    err = hal_gpio_write(LED_PIN, state);
    if (err != 0)
    {
      printf("ERROR: Failed to write GPIO%d\n", LED_PIN);
    }

    // Get current time
    uint32_t current_ms = hal_millis();

    // Print status
    printf("[%6" PRIu32 "] LED %s | Time: %" PRIu32 " ms\n", loop_count,
           state ? "ON " : "OFF", current_ms);

    // Toggle state for next iteration
    state = !state;
    loop_count++;

    // Wait before next toggle
    hal_delay_ms(BLINK_INTERVAL_MS);
  }
}
