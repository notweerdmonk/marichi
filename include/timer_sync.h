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
 * @file timer_sync.h
 * @author notweerdmonk
 * @brief header file for synchronization of Timer/Counters
 */

#ifndef _TIMER_SYNC_H_
#define _TIMER_SYNC_H_

#include <port.h>

#define TIMER_SYNCHRONIZATION_MODE_START() \
  PORT_TIMER_SYNCHRONIZATION_MODE_START()

#define TIMER_SYNCHRONIZATION_MODE_END() \
  PORT_TIMER_SYNCHRONIZATION_MODE_END()

#define SYNC_TIMER_PRESCALER_RESET() \
  PORT_SYNC_TIMER_PRESCALER_RESET()

#define ASYNC_TIMER_PRESCALER_RESET() \
  PORT_ASYNC_TIMER_PRESCALER_RESET()

#endif /* _TIMER_SYNC_H_ */
