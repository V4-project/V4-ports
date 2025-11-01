#!/usr/bin/env python3
# Test single 200ms delay

OP_LIT_U8 = 0x76
OP_SYS = 0x60
OP_RET = 0x51
SYS_DELAY_MS = 0x22

# Single 200ms delay, then RET
bytecode = [
    OP_LIT_U8, 200,  # push 200
    OP_SYS, SYS_DELAY_MS,  # DELAY_MS(200)
    OP_RET,
]

with open("examples/test_single_delay.bin", "wb") as f:
    f.write(bytes(bytecode))
print(f"Generated test_single_delay.bin ({len(bytecode)} bytes): {bytes(bytecode).hex()}")
