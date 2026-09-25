/*
 * marichi - cooperative kernel for AVR (R) Mega microcontrollers
 * Copyright (C) 2026  notweerdmonk
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef _UART_CONFIG_H
#define _UART_CONFIG_H

/**
 * @file uart_config.h
 * @author notweerdmonk
 * @brief Configuration file for UART
 */


#define c_DEFAULT_BAUD_RATE 19200

#define c_UART_TX_BUFFER_LEN 64
#define c_UART_RX_BUFFER_LEN 64

#define c_UART_MAX_SEQ_LEN 6
#define c_UART_MAX_RECV_CB 3

#define UART_BAUD_DEFAULT c_DEFAULT_BAUD_RATE
#define UART_CHAR_SIZE 8
#define UART_STOP_BITS 1

#endif /* _UART_CONFIG_H */
