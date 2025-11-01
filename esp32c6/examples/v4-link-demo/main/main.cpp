/**
 * @file main.cpp
 * @brief V4-link Demo for ESP32-C6
 *
 * Demonstrates V4-link bytecode transfer over USB Serial/JTAG.
 * Receives compiled Forth bytecode from host and executes it on V4 VM.
 *
 * @copyright Copyright 2025 Akihito Kirisaki
 * @license Dual-licensed under MIT or Apache-2.0
 */

#include <stdio.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "v4/vm_api.h"
#include "v4_link_port.hpp"

static const char* TAG = "v4_link_demo";

// VM memory (4KB)
static uint8_t vm_memory[4096];

extern "C" void app_main()
{
  ESP_LOGI(TAG, "V4-link Demo starting...");
  ESP_LOGI(TAG, "ESP-IDF version: %s", esp_get_idf_version());

  // Initialize V4 VM
  VmConfig cfg = {
      .mem = vm_memory,
      .mem_size = sizeof(vm_memory),
      .mmio = nullptr,
      .mmio_count = 0,
      .arena = nullptr,
  };

  Vm* vm = vm_create(&cfg);
  if (vm == nullptr)
  {
    ESP_LOGE(TAG, "Failed to create VM");
    return;
  }

  ESP_LOGI(TAG, "V4 VM created (memory: %u bytes)", sizeof(vm_memory));

  try
  {
    // Initialize V4-link port
    // Uses USB Serial/JTAG
    v4ports::Esp32c6LinkPort link(vm, 512);

    ESP_LOGI(TAG, "V4-link ready on USB Serial/JTAG");
    ESP_LOGI(TAG, "Buffer capacity: %u bytes", link.buffer_capacity());
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "Waiting for bytecode from host...");
    ESP_LOGI(TAG, "Reducing log level to ERROR to avoid USB Serial/JTAG conflicts");
    ESP_LOGI(TAG, "");

    // Reduce log level to ERROR to minimize USB Serial/JTAG traffic
    // This prevents ESP-IDF log output from interfering with V4-link frames
    esp_log_level_set("*", ESP_LOG_ERROR);

    // Main loop: poll for incoming data
    while (true)
    {
      link.poll();

      // Small delay to prevent busy-wait
      vTaskDelay(pdMS_TO_TICKS(1));
    }
  }
  catch (const std::exception& e)
  {
    ESP_LOGE(TAG, "Exception: %s", e.what());
  }

  // Cleanup (never reached in normal operation)
  vm_destroy(vm);
}
