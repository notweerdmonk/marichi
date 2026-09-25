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


#ifndef _INTERNAL_INTERFACE_H_
#define _INTERNAL_INTERFACE_H_

/**
 * @file internal_interface.h
 * @author notweerdmonk
 * @brief header file for internal interfaces subsystem
 */

#define INTERNAL_INTERFACE_ADDR_SPACE \
  __flash

#define INTERNAL_INTERFACE_BYTE_PTR_STORAGE_CLASS \
  extern

#define INTERNAL_INTERFACE_ACCESS_QUALIFER \
  const

#define INTERNAL_INTERFACE_BYTE_TYPE \
  char

#define INTERNAL_INTERFACE_BYTE_PTR \
  INTERNAL_INTERFACE_ACCESS_QUALIFER \
  INTERNAL_INTERFACE_ADDR_SPACE \
  INTERNAL_INTERFACE_BYTE_TYPE \
  *

#define INTERNAL_INTERFACE_START(subsystem) \
  __start_ ## subsystem ## _interface_registry

#define INTERNAL_INTERFACE_STOP(subsystem) \
  __stop_ ## subsystem ## _interface_registry

#define DEFINE_INTERNAL_INTERFACE_START_PTR(subsystem, variable_name) \
  INTERNAL_INTERFACE_ACCESS_QUALIFER \
  INTERNAL_INTERFACE_ADDR_SPACE \
  INTERNAL_INTERFACE_BYTE_TYPE \
  *variable_name = &INTERNAL_INTERFACE_START(subsystem)

#define DEFINE_INTERNAL_INTERFACE_END_PTR(subsystem, variable_name) \
  INTERNAL_INTERFACE_ACCESS_QUALIFER \
  INTERNAL_INTERFACE_ADDR_SPACE \
  INTERNAL_INTERFACE_BYTE_TYPE \
  *variable_name = &INTERNAL_INTERFACE_STOP(subsystem)

#define DECLARE_INTERNAL_INTERFACE_START(subsystem) \
  INTERNAL_INTERFACE_BYTE_PTR_STORAGE_CLASS \
  INTERNAL_INTERFACE_ACCESS_QUALIFER \
  INTERNAL_INTERFACE_ADDR_SPACE \
  INTERNAL_INTERFACE_BYTE_TYPE \
  INTERNAL_INTERFACE_START(subsystem)

#define DECLARE_INTERNAL_INTERFACE_STOP(subsystem) \
  INTERNAL_INTERFACE_BYTE_PTR_STORAGE_CLASS \
  INTERNAL_INTERFACE_ACCESS_QUALIFER \
  INTERNAL_INTERFACE_ADDR_SPACE \
  INTERNAL_INTERFACE_BYTE_TYPE \
  INTERNAL_INTERFACE_STOP(subsystem)


#define INTERNAL_INTERFACE_PTR_STORAGE_CLASS \
  extern

#define INTERNAL_INTERFACE_TYPE(subsystem) \
  struct subsystem ## _internal_interface

#define INTERNAL_INTERFACE_PTR(subsystem) \
  INTERNAL_INTERFACE_ACCESS_QUALIFER \
  INTERNAL_INTERFACE_ADDR_SPACE \
  INTERNAL_INTERFACE_TYPE(subsystem) \
  *

#define INTERNAL_INTERFACE(subsystem) \
  INTERNAL_INTERFACE_ACCESS_QUALIFER \
  INTERNAL_INTERFACE_TYPE(subsystem)

#define INTERNAL_INTERFACE_PTR_CAST(subsystem, variable_name) \
  (INTERNAL_INTERFACE_PTR(subsystem)) variable_name

#define SIZEOF_INTERNAL_INTERFACE(subsystem) \
  sizeof(DECLARE_INTERNAL_INTERFACE(subsystem))

#define DECLARE_INTERNAL_INTERFACE(subsystem) \
  struct subsystem ## _internal_interface

/*
 * Use of __flash named address space is redundant with section being specified
 */
#define DEFINE_INTERNAL_INTERFACE(subsystem, name, ...) \
  __attribute__((used, section(#subsystem"_interface_registry"))) \
  INTERNAL_INTERFACE(subsystem) name = { \
    __VA_ARGS__ \
  }

#endif /* _INTERNAL_INTERFACE_H_ */
