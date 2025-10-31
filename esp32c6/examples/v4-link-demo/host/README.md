# V4-link Host Scripts

Python scripts for sending bytecode to ESP32-C6 running V4-link demo.

## Prerequisites

```bash
pip install -r requirements.txt
```

## Usage

### 1. Generate Example Bytecode

```bash
python generate_examples.py
```

This creates example bytecode files in the `examples/` directory:

**Arithmetic examples:**
- `lit42.bin` - Push 42 to stack
- `add.bin` - Compute 10 + 20
- `mul.bin` - Compute 7 × 6
- `compact.bin` - Push 42 using compact opcode
- `compact_add.bin` - Compute 5 + 10 using compact opcodes

**LED control examples (GPIO7 on NanoC6):**
- `led_blink.bin` - Blink LED 3 times (200ms ON/OFF)
- `led_fast.bin` - Fast blink 5 times (100ms ON/OFF)
- `led_sos.bin` - SOS signal pattern in Morse code (··· ─── ···)

### 2. Send Commands to ESP32-C6

Make sure the ESP32-C6 is flashed with v4-link-demo and connected via USB.

**Check connection (PING):**
```bash
python v4_link_send.py --port /dev/ttyACM0 --ping
```

**Execute arithmetic bytecode:**
```bash
python v4_link_send.py --port /dev/ttyACM0 --exec examples/lit42.bin
python v4_link_send.py --port /dev/ttyACM0 --exec examples/add.bin
```

**Execute LED control bytecode:**
```bash
# Normal blink pattern
python v4_link_send.py --port /dev/ttyACM0 --exec examples/led_blink.bin

# Fast blink pattern
python v4_link_send.py --port /dev/ttyACM0 --exec examples/led_fast.bin

# SOS signal in Morse code
python v4_link_send.py --port /dev/ttyACM0 --exec examples/led_sos.bin
```

**Reset VM:**
```bash
python v4_link_send.py --port /dev/ttyACM0 --reset
```

**Combine commands:**
```bash
python v4_link_send.py --port /dev/ttyACM0 --ping --exec examples/led_blink.bin
```

### 3. Find Serial Port

**Linux:**
```bash
ls /dev/ttyACM*
# or
ls /dev/ttyUSB*
```

**macOS:**
```bash
ls /dev/cu.usbmodem*
```

**Windows:**
```
# Check Device Manager for COM port
python v4_link_send.py --port COM3 --ping
```

## Example Session

```bash
$ python generate_examples.py
Generated examples/lit42.bin (6 bytes)
Generated examples/add.bin (11 bytes)
Generated examples/mul.bin (11 bytes)
Generated examples/compact.bin (3 bytes)
Generated examples/compact_add.bin (5 bytes)

✅ All example bytecode files generated

$ python v4_link_send.py --port /dev/ttyACM0 --ping
Connected to /dev/ttyACM0 at 115200 baud
Sending PING...
Sending frame (5 bytes): a500000020a7
Received response (5 bytes): a50100000082
Response: OK

✅ All commands completed successfully

$ python v4_link_send.py --port /dev/ttyACM0 --exec examples/lit42.bin
Connected to /dev/ttyACM0 at 115200 baud
Sending EXEC with 6 bytes of bytecode...
Bytecode: 002a00000051
Sending frame (11 bytes): a5060000102a0000005163
Received response (5 bytes): a50100000082
Response: OK

✅ All commands completed successfully

$ python v4_link_send.py --port /dev/ttyACM0 --exec examples/add.bin
Connected to /dev/ttyACM0 at 115200 baud
Sending EXEC with 11 bytes of bytecode...
Bytecode: 000a00000000140000001051
Sending frame (16 bytes): a50b0000100a00000000140000001051c8
Received response (5 bytes): a50100000082
Response: OK

✅ All commands completed successfully

$ python v4_link_send.py --port /dev/ttyACM0 --exec examples/led_blink.bin
Connected to /dev/ttyACM0 at 115200 baud
Sending EXEC with 50 bytes of bytecode...
Bytecode: 76077601600002...
Sending frame (55 bytes): a5320000107607760160000276077601600102...
Received response (5 bytes): a50100000082
Response: OK

✅ All commands completed successfully
# Watch the LED on GPIO7 blink 3 times!
```

## Protocol Reference

### Frame Format

```
[STX(0xA5)][LEN_L][LEN_H][CMD][DATA...][CRC8]
```

- **STX**: Start byte (0xA5)
- **LEN_L, LEN_H**: Payload length (little-endian u16)
- **CMD**: Command byte
- **DATA**: Payload data (up to 512 bytes)
- **CRC8**: CRC-8 checksum (polynomial 0x07)

### Commands

| Command | Code | Description |
|---------|------|-------------|
| EXEC    | 0x10 | Execute bytecode |
| PING    | 0x20 | Connection check |
| RESET   | 0xFF | Reset VM |

### Response Format

```
[STX(0xA5)][0x01][0x00][ERR_CODE][CRC8]
```

### Error Codes

| Code | Name | Description |
|------|------|-------------|
| 0x00 | OK | Success |
| 0x01 | ERROR | General error |
| 0x02 | INVALID_FRAME | Frame format error |
| 0x03 | BUFFER_FULL | Bytecode buffer full |
| 0x04 | VM_ERROR | VM execution error |

## Troubleshooting

**Error: `pyserial` is required**
```bash
pip install pyserial
```

**Error: Permission denied on Linux**
```bash
sudo usermod -a -G dialout $USER
# Log out and log back in
```

**Error: Device not found**
- Check USB cable connection
- Verify ESP32-C6 is powered on
- Check port with `ls /dev/ttyACM*` (Linux) or Device Manager (Windows)

**Error: Timeout waiting for response**
- Ensure v4-link-demo is flashed and running
- Check `idf.py monitor` output for errors
- Verify baud rate (default: 115200)

## Writing Custom Bytecode

See V4 opcodes in `V4/include/v4/opcodes.def` and system calls in `V4/include/v4/sys_ids.h`.

### Example 1: Arithmetic Operation

**Push two numbers and multiply:**

```python
OP_LIT = 0x00
OP_MUL = 0x12
OP_RET = 0x51

bytecode = [
    OP_LIT, 7, 0, 0, 0,   # Push 7
    OP_LIT, 6, 0, 0, 0,   # Push 6
    OP_MUL,                # Multiply
    OP_RET,                # Return
]

with open("custom.bin", "wb") as f:
    f.write(bytes(bytecode))
```

### Example 2: LED Control

**Turn LED on for 1 second:**

```python
OP_LIT_U8 = 0x76
OP_SYS = 0x60
OP_DROP = 0x02
OP_RET = 0x51

SYS_GPIO_INIT = 0x00
SYS_GPIO_WRITE = 0x01
SYS_DELAY_MS = 0x22

GPIO_OUTPUT = 1
GPIO_HIGH = 1

bytecode = [
    # Init GPIO7 as OUTPUT
    OP_LIT_U8, 7,           # pin 7
    OP_LIT_U8, GPIO_OUTPUT, # OUTPUT mode
    OP_SYS, SYS_GPIO_INIT,  # GPIO_INIT
    OP_DROP,                 # drop error code

    # Turn LED ON
    OP_LIT_U8, 7,           # pin 7
    OP_LIT_U8, GPIO_HIGH,   # HIGH
    OP_SYS, SYS_GPIO_WRITE, # GPIO_WRITE
    OP_DROP,                 # drop error code

    # Delay 1000ms
    0x00, 0xE8, 0x03, 0, 0, # LIT 1000 (little-endian)
    OP_SYS, SYS_DELAY_MS,   # DELAY_MS

    # Turn LED OFF
    OP_LIT_U8, 7,           # pin 7
    OP_LIT_U8, 0,           # LOW
    OP_SYS, SYS_GPIO_WRITE, # GPIO_WRITE
    OP_DROP,                 # drop error code

    OP_RET,                  # Return
]

with open("led_custom.bin", "wb") as f:
    f.write(bytes(bytecode))
```

### Send Custom Bytecode

```bash
python v4_link_send.py --port /dev/ttyACM0 --exec custom.bin
python v4_link_send.py --port /dev/ttyACM0 --exec led_custom.bin
```
