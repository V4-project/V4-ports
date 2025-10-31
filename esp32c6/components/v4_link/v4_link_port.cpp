/**
 * @file v4_link_port.cpp
 * @brief V4-link port implementation for ESP32-C6
 *
 * @copyright Copyright 2025 Akihito Kirisaki
 * @license Dual-licensed under MIT or Apache-2.0
 */

#include "v4_link_port.hpp"

#include <stdexcept>

#include "esp_log.h"

static const char* TAG = "v4_link_port";

namespace v4ports
{

Esp32c6LinkPort::Esp32c6LinkPort(Vm* vm, uart_port_t uart_num, uint32_t baud_rate,
                                 size_t buffer_size, int tx_pin, int rx_pin)
    : uart_num_(uart_num), link_(nullptr)
{
  if (vm == nullptr)
  {
    throw std::runtime_error("VM pointer is null");
  }

  // UART configuration
  uart_config_t uart_config = {
      .baud_rate = static_cast<int>(baud_rate),
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .rx_flow_ctrl_thresh = 0,
      .source_clk = UART_SCLK_DEFAULT,
  };

  // Install UART driver
  esp_err_t ret =
      uart_driver_install(uart_num_, UART_BUF_SIZE * 2, 0, UART_QUEUE_SIZE, nullptr, 0);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "Failed to install UART driver: %s", esp_err_to_name(ret));
    throw std::runtime_error("UART driver installation failed");
  }

  // Configure UART parameters
  ret = uart_param_config(uart_num_, &uart_config);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "Failed to configure UART: %s", esp_err_to_name(ret));
    uart_driver_delete(uart_num_);
    throw std::runtime_error("UART configuration failed");
  }

  // Set UART pins (if specified)
  if (tx_pin >= 0 || rx_pin >= 0)
  {
    ret = uart_set_pin(uart_num_, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (ret != ESP_OK)
    {
      ESP_LOGE(TAG, "Failed to set UART pins: %s", esp_err_to_name(ret));
      uart_driver_delete(uart_num_);
      throw std::runtime_error("UART pin configuration failed");
    }
  }

  // Create V4-link instance with UART write callback
  link_ = std::make_unique<v4::link::Link>(
      vm,
      [this](const uint8_t* data, size_t len)
      {
        // Send data via UART
        int written = uart_write_bytes(uart_num_, data, len);
        if (written < 0)
        {
          ESP_LOGE(TAG, "UART write failed");
        }
      },
      buffer_size);

  ESP_LOGI(TAG, "V4-link initialized on UART%d at %lu baud", uart_num_, baud_rate);
}

Esp32c6LinkPort::~Esp32c6LinkPort()
{
  if (uart_num_ >= 0)
  {
    uart_driver_delete(uart_num_);
    ESP_LOGI(TAG, "UART%d driver deleted", uart_num_);
  }
}

void Esp32c6LinkPort::poll()
{
  uint8_t buffer[128];

  // Read all available data from UART (non-blocking)
  int len = uart_read_bytes(uart_num_, buffer, sizeof(buffer), 0);

  if (len > 0)
  {
    // Feed each byte to V4-link
    for (int i = 0; i < len; ++i)
    {
      link_->feed_byte(buffer[i]);
    }
  }
  else if (len < 0)
  {
    ESP_LOGW(TAG, "UART read error");
  }
}

void Esp32c6LinkPort::reset()
{
  link_->reset();
  ESP_LOGI(TAG, "VM reset");
}

size_t Esp32c6LinkPort::buffer_capacity() const
{
  return link_->buffer_capacity();
}

}  // namespace v4ports
