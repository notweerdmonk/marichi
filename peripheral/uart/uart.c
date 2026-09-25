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
 * @file uart.c
 * @author notweerdmonk
 * @brief driver for UART module
 */

#include <config.h>
#include <common.h>
#include <utility.h>
#include <uart_config.h>
#include <uart.h>
#include <port.h>

/*****************************************************************************/

typedef struct _uart {
/*
 * NOTE:
 * why using char instead of int8_t results in lesser instructions?
 * (provided buffer size is <= 128)
 */
 volatile uint8_t rx_count;
 volatile uint8_t tx_count;
          char *p_rx_in, *p_rx_out;
          char *p_tx_in, *p_tx_out;
          char rx_buffer[c_UART_RX_BUFFER_LEN];
          char tx_buffer[c_UART_RX_BUFFER_LEN];
} uart_t;

typedef struct _match {
  event_id_t ev;
  uint8_t count : 4;
  uint8_t len   : 4;
  char seq[c_UART_MAX_SEQ_LEN];
} match_t;

typedef struct _uart_match {
  uint8_t match_idx_max;
  match_t match[c_UART_MAX_RECV_CB];
} uart_match_t;

/*****************************************************************************/

#ifdef __ENABLE_UART

static uart_t uart;

static uart_match_t match;

/*****************************************************************************/

#ifdef UART_IOSTREAM

/* functions */
int uart_stream_put_char(char c, FILE *stream) {
  if (c == '\n') {
    uart_stream_put_char('\r', stream);
  }
  uart_put_char(c);
  return 0;
}

int uart_stream_get_char(UNUSED_VARIABLE FILE *stream) {
  return uart_get_char2();
}

FILE uart_stream = FDEV_SETUP_STREAM(uart_stream_put_char,
                                     uart_stream_get_char,
                                     _FDEV_SETUP_RW);
#endif /* UART_IOSTREAM */

void uart_init(uart_config_t *config) {
#ifdef UART_IOSTREAM

  stdout = stdin = stderr = &uart_stream;

#endif

  if (config->baud_rate == 0) {
    config->baud_rate = c_DEFAULT_BAUD_RATE;
  }

  PORT_UART_SET_BAUD_RATE(config->baud_rate);
  PORT_UART_SET_CHAR_SIZE(config->char_size);
  PORT_UART_SET_STOP_BITS(config->stop_bits);
  PORT_UART_SET_PARITY(config->parity);
  PORT_UART_INIT();

  UART_FLUSH();
}

uint8_t uart_register_match(const char *str, event_handler handler, task_data_t data) {
  uint8_t ret = -1;

  if ((handler != NULL) &&
      (match.match_idx_max < c_UART_MAX_RECV_CB)) {

    match_t *p_match = &match.match[match.match_idx_max];
    uint8_t i;

    for (i = 0; (i < c_UART_MAX_SEQ_LEN) && (*str != 0); i++ ) {
      p_match->seq[i] = *str++;
    }
    p_match->len = i;

    p_match->ev = register_event();
    if (p_match->ev != INVALID_ID) {
      if (!register_event_handler(p_match->ev, handler, data)) {
        match.match_idx_max++;
        ret = 0;
      }
    }
  }
  return ret;
}

void uart_deregister_match(const char *c) {
  if (c != NULL) {
    uint8_t i;
    boolean found = FALSE;

    for (i = 0; i < match.match_idx_max; i++) {
      if (found == TRUE) {
        match.match[i] = match.match[i+1];
      }
      else if (match.match[i].seq[0] == *c) {
        found = TRUE;
      }
    }

    if (found) {
      match.match_idx_max--;
    }
  }
}

ISR(PORT_UDRE_VECT, ISR_BLOCK) {
  if (uart.tx_count > 0) {
    PORT_UDR = *uart.p_tx_out;

    if(++uart.p_tx_out >= uart.tx_buffer + c_UART_TX_BUFFER_LEN) {
      uart.p_tx_out = uart.tx_buffer;
    }

    if(--uart.tx_count == 0) {
      PORT_DISABLE_UDRE_INTERRUPT();
    }
  }
}

ISR(PORT_RXC_VECT, ISR_BLOCK) {
  uint8_t udr = PORT_UDR;

  if (match.match_idx_max > 0) {

    for (uint8_t i = 0; i < match.match_idx_max; i++) {
      match_t *p_match = &match.match[i];

      if (p_match->seq[p_match->count] == udr) {
        if (++p_match->count == p_match->len) {
          p_match->count = 0;
          trigger_event(p_match->ev, EV_DEFER);
        }
        return;
      }
      else {
        p_match->count = 0;
      }
    }
  }

  *uart.p_rx_in = udr;
  ++uart.rx_count;

  if(++uart.p_rx_in >= uart.rx_buffer + c_UART_RX_BUFFER_LEN) {
    uart.p_rx_in = uart.rx_buffer;
  }
}

void uart_flush_rx() {
  uart.p_rx_in = uart.p_rx_out = uart.rx_buffer;
  uart.rx_count = 0;
}

void uart_flush_tx() {
  while(uart.tx_count > 0);

  uart.p_tx_in = uart.p_tx_out = uart.tx_buffer;
  uart.tx_count = 0;
}

char uart_peek_char(void) {
  return (uart.rx_count > 0 ? *uart.p_rx_out : 0);
}

/**
 * Read a single charater from serial buffer.
 *
 * Calls to this function will not block. A NULL character is returned when the
 * buffer is empty.
 *
 * @return char A character from the serial buffer.
 */
char uart_get_char() {
  return (uart.rx_count > 0 ?
      ({
        char c;

        PORT_DISABLE_RXC_INTERRUPT();

        uart.rx_count--;
        c = *uart.p_rx_out;
        if (++uart.p_rx_out >= uart.rx_buffer + c_UART_RX_BUFFER_LEN) {
          uart.p_rx_out = uart.rx_buffer;
        }

        PORT_ENABLE_RXC_INTERRUPT();

        c;
      }) :
      0);
}

/**
 * Read a single charater from serial buffer, blockingly.
 *
 * Calls to this function will block unitl a character is available in the
 * serial buffer.
 *
 * @return char A character from the serial buffer.
 */
char uart_get_char2() {
  while (!uart.rx_count);
  char c;

  PORT_DISABLE_RXC_INTERRUPT();

  uart.rx_count--;
  c = *uart.p_rx_out;
  if (++uart.p_rx_out >= uart.rx_buffer + c_UART_RX_BUFFER_LEN) {
    uart.p_rx_out = uart.rx_buffer;
  }

  PORT_ENABLE_RXC_INTERRUPT();

  return c;
}

size_t uart_peek_string(char* str, size_t n) {
  return (uart.rx_count > 0 ?
      ({
        size_t len = 0;
        char* p_char = str;
        char* p_rx_out = uart.p_rx_out;

        while(--n) {
          if (*p_rx_out) {
            *p_char++ = *p_rx_out++, len++;
            if(p_rx_out >= uart.rx_buffer + c_UART_RX_BUFFER_LEN) {
              p_rx_out = uart.rx_buffer;
            }
          }
          else
            break;
        }
        *p_char = 0;

        len;
      }) :
      0);
}

/* TODO: handle case where n is greater than rx_count */
size_t uart_get_string(char* str, size_t n) {
  if (n > uart.rx_count) {
    n = uart.rx_count;
  }

  char c;
  size_t len = 0;

  while(--n) {
    if ((c = uart_get_char()))
      *str++ = c, len++;
    else
      break;
  }
  *str = 0;

  return len;
}

void uart_put_char(char c) {
  while (uart.tx_count == c_UART_TX_BUFFER_LEN);

  PORT_DISABLE_UDRE_INTERRUPT();

  ++uart.tx_count;
  *uart.p_tx_in = c;
  if (++uart.p_tx_in >= uart.tx_buffer + c_UART_TX_BUFFER_LEN) {
    uart.p_tx_in = uart.tx_buffer;
  }

  PORT_ENABLE_UDRE_INTERRUPT();
}

void uart_put_string(const char *s) {
  while (*s) uart_put_char(*s++);
}


void uart_put_pgm_string(PGM_P s) {
  for (char c = pgm_read_byte(s); c != 0; c = pgm_read_byte(++s)) {
    uart_put_char(c);
  } 
}

void uart_put_uint(unsigned int u) {
#if (__SIZEOF_INT__ == 1)
  uint8_t digits[4];
#elif (__SIZEOF_INT__ == 2)
  uint8_t digits[6];
#endif
  uint8_t idx = 0;

  if (u == 0) {
    digits[idx++] = 0;
  } else {
    while (u > 0) {
      digits[idx++] = u % 10;
      u /= 10;
    }
  }

  while (idx > 0) {
    uart_put_char(ASCII(digits[--idx]));
  }
}

void uart_put_int(int n) {
  if (n < 0) {
    uart_put_char('-');
    n = -n;
  }

  uart_put_uint(n);
}

/* TODO: sprintf with -lprintf_flt or dtostrf, but bigger size */
void uart_put_float(float f, uint8_t m) {
  /* Fractional part greater than 4 does not work */
  if (m > 4) m = 4;

  uart_put_int(f);
  uart_put_char('.');

  f = f - (int)f;
  for (uint8_t i = 0; i < m; i++) {
    f = f * 10;
  }

  uart_put_int(f);
}

void uart_new_line() {
  uart_put_string(NEWLINE_STRING);
}

void uart_clear() {
  uart_put_string(CLEARSCREEN_STRING);
}

#endif /* __ENABLE_UART */
