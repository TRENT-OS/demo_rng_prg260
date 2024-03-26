/*
 * Platform defaults for the RPi4.
 *
 * Copyright (C) 2021-2024, HENSOLDT Cyber GmbH
 * 
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * For commercial licensing, contact: info.cyber@hensoldt.net
 */


// bluetooth uses UART_0, kernel log uses UART_1, so we can use UART_2 to UART_5
// for i/o test
#define UART_IO     UART_2
