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


#ifndef _TASK_INTERFACE_H_
#define _TASK_INTERFACE_H_

/**
 * @file tasks_interface.h
 * @author notweerdmonk
 * @brief Header that declare the internal interface for tasks
 */

#include <internal_interface.h>
#include <kernel.h>

DECLARE_INTERNAL_INTERFACE(tasks) {
  task_handler fn;
  task_data_t data;
  boolean is_service;
};

DECLARE_INTERNAL_INTERFACE_START(tasks);
DECLARE_INTERNAL_INTERFACE_STOP(tasks);

#endif /* _TASK_INTERFACE_H_ */
