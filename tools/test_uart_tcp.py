#! /usr/bin/python
#
# Copyright (C) 2023-2024, HENSOLDT Cyber GmbH
# 
# SPDX-License-Identifier: GPL-2.0-or-later
#
# For commercial licensing, contact: info.cyber@hensoldt.net
#

import socket
import sys
import textwrap
import random


slice_size = 64

def rand_letters(amount):
    return ''.join(random.choices(string.ascii_uppercase + string.digits, k=amount))


def send_and_receive(sock, data):
    sock.sendall(data.encode("utf-8"))
    recv_data = ""
    count = 0
    while recv_data != data:
        count += 1
        recv_data += sock.recv(slice_size).decode()
        print(f"Loop iteration: {count} recvdata: {len(recv_data)}")
        print(recv_data)

def init_sock():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((sys.argv[1], int(sys.argv[2])))
    return sock


def run():
    amount_of_bytes = eval(sys.argv[3])
    data = rand_letters(amount_of_bytes)

    sock = init_sock()

    data_slices = textwrap.wrap(data, slice_size)
    recv_data = ""
    total_slices = len(data_slices)

    print(f"Sending {amount_of_bytes} Bytes")
    for i, element in enumerate(data_slices):
        print(f"Sending Slice {i} of {total_slices}")
        send_and_receive(sock, element)

    print(f"Success Received: {len(recv_data)} Bytes")


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print(f"Usage:\n{sys.argv[0]} IP PORT BYTES")
        print(f"Example:\n{sys.argv[0]} 172.17.0.2 14540 \"1024**2\"")
        sys.exit(-1)
    run()
