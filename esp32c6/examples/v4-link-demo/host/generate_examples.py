#!/usr/bin/env python3
"""
Generate example bytecode files for V4-link demo.

V4 Opcodes:
    LIT (0x00) + i32 - Push 32-bit literal
    DROP (0x02) - Drop TOS
    ADD (0x10) - Pop b, pop a, push a+b
    MUL (0x12) - Pop b, pop a, push a*b
    RET (0x51) - Return from execution
    SYS (0x60) + id - System call
    LIT_U8 (0x76) + u8 - Push 8-bit unsigned literal

V4 System Calls:
    GPIO_INIT (0x00) - Initialize GPIO (pin, mode → err)
    GPIO_WRITE (0x01) - Write GPIO (pin, value → err)
    DELAY_MS (0x22) - Delay milliseconds (ms → )
"""

import struct
from pathlib import Path

# Opcodes
OP_LIT = 0x00
OP_DROP = 0x02
OP_ADD = 0x10
OP_MUL = 0x12
OP_RET = 0x51
OP_SYS = 0x60
OP_LIT_U8 = 0x76

# SYS IDs
SYS_GPIO_INIT = 0x00
SYS_GPIO_WRITE = 0x01
SYS_DELAY_MS = 0x22

# GPIO constants
GPIO_LED = 7  # NanoC6 LED on GPIO7
GPIO_OUTPUT = 3  # HAL_GPIO_OUTPUT (was incorrectly set to 1 = HAL_GPIO_INPUT_PULLUP)
GPIO_HIGH = 1
GPIO_LOW = 0


def write_bytecode(filename, bytecode):
    """Write bytecode to file."""
    path = Path("examples") / filename
    path.parent.mkdir(exist_ok=True)
    path.write_bytes(bytes(bytecode))
    print(f"Generated {path} ({len(bytecode)} bytes)")


def main():
    # Example 1: LIT 42, RET
    # Pushes 42 to stack and returns
    bytecode = [
        OP_LIT,  # LIT opcode
        42,
        0,
        0,
        0,  # 42 in little-endian i32
        OP_RET,  # RET opcode
    ]
    write_bytecode("lit42.bin", bytecode)

    # Example 2: LIT 10, LIT 20, ADD, RET
    # Computes 10 + 20 = 30
    bytecode = [
        OP_LIT,
        10,
        0,
        0,
        0,  # LIT 10
        OP_LIT,
        20,
        0,
        0,
        0,  # LIT 20
        OP_ADD,  # ADD
        OP_RET,  # RET
    ]
    write_bytecode("add.bin", bytecode)

    # Example 3: LIT 7, LIT 6, MUL, RET
    # Computes 7 * 6 = 42
    bytecode = [
        OP_LIT,
        7,
        0,
        0,
        0,  # LIT 7
        OP_LIT,
        6,
        0,
        0,
        0,  # LIT 6
        OP_MUL,  # MUL
        OP_RET,  # RET
    ]
    write_bytecode("mul.bin", bytecode)

    # Example 4: LIT_U8 42, RET
    # Compact form: pushes 42 using 1 byte instead of 4
    bytecode = [
        OP_LIT_U8,  # LIT_U8 opcode
        42,  # value (1 byte)
        OP_RET,  # RET opcode
    ]
    write_bytecode("compact.bin", bytecode)

    # Example 5: LIT_U8 5, LIT_U8 10, ADD, RET
    # Compact form of addition
    bytecode = [
        OP_LIT_U8,
        5,  # LIT_U8 5
        OP_LIT_U8,
        10,  # LIT_U8 10
        OP_ADD,  # ADD
        OP_RET,  # RET
    ]
    write_bytecode("compact_add.bin", bytecode)

    # Example 6: LED blink (GPIO7 on NanoC6)
    # Init GPIO → Blink 3 times (ON 200ms, OFF 200ms)
    bytecode = []

    # GPIO_INIT: pin=7, mode=OUTPUT
    bytecode.extend(
        [
            OP_LIT_U8,
            GPIO_LED,  # pin 7
            OP_LIT_U8,
            GPIO_OUTPUT,  # OUTPUT mode
            OP_SYS,
            SYS_GPIO_INIT,  # GPIO_INIT
            OP_DROP,  # drop error code
        ]
    )

    # Blink 3 times
    for _ in range(3):
        # LED ON
        bytecode.extend(
            [
                OP_LIT_U8,
                GPIO_LED,  # pin 7
                OP_LIT_U8,
                GPIO_HIGH,  # HIGH
                OP_SYS,
                SYS_GPIO_WRITE,  # GPIO_WRITE
                OP_DROP,  # drop error code
            ]
        )

        # Delay 200ms
        bytecode.extend(
            [
                OP_LIT_U8,
                200,  # 200ms
                OP_SYS,
                SYS_DELAY_MS,  # DELAY_MS
            ]
        )

        # LED OFF
        bytecode.extend(
            [
                OP_LIT_U8,
                GPIO_LED,  # pin 7
                OP_LIT_U8,
                GPIO_LOW,  # LOW
                OP_SYS,
                SYS_GPIO_WRITE,  # GPIO_WRITE
                OP_DROP,  # drop error code
            ]
        )

        # Delay 200ms
        bytecode.extend(
            [
                OP_LIT_U8,
                200,  # 200ms
                OP_SYS,
                SYS_DELAY_MS,  # DELAY_MS
            ]
        )

    bytecode.append(OP_RET)
    write_bytecode("led_blink.bin", bytecode)

    # Example 6b: LED slow blink (1 second interval, 2 times) for clear visibility
    bytecode = []

    # GPIO_INIT: pin=7, mode=OUTPUT
    bytecode.extend(
        [
            OP_LIT_U8,
            GPIO_LED,
            OP_LIT_U8,
            GPIO_OUTPUT,
            OP_SYS,
            SYS_GPIO_INIT,
            OP_DROP,
        ]
    )

    # Blink 2 times with 1 second intervals
    for _ in range(2):
        # LED ON
        bytecode.extend(
            [
                OP_LIT_U8,
                GPIO_LED,
                OP_LIT_U8,
                GPIO_HIGH,
                OP_SYS,
                SYS_GPIO_WRITE,
                OP_DROP,
            ]
        )

        # Delay 1000ms
        bytecode.extend(
            [
                OP_LIT,
                232,
                3,
                0,
                0,  # LIT 1000 (0x03E8)
                OP_SYS,
                SYS_DELAY_MS,
            ]
        )

        # LED OFF
        bytecode.extend(
            [
                OP_LIT_U8,
                GPIO_LED,
                OP_LIT_U8,
                GPIO_LOW,
                OP_SYS,
                SYS_GPIO_WRITE,
                OP_DROP,
            ]
        )

        # Delay 1000ms
        bytecode.extend(
            [
                OP_LIT,
                232,
                3,
                0,
                0,  # LIT 1000
                OP_SYS,
                SYS_DELAY_MS,
            ]
        )

    bytecode.append(OP_RET)
    write_bytecode("led_slow.bin", bytecode)

    # Example 6c: LED medium blink (250ms interval using LIT_U8, 2 times)
    bytecode = []

    # GPIO_INIT: pin=7, mode=OUTPUT
    bytecode.extend(
        [
            OP_LIT_U8,
            GPIO_LED,
            OP_LIT_U8,
            GPIO_OUTPUT,
            OP_SYS,
            SYS_GPIO_INIT,
            OP_DROP,
        ]
    )

    # Blink 2 times with 250ms intervals
    for _ in range(2):
        # LED ON
        bytecode.extend(
            [
                OP_LIT_U8,
                GPIO_LED,
                OP_LIT_U8,
                GPIO_HIGH,
                OP_SYS,
                SYS_GPIO_WRITE,
                OP_DROP,
            ]
        )

        # Delay 250ms (using LIT_U8)
        bytecode.extend([OP_LIT_U8, 250, OP_SYS, SYS_DELAY_MS])

        # LED OFF
        bytecode.extend(
            [
                OP_LIT_U8,
                GPIO_LED,
                OP_LIT_U8,
                GPIO_LOW,
                OP_SYS,
                SYS_GPIO_WRITE,
                OP_DROP,
            ]
        )

        # Delay 250ms
        bytecode.extend([OP_LIT_U8, 250, OP_SYS, SYS_DELAY_MS])

    bytecode.append(OP_RET)
    write_bytecode("led_medium.bin", bytecode)

    # Example 6d: Simple LED ON test (no delay, just turn on and return)
    bytecode = [
        OP_LIT_U8, GPIO_LED,
        OP_LIT_U8, GPIO_OUTPUT,
        OP_SYS, SYS_GPIO_INIT,
        OP_DROP,
        OP_LIT_U8, GPIO_LED,
        OP_LIT_U8, GPIO_HIGH,
        OP_SYS, SYS_GPIO_WRITE,
        OP_DROP,
        OP_RET,
    ]
    write_bytecode("led_on.bin", bytecode)

    # Example 6e: Simple LED OFF test
    bytecode = [
        OP_LIT_U8, GPIO_LED,
        OP_LIT_U8, GPIO_OUTPUT,
        OP_SYS, SYS_GPIO_INIT,
        OP_DROP,
        OP_LIT_U8, GPIO_LED,
        OP_LIT_U8, GPIO_LOW,
        OP_SYS, SYS_GPIO_WRITE,
        OP_DROP,
        OP_RET,
    ]
    write_bytecode("led_off.bin", bytecode)

    # Example 6f: Toggle LED 3 times without delay (to test if GPIO control works)
    bytecode = [
        OP_LIT_U8, GPIO_LED,
        OP_LIT_U8, GPIO_OUTPUT,
        OP_SYS, SYS_GPIO_INIT,
        OP_DROP,
    ]
    for _ in range(3):
        # LED ON
        bytecode.extend([
            OP_LIT_U8, GPIO_LED,
            OP_LIT_U8, GPIO_HIGH,
            OP_SYS, SYS_GPIO_WRITE,
            OP_DROP,
        ])
        # LED OFF (no delay)
        bytecode.extend([
            OP_LIT_U8, GPIO_LED,
            OP_LIT_U8, GPIO_LOW,
            OP_SYS, SYS_GPIO_WRITE,
            OP_DROP,
        ])
    bytecode.append(OP_RET)
    write_bytecode("led_toggle.bin", bytecode)

    # Example 7: LED fast blink (100ms interval, 5 times)
    bytecode = []

    # GPIO_INIT: pin=7, mode=OUTPUT
    bytecode.extend(
        [
            OP_LIT_U8,
            GPIO_LED,
            OP_LIT_U8,
            GPIO_OUTPUT,
            OP_SYS,
            SYS_GPIO_INIT,
            OP_DROP,
        ]
    )

    # Fast blink 5 times
    for _ in range(5):
        # LED ON
        bytecode.extend(
            [
                OP_LIT_U8,
                GPIO_LED,
                OP_LIT_U8,
                GPIO_HIGH,
                OP_SYS,
                SYS_GPIO_WRITE,
                OP_DROP,
            ]
        )

        # Delay 100ms
        bytecode.extend([OP_LIT_U8, 100, OP_SYS, SYS_DELAY_MS])

        # LED OFF
        bytecode.extend(
            [
                OP_LIT_U8,
                GPIO_LED,
                OP_LIT_U8,
                GPIO_LOW,
                OP_SYS,
                SYS_GPIO_WRITE,
                OP_DROP,
            ]
        )

        # Delay 100ms
        bytecode.extend([OP_LIT_U8, 100, OP_SYS, SYS_DELAY_MS])

    bytecode.append(OP_RET)
    write_bytecode("led_fast.bin", bytecode)

    # Example 8: LED SOS pattern (... --- ...)
    bytecode = []

    # GPIO_INIT
    bytecode.extend(
        [
            OP_LIT_U8,
            GPIO_LED,
            OP_LIT_U8,
            GPIO_OUTPUT,
            OP_SYS,
            SYS_GPIO_INIT,
            OP_DROP,
        ]
    )

    def add_dot(bytecode):
        """Short blink (100ms ON, 100ms OFF)"""
        bytecode.extend(
            [
                OP_LIT_U8,
                GPIO_LED,
                OP_LIT_U8,
                GPIO_HIGH,
                OP_SYS,
                SYS_GPIO_WRITE,
                OP_DROP,
                OP_LIT_U8,
                100,
                OP_SYS,
                SYS_DELAY_MS,
                OP_LIT_U8,
                GPIO_LED,
                OP_LIT_U8,
                GPIO_LOW,
                OP_SYS,
                SYS_GPIO_WRITE,
                OP_DROP,
                OP_LIT_U8,
                100,
                OP_SYS,
                SYS_DELAY_MS,
            ]
        )

    def add_dash(bytecode):
        """Long blink (300ms ON, 100ms OFF)"""
        bytecode.extend(
            [
                OP_LIT_U8,
                GPIO_LED,
                OP_LIT_U8,
                GPIO_HIGH,
                OP_SYS,
                SYS_GPIO_WRITE,
                OP_DROP,
            ]
        )
        # 300ms delay requires LIT (not LIT_U8 since 300 > 255)
        bytecode.extend([OP_LIT, 44, 1, 0, 0])  # 300 in little-endian
        bytecode.extend(
            [
                OP_SYS,
                SYS_DELAY_MS,
                OP_LIT_U8,
                GPIO_LED,
                OP_LIT_U8,
                GPIO_LOW,
                OP_SYS,
                SYS_GPIO_WRITE,
                OP_DROP,
                OP_LIT_U8,
                100,
                OP_SYS,
                SYS_DELAY_MS,
            ]
        )

    # S: ...
    for _ in range(3):
        add_dot(bytecode)

    # Pause between letters
    bytecode.extend([OP_LIT_U8, 200, OP_SYS, SYS_DELAY_MS])

    # O: ---
    for _ in range(3):
        add_dash(bytecode)

    # Pause between letters
    bytecode.extend([OP_LIT_U8, 200, OP_SYS, SYS_DELAY_MS])

    # S: ...
    for _ in range(3):
        add_dot(bytecode)

    bytecode.append(OP_RET)
    write_bytecode("led_sos.bin", bytecode)

    print("\n✅ All example bytecode files generated")
    print("\nArithmetic examples:")
    print("  python v4_link_send.py --port /dev/ttyACM0 --exec examples/lit42.bin")
    print("  python v4_link_send.py --port /dev/ttyACM0 --exec examples/add.bin")
    print("\nLED control examples (NanoC6 GPIO7):")
    print("  python v4_link_send.py --port /dev/ttyACM0 --exec examples/led_blink.bin")
    print("  python v4_link_send.py --port /dev/ttyACM0 --exec examples/led_fast.bin")
    print("  python v4_link_send.py --port /dev/ttyACM0 --exec examples/led_sos.bin")


if __name__ == "__main__":
    main()
