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
 * config.h
 *
 * Created: 09-07-2017 12:50:22
 *  Author: weerdmonk 
 */


#ifndef CONFIG_H_
#define CONFIG_H_

#define MAJOR_VERSION 0
#define MINOR_VERSION 1
#define REVISION      80

/* !!! IMPORTANT !!! always define F_CPU before including any AVR headers */
#ifndef F_CPU
#error *** F_CPU not defined! ***
#endif

/* System tick frequency */
#define F_SYS_TICK 10000UL

/* Restrict use of hardware timer used for sys tick */
#define SYS_TICK_TIMER "Timer/Counter 1"
#define RESTRICT_SYS_TICK_TIMER() _Static_assert(0, "Cannot use "SYS_TICK_TIMER)

/* Optiboot bootloader version for Arduino boards */
#define OPTIBOOT_MAJOR 4
#define OPTIBOOT_MINOR 4

/* Print value of MCUSR on boot */
#if 0
#define __PRINT_MCUSR
#endif

/* Define stdio streams for UART communications */
#if 1
#define UART_IOSTREAM
#endif

/*
 * LCD data bus width
 * - LCD_4BIT
 * - LCD_8BIT
 */
#define LCD_4BIT

/* Verbosity level */
#define VERBOSE0 0
#define VERBOSE1 1
#define VERBOSE2 2

#ifndef VERBOSITY
#define VERBOSITY VERBOSE0
#endif

#endif /* CONFIG_H_ */
