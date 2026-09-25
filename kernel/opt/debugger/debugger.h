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
 * @file debugger.h
 * @author notweerdmonk
 * @brief header file for debugger module
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <common.h>
#include <config.h>
#include <kernel.h>

#ifdef __ENABLE_DEBUGGER

typedef struct _debugger_params {
  uint8_t verbosity;
} debugger_params_t;

void* debugger_task(task_data_t data);

#endif /* __ENABLE_DEBUGGER */

#endif /* DEBUG_H_ */
