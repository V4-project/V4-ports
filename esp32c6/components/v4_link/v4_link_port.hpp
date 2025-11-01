/**
 * @file v4_link_port.hpp
 * @brief V4-link port for ESP32-C6
 *
 * Wraps V4-link with ESP-IDF UART driver for easy integration.
 *
 * @copyright Copyright 2025 Akihito Kirisaki
 * @license Dual-licensed under MIT or Apache-2.0
 */

#pragma once

#include <memory>

#include "driver/usb_serial_jtag.h"
#include "v4/vm_api.h"
#include "v4link/link.hpp"

namespace v4ports
{

/**
 * @brief ESP32-C6 V4-link port
 *
 * Provides easy-to-use interface for V4-link on ESP32-C6.
 * Handles UART initialization and data transfer automatically.
 *
 * Example usage:
 * @code
 * // Create VM
 * uint8_t vm_memory[4096];
 * VmConfig cfg = {vm_memory, sizeof(vm_memory), nullptr, 0, nullptr};
 * Vm* vm = vm_create(&cfg);
 *
 * // Create V4-link port
 * v4ports::Esp32c6LinkPort link(vm);
 *
 * // Main loop
 * while (true) {
 *   link.poll();
 *   vTaskDelay(pdMS_TO_TICKS(1));
 * }
 * @endcode
 */
class Esp32c6LinkPort
{
 public:
  /**
   * @brief Construct ESP32-C6 link port
   *
   * Initializes USB Serial/JTAG and V4-link layer.
   *
   * @param vm           Pointer to initialized V4 VM
   * @param buffer_size  Bytecode buffer size (default: 512)
   *
   * @throws std::runtime_error if USB Serial/JTAG initialization fails
   */
  Esp32c6LinkPort(Vm* vm, size_t buffer_size = 512);

  /**
   * @brief Destructor
   *
   * Cleans up USB Serial/JTAG driver.
   */
  ~Esp32c6LinkPort();

  // Non-copyable
  Esp32c6LinkPort(const Esp32c6LinkPort&) = delete;
  Esp32c6LinkPort& operator=(const Esp32c6LinkPort&) = delete;

  /**
   * @brief Poll for incoming data and process
   *
   * Call this from main loop. Non-blocking.
   * Reads all available USB Serial/JTAG data and feeds to V4-link.
   */
  void poll();

  /**
   * @brief Reset VM to initial state
   *
   * Clears VM stacks and dictionary.
   */
  void reset();

  /**
   * @brief Get buffer capacity
   *
   * @return Maximum bytecode buffer size in bytes
   */
  size_t buffer_capacity() const;

 private:
  std::unique_ptr<v4::link::Link> link_;

  static constexpr size_t USB_BUF_SIZE = 1024;
};

}  // namespace v4ports
