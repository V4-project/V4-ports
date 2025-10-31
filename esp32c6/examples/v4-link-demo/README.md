# V4-link Demo for ESP32-C6

Ultra-lightweight bytecode transfer demonstration using V4-link protocol.

## Overview

This example demonstrates V4-link bytecode transfer over USB Serial/JTAG. The ESP32-C6 receives compiled Forth bytecode from a host PC and executes it on the V4 VM.

### Features

- Minimal footprint bytecode transfer (~1.5KB Flash)
- Frame-based protocol with CRC-8 error detection
- USB Serial/JTAG communication (115200 baud)
- 4KB VM memory
- 512B bytecode buffer

## Hardware Required

- ESP32-C6 development board (DevKit-C, NanoC6, etc.)
- USB cable

## Building and Flashing

### Prerequisites

- ESP-IDF v5.3 or later
- V4, V4-hal, and V4-link repositories (automatically fetched or use local)

### Environment Setup (Optional)

To use local V4 repositories instead of fetching from GitHub:

```bash
export V4_PATH=/path/to/V4
export V4_HAL_PATH=/path/to/V4-hal
export V4_LINK_PATH=/path/to/V4-link
```

### Build

```bash
cd esp32c6/examples/v4-link-demo
idf.py build
```

### Flash and Monitor

```bash
idf.py flash monitor
```

Expected output:

```
I (xxx) v4_link_demo: V4-link Demo starting...
I (xxx) v4_link_demo: V4 VM created (memory: 4096 bytes)
I (xxx) v4_link_port: V4-link initialized on UART0 at 115200 baud
I (xxx) v4_link_demo: V4-link ready on USB Serial/JTAG (115200 baud)
I (xxx) v4_link_demo: Buffer capacity: 512 bytes
I (xxx) v4_link_demo: Waiting for bytecode from host...
```

## Using V4-link

### Protocol Overview

V4-link uses a simple frame-based protocol:

```
Frame: [STX(0xA5)][LEN_L][LEN_H][CMD][DATA...][CRC8]
```

**Commands:**
- `0x10 EXEC`: Execute bytecode
- `0x20 PING`: Connection check
- `0xFF RESET`: Reset VM

**Response:**
- `[STX][0x00][0x01][ERR_CODE][CRC8]`

### Sending Bytecode (Manual)

For testing, you can use a serial terminal to send raw frames. However, it's recommended to use the V4-cli tool (future release) or write a simple host script.

### Example: Python Host Script

```python
#!/usr/bin/env python3
import serial
import struct

STX = 0xA5
CMD_EXEC = 0x10
CMD_PING = 0x20

def calc_crc8(data):
    crc = 0x00
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 0x80:
                crc = (crc << 1) ^ 0x07
            else:
                crc <<= 1
        crc &= 0xFF
    return crc

def encode_frame(cmd, payload=b''):
    length = len(payload)
    frame = struct.pack('<BBBH', STX, length & 0xFF, (length >> 8) & 0xFF, cmd)
    frame += payload
    crc = calc_crc8(frame[1:])
    frame += bytes([crc])
    return frame

# Connect to ESP32-C6
ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)

# Send PING
ping_frame = encode_frame(CMD_PING)
ser.write(ping_frame)
response = ser.read(5)
print(f"PING response: {response.hex()}")

# Example bytecode: LIT 42 (push 42 to stack)
# Op::LIT=0x00, followed by 4-byte little-endian value
bytecode = bytes([0x00, 42, 0x00, 0x00, 0x00, 0x51])  # LIT 42, RET
exec_frame = encode_frame(CMD_EXEC, bytecode)
ser.write(exec_frame)
response = ser.read(5)
print(f"EXEC response: {response.hex()}")
```

## Memory Map

```
Flash Usage (typical):
- V4 VM core:       ~8KB
- V4-link:          ~1.5KB
- V4-hal:           ~3KB
- ESP-IDF core:     ~150KB
- Total:            ~163KB

RAM Usage:
- VM memory:        4KB
- VM stacks:        1KB (256×4B DS + 64×4B RS)
- V4-link buffer:   512B
- FreeRTOS:         ~8KB
- Total:            ~14KB
```

## Troubleshooting

### Build Errors

**Error: V4 not found**
```
Solution: Set V4_PATH environment variable or place V4 at ../../../V4
```

**Error: undefined reference to vm_create**
```
Solution: Clean build and rebuild
$ idf.py fullclean && idf.py build
```

### Runtime Issues

**No output on serial monitor**
```
Solution: Check USB cable and try re-flashing
$ idf.py erase-flash flash monitor
```

**UART initialization failed**
```
Solution: Check sdkconfig.defaults has CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG=y
```

## Related Examples

- `v4-blink`: Basic V4 VM example with GPIO
- `v4-repl-demo`: Interactive REPL with V4-front compiler

## Next Steps

- Use V4-cli (future release) for easier bytecode deployment
- Try V4-repl for interactive Forth development
- Integrate V4-link into your own application

## License

MIT + Apache-2.0 dual licensing
