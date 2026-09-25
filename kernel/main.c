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
 * @file main.c
 * @author notweerdmonk
 * @brief main function for standalone kernel
 */

#include <config.h>
#include <kernel.h>
#include <utility.h>

#ifdef __STANDALONE

#if defined __ENABLE_SIMULATION
/*
 * avr-gcc does not have /usr/include in its default search paths.
 * but we use a user local installtion with pkg-config
 */
#include <avr_mcu_section.h>

AVR_MCU(16000000, "atmega328p");
AVR_MCU_VCD_FILE("simulation/avr_kernel_trace.vcd", 1000);

const struct avr_mmcu_vcd_trace_t _avr_kernel_trace[]  _MMCU_ = {
  { AVR_MCU_VCD_SYMBOL("SYSTICK"), .mask = (1 << OCF1A), .what = (void*)&TIFR1, },
  { AVR_MCU_VCD_SYMBOL("LED"), .mask = (1 << PB5), .what = (void*)&PORTB, },
  { AVR_MCU_VCD_SYMBOL("WDT"), .mask = (1 << WDIF), .what = (void*)&WDTCSR, },
  { AVR_MCU_VCD_SYMBOL("TASK_TIMEOUT"), .mask = (1 << INTF1), .what = (void*)&EIFR, },
  { AVR_MCU_VCD_SYMBOL("TIMER0_OCRA"), .mask = (1 << OCF0A), .what = (void*)&TIFR0, },
  { AVR_MCU_VCD_SYMBOL("TXD"), .mask = (1 << PD1), .what = (void*)&PORTD, },
  { AVR_MCU_VCD_SYMBOL("RXD"), .mask = (1 << PD0), .what = (void*)&PORTD, },
  { AVR_MCU_VCD_SYMBOL("UDR0"), .what = (void*)&UDR0, },
  { AVR_MCU_VCD_SYMBOL("UDRE0"), .mask = (1 << UDRE0), .what = (void*)&UCSR0A, },
};
#endif


/**************** main *******************/
int main(void) {
  /*
   * NOTE:
   * set Pin 13 (PB5) as output for Arduino UNO to avoid turning the LED on
   */
  //OUTPUT_PORT(B);

  /* Stop clock to peripherals in order to reduce power */
  set_power_reduction(SYS_RES_TWI);
  set_power_reduction(SYS_RES_SPI);
  set_power_reduction(SYS_RES_TIMER2);

  /* Register applications */
  tasks_main();

  return 0;
}

#endif /* __STANDALONE */
