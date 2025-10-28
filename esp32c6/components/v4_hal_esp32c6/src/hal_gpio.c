/**
 * @file hal_gpio.c
 * @brief ESP32-C6 GPIO HAL implementation for V4 VM
 */

#include <stdio.h>

#include "driver/gpio.h"
#include "v4/v4_hal.h"
#include "v4_hal_esp32c6.h"

/**
 * @brief Set GPIO pin mode
 *
 * @param pin GPIO pin number (0-30 for ESP32-C6, some reserved)
 * @param mode GPIO mode (INPUT, OUTPUT, INPUT_PULLUP)
 * @return V4_ERR_OK on success, error code otherwise
 */
v4_err v4_hal_gpio_set_mode(int pin, V4_GPIO_MODE mode)
{
  if (pin < 0 || pin >= GPIO_NUM_MAX)
  {
    return V4_ERR_INVALID_ARG;
  }

  gpio_config_t io_conf = {
      .pin_bit_mask = (1ULL << pin),
      .intr_type = GPIO_INTR_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .pull_up_en = GPIO_PULLUP_DISABLE,
  };

  switch (mode)
  {
    case V4_GPIO_MODE_INPUT:
      io_conf.mode = GPIO_MODE_INPUT;
      break;

    case V4_GPIO_MODE_OUTPUT:
      io_conf.mode = GPIO_MODE_OUTPUT;
      break;

    case V4_GPIO_MODE_INPUT_PULLUP:
      io_conf.mode = GPIO_MODE_INPUT;
      io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
      break;

    case V4_GPIO_MODE_INPUT_PULLDOWN:
      io_conf.mode = GPIO_MODE_INPUT;
      io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
      break;

    default:
      return V4_ERR_INVALID_ARG;
  }

  esp_err_t err = gpio_config(&io_conf);
  if (err != ESP_OK)
  {
    printf("ERROR: gpio_config failed for pin %d: %s\n", pin, esp_err_to_name(err));
    return V4_ERR_IO;
  }

  return V4_ERR_OK;
}

/**
 * @brief Write digital value to GPIO pin
 *
 * @param pin GPIO pin number
 * @param value 0 for LOW, non-zero for HIGH
 * @return V4_ERR_OK on success, error code otherwise
 */
v4_err v4_hal_gpio_write(int pin, int value)
{
  if (pin < 0 || pin >= GPIO_NUM_MAX)
  {
    return V4_ERR_INVALID_ARG;
  }

  esp_err_t err = gpio_set_level(pin, value ? 1 : 0);
  if (err != ESP_OK)
  {
    return V4_ERR_IO;
  }

  return V4_ERR_OK;
}

/**
 * @brief Read digital value from GPIO pin
 *
 * @param pin GPIO pin number
 * @param value Pointer to store read value (0 or 1)
 * @return V4_ERR_OK on success, error code otherwise
 */
v4_err v4_hal_gpio_read(int pin, int *value)
{
  if (!value)
  {
    return V4_ERR_INVALID_ARG;
  }

  if (pin < 0 || pin >= GPIO_NUM_MAX)
  {
    return V4_ERR_INVALID_ARG;
  }

  int level = gpio_get_level(pin);
  *value = level;

  return V4_ERR_OK;
}

/**
 * @brief Toggle GPIO pin output
 *
 * @param pin GPIO pin number
 * @return V4_ERR_OK on success, error code otherwise
 */
v4_err v4_hal_gpio_toggle(int pin)
{
  int current_value;
  v4_err err = v4_hal_gpio_read(pin, &current_value);
  if (err != V4_ERR_OK)
  {
    return err;
  }

  return v4_hal_gpio_write(pin, !current_value);
}
