# Troubleshooting Guide

Common issues and solutions for V4-ports ESP32-C6 development.

## Table of Contents

- [Build Issues](#build-issues)
- [Flash Issues](#flash-issues)
- [Runtime Issues](#runtime-issues)
- [HAL Issues](#hal-issues)
- [Development Environment](#development-environment)

---

## Build Issues

### Error: `v4/v4_hal.h: No such file or directory`

**Symptom**: Build fails with header not found error.

```
fatal error: v4/v4_hal.h: No such file or directory
 #include "v4/v4_hal.h"
          ^~~~~~~~~~~~~~
```

**Cause**: V4-core not found in expected location.

**Solutions**:

1. **Verify v4-core location**:
   ```bash
   ls ../../../v4-core/include/v4/v4_hal.h
   ```

2. **Set V4_CORE_PATH environment variable**:
   ```bash
   export V4_CORE_PATH=/path/to/v4-core
   idf.py build
   ```

3. **Check CMakeLists.txt** in your example:
   ```cmake
   set(EXTRA_COMPONENT_DIRS
       "${CMAKE_CURRENT_LIST_DIR}/../../components"
       "${CMAKE_CURRENT_LIST_DIR}/../../../v4-core"  # Verify this path
   )
   ```

---

### Error: `idf_component_register` command not found

**Symptom**: CMake configuration fails.

```
CMake Error: Unknown CMake command "idf_component_register"
```

**Cause**: ESP-IDF environment not properly activated.

**Solution**:

```bash
# Make sure ESP-IDF is sourced
. $IDF_PATH/export.sh

# Verify IDF_PATH
echo $IDF_PATH
# Should output: /path/to/esp-idf

# Re-run build
cd esp32c6/examples/v4-blink
idf.py build
```

---

### Error: Out of memory during compilation

**Symptom**: Compiler crashes or linker runs out of memory.

```
c++: fatal error: Killed signal terminated program cc1plus
```

**Cause**: Insufficient RAM, especially in Docker or VM.

**Solutions**:

1. **Limit parallel jobs**:
   ```bash
   idf.py build -j 2  # Use only 2 parallel jobs
   ```

2. **Increase Docker memory** (Docker Desktop):
   - Settings → Resources → Memory
   - Increase to at least 4GB

3. **Use optimization flags**:
   ```bash
   idf.py menuconfig
   # → Compiler options → Optimization level → Optimize for size (-Os)
   ```

---

### Error: `undefined reference to` V4 functions

**Symptom**: Linker fails with undefined references.

```
undefined reference to `v4_hal_gpio_set_mode'
```

**Cause**: HAL component not linked properly.

**Solution**: Check `main/CMakeLists.txt`:

```cmake
idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES v4_hal_esp32c6  # ← Ensure this is present
)
```

---

## Flash Issues

### Error: `Failed to connect to ESP32-C6`

**Symptom**: Flash command fails to connect to device.

```
A fatal error occurred: Failed to connect to ESP32-C6: No serial data received.
```

**Solutions**:

1. **Enter download mode manually**:
   - Hold **BOOT** button
   - Press and release **RESET** button
   - Release **BOOT** button
   - Retry: `idf.py flash`

2. **Check USB cable**:
   - Use data-capable cable (not power-only)
   - Try different USB port
   - Check with: `ls /dev/ttyUSB* /dev/ttyACM*` (Linux)

3. **Lower baud rate**:
   ```bash
   idf.py -b 115200 flash
   ```

4. **Check permissions** (Linux):
   ```bash
   sudo usermod -a -G dialout $USER
   # Log out and back in
   ```

---

### Error: Permission denied on serial port

**Symptom**: Cannot access `/dev/ttyUSB0` or similar.

```
serial.serialutil.SerialException: [Errno 13] could not open port /dev/ttyUSB0: [Errno 13] Permission denied
```

**Solutions**:

1. **Add user to dialout group** (Linux):
   ```bash
   sudo usermod -a -G dialout $USER
   newgrp dialout  # Or log out and back in
   ```

2. **Temporary fix with sudo**:
   ```bash
   sudo chmod 666 /dev/ttyUSB0
   ```

3. **Docker device access**:

   Edit `docker-compose.yml`:
   ```yaml
   services:
     esp-idf:
       devices:
         - /dev/ttyUSB0:/dev/ttyUSB0
       privileged: true
   ```

---

### Error: `A fatal error occurred: MD5 of file does not match data in flash`

**Symptom**: Flash verification fails.

**Cause**: Flash corruption or unstable power.

**Solutions**:

1. **Erase flash completely**:
   ```bash
   idf.py erase-flash
   idf.py flash
   ```

2. **Check power supply**:
   - Use high-quality USB cable
   - Try powered USB hub
   - Check for brownout resets in monitor

3. **Lower flash speed**:
   ```bash
   idf.py menuconfig
   # → Serial flasher config → Flash SPI speed
   # → Change to 40MHz or lower
   ```

---

## Runtime Issues

### Issue: Device continuously resets (boot loop)

**Symptom**: Serial monitor shows repeated boot messages.

```
rst:0x3 (SW_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
[... reset loop ...]
```

**Causes & Solutions**:

1. **Stack overflow**:
   ```bash
   idf.py menuconfig
   # → Component config → ESP System Settings
   # → Main task stack size → Increase to 8192
   ```

2. **Watchdog timeout**:
   - Check for infinite loops without delays
   - Add `v4_hal_delay_ms(10)` in tight loops

3. **Brownout**:
   - Check power supply
   - Disable brownout detector temporarily:
     ```bash
     idf.py menuconfig
     # → Component config → ESP System Settings
     # → Hardware brownout detect → Disable
     ```

---

### Issue: UART prints garbled characters

**Symptom**: Serial output is unreadable.

```
����������������
```

**Cause**: Baud rate mismatch.

**Solution**:

1. **Check monitor baud rate**:
   ```bash
   idf.py -b 115200 monitor
   ```

2. **Verify sdkconfig**:
   ```
   CONFIG_ESPTOOLPY_MONITOR_BAUD=115200
   ```

3. **Match UART init baud rate** in code:
   ```c
   v4_hal_uart_init(UART_PORT, 115200);
   ```

---

### Issue: LED not blinking

**Symptom**: v4-blink example runs but LED doesn't toggle.

**Solutions**:

1. **Check GPIO pin number**:
   - Verify `LED_PIN` matches your hardware
   - Common pins: GPIO8, GPIO10

2. **Check LED polarity**:
   - Anode (+, longer leg) to GPIO
   - Cathode (-, shorter leg) to GND
   - Add 220Ω-1kΩ resistor

3. **Test GPIO with multimeter**:
   - Should toggle between 0V and 3.3V
   - If stuck at 0V or 3.3V, GPIO config issue

4. **Verify GPIO is not reserved**:
   - Avoid GPIO6, GPIO7 (Flash)
   - Avoid GPIO12, GPIO13 (USB)

---

### Issue: v4-repl-demo not responding to input

**Symptom**: REPL prompt appears but doesn't echo characters.

**Causes & Solutions**:

1. **UART not initialized**:
   - Check serial monitor for "UART initialized" message
   - Verify return value: `v4_hal_uart_init(0, 115200)`

2. **Echo not working**:
   - Check `v4_hal_uart_write()` in input loop
   - Verify `available > 0` before reading

3. **Line termination**:
   - Try both Enter and Ctrl+J (Line Feed)
   - Some terminals send only CR or LF

---

## HAL Issues

### Issue: `v4_hal_gpio_set_mode()` returns error

**Symptom**: GPIO initialization fails.

```c
v4_err err = v4_hal_gpio_set_mode(LED_PIN, V4_GPIO_MODE_OUTPUT);
// err == V4_ERR_IO or V4_ERR_INVALID_ARG
```

**Solutions**:

1. **Check pin number validity**:
   ```c
   if (pin < 0 || pin >= GPIO_NUM_MAX) {
       // Invalid pin
   }
   ```

2. **Avoid reserved pins**:
   - Don't use GPIO6, GPIO7 (Flash)
   - Check ESP32-C6 datasheet for restrictions

3. **Enable debug output**:
   ```c
   printf("Setting GPIO%d to mode %d\n", pin, mode);
   ```

---

### Issue: `v4_hal_uart_read()` always returns 0 bytes

**Symptom**: UART receive not working.

**Cause**: UART driver not installed or RX buffer empty.

**Solutions**:

1. **Verify UART initialization**:
   ```c
   v4_err err = v4_hal_uart_init(0, 115200);
   if (err != V4_ERR_OK) {
       printf("UART init failed: %d\n", err);
   }
   ```

2. **Check available bytes first**:
   ```c
   int available;
   v4_hal_uart_available(0, &available);
   printf("Available bytes: %d\n", available);
   ```

3. **Increase read buffer size** in `hal_uart.c`:
   ```c
   #define UART_BUF_SIZE 2048  // Increase from 1024
   ```

---

### Issue: Timer delays are inaccurate

**Symptom**: `v4_hal_delay_ms()` takes much longer than expected.

**Cause**: FreeRTOS tick period.

**Solution**:

1. **Check tick frequency**:
   ```bash
   idf.py menuconfig
   # → Component config → FreeRTOS → Tick rate (Hz)
   # → Set to 1000 (default)
   ```

2. **For precise delays < 10ms**, use `v4_hal_delay_us()`:
   ```c
   v4_hal_delay_us(5000);  // 5ms, more precise
   ```

3. **Note tick granularity**:
   - With 1000Hz tick, delays round to 1ms
   - `delay_ms(1)` may delay 1-2ms

---

## Development Environment

### Issue: Docker container exits immediately

**Symptom**: `docker compose run` exits without shell.

**Cause**: Command override missing.

**Solution**: Check `docker-compose.yml`:

```yaml
services:
  esp-idf:
    command: /bin/bash  # ← Ensure this is present
    tty: true           # ← Required
    stdin_open: true    # ← Required
```

---

### Issue: Changes not reflected in Docker build

**Symptom**: Code changes don't appear in container.

**Cause**: Volume mount issue.

**Solutions**:

1. **Verify volume mount**:
   ```bash
   docker compose run --rm esp-idf
   ls /project  # Should show your source files
   ```

2. **Recreate container**:
   ```bash
   docker compose down
   docker compose run --rm esp-idf
   ```

3. **Clean build**:
   ```bash
   idf.py fullclean
   idf.py build
   ```

---

### Issue: `clang-format` not available

**Symptom**: Format check fails in CI.

**Solution**: Install clang-format in container:

```bash
# Inside Docker container
apt-get update
apt-get install -y clang-format

# Or use ESP-IDF's version
clang-format --version
```

---

### Issue: CI build fails but local build succeeds

**Symptom**: GitHub Actions build fails with errors not seen locally.

**Causes & Solutions**:

1. **Dependency paths differ**:
   - CI uses `$GITHUB_WORKSPACE`
   - Local uses relative paths
   - Update `.github/workflows/ci.yml` with correct paths

2. **Missing submodules**:
   ```yaml
   - uses: actions/checkout@v4
     with:
       submodules: recursive  # ← Add this
   ```

3. **ESP-IDF version mismatch**:
   - Local: Check with `idf.py --version`
   - CI: Verify container image tag in workflow

---

## Getting More Help

### Enable Verbose Logging

```bash
idf.py menuconfig
# → Component config → Log output
# → Default log verbosity → Verbose
```

In code:
```c
esp_log_level_set("*", ESP_LOG_VERBOSE);
```

### Capture Logs

```bash
idf.py monitor | tee monitor.log
```

### Report Issues

When reporting bugs, include:

1. **Environment**:
   - ESP-IDF version: `idf.py --version`
   - OS and version
   - Docker or native

2. **Full build log**:
   ```bash
   idf.py fullclean build 2>&1 | tee build.log
   ```

3. **Serial output**:
   ```bash
   idf.py monitor | tee serial.log
   ```

4. **Steps to reproduce**

Submit at: [github.com/your-org/v4-ports/issues](https://github.com/your-org/v4-ports/issues)

---

## Additional Resources

- [ESP-IDF Troubleshooting](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-guides/tools/idf-monitor.html)
- [ESP32-C6 Forum](https://www.esp32.com/)
- [V4 Project Issues](https://github.com/your-org/v4-ports/issues)
