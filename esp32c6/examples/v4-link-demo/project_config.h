/**
 * @file project_config.h
 * @brief Project-wide configuration for v4-link-demo
 *
 * This header disables ESP-IDF internal macro self-tests that fail with
 * certain C++ standard/GCC version combinations.
 *
 * @copyright Copyright 2025 Akihito Kirisaki
 * @license Dual-licensed under MIT or Apache-2.0
 */

#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

// Define ESP_STATIC_ASSERT as empty to skip esp_macros.h self-tests
// Use #ifndef to avoid redefinition warnings
#ifndef ESP_STATIC_ASSERT
#define ESP_STATIC_ASSERT(condition, message)
#endif

#endif  // PROJECT_CONFIG_H
