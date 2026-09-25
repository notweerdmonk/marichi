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
 * @file wdt.c
 * @author notweerdmonk
 * @brief WDT related functions
 */

#include <kernel_config.h>
#include <kernel.h>
#include <setjmp.h>

/*****************************************************************************/
/* Save MCUSR at startup                                                     */
/*****************************************************************************/

/*
 * Save MCUSR to examine system reset cause and disable watchdog timer during
 * startup.
 */

__attribute__((section (".noinit")))
static uint8_t mcusr_mirror;

void wdt_first(void) __attribute__((naked)) __attribute__((section(".init3")));

void wdt_first(void) {
#if defined OPTIBOOT_MAJOR && defined OPTIBOOT_MINOR

#if OPTIBOOT_MAJOR >= 4 && OPTIBOOT_MINOR > 4

  /* on Arduino boards with Optiboot v4.6 and later, MCUSR is saved in r2 */
  __asm__ __volatile__ ("sts %0, r2\n" : "=m" (mcusr_mirror) :);

#else

  /* older versions will clear MCUSR */
  mcusr_mirror = 0;

#endif /* OPTIBOOT_MAJOR >= 4 && OPTIBOOT_MINOR > 4 */

#else

  mcusr_mirror = MCUSR;
  MCUSR = 0;

#endif /* defined OPTIBOOT_MAJOR && defined OPTIBOOT_MINOR */

  wdt_disable();
}

/**
 * @brief Get the value of MCUSR at boot
 * @return uint8_t The vlaue of internal MCUSR mirror captured at boot
 */
uint8_t get_atboot_mcusr() {
  return mcusr_mirror;
}

/******************************************************************************/
/* Task policing                                                              */
/******************************************************************************/

/*
 * Watchdog timer interrupt is used for policing tasks on newer devices.
 */

#ifdef PORT_WDT_VECT

ISR(PORT_WDT_VECT, ISR_NAKED) {
  wdt_reset();

  /* release any resources owned by current task */
  release_all_resources();

  LONGJMP(*kernel_task_reset_jump_buffer());
  asm("reti");
}

#endif
