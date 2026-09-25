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
 * @file utility.h
 * @author notweerdmonk
 * @brief header file with utility and helper macros
 */

#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <common.h>

#define DELAY_MS(x) _delay_ms(x)
#define DELAY_US(x) _delay_us(x)

#define TIMER_VAL_FROM_SEC(x)   (x)*1000

#define CONCAT_EXPAND(A, B) A ## B
#define CONCAT(A, B) CONCAT_EXPAND(A, B)

#define STRINGIFY_HELPER(s) #s
#define STRINGIFY(s) STRINGIFY_HELPER(s)

/* get 8-bit mask from 8-bit value */
#define BITMASK(byte) \
  ({                                  \
    register uint8_t mask;            \
    __asm__ __volatile__(             \
        "ldi %0, 1"           "\n\t"  \
        "mov __tmp_reg__, %1" "\n\t"  \
        "rjmp .+2"            "\n\t"  \
        "lsl %0"              "\n\t"  \
        "dec __tmp_reg__"     "\n\t"  \
        "brpl .-6"            "\n\t"  \
        : "=&r" (mask)                \
        : "r"   (byte)                \
        : "r0" );                     \
    mask;                             \
  })

/* bitmask manipulation */
/*****************************************************************************/
#define SET_MASK(x, y)      (x) |= (y)
#define CLR_MASK(x, y)      (x) &= ~(y)
#define ZERO_MASK(x)        (x) = 0

#define SET_BIT(x, y)       (x) |= BITMASK(y)
#define CLR_BIT(x, y)       (x) &= ~BITMASK(y)
#define GET_BIT(x, y)       ((x) & BITMASK(y))
/*****************************************************************************/

/* register manipulation */
/*****************************************************************************/
#define SET_DDR(x, y)       (CONCAT(DDR,x) |= (y))
#define CLR_DDR(x, y)       (CONCAT(DDR,x) &= ~(y))

#define SET_PORT(x, y)      (CONCAT(PORT,x) |= (y))
#define CLR_PORT(x, y)      (CONCAT(PORT,x) &= ~(y))
#define ZERO_PORT(x)        (CONCAT(PORT,x) = 0)
#define GET_PORT(x)         (CONCAT(PORT,x))

#define OUTPUT_PORT(x)      (CONCAT(DDR,x) = 0xFF)
#define INPUT_PORT(x)       (CONCAT(DDR,x) = 0x00)

#define OUTPUT_PIN(x, y)    (CONCAT(DDR,x) |= (BITMASK(y)))
#define INPUT_PIN(x, y)     (CONCAT(DDR,x) &=	~(BITMASK(y)))

#define SET_PIN(x, y)       (CONCAT(PORT,x) |= (BITMASK(y)))
#define CLR_PIN(x, y)       (CONCAT(PORT,x) &= ~(BITMASK(y)))
#define GET_PIN(x, y)       (CONCAT(PIN,x) & (BITMASK(y)))
/*****************************************************************************/

/*****************************************************************************/
/* takes number literals as argument for bit */
#define OUTPUT_PIN_IMM(x, y) \
  asm volatile(                                   \
      "sbi %[ddr], %[bit]\n"                      \
      :: [ddr] "I" _SFR_IO_ADDR(CONCAT(DDR, x)),  \
      [bit] "I" (y))

#define INPUT_PIN_IMM(x, y) \
  asm volatile(                                   \
      "cbi %[ddr], %[bit]\n"                      \
      :: [ddr] "I" _SFR_IO_ADDR(CONCAT(DDR, x)),  \
      [bit] "I" (y))

/* takes number literals as argument for bit */
#define SET_PIN_IMM(x, y) \
  asm volatile(                                     \
      "sbi %[port], %[bit]\n"                       \
      :: [port] "I" _SFR_IO_ADDR(CONCAT(PORT, x)),  \
      [bit] "I" (y))

#define CLR_PIN_IMM(x, y) \
  asm volatile(                                     \
      "cbi %[port], %[bit]\n"                       \
      :: [port] "I" _SFR_IO_ADDR(CONCAT(PORT, x)),  \
      [bit] "I" (y))

#if defined(__AVR_ATmega48__) || defined(__AVR_ATmega48A__) ||      \
    defined(__AVR_ATmega48P__) || defined(__AVR_ATmega48PA__) ||    \
    defined(__AVR_ATmega88__) || defined(__AVR_ATmega88A__) ||      \
    defined(__AVR_ATmega88P__) || defined(__AVR_ATmega88PA__) ||    \
    defined(__AVR_ATmega168__) || defined(__AVR_ATmega168A__) ||    \
    defined(__AVR_ATmega168P__) || defined(__AVR_ATmega168PA__) ||  \
    defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)

#define TOGGLE_PIN(x, y)   (CONCAT(PIN, x) |= (BITMASK(y)))

/* takes number literals as argument for bit */
#define TOGGLE_PIN_IMM(x, y) \
  asm volatile(                                   \
      "sbi %[pin], %[bit]\n"                      \
      :: [pin] "I" _SFR_IO_ADDR(CONCAT(PIN, x)),  \
      [bit] "I" (y))

#else

#define TOGGLE_PIN(x, y)   (CONCAT(PORT,x) ^= (BITMASK(y)))

#endif

#define POLL_PIN_LEVEL(port, pin, state) \
  ({                                                      \
    static uint8_t _debounce = 0;                         \
    if ( (CONCAT(PIN,port) & (BITMASK(pin))) == state )   \
      {if (++_debounce == -1) _debounce = 3;}             \
    else _debounce = 0;                                   \
    (_debounce >= 3) ? TRUE : FALSE;                      \
  })
/*****************************************************************************/

#define ATOMIC_COPY_BYTE(desc, src) \
  do { cli(); dest = src; sei(); } while (0)

#define ATOMIC_COPY_WORD(dest, src) \
  do { cli(); dest = src; sei(); } while (0)

#define GET_PROGMEM_ADDR(pgm_ptr, ...) \
  pgm_read_word((pgm_ptr) + __VA_ARGS__)

#define GET_PROGMEM_STR(pgm_ptr) \
  ({                                      \
     char __buf[128];                     \
     strcpy_P(__buf, (PGM_P)(pgm_ptr));   \
     __buf;                               \
  })

#define HEX_TO_BYTE(hex_ptr) \
  (uint8_t)( (hex_to_nibble(hex_ptr) << 4) |  \
             (hex_to_nibble(hex_ptr + 1)) )

#define HEX_TO_WORD(hex_ptr) \
  (uint16_t)( (hex_to_nibble(hex_ptr) << 12) |      \
              (hex_to_nibble(hex_ptr + 1) << 8) |   \
              (hex_to_nibble(hex_ptr + 2) << 4) |   \
              (hex_to_nibble(hex_ptr + 3)) )

/* functions */
/*****************************************************************************/
void byte_to_hex(uint8_t byte, char *str);

void word_to_hex(uint16_t word, char *str);

uint8_t hex_to_nibble(const char *hex);
/*****************************************************************************/

#endif /* _UTILITY_H_ */
