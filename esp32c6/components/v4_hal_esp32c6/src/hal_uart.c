/**
 * @file hal_uart.c
 * @brief ESP32-C6 UART HAL implementation for V4 VM
 */

#include "v4/v4_hal.h"
#include "v4_hal_esp32c6.h"
#include "driver/uart.h"
#include <stdio.h>
#include <string.h>

// Default UART configuration
#define UART_PORT_NUM UART_NUM_0 // USB-CDC on ESP32-C6
#define UART_BUF_SIZE 1024
#define UART_RX_BUF_SIZE (UART_BUF_SIZE * 2)
#define UART_TX_BUF_SIZE 0 // No TX buffer (blocking)

static bool uart_initialized = false;

/**
 * @brief Initialize UART peripheral
 *
 * @param port UART port number (0-1 for ESP32-C6)
 * @param baudrate Baud rate (e.g., 115200)
 * @return V4_ERR_OK on success, error code otherwise
 */
v4_err v4_hal_uart_init(int port, int baudrate) {
    if (port < 0 || port >= UART_NUM_MAX) {
        return V4_ERR_INVALID_ARG;
    }

    if (baudrate <= 0) {
        return V4_ERR_INVALID_ARG;
    }

    // Skip re-initialization
    if (uart_initialized) {
        printf("UART already initialized, skipping\n");
        return V4_ERR_OK;
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
    if (err != ESP_OK) {
        printf("ERROR: uart_param_config failed: %s\n", esp_err_to_name(err));
        return V4_ERR_IO;
    }

    // Set UART pins (use default pins for UART0/USB-CDC)
    err = uart_set_pin(UART_PORT_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,
                       UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (err != ESP_OK) {
        printf("ERROR: uart_set_pin failed: %s\n", esp_err_to_name(err));
        return V4_ERR_IO;
    }

    // Install UART driver
    err = uart_driver_install(UART_PORT_NUM, UART_RX_BUF_SIZE, UART_TX_BUF_SIZE,
                              0, NULL, 0);
    if (err != ESP_OK) {
        printf("ERROR: uart_driver_install failed: %s\n", esp_err_to_name(err));
        return V4_ERR_IO;
    }

    uart_initialized = true;
    printf("UART initialized: port=%d, baudrate=%d\n", port, baudrate);

    return V4_ERR_OK;
}

/**
 * @brief Write data to UART
 *
 * @param port UART port number
 * @param data Data buffer to write
 * @param len Length of data to write
 * @return V4_ERR_OK on success, error code otherwise
 */
v4_err v4_hal_uart_write(int port, const char *data, int len) {
    if (!data || len <= 0) {
        return V4_ERR_INVALID_ARG;
    }

    if (!uart_initialized) {
        return V4_ERR_NOT_READY;
    }

    int written = uart_write_bytes(UART_PORT_NUM, data, len);
    if (written != len) {
        printf("ERROR: uart_write_bytes failed: expected %d, wrote %d\n", len,
               written);
        return V4_ERR_IO;
    }

    return V4_ERR_OK;
}

/**
 * @brief Read data from UART (non-blocking)
 *
 * @param port UART port number
 * @param buf Buffer to store read data
 * @param max_len Maximum length to read
 * @param read_len Pointer to store actual read length
 * @return V4_ERR_OK on success, error code otherwise
 */
v4_err v4_hal_uart_read(int port, char *buf, int max_len, int *read_len) {
    if (!buf || !read_len || max_len <= 0) {
        return V4_ERR_INVALID_ARG;
    }

    if (!uart_initialized) {
        *read_len = 0;
        return V4_ERR_NOT_READY;
    }

    // Non-blocking read (timeout = 0)
    int len = uart_read_bytes(UART_PORT_NUM, (uint8_t *)buf, max_len, 0);
    *read_len = (len >= 0) ? len : 0;

    return V4_ERR_OK;
}

/**
 * @brief Get number of bytes available in UART RX buffer
 *
 * @param port UART port number
 * @param count Pointer to store available byte count
 * @return V4_ERR_OK on success, error code otherwise
 */
v4_err v4_hal_uart_available(int port, int *count) {
    if (!count) {
        return V4_ERR_INVALID_ARG;
    }

    if (!uart_initialized) {
        *count = 0;
        return V4_ERR_NOT_READY;
    }

    size_t available;
    esp_err_t err = uart_get_buffered_data_len(UART_PORT_NUM, &available);
    if (err != ESP_OK) {
        *count = 0;
        return V4_ERR_IO;
    }

    *count = (int)available;
    return V4_ERR_OK;
}

/**
 * @brief Flush UART TX buffer
 *
 * @param port UART port number
 * @return V4_ERR_OK on success, error code otherwise
 */
v4_err v4_hal_uart_flush(int port) {
    if (!uart_initialized) {
        return V4_ERR_NOT_READY;
    }

    esp_err_t err = uart_wait_tx_done(UART_PORT_NUM, portMAX_DELAY);
    if (err != ESP_OK) {
        return V4_ERR_IO;
    }

    return V4_ERR_OK;
}
