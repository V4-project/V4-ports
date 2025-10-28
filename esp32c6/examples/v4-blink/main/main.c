/**
 * @file main.c
 * @brief V4 Blink Example for ESP32-C6
 *
 * Simple LED blink example to verify GPIO and Timer HAL implementation.
 * Toggles LED at 1Hz and prints status to serial console.
 */

#include "v4/v4_hal.h"
#include <stdio.h>
#include <inttypes.h>

// LED pin configuration for ESP32-C6 (NanoC6)
// Adjust this pin number according to your hardware
// Common pins: GPIO8 (on-board LED for some boards)
#define LED_PIN 8

// Blink interval in milliseconds
#define BLINK_INTERVAL_MS 500

void app_main(void) {
    printf("\n");
    printf("========================================\n");
    printf("V4 Blink Example - ESP32-C6\n");
    printf("========================================\n");
    printf("LED Pin: GPIO%d\n", LED_PIN);
    printf("Blink Interval: %d ms\n", BLINK_INTERVAL_MS);
    printf("========================================\n\n");

    // Initialize GPIO pin for LED output
    v4_err err = v4_hal_gpio_set_mode(LED_PIN, V4_GPIO_MODE_OUTPUT);
    if (err != V4_ERR_OK) {
        printf("ERROR: Failed to initialize GPIO%d (error code: %d)\n", LED_PIN,
               err);
        return;
    }
    printf("GPIO%d initialized as OUTPUT\n\n", LED_PIN);

    // LED state
    int state = 0;
    uint32_t loop_count = 0;

    // Main blink loop
    while (1) {
        // Toggle LED state
        err = v4_hal_gpio_write(LED_PIN, state);
        if (err != V4_ERR_OK) {
            printf("ERROR: Failed to write GPIO%d\n", LED_PIN);
        }

        // Get current time
        uint32_t current_ms;
        err = v4_hal_millis(&current_ms);
        if (err != V4_ERR_OK) {
            printf("ERROR: Failed to get current time\n");
            current_ms = 0;
        }

        // Print status
        printf("[%6" PRIu32 "] LED %s | Time: %" PRIu32 " ms\n", loop_count,
               state ? "ON " : "OFF", current_ms);

        // Toggle state for next iteration
        state = !state;
        loop_count++;

        // Wait before next toggle
        err = v4_hal_delay_ms(BLINK_INTERVAL_MS);
        if (err != V4_ERR_OK) {
            printf("ERROR: Failed to delay\n");
        }
    }
}
