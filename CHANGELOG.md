# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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

[0.1.0]: https://github.com/kirisaki/V4-ports/releases/tag/v0.1.0
