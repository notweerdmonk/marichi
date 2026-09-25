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
 * @file tasks_main.c
 * @author notweerdmonk
 * @brief Register tasks with the kernel.
 */

#include <kernel.h>
#include <tasks_interface.h>
#include <avr/pgmspace.h>

static
UNUSED_FUNCTION
INTERNAL_INTERFACE_BYTE_PTR
tasks_interface_ptr(size_t index) {
  return &__start_tasks_interface_registry + index;
}

static
INTERNAL_INTERFACE_PTR(tasks) next_tasks_interface() {
  /*
   * Treat the section as a raw byte array to iterate safely over variable-sized
   * structures
   */

  static DEFINE_INTERNAL_INTERFACE_START_PTR(tasks, ptr);

  DEFINE_INTERNAL_INTERFACE_END_PTR(tasks, end);

  if (ptr < end) {
    /*
     * We look at the very first element (the name string pointer) to identify
     * the block
     */
    INTERNAL_INTERFACE_PTR(tasks) api =
      INTERNAL_INTERFACE_PTR_CAST(tasks, ptr);

    ptr += SIZEOF_INTERNAL_INTERFACE(tasks);

    /*
     * Advance pointer by the size of this interface block
     */
    return api;
  }

  return NULL;
}

void tasks_main() {
  /* Stop clock to peripherals in order to reduce power */
  //set_power_reduction(SYS_RES_TWI);
  //set_power_reduction(SYS_RES_SPI);
  //set_power_reduction(SYS_RES_TIMER2);

  /* Register tasks */
  for (
      INTERNAL_INTERFACE_PTR(tasks) api = NULL;
      (api = next_tasks_interface());
      register_task(api->fn, api->data, api->is_service)
  );
}
