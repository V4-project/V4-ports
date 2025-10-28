/**
 * @file hal_uart.c
 * @brief ESP32-C6 UART HAL implementation for V4 VM
 */

#include <stdio.h>
#include <string.h>

#include "driver/uart.h"
#include "v4/v4_hal.h"
#include "v4_hal_esp32c6.h"

// Default UART configuration
#define UART_PORT_NUM UART_NUM_0  // USB-CDC on ESP32-C6
#define UART_BUF_SIZE 1024
#define UART_RX_BUF_SIZE (UART_BUF_SIZE * 2)
#define UART_TX_BUF_SIZE 0  // No TX buffer (blocking)

static bool uart_initialized = false;

/**
 * @brief Initialize a UART port
 *
 * @param port UART port number (0-1 for ESP32-C6)
 * @param baudrate Baud rate (e.g., 115200)
 * @return 0 on success, negative error code on failure
 */
v4_err v4_hal_uart_init(int port, int baudrate)
{
  if (port < 0 || port >= UART_NUM_MAX)
  {
    return -1;  // Invalid argument
  }

  if (baudrate <= 0)
  {
    return -1;  // Invalid argument
  }

  // Skip re-initialization
  if (uart_initialized)
  {
    printf("UART already initialized, skipping\n");
    return 0;  // Success
  }

  uart_config_t uart_config = {
      .baud_rate = baudrate,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_DEFAULT,
  };

  esp_err_t err;

  // Configure UART parameters
  err = uart_param_config(UART_PORT_NUM, &uart_config);
  if (err != ESP_OK)
  {
    printf("ERROR: uart_param_config failed: %s\n", esp_err_to_name(err));
    return -2;  // IO error
  }

  // Set UART pins (use default pins for UART0/USB-CDC)
  err = uart_set_pin(UART_PORT_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,
                     UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  if (err != ESP_OK)
  {
    printf("ERROR: uart_set_pin failed: %s\n", esp_err_to_name(err));
    return -2;  // IO error
  }

  // Install UART driver
  err =
      uart_driver_install(UART_PORT_NUM, UART_RX_BUF_SIZE, UART_TX_BUF_SIZE, 0, NULL, 0);
  if (err != ESP_OK)
  {
    printf("ERROR: uart_driver_install failed: %s\n", esp_err_to_name(err));
    return -2;  // IO error
  }

  uart_initialized = true;
  printf("UART initialized: port=%d, baudrate=%d\n", port, baudrate);

  return 0;  // Success
}

/**
 * @brief Send a single character via UART
 *
 * @param port UART port number
 * @param c Character to send
 * @return 0 on success, negative error code on failure
 */
v4_err v4_hal_uart_putc(int port, char c)
{
  if (!uart_initialized)
  {
    return -3;  // Not ready
  }

  int written = uart_write_bytes(UART_PORT_NUM, &c, 1);
  if (written != 1)
  {
    return -2;  // IO error
  }

  return 0;  // Success
}

/**
 * @brief Receive a single character from UART
 *
 * @param port UART port number
 * @param out_c Pointer to store the received character
 * @return 0 on success, negative error code if no data or error
 */
v4_err v4_hal_uart_getc(int port, char *out_c)
{
  if (!out_c)
  {
    return -1;  // Invalid argument
  }

  if (!uart_initialized)
  {
    return -3;  // Not ready
  }

  // Non-blocking read (timeout = 0)
  int len = uart_read_bytes(UART_PORT_NUM, (uint8_t *)out_c, 1, 0);
  if (len <= 0)
  {
    return -4;  // No data available
  }

  return 0;  // Success
}

/**
 * @brief Send a buffer via UART
 *
 * @param port UART port number
 * @param buf Pointer to data buffer
 * @param len Number of bytes to send
 * @return 0 on success, negative error code on failure
 */
v4_err v4_hal_uart_write(int port, const char *buf, int len)
{
  if (!buf || len <= 0)
  {
    return -1;  // Invalid argument
  }

  if (!uart_initialized)
  {
    return -3;  // Not ready
  }

  int written = uart_write_bytes(UART_PORT_NUM, buf, len);
  if (written != len)
  {
    printf("ERROR: uart_write_bytes failed: expected %d, wrote %d\n", len, written);
    return -2;  // IO error
  }

  return 0;  // Success
}

/**
 * @brief Receive data from UART
 *
 * @param port UART port number
 * @param buf Pointer to destination buffer
 * @param max_len Maximum number of bytes to read
 * @param out_len Pointer to store actual bytes read
 * @return 0 on success, negative error code on failure
 */
v4_err v4_hal_uart_read(int port, char *buf, int max_len, int *out_len)
{
  if (!buf || !out_len || max_len <= 0)
  {
    return -1;  // Invalid argument
  }

  if (!uart_initialized)
  {
    *out_len = 0;
    return -3;  // Not ready
  }

  // Non-blocking read (timeout = 0)
  int len = uart_read_bytes(UART_PORT_NUM, (uint8_t *)buf, max_len, 0);
  *out_len = (len >= 0) ? len : 0;

  return 0;  // Success
}
