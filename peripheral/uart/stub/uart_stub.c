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

/**
 * @file uart_stub.c
 * @author notweerdmonk
 * @brief Weak linkage stubs for UART module
 */

#include <common.h>
#include <uart.h>

WEAK
void uart_init(UNUSED_VARIABLE uart_config_t *config) { }

WEAK
uint8_t uart_register_match(
    UNUSED_VARIABLE const char *c,
    UNUSED_VARIABLE event_handler handler,
    UNUSED_VARIABLE task_data_t data
) {
  return 0;
}

WEAK
void uart_deregister_match(UNUSED_VARIABLE const char *c) { }

WEAK
void uart_flush_rx(void) { }

WEAK
void uart_flush_tx(void) { }

#define UART_FLUSH() uart_flush_rx(),uart_flush_tx()

WEAK
char uart_peek_char(void) { return 0; }

WEAK
char uart_get_char(void) { return 0; }

WEAK
void uart_put_char(UNUSED_VARIABLE char c) { }

WEAK
size_t uart_peek_string(UNUSED_VARIABLE char* str,
    UNUSED_VARIABLE size_t n) { return 0; }

WEAK
size_t uart_get_string(UNUSED_VARIABLE char* str,
    UNUSED_VARIABLE size_t n) { return 0; }

WEAK
void uart_put_string(UNUSED_VARIABLE const char* str) { }

WEAK
void uart_put_pgm_string(UNUSED_VARIABLE PGM_P str) { }

WEAK
void uart_put_uint(UNUSED_VARIABLE unsigned int u) { }

WEAK
void uart_put_int(UNUSED_VARIABLE int n) { }

WEAK
void uart_put_float(UNUSED_VARIABLE float f, UNUSED_VARIABLE uint8_t m) { }

WEAK
void uart_new_line(void) { }

WEAK
void uart_clear(void) { }
