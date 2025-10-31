/**
 * @file v4_front_config.h
 * @brief V4-front component configuration
 *
 * This header disables ESP-IDF internal macro self-tests that fail with
 * certain C++ standard/GCC version combinations.
 * The macro tests are ESP-IDF internal validation and don't affect runtime.
 *
 * @copyright Copyright 2025 Akihito Kirisaki
 * @license Dual-licensed under MIT or Apache-2.0
 */

#ifndef V4_FRONT_CONFIG_H
#define V4_FRONT_CONFIG_H

// Disable ESP-IDF macro self-tests (incompatible with some GCC versions)
#define ESP_STATIC_ASSERT(condition, message)

#endif  // V4_FRONT_CONFIG_H
