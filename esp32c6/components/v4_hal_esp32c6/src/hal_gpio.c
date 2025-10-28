/**
 * @file hal_gpio.c
 * @brief ESP32-C6 GPIO HAL implementation for V4 VM
 */

#include <stdio.h>

#include "driver/gpio.h"
#include "v4/v4_hal.h"
#include "v4_hal_esp32c6.h"

/**
 * @brief Initialize a GPIO pin
 *
 * @param pin GPIO pin number (0-30 for ESP32-C6, some reserved)
 * @param mode GPIO mode (input, output, pull-up, pull-down)
 * @return 0 on success, negative error code on failure
 */
v4_err v4_hal_gpio_init(int pin, v4_hal_gpio_mode mode)
{
  if (pin < 0 || pin >= GPIO_NUM_MAX)
  {
    return -1; // Invalid argument
  }

  gpio_config_t io_conf = {
      .pin_bit_mask = (1ULL << pin),
      .intr_type = GPIO_INTR_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .pull_up_en = GPIO_PULLUP_DISABLE,
  };

  switch (mode)
  {
    case V4_HAL_GPIO_MODE_INPUT:
      io_conf.mode = GPIO_MODE_INPUT;
      break;

    case V4_HAL_GPIO_MODE_OUTPUT:
      io_conf.mode = GPIO_MODE_OUTPUT;
      break;

    case V4_HAL_GPIO_MODE_INPUT_PULLUP:
      io_conf.mode = GPIO_MODE_INPUT;
      io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
      break;

    case V4_HAL_GPIO_MODE_INPUT_PULLDOWN:
      io_conf.mode = GPIO_MODE_INPUT;
      io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
      break;

    default:
      return -1; // Invalid mode
  }

  esp_err_t err = gpio_config(&io_conf);
  if (err != ESP_OK)
  {
    printf("ERROR: gpio_config failed for pin %d: %s\n", pin, esp_err_to_name(err));
    return -2; // IO error
  }

  return 0; // Success
}

/**
 * @brief Write a value to a GPIO pin
 *
 * @param pin GPIO pin number
 * @param value 0 for LOW, non-zero for HIGH
 * @return 0 on success, negative error code on failure
 */
v4_err v4_hal_gpio_write(int pin, int value)
{
  if (pin < 0 || pin >= GPIO_NUM_MAX)
  {
    return -1; // Invalid argument
  }

  esp_err_t err = gpio_set_level(pin, value ? 1 : 0);
  if (err != ESP_OK)
  {
    return -2; // IO error
  }

  return 0; // Success
}

/**
 * @brief Read a value from a GPIO pin
 *
 * @param pin GPIO pin number
 * @param out_value Pointer to store the read value (0 or 1)
 * @return 0 on success, negative error code on failure
 */
v4_err v4_hal_gpio_read(int pin, int *out_value)
{
  if (!out_value)
  {
    return -1; // Invalid argument
  }

  if (pin < 0 || pin >= GPIO_NUM_MAX)
  {
    return -1; // Invalid argument
  }

  int level = gpio_get_level(pin);
  *out_value = level;

  return 0; // Success
}
