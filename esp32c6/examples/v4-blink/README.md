# V4 Blink Example - ESP32-C6

Simple LED blink example to verify GPIO and Timer HAL implementation.

## Features

- GPIO initialization and control
- Timer functions (millis, delay)
- Serial console output
- LED toggles at 1Hz (500ms interval)

## Hardware Requirements

- ESP32-C6 development board (ESP32-C6-DevKitC-1 or compatible)
- USB-C cable for power and serial communication
- External LED with 330Ω resistor (optional, for testing other GPIO pins)

### LED Connection

**On-board LEDs:**
- **GPIO7**: Simple LED (can be controlled with standard GPIO) ✅ **Default**
- **GPIO8**: WS2812 RGB LED (requires special driver)

**Pin Configuration:**
- Default LED pin: `GPIO7` (on-board LED)
- You can change the pin in `main.c` by modifying `LED_PIN`
- Available GPIO pins: 0-7, 9-30 (avoid strapping pins)

**For external LED:**
```
ESP32-C6          External LED
GPIOx ----[330Ω resistor]---- LED(+) ---- GND
```

## Build and Flash

```bash
# Using Docker
docker compose run --rm esp-idf bash
cd esp32c6/examples/v4-blink

# Build
idf.py build

# Flash to device
idf.py -p /dev/ttyACM0 flash

# Monitor serial output
idf.py -p /dev/ttyACM0 monitor

# Or do all at once
idf.py -p /dev/ttyACM0 flash monitor
```

## Expected Output

```
========================================
V4 Blink Example - ESP32-C6
========================================
LED Pin: GPIO7
Blink Interval: 500 ms
System: ESP32-C6 (0.1.0)
========================================

GPIO7 initialized as OUTPUT

[     0] LED OFF | Time: 5 ms
[     1] LED ON  | Time: 504 ms
[     2] LED OFF | Time: 1004 ms
[     3] LED ON  | Time: 1504 ms
...
```

## Using the On-Board WS2812 LED (Advanced)

If you want to use the on-board WS2812 RGB LED (GPIO8), you'll need to implement a WS2812 driver using ESP-IDF's RMT peripheral. This is beyond the scope of the simple V4 HAL GPIO API.

See ESP-IDF's `led_strip` example for reference:
- https://github.com/espressif/esp-idf/tree/master/examples/peripherals/rmt/led_strip

## Troubleshooting

### LED doesn't blink

1. **Check hardware connection**: Verify LED polarity and resistor value
2. **Check GPIO pin**: Make sure the pin is not used for other purposes
3. **Check serial output**: If you see "LED ON/OFF" messages, the code is working
4. **Try a different pin**: Some pins may have hardware conflicts

### Build errors

```bash
# Clean build directory
rm -rf build
idf.py build
```

### Flash errors

```bash
# Check device connection
ls -l /dev/ttyACM0

# Try manual reset
# Press and hold BOOT button, press RESET button, release BOOT button

# Flash with slower baud rate
idf.py -p /dev/ttyACM0 -b 115200 flash
```

## Code Structure

```
v4-blink/
├── CMakeLists.txt       # Project build configuration
├── README.md            # This file
├── sdkconfig.defaults   # ESP-IDF configuration defaults
└── main/
    ├── CMakeLists.txt   # Main component build config
    └── main.c           # Main application code
```

## V4 HAL Functions Used

This example demonstrates the following V4 HAL functions:

- `v4_hal_gpio_init()` - Initialize GPIO pin as output
- `v4_hal_gpio_write()` - Write HIGH/LOW to GPIO pin
- `v4_hal_millis()` - Get milliseconds since boot
- `v4_hal_delay_ms()` - Blocking delay in milliseconds
- `v4_hal_system_info()` - Get system information string

## License

See the LICENSE files in the repository root.
