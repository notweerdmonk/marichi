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
 * @file adc_autotrigger_task.c
 * @author notweerdmonk
 * @brief Use TIMER0 output compare match A interrupt to trigger ADC coversion
 * and loop over 3 channels printing ADC readings.
 */

#include <common.h>
#include <coroutine/coroutine.h>
#include <adc.h>
#include <uart.h>
#include <lcd.h>

/*
 * adc_autotrigger_app depends on the `trigger_flag` variable that is set in the
 * TIMER0 compare match A interrupt handler. TIMER0 compare match A is required
 * for counter_app. See apps/counter_app.c.
 */

static const char adc_autotrigger_app_string1[] PROGMEM = "ADC auto-trigger app is running";
static const char adc_autotrigger_app_string2[] PROGMEM = "ADC channel: ";
static const char adc_autotrigger_app_string3[] PROGMEM = " ADC value: ";

volatile char trigger_flag = 0;
volatile uint16_t adc_value;

ISR(PORT_ADC_VECT) {
  if (!acquire_resource(SYS_RES_ADC, FALSE, FALSE)) {
    adc_value = ADC_READ();
    release_resource(SYS_RES_ADC);
  }
}

void* adc_autotrigger_app(UNUSED_VARIABLE task_data_t data) {

  COROUTINE_BEGIN();

  if (!is_lcd_ready()) {
    wait_on_event(get_lcd_ready_event());
    COROUTINE_YIELD();
  }

  if (!acquire_resource(SYS_RES_ADC, FALSE, FALSE)) {
    ADC_INIT(128, AVCC);
    /* This auto-trigger is dependent of counter_app. */
    ADC_SET_AT_TIMER0_COMPARE_MATCH_A();
    ADC_ENABLE_AUTO_TRIGGER();
    ADC_ENABLE_INTERRUPT();

    release_resource(SYS_RES_ADC);
  }

  if (!acquire_resource(SYS_RES_UART, FALSE, FALSE)) {
#ifdef UART_IOSTREAM
    printf("%s\n", adc_autotrigger_app_string1);
#else
    uart_new_line();
    uart_put_pgm_string(adc_autotrigger_app_string1);
    uart_new_line();
#endif

    release_resource(SYS_RES_UART);
  }

  COROUTINE_YIELD();

  if (resumed()) {
    COROUTINE_RESET();
  }

  if (trigger_flag == 1) {
    static uint8_t channel = 0;

    if (!acquire_resource(SYS_RES_UART, FALSE, FALSE)) {
      uart_put_pgm_string(adc_autotrigger_app_string2);
      uart_put_uint(channel);
      uart_put_pgm_string(adc_autotrigger_app_string3);
      uart_put_uint(adc_value);
      uart_new_line();
      release_resource(SYS_RES_UART);
    }
    if (++channel > 2) {
      channel = 0;
    }
    if (!acquire_resource(SYS_RES_ADC, FALSE, FALSE)) {
      ADC_SET_CHAN(channel);
      release_resource(SYS_RES_ADC);
    }

    trigger_flag = 0;
  }

  COROUTINE_END();
}
