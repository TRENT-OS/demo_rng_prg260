#! /usr/bin/python3
#
# Copyright (C) 2023-2024, HENSOLDT Cyber GmbH
# 
# SPDX-License-Identifier: GPL-2.0-or-later
#
# For commercial licensing, contact: info.cyber@hensoldt.net
#

import sys
import random
import string

try:
    import serial
except ModuleNotFoundError:
    import subprocess
    subprocess.run([sys.executable, '-m', 'pip', 'install', '--user', 'pyserial'])
    import serial

#----------------------------------------------------------------------------

cap = 1024
failures = 0

#----------------------------------------------------------------------------
def rand_letters(amount):
    return ''.join(random.choices(string.ascii_uppercase + string.digits, k=amount))


def flush_uart(device):
    while device.read():
        pass


def send_and_receive(device, data):
    print(f"Sending: {data}")
    device.write(data.encode("utf-8"))

    recv_data = ""
    while True:
        buf = device.read().decode()
        if not len(buf):
            break
        recv_data += buf

    print(f"Received: {recv_data}")
    return data == recv_data


def init_serial():
    return serial.Serial(sys.argv[1], 115200, timeout=1)


def failure():
    global failures
    print("Failure invalid bytes received")
    failures += 1


def run():
    total_b = eval(sys.argv[2])

    device = init_serial()

    packets = int(total_b / cap)
    print(f"Sending {total_b} Bytes")
    for i in range(packets):
        print(f"\nPacket {i+1} of {packets}")
        if not send_and_receive(device, rand_letters(cap)):
            failure()

    if total_b % cap:
        if not send_and_receive(device, rand_letters(total_b % cap)):
            failure()

    if failures:
        print(f"Failed packets: {failures}")
        exit(-1)
    print(f"Success Received: {total_b} Bytes")


#----------------------------------------------------------------------------
if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage:\n{sys.argv[0]} DEVICE BYTES")
        print(f"Example:\n{sys.argv[0]} /dev/ttyUSB1 \"1024**2\"")
        sys.exit(-1)
    run()
