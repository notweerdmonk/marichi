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
 * @file adc_stub.c
 * @author notweerdmonk
 * @brief implements ADC module
 * @brief Weak linkage stubs for ADC module
 */

#include <common.h>
#include <adc.h>

inline
uint16_t adc_single_conversion(UNUSED_VARIABLE uint8_t channel) {
  return 0xffff;
}

inline
uint16_t adc_single_conversion_async(UNUSED_VARIABLE uint8_t channel) {
  return 0xffff;
}

#define ADC_INIT(ps, ref)

#define ADC_ENABLE()

#define ADC_DISABLE()

#define ADC_SET_PS(ps)

#define ADC_SET_REF_AREF()

#define ADC_SET_REF_AVCC()

#define ADC_SET_REF_INTERNAL()

#define ADC_SET_CHAN_0()

#define ADC_SET_CHAN_1()

#define ADC_SET_CHAN_2()

#define ADC_SET_CHAN_3()

#define ADC_SET_CHAN_4()

#define ADC_SET_CHAN_5()

#define ADC_SET_CHAN_6()

#define ADC_SET_CHAN_7()

#define ADC_SET_CHAN_8()

#define ADC_SET_CHAN_INTERNAL()

#define ADC_SET_CHAN_GND()

#define ADC_SET_LAR()

#define ADC_SET_NO_LAR()

#define ADC_START_CONVERSION()

#define ADC_ENABLE_AUTO_TRIGGER()

#define ADC_DISABLE_AUTO_TRIGGER()

#define ADC_SET_FREE_RUNNING()

#define ADC_SET_AT_ANALOG_COMPARATOR()

#define ADC_SET_AT_EXTERNAL_IRQ0()

#define ADC_SET_AT_TIMER0_COMPARE_MATCH_A()

#define ADC_SET_AT_TIMER0_OVERFLOW()

#define ADC_SET_AT_TIMER1_COMPARE_MATCH_B()

#define ADC_SET_AT_TIMER1_OVERFLOW()

#define ADC_SET_AT_TIMER1_CAPTURE_EVENT()

#define ADC_ENABLE_INTERRUPT()

#define ADC_DISABLE_INTERRUPT()

#define ADC_CLEAR_INTERRUPT_FLAG()

#define DIGITAL_INPUT_DISABLE_PIN(pin)

typedef enum _port_adc_prescaler adc_prescaler_t;

typedef enum _port_adc_reference adc_reference_t;

inline
void adc_set_prescaler(UNUSED_VARIABLE adc_prescaler_t ps) { }

inline
void adc_set_reference(UNUSED_VARIABLE adc_reference_t ref) { }

inline
void adc_set_channel(UNUSED_VARIABLE uint8_t channel) { }
