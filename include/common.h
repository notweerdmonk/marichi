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
 * @file common.h
 * @author notweerdmonk
 * @brief common declarations and header files used in all modules
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#include <avr/io.h>
#include <avr/common.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define UNUSED_FUNCTION __attribute__ (( unused ))

#define UNUSED_VARIABLE __attribute__ (( unused ))

#define DEPRECATED_FUNCTION(msg) __attribute__ (( deprecated(msg) ))

#define DEPRECATED_VARIABLE(msg) __attribute__ (( deprecated(msg) ))

#define WEAK __attribute__ (( weak ))

typedef uint8_t boolean;

#define TRUE  (boolean)1
#define FALSE 0

#define INVALID_ID 255

/* ASCII character codes */
#define c_RETURN   0x0D
#define c_NEWLINE  0x0A
#define c_TAB      0x09
#define c_BKSPACE  0x08
#define c_ESCAPE   0x1B
#define c_DEL      0x7F

#define ASCII(c) (48 + c)

#define CLEARSCREEN_STRING "\x1b\x5b\x48\x1b\x5b\x32\x4a"
#define NEWLINE_STRING   "\x0d\x0a"

/* pin that is not connected */
#define PIN_NC 0x1f

#endif /* _COMMON_H_ */
