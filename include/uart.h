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
 * @file uart.h
 * @author notweerdmonk
 * @brief header file for UART module
 */

#ifndef _UART_H_
#define _UART_H_

#include <common.h>
#include <kernel.h>
#include <uart/uart_config.h>

typedef enum _uart_parity {
  UART_PARITY_DISABLED,
  UART_PARITY_EVEN,
  UART_PARITY_ODD
} uart_parity_t;

typedef struct _uart_config {
  uint32_t baud_rate;
  uint8_t  char_size : 4;
  uint8_t  stop_bits : 2;
  uint8_t  parity : 2;
} uart_config_t;

WEAK
void uart_init(uart_config_t *config);

WEAK
uint8_t uart_register_match(const char *c, event_handler handler, task_data_t data);

WEAK
void uart_deregister_match(const char *c);

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN(SYS_RES_UART);
 */
WEAK
void uart_flush_rx(void);

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN(SYS_RES_UART);
 */
WEAK
void uart_flush_tx(void);

#define UART_FLUSH() uart_flush_rx(),uart_flush_tx()

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN_VALUE(SYS_RES_UART, 0);
 */
WEAK
char uart_peek_char(void);

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN_VALUE(SYS_RES_UART, 0);
 */
WEAK
char uart_get_char(void);

WEAK
char uart_get_char2(void);

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN(SYS_RES_UART);
 */
WEAK
void uart_put_char(char c);

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN_VALUE(SYS_RES_UART, 0);
 */
WEAK
size_t uart_peek_string(char* str, size_t n);

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN_VALUE(SYS_RES_UART, 0);
 */
WEAK
size_t uart_get_string(char* str, size_t n);

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN(SYS_RES_UART);
 */
WEAK
void uart_put_string(const char* str);

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN(SYS_RES_UART);
 */
WEAK
void uart_put_pgm_string(PGM_P str);

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN(SYS_RES_UART);
 */
WEAK
void uart_put_uint(unsigned int u);

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN(SYS_RES_UART);
 */
WEAK
void uart_put_int(int n);

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN(SYS_RES_UART);
 */
WEAK
void uart_put_float(float f, uint8_t m);

WEAK
void uart_new_line(void);

WEAK
void uart_clear(void);

#endif /* _UART_H_ */
