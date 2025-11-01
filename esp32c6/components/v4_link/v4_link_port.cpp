/**
 * @file v4_link_port.cpp
 * @brief V4-link port implementation for ESP32-C6
 *
 * @copyright Copyright 2025 Akihito Kirisaki
 * @license Dual-licensed under MIT or Apache-2.0
 */

#include "v4_link_port.hpp"

#include <cassert>

#include "esp_log.h"

static const char* TAG = "v4_link_port";

namespace v4ports
{

Esp32c6LinkPort::Esp32c6LinkPort(Vm* vm, size_t buffer_size) : link_(nullptr)
{
  // Assert on null VM pointer (programming error)
  assert(vm != nullptr && "VM pointer must not be null");

  // USB Serial/JTAG configuration
  usb_serial_jtag_driver_config_t usb_config = {
      .tx_buffer_size = USB_BUF_SIZE,
      .rx_buffer_size = USB_BUF_SIZE,
  };

  // Install USB Serial/JTAG driver
  esp_err_t ret = usb_serial_jtag_driver_install(&usb_config);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "Failed to install USB Serial/JTAG driver: %s", esp_err_to_name(ret));
    abort();
  }

  // Create V4-link instance with USB Serial/JTAG write callback
  link_ = std::make_unique<v4::link::Link>(
      vm,
      [](const uint8_t* data, size_t len)
      {
        // Send data via USB Serial/JTAG
        int written = usb_serial_jtag_write_bytes((const char*)data, len, portMAX_DELAY);
        if (written < 0)
        {
          ESP_LOGE(TAG, "USB Serial/JTAG write failed");
        }
      },
      buffer_size);

  ESP_LOGI(TAG, "V4-link initialized on USB Serial/JTAG");
}

Esp32c6LinkPort::~Esp32c6LinkPort()
{
  usb_serial_jtag_driver_uninstall();
  ESP_LOGI(TAG, "USB Serial/JTAG driver uninstalled");
}

void Esp32c6LinkPort::poll()
{
  uint8_t buffer[128];

  // Read all available data from USB Serial/JTAG (non-blocking)
  int len = usb_serial_jtag_read_bytes(buffer, sizeof(buffer), 0);

  if (len > 0)
  {
    // Feed each byte to V4-link
    for (int i = 0; i < len; ++i)
    {
      link_->feed_byte(buffer[i]);
    }
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
