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


#ifndef PORT_MEGA328P_H
#define PORT_MEGA328P_H

/**
 * @file port_mega32.h
 * @author notweerdmonk
 * @brief Portability layer macros fro ATmega32
 */

#ifdef __AVR_ATmega32__

#define port_sys_tick_cb TIMER1_COMPA_vect

#define port_init_systick_timer(duration) \
  do { \
    TCCR1B = (1<<WGM12);  /*CTC mode*/ \
    TIMSK |= (1<<OCIE1A); /*enable output compare interrupt for channel a*/ \
    TCNT1 = 0;            /*reset timer counter*/ \
    OCR1A = duration;     /*set the top of CTC on channel a*/ \
    TCCR1B |= (1<<CS10);  /*cs2..0=0b001, activate timer, 1:1 pre-scaler*/ \
  } while (0)

#define PORT_UDRE_VECT USART_UDRE_vect

#define PORT_RXC_VECT USART_RXC_vect

#define PORT_UDR UDR

#define port_set_uart_char_size(n) \
  if (n > 5) { \
  if (n == 9) UCSRC = (1<<URSEL) | (3<<UCSZ0); \
  else UCSRC = (1<<URSEL) | ((n-5)<<UCSZ0); }

#define port_set_uart_baud_rate(b) \
  do { \
    UBRRL = baud; \
    UBRRH = baud >> 8; \
  } while (0)

#define port_uart_init() \
  do { \
    UCSRB = (1<<RXCIE) | (1<<RXEN) | (1<<TXEN); \
    UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0); \
  } while (0)

#define port_enable_udre_interrupt() \
  UCSRB |= (1<<UDRIE)

#define port_disable_udre_interrupt() \
  UCSRB &= ~(1<<UDRIE)

#define port_enable_rxc_interrupt() \
  UCSRB |= (1<<RXCIE)

#define port_disable_rxc_interrupt() \
  UCSRB &= ~(1<<RXCIE)

#endif

#endif /* PORT_MEGA328P_H */
