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
 * @file kernel_config.h
 * @author notweerdmonk
 * @brief header file with configuration for kernel module
 */

#ifndef _KERNEL_CONFIG_H_
#define _KERNEL_CONFIG_H_

#define c_SYS_WDT_TIMEOUT PORT_WDT_TIMEOUT_120MS

#define c_SYS_MS_TICKS (uint16_t)(F_SYS_TICK/1000)

#define c_SYS_MAX_TICKS (uint16_t)(0xffff - (0xffff % c_SYS_MS_TICKS))

#define c_SYS_MAX_TASKS 8

#define c_SYS_MAX_EVENTS 8

#define c_SYS_MAX_EVT_HANDLERS 4

#define c_SYS_MAX_SW_TIMERS 8

#define c_SYS_PWM_MAX 255

#endif /* _KERNEL_CONFIG_H_ */
