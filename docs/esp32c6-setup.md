# ESP32-C6 Setup Guide

This guide will help you set up your development environment for ESP32-C6 and run V4 VM examples.

## Table of Contents

1. [Hardware Setup](#hardware-setup)
2. [Software Installation](#software-installation)
3. [Building Examples](#building-examples)
4. [Flashing and Debugging](#flashing-and-debugging)
5. [Pin Configuration](#pin-configuration)
6. [Troubleshooting](#troubleshooting)

## Hardware Setup

### Required Hardware

- **ESP32-C6 Development Board** (e.g., ESP32-C6-DevKitC-1 or NanoC6)
- **USB-C Cable** (for power and programming)
- **LED** (optional, for v4-blink example)
- **Resistor** 220Ω-1kΩ (for LED current limiting)

### Board Connections

#### NanoC6 Board

```
┌─────────────────┐
│   ESP32-C6      │
│                 │
│  [USB-C Port]   │ ← Connect to PC
│                 │
│  GPIO8 ─────┐   │
│             │   │
│  GND ───────┤   │
└─────────────┼───┘
              │
           [LED]
              │
          [Resistor]
              │
             GND
```

#### Pin Assignments

| Function | GPIO | Notes |
|----------|------|-------|
| LED (default) | GPIO8 | On-board LED on some boards |
| UART0 TX | USB-CDC | Virtual serial port |
| UART0 RX | USB-CDC | Virtual serial port |
| Alternative LED | GPIO2, GPIO10 | Can be configured in code |

## Software Installation

### Option 1: Docker (Recommended)

#### Prerequisites

- Docker Desktop or Docker Engine
- Docker Compose

#### Steps

1. **Install Docker**

   - **Linux**: `sudo apt install docker.io docker-compose`
   - **macOS**: Install Docker Desktop from [docker.com](https://www.docker.com/products/docker-desktop)
   - **Windows**: Install Docker Desktop with WSL2 backend

2. **Verify Installation**

   ```bash
   docker --version
   docker compose version
   ```

3. **Clone Repository**

   ```bash
   git clone https://github.com/your-org/v4-ports.git
   cd v4-ports

   # Clone dependencies
   git clone https://github.com/your-org/v4-core.git
   git clone https://github.com/your-org/v4-front.git
   ```

4. **Start Container**

   ```bash
   docker compose run --rm esp-idf
   ```

   You should now be inside the ESP-IDF container with all tools available.

### Option 2: Native ESP-IDF Installation

#### Prerequisites

- Linux, macOS, or Windows with WSL2
- Git
- Python 3.8 or later
- CMake 3.16 or later

#### Steps (Linux/macOS)

1. **Install Prerequisites**

   ```bash
   # Ubuntu/Debian
   sudo apt-get install git wget flex bison gperf python3 python3-pip \
        python3-venv cmake ninja-build ccache libffi-dev libssl-dev \
        dfu-util libusb-1.0-0

   # macOS
   brew install cmake ninja dfu-util
   ```

2. **Install ESP-IDF**

   ```bash
   mkdir -p ~/esp
   cd ~/esp
   git clone --recursive https://github.com/espressif/esp-idf.git
   cd esp-idf
   git checkout v5.3
   ./install.sh esp32c6
   ```

3. **Setup Environment**

   Add to your `~/.bashrc` or `~/.zshrc`:

   ```bash
   alias get_idf='. $HOME/esp/esp-idf/export.sh'
   ```

   Then run:

   ```bash
   get_idf
   ```

4. **Verify Installation**

   ```bash
   idf.py --version
   # Should show ESP-IDF v5.3.x
   ```

## Building Examples

### v4-blink Example

1. **Navigate to Example**

   ```bash
   cd esp32c6/examples/v4-blink
   ```

2. **Configure Project (Optional)**

   ```bash
   idf.py menuconfig
   ```

   Key settings:
   - `Component config → ESP System Settings → Main task stack size` (default: 4096)
   - `Serial flasher config → Flash size` (4MB recommended)

3. **Build**

   ```bash
   idf.py build
   ```

   First build takes 5-10 minutes. Subsequent builds are much faster.

### v4-repl-demo Example

1. **Ensure Dependencies are Available**

   ```bash
   # v4-core and v4-front should be in parent directory
   ls ../../v4-core
   ls ../../v4-front
   ```

2. **Navigate and Build**

   ```bash
   cd esp32c6/examples/v4-repl-demo
   idf.py build
   ```

## Flashing and Debugging

### Flashing

1. **Connect Board**

   Connect ESP32-C6 to PC via USB-C cable.

2. **Identify Serial Port**

   ```bash
   # Linux
   ls /dev/ttyUSB* /dev/ttyACM*

   # macOS
   ls /dev/cu.usbserial-* /dev/cu.usbmodem*

   # Or use ESP-IDF tool
   idf.py -p /dev/ttyUSB0 monitor
   ```

3. **Flash Firmware**

   ```bash
   idf.py flash

   # Or specify port
   idf.py -p /dev/ttyUSB0 flash
   ```

4. **Flash and Monitor in One Command**

   ```bash
   idf.py flash monitor
   ```

### Monitoring Serial Output

```bash
idf.py monitor

# Or with specific port
idf.py -p /dev/ttyUSB0 monitor
```

**Exit monitor**: Press `Ctrl+]`

### Common Flash Options

```bash
# Erase flash before flashing
idf.py erase-flash flash

# Increase flash baud rate
idf.py -b 921600 flash

# Full rebuild and flash
idf.py fullclean build flash monitor
```

## Pin Configuration

### Customizing LED Pin

Edit `esp32c6/examples/v4-blink/main/main.c`:

```c
// Change this line to your desired GPIO
#define LED_PIN 8  // Default
```

Common GPIO pins on ESP32-C6:

| Pin | Function | Safe for GPIO? |
|-----|----------|----------------|
| GPIO0-5 | Strapping pins | ⚠️ Use with caution |
| GPIO6-7 | Flash | ❌ Do not use |
| GPIO8-27 | General purpose | ✅ Safe |

### GPIO Constraints

**Do NOT use these pins**:
- GPIO6, GPIO7: Flash interface
- GPIO12, GPIO13: USB (if using USB-CDC)

**Strapping pins** (check state at boot):
- GPIO2, GPIO8, GPIO9: Pull-up/down affects boot mode

## Troubleshooting

### Issue: `idf.py: command not found`

**Solution**: ESP-IDF environment not activated.

```bash
# Docker: Already activated in container
# Native: Run this in every terminal session
. $HOME/esp/esp-idf/export.sh
```

### Issue: Serial port permission denied

**Solution**: Add user to dialout group (Linux).

```bash
sudo usermod -a -G dialout $USER
# Log out and back in
```

Or use sudo:

```bash
sudo idf.py flash monitor
```

### Issue: Build fails with "v4/v4_hal.h not found"

**Solution**: Ensure v4-core is in the correct location.

```bash
# Check path
ls ../../../v4-core/include/v4/v4_hal.h

# Or set environment variable
export V4_CORE_PATH=/path/to/v4-core
```

### Issue: UART not working

**Solution**: ESP32-C6 uses USB-CDC for UART0.

- Ensure USB cable supports data (not power-only)
- Try different USB port
- Check `dmesg | tail` (Linux) for connection messages

### Issue: LED not blinking

**Solution**: Check pin configuration and connections.

1. Verify GPIO pin number in code matches hardware
2. Check LED polarity (anode to GPIO, cathode to GND)
3. Measure GPIO voltage with multimeter (should toggle 0V ↔ 3.3V)

### Issue: "Failed to connect to ESP32-C6"

**Solutions**:

1. **Enter Download Mode Manually**
   - Hold BOOT button
   - Press RESET button
   - Release RESET, then BOOT
   - Retry flashing

2. **Check USB Cable**
   - Try different cable (data-capable)
   - Try different USB port

3. **Lower Baud Rate**
   ```bash
   idf.py -b 115200 flash
   ```

### Issue: Brownout detector triggered

**Solution**: Power supply issue.

- Use high-quality USB cable
- Try powered USB hub
- Disable brownout detector (not recommended):
  ```bash
  idf.py menuconfig
  # → Component config → ESP System Settings
  # → Uncheck "Hardware brownout detect"
  ```

## Advanced Topics

### Debugging with JTAG

ESP32-C6 has built-in USB-JTAG. No external debugger needed!

```bash
# In one terminal
openocd -f board/esp32c6-builtin.cfg

# In another terminal
riscv32-esp-elf-gdb build/v4-blink.elf
(gdb) target remote :3333
(gdb) mon reset halt
(gdb) break app_main
(gdb) continue
```

### Core Dumps

Enable core dumps for debugging crashes:

```bash
idf.py menuconfig
# → Component config → Core dump
# → Data destination: UART
```

After crash, decode with:

```bash
idf.py coredump-info
```

### Performance Profiling

```bash
# Enable profiling
idf.py menuconfig
# → Component config → Application Level Tracing

# Use heap tracing
idf.py monitor
# In serial console
heap_trace_start(HEAP_TRACE_LEAKS);
// ... your code ...
heap_trace_stop();
heap_trace_dump();
```

## Next Steps

- Try the [v4-repl-demo](../esp32c6/examples/v4-repl-demo) for interactive Forth
- Read [V4 HAL API Documentation](https://github.com/your-org/v4-core/docs/hal-api.md)
- Explore [ESP32-C6 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32-c6_technical_reference_manual_en.pdf)

## Additional Resources

- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/v5.3/esp32c6/)
- [ESP32-C6 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c6_datasheet_en.pdf)
- [ESP32-C6 Hardware Design Guidelines](https://www.espressif.com/sites/default/files/documentation/esp32-c6_hardware_design_guidelines_en.pdf)
- [Espressif Forums](https://www.esp32.com/)

---

**Need Help?** Open an issue at [github.com/your-org/v4-ports/issues](https://github.com/your-org/v4-ports/issues)
