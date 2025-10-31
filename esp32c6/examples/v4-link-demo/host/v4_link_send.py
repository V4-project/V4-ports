#!/usr/bin/env python3
"""
V4-link Host Script

Send bytecode to ESP32-C6 running V4-link demo via USB Serial/JTAG.
Supports PING, EXEC, and RESET commands.

Usage:
    python v4_link_send.py --port /dev/ttyACM0 --ping
    python v4_link_send.py --port /dev/ttyACM0 --exec examples/lit42.bin
    python v4_link_send.py --port /dev/ttyACM0 --exec examples/hello.bin
    python v4_link_send.py --port /dev/ttyACM0 --reset
"""

import argparse
import struct
import sys
import time
from pathlib import Path

try:
    import serial
except ImportError:
    print("Error: pyserial is required. Install with: pip install pyserial")
    sys.exit(1)

# Protocol constants
STX = 0xA5
CMD_EXEC = 0x10
CMD_PING = 0x20
CMD_RESET = 0xFF

# Error codes
ERR_OK = 0x00
ERR_ERROR = 0x01
ERR_INVALID_FRAME = 0x02
ERR_BUFFER_FULL = 0x03
ERR_VM_ERROR = 0x04

ERROR_NAMES = {
    ERR_OK: "OK",
    ERR_ERROR: "ERROR",
    ERR_INVALID_FRAME: "INVALID_FRAME",
    ERR_BUFFER_FULL: "BUFFER_FULL",
    ERR_VM_ERROR: "VM_ERROR",
}


def calc_crc8(data):
    """Calculate CRC-8 with polynomial 0x07."""
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


def encode_frame(cmd, payload=b""):
    """Encode a V4-link frame with CRC-8."""
    length = len(payload)
    if length > 512:
        raise ValueError(f"Payload too large: {length} bytes (max 512)")

    # Frame: [STX][LEN_L][LEN_H][CMD][DATA...][CRC8]
    frame = struct.pack("<BBH", STX, length & 0xFF, (length >> 8) & 0xFF)
    frame += struct.pack("<B", cmd)
    frame += payload

    # CRC over everything except STX
    crc = calc_crc8(frame[1:])
    frame += bytes([crc])
    return frame


def decode_response(data):
    """Decode V4-link response frame."""
    if len(data) < 5:
        return None, "Response too short"

    if data[0] != STX:
        return None, f"Invalid STX: 0x{data[0]:02x}"

    length = data[1] | (data[2] << 8)
    if length != 1:
        return None, f"Invalid response length: {length}"

    err_code = data[3]
    expected_crc = calc_crc8(data[1:4])
    actual_crc = data[4]

    if expected_crc != actual_crc:
        return None, f"CRC mismatch: expected 0x{expected_crc:02x}, got 0x{actual_crc:02x}"

    return err_code, None


def send_command(ser, cmd, payload=b"", timeout=1.0):
    """Send command and wait for response."""
    frame = encode_frame(cmd, payload)

    print(f"Sending frame ({len(frame)} bytes): {frame.hex()}")
    ser.write(frame)
    ser.flush()

    # Wait for response
    start = time.time()
    response = b""
    while len(response) < 5 and (time.time() - start) < timeout:
        if ser.in_waiting > 0:
            response += ser.read(ser.in_waiting)
        time.sleep(0.01)

    if len(response) < 5:
        return None, "Timeout waiting for response"

    print(f"Received response ({len(response)} bytes): {response.hex()}")

    err_code, error = decode_response(response)
    if error:
        return None, error

    return err_code, None


def cmd_ping(ser):
    """Send PING command."""
    print("Sending PING...")
    err_code, error = send_command(ser, CMD_PING)
    if error:
        print(f"Error: {error}")
        return False

    err_name = ERROR_NAMES.get(err_code, f"UNKNOWN(0x{err_code:02x})")
    print(f"Response: {err_name}")
    return err_code == ERR_OK


def cmd_exec(ser, bytecode):
    """Send EXEC command with bytecode."""
    print(f"Sending EXEC with {len(bytecode)} bytes of bytecode...")
    print(f"Bytecode: {bytecode.hex()}")

    err_code, error = send_command(ser, CMD_EXEC, bytecode)
    if error:
        print(f"Error: {error}")
        return False

    err_name = ERROR_NAMES.get(err_code, f"UNKNOWN(0x{err_code:02x})")
    print(f"Response: {err_name}")
    return err_code == ERR_OK


def cmd_reset(ser):
    """Send RESET command."""
    print("Sending RESET...")
    err_code, error = send_command(ser, CMD_RESET)
    if error:
        print(f"Error: {error}")
        return False

    err_name = ERROR_NAMES.get(err_code, f"UNKNOWN(0x{err_code:02x})")
    print(f"Response: {err_name}")
    return err_code == ERR_OK


def main():
    parser = argparse.ArgumentParser(
        description="V4-link host script for sending bytecode to ESP32-C6"
    )
    parser.add_argument(
        "-p", "--port", required=True, help="Serial port (e.g., /dev/ttyACM0, COM3)"
    )
    parser.add_argument(
        "-b", "--baud", type=int, default=115200, help="Baud rate (default: 115200)"
    )
    parser.add_argument("--ping", action="store_true", help="Send PING command")
    parser.add_argument(
        "--exec", metavar="FILE", help="Send EXEC command with bytecode from file"
    )
    parser.add_argument("--reset", action="store_true", help="Send RESET command")
    parser.add_argument(
        "-t", "--timeout", type=float, default=2.0, help="Response timeout in seconds"
    )

    args = parser.parse_args()

    # Check that at least one command is specified
    if not any([args.ping, args.exec, args.reset]):
        parser.error("Must specify at least one command: --ping, --exec, or --reset")

    # Open serial port
    try:
        ser = serial.Serial(args.port, args.baud, timeout=args.timeout)
        print(f"Connected to {args.port} at {args.baud} baud")
        time.sleep(0.1)  # Wait for connection to stabilize
    except serial.SerialException as e:
        print(f"Error opening serial port: {e}")
        sys.exit(1)

    try:
        success = True

        # Execute commands in order
        if args.ping:
            if not cmd_ping(ser):
                success = False

        if args.exec:
            bytecode_path = Path(args.exec)
            if not bytecode_path.exists():
                print(f"Error: Bytecode file not found: {bytecode_path}")
                success = False
            else:
                bytecode = bytecode_path.read_bytes()
                if not cmd_exec(ser, bytecode):
                    success = False

        if args.reset:
            if not cmd_reset(ser):
                success = False

        if success:
            print("\n✅ All commands completed successfully")
        else:
            print("\n❌ Some commands failed")
            sys.exit(1)

    finally:
        ser.close()


if __name__ == "__main__":
    main()
