# V4-ports

Hardware Abstraction Layer (HAL) implementations for V4 VM across various microcontrollers and SoCs.

## Overview

**V4-ports** provides platform-specific HAL implementations that enable V4 VM to run on embedded hardware. Each port implements the HAL API defined in [V4](https://github.com/kirisaki/v4).

### Architecture

```
V4-ports (HAL implementation, MCU-specific) ← This repository
    ↓ depends on
V4 (VM core, ISA, HAL API contract)
    ↓ depends on
V4-front (Forth → Bytecode compiler)

V4-repl (Interactive loop, optional)
```

## Supported Platforms

| Platform | Status | MCU/SoC | Architecture | Notes |
|----------|--------|---------|--------------|-------|
| **ESP32-C6** | ✅ Stable | ESP32-C6 | RISC-V 32-bit | NanoC6 reference |
| CH32V | 🚧 Planned | CH32V003/307 | RISC-V 32-bit | Coming soon |
| RP2350 | 🚧 Planned | RP2350 | ARM Cortex-M33 + RISC-V | Coming soon |

## ESP32-C6 Port

### Hardware Requirements

- **Board**: ESP32-C6 DevKit or NanoC6
- **CPU**: RISC-V 32-bit @ 160MHz
- **RAM**: 512KB SRAM
- **Flash**: 4MB (external)
- **Connectivity**: USB-CDC (UART0)

### Software Requirements

- **ESP-IDF**: v5.3 or later
- **CMake**: 3.16 or later
- **Python**: 3.8 or later
- **Toolchain**: RISC-V GCC (included in ESP-IDF)

### Quick Start

#### Option 1: Using Docker (Recommended)

```bash
# Clone repository
git clone https://github.com/kirisaki/v4-ports.git
cd v4-ports

# Clone dependencies (v4 and v4-front)
git clone https://github.com/kirisaki/v4.git
git clone https://github.com/kirisaki/v4-front.git

# Start Docker container
docker compose run --rm esp-idf

# Inside container: Build v4-blink example
cd esp32c6/examples/v4-blink
idf.py build

# Flash to device (connect USB cable)
idf.py flash monitor
```

#### Option 2: Native ESP-IDF

```bash
# Install ESP-IDF (if not already installed)
# See: https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/get-started/

# Clone repository and dependencies
git clone https://github.com/kirisaki/v4-ports.git
cd v4-ports
git clone https://github.com/kirisaki/v4.git
git clone https://github.com/kirisaki/v4-front.git

# Setup ESP-IDF environment
. $IDF_PATH/export.sh

# Build v4-blink example
cd esp32c6/examples/v4-blink
idf.py build

# Flash and monitor
idf.py flash monitor
```

### Examples

#### 1. v4-blink

Simple LED blink example to verify GPIO and Timer HAL.

```bash
cd esp32c6/examples/v4-blink
idf.py build flash monitor
```

**Features**:
- Toggles LED at 1Hz
- Prints status to serial console
- Demonstrates GPIO output and timer functions

**Hardware Setup**:
- Uses on-board LED on **GPIO7** (standard LED)
- **Note**: GPIO8 is WS2812 RGB LED (requires special driver)
- No external components required
- See `esp32c6/examples/v4-blink/README.md` for details

#### 2. v4-repl-demo

Interactive Forth REPL over UART.

```bash
cd esp32c6/examples/v4-repl-demo
idf.py build flash monitor
```

**Features**:
- Interactive Forth shell
- UART-based command input
- Compile and execute Forth code on-the-fly
- Stack inspection

**Example session**:
```
v4> 2 3 +
 => 5 (0x00000005)
v4> : SQUARE DUP * ;
ok
v4> 7 SQUARE
 => 49 (0x00000031)
v4>
```

#### 3. v4-link-demo

Bytecode transfer via V4-link protocol over USB Serial/JTAG.

```bash
cd esp32c6/examples/v4-link-demo
idf.py build flash monitor
```

**Features**:
- Frame-based bytecode transfer protocol
- CRC-8 error detection
- Commands: EXEC, PING, RESET
- Non-blocking UART communication
- 512B bytecode buffer, 4KB VM memory

**Protocol**:
```
Frame: [STX(0xA5)][LEN_L][LEN_H][CMD][DATA...][CRC8]
```

**Usage**:
- Send compiled bytecode from host PC
- Executes bytecode on V4 VM
- Suitable for remote code deployment
- See `esp32c6/examples/v4-link-demo/README.md` for Python host script example

## HAL API Implementation

The ESP32-C6 port implements the following V4 HAL APIs:

### GPIO Control
- `v4_hal_gpio_init()` - Initialize GPIO pin with mode (input/output/pullup/pulldown)
- `v4_hal_gpio_write()` - Write digital output (HIGH/LOW)
- `v4_hal_gpio_read()` - Read digital input state

### UART Control
- `v4_hal_uart_init()` - Initialize UART peripheral with baud rate
- `v4_hal_uart_putc()` - Send a single character
- `v4_hal_uart_getc()` - Receive a single character (non-blocking)
- `v4_hal_uart_write()` - Write data buffer to UART
- `v4_hal_uart_read()` - Read data from UART (non-blocking)

### Timer Control
- `v4_hal_delay_ms()` - Blocking delay in milliseconds
- `v4_hal_delay_us()` - Blocking delay in microseconds
- `v4_hal_millis()` - Get milliseconds since boot
- `v4_hal_micros()` - Get microseconds since boot

### System Control
- `v4_hal_system_reset()` - Reset the system
- `v4_hal_system_info()` - Get system information string

## Directory Structure

```
v4-ports/
├── esp32c6/                    # ESP32-C6 port
│   ├── components/
│   │   ├── v4_hal_esp32c6/    # HAL implementation
│   │   │   ├── CMakeLists.txt
│   │   │   ├── include/
│   │   │   │   └── v4_hal_esp32c6.h
│   │   │   └── src/
│   │   │       ├── hal_gpio.c
│   │   │       ├── hal_uart.c
│   │   │       ├── hal_timer.c
│   │   │       └── hal_system.c
│   │   └── v4_link/           # V4-link bytecode transfer
│   │       ├── CMakeLists.txt
│   │       ├── idf_component.yml
│   │       ├── v4_link_port.hpp
│   │       └── v4_link_port.cpp
│   └── examples/
│       ├── v4-blink/          # LED blink example
│       ├── v4-repl-demo/      # REPL example
│       └── v4-link-demo/      # Bytecode transfer example
├── .github/
│   └── workflows/
│       └── ci.yml             # GitHub Actions CI
├── docker-compose.yml         # Docker development environment
└── README.md
```

## Development

### Building with Docker

```bash
# Start container
docker compose run --rm esp-idf

# Build specific example
cd esp32c6/examples/v4-blink
idf.py build

# Clean build
idf.py fullclean
```

### Code Formatting

Before committing, format your code:

```bash
# Format all C/C++ and CMake files
make format

# Check formatting (without modifying files)
make format-check
```

**Requirements**:
- `clang-format` - C/C++ code formatting
- `cmake-format` - CMake files formatting

**Install tools**:
```bash
# Ubuntu/Debian
sudo apt-get install clang-format
pip install cmake-format

# macOS
brew install clang-format
pip install cmake-format

# Docker (ESP-IDF container)
pip install cmake-format
# clang-format is already included
```

### Adding a New HAL Function

1. Define the function in `v4/include/v4/v4_hal.h`
2. Implement it in `esp32c6/components/v4_hal_esp32c6/src/hal_*.c`
3. Update tests and examples
4. Run formatting and CI checks

## CI/CD

This repository uses GitHub Actions for continuous integration:

- **Build Check**: Builds all examples for ESP32-C6
- **Format Check**: Verifies code formatting with clang-format
- **Artifact Upload**: Stores compiled binaries

See `.github/workflows/ci.yml` for details.

## Troubleshooting

### Common Issues

**Problem**: `idf.py: command not found`
- **Solution**: Run `. $IDF_PATH/export.sh` to setup ESP-IDF environment

**Problem**: `v4/v4_hal.h: No such file or directory`
- **Solution**: Ensure v4 is cloned in the parent directory or set `V4_PATH`

**Problem**: UART not working on ESP32-C6
- **Solution**: ESP32-C6 uses USB-CDC for UART0. Connect via USB and use the CDC serial port.

**Problem**: Build fails with memory errors
- **Solution**: Increase `CONFIG_ESP_MAIN_TASK_STACK_SIZE` in `sdkconfig.defaults`

### Getting Help

- ESP-IDF Documentation: https://docs.espressif.com/projects/esp-idf/
- ESP32-C6 Datasheet: https://www.espressif.com/sites/default/files/documentation/esp32-c6_datasheet_en.pdf
- V4 Project Issues: https://github.com/kirisaki/v4-ports/issues

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Format code with `make format`
5. Verify formatting with `make format-check`
6. Submit a pull request

### Coding Standards

- Follow ESP-IDF coding style
- Use C language for HAL implementations
- Add comments for complex logic
- Update documentation as needed

## License

This project is dual-licensed under:

- Apache License 2.0 (`LICENSE-APACHE`)
- MIT License (`LICENSE-MIT`)

You may choose either license for your use.

## Related Projects

- [V4](https://github.com/kirisaki/v4) - VM core implementation
- [V4-front](https://github.com/kirisaki/v4-front) - Forth compiler frontend
- [V4-repl](https://github.com/kirisaki/v4-repl) - Interactive REPL
- [V4-link](https://github.com/V4-project/V4-link) - Bytecode transfer protocol

---

**Status**: Active Development | **Version**: 0.1.0 | **Last Updated**: 2025
