# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- V4-link protocol implementation for ESP32-C6 (bytecode transfer over USB Serial/JTAG)
- v4-link-demo example with PING, RESET, and EXEC commands
- Multiple LED blink pattern examples (slow, medium, fast, on, off, toggle, SOS)
- V4-link volume mount in docker-compose.yml
- dependencies.lock to .gitignore (ESP-IDF build artifact)

### Changed
- Migrated v4-link port from UART to USB Serial/JTAG driver (native USB on ESP32-C6)
- Simplified Esp32c6LinkPort constructor (no baud rate or pin configuration needed)
- Increased v4_link_send.py timeout from 2s to 5s for improved reliability

### Fixed
- **Critical:** GPIO_OUTPUT constant was incorrectly set to 1 (HAL_GPIO_INPUT_PULLUP) instead of 3 (HAL_GPIO_OUTPUT)
  - This caused GPIO pins to be configured as input with pull-up, resulting in dimly lit LEDs
  - GPIO control now works correctly with proper brightness
- Frame encoding in v4_link_send.py (struct.pack format correction)
- USB Serial/JTAG log interference by reducing log level to ERROR during V4-link operation
- **Critical:** Timeout parameter not propagated in v4_link_send.py
  - send_command() had hardcoded 1.0s timeout, ignoring the --timeout CLI argument
  - Bytecode execution >1s (e.g., 6×200ms delays = 1200ms) would timeout
  - Now properly passes timeout through cmd_ping(), cmd_exec(), and cmd_reset()

## [0.2.1] - 2025-11-01

### Fixed
- Control structure support in v4-repl-demo (IF/THEN/ELSE now work correctly)
- Word registration to compiler context enabling use of defined words in conditionals
- led-toggle implementation using VM memory state for proper on/off alternation
- Error message formatting with position indicators using v4front_format_error

## [0.2.0] - 2025-11-01

### Added
- V4-hal library integration for ESP32-C6 port
- CI workflow updates for V4-hal dependency checkout

### Changed
- Migrated ESP32-C6 port to use V4-hal library
- Updated v4-repl-demo to use new HAL API
- Component name updated from v4_hal_esp32c6 to v4_hal

### Fixed
- USB Serial/JTAG console for stdin in v4-repl-demo
- CI environment variables for build paths

## [0.1.0] - 2025-10-29

### Added
- ESP32-C6 HAL implementation for V4
- `v4-blink` example application for ESP32-C6
- `v4-repl-demo` example application for ESP32-C6
- V4 REPL support with USB Serial/JTAG and GPIO LED control
- Code formatting configuration (clang-format, cmake-format)
- CI/CD pipeline with formatting and linting checks
- Docker environment for consistent builds

### Fixed
- HAL implementation rewritten to match V4 API specification
- Docker volume mounts for V4 and V4-front directories
- CI environment variables for build paths
- Repository references updated

### Changed
- Project structure initialized with proper documentation

[0.2.1]: https://github.com/kirisaki/V4-ports/releases/tag/v0.2.1
[0.2.0]: https://github.com/kirisaki/V4-ports/releases/tag/v0.2.0
[0.1.0]: https://github.com/kirisaki/V4-ports/releases/tag/v0.1.0
