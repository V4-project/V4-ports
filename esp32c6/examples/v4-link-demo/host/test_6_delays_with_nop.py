#!/usr/bin/env python3
# Test 6 delays with NOP-like operations between them

OP_LIT_U8 = 0x76
OP_SYS = 0x60
OP_DROP = 0x02
OP_RET = 0x51
SYS_DELAY_MS = 0x22

# 6 x 200ms delay, but with DROP between each (to break up the pattern)
bytecode = []
for i in range(6):
    bytecode.extend([OP_LIT_U8, 200, OP_SYS, SYS_DELAY_MS])
    # Add a NOP-like operation: push 0 and drop it
    if i < 5:  # Don't add after the last delay
        bytecode.extend([OP_LIT_U8, 0, OP_DROP])
bytecode.append(OP_RET)

with open("examples/test_6_delays_nop.bin", "wb") as f:
    f.write(bytes(bytecode))
print(f"Generated test_6_delays_nop.bin ({len(bytecode)} bytes)")
print(f"Hex: {bytes(bytecode).hex()}")
