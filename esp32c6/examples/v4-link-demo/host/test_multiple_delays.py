#!/usr/bin/env python3
# Test multiple 200ms delays

OP_LIT_U8 = 0x76
OP_SYS = 0x60
OP_RET = 0x51
SYS_DELAY_MS = 0x22

# Test different numbers of delays
for count in [2, 3, 4, 5, 6]:
    bytecode = []
    for _ in range(count):
        bytecode.extend([OP_LIT_U8, 200, OP_SYS, SYS_DELAY_MS])
    bytecode.append(OP_RET)

    filename = f"test_{count}_delays.bin"
    with open(f"examples/{filename}", "wb") as f:
        f.write(bytes(bytecode))
    print(f"{filename}: {len(bytecode)} bytes, total delay: {count * 200}ms")
