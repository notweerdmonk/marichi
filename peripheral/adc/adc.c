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
 * @file adc.c
 * @author notweerdmonk
 * @brief implements ADC module
 */

#include <common.h>
#include <utility.h>
#include <kernel.h>
#include <adc.h>

/*
 * NOTE: Internal temperature sensor
 *
 * ADC channel 8 is connected to the internal temperature sensor. The ADC
 * should use the internal reference of 1.1V. A temperature offset (Toffset)
 * and a fixed coefficient (k) are required to calculate the temperature.
 *
 * T = ( (ADCH << 8 | ADCL) - Toffset ) / k
 *
 * Searching the internet provided respective values:
 *
 * T = ( (ADCH << 8 | ADCL) - 324.31 ) / 1.22
 */

uint16_t adc_single_conversion(uint8_t channel) {
  if (channel > 8) {
    return 0xffff;
  }

  /* select channel */
  PORT_ADC_SET_CHAN(channel);

  /* start single conversion */
  PORT_ADC_START_CONVERSION();

  /* wait for conversion to complete */
  while (!PORT_ADC_IS_INTERRUPT_FLAG_SET());

  /* clear ADIF by writing one to it */
  PORT_ADC_CLEAR_INTERRUPT_FLAG();

  return PORT_ADC_DATA_REGISTER;
}

void adc_single_conversion_async(uint8_t channel) {
  if (channel > 8) {
    return;
  }

  /* select channel */
  PORT_ADC_SET_CHAN(channel);

  /* start single conversion */
  PORT_ADC_START_CONVERSION();
}

void adc_set_prescaler(adc_prescaler_t ps) {
  SET_MASK(PORT_ADC_CONTROL_STATUS_REGISTER_A, \
      ((ps & 1) <<  ADPS0) | (((ps >> 1) & 1) << ADPS1) | \
      (((ps >> 2) & 1) << ADPS2));
}

void adc_set_reference(adc_reference_t ref) {
  SET_MASK(PORT_ADC_MUX_CONTROL_REGISTER, \
      ((ref & 1) << REFS0) | (((ref >> 1) & 1) << REFS1));
}

void adc_set_channel(uint8_t ch) {
  if ((ch < 9) || (ch == 14) || (ch == 15)) { 
    PORT_ADC_MUX_CONTROL_REGISTER = \
      (PORT_ADC_MUX_CONTROL_REGISTER & 0xf0) | ch;
  }
}
