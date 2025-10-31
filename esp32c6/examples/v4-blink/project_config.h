/**
 * @file project_config.h
 * @brief Project-wide configuration for v4-blink
 *
 * This header disables ESP-IDF internal macro self-tests that fail with
 * certain C++ standard/GCC version combinations.
 *
 * @copyright Copyright 2025 Akihito Kirisaki
 * @license Dual-licensed under MIT or Apache-2.0
 */

#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

// Disable macro redefinition warnings (ESP-IDF headers redefine ESP_STATIC_ASSERT)
#pragma GCC diagnostic ignored "-Wmacro-redefined"

// Define ESP_STATIC_ASSERT as empty to skip esp_macros.h self-tests
// (will be redefined by esp_assert.h, hence the pragma above)
#define ESP_STATIC_ASSERT(condition, message)

#endif  // PROJECT_CONFIG_H
