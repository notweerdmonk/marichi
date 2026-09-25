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
 * @file counter_task.c
 * @author notweerdmonk
 * @brief Use TIMER0 compare match A interrupt to increment a variable roughly
 * every second and use events to wake up the task and print the variable on
 * LCD.
 */

#include <common.h>
#include <coroutine/coroutine.h>
#include <tasks_interface.h>
#include <timer0.h>
#include <uart.h>
#include <lcd.h>

enum { COUNTER_SCALE = 125 };
event_id_t counter_event = INVALID_ID;
volatile unsigned int count;
unsigned char scale;
extern char trigger_flag;

extern event_id_t adc_app_event;

ISR(PORT_TIMER0_COMPA_VECT) {
  if (++scale == COUNTER_SCALE) {
    scale = 0;
    count++;

    trigger_event(counter_event, EV_NOW);

    /* For adc_autotrigger_app */
    trigger_flag = 1;
  }
}

void* counter_app(UNUSED_VARIABLE task_data_t data) {

  static const char counter_app_string[] PROGMEM = "count:";

  COROUTINE_BEGIN();

  if (restarted()) {
    scale = 0;
    count = 0;
  }

  if (!is_lcd_ready()) {
    wait_on_event(get_lcd_ready_event());
    COROUTINE_YIELD();
  }

  /*
   * Synchronize with ADC app if it has registered an event and itself to avoid
   * contention for LCD and UART
   */
  wait_on_event(adc_app_event);
  COROUTINE_YIELD();

  if (!acquire_resource(SYS_RES_LCD, FALSE, FALSE)) {
    lcd_set_cursor(0, 0);
    LCD_CLEAR_LINE();
    lcd_put_pgm_string(counter_app_string);

    release_resource(SYS_RES_LCD);
  }

  if (!acquire_resource(SYS_RES_UART, FALSE, FALSE)) {
    uart_put_string("count:");
    uart_put_int(count);
    uart_new_line();

    release_resource(SYS_RES_UART);
  }

  if (counter_event == INVALID_ID) {
    if ((counter_event = register_event()) == INVALID_ID) {
      crash(ERR_INVALID_RETURN);
    }
  }

  if (!acquire_resource(SYS_RES_TIMER0, FALSE, FALSE)) {
    TIMER0_SET_WAVEFORM_GENERATION_MODE(PORT_TIMER0_WGM_CTC);
    TIMER0_ENABLE_OCR_INTERRUPT(A);
    TIMER0_SET_COUNT(0);
    TIMER0_SET_OCR(A, 125);
    TIMER0_SET_CS(1024);

    /* Do not release TIMER0 */
  }

  COROUTINE_YIELD();

  /* Trigger the watchdog interrupt or raise task timeout exception. */
  if (count % 10 == 0) {
    while (1);
  }

  if (!acquire_resource(SYS_RES_LCD, FALSE, FALSE)) {

    lcd_set_cursor(0, 6);
    lcd_clear_till(6);
    lcd_put_uint(count);

    release_resource(SYS_RES_LCD);
  }

  if (!acquire_resource(SYS_RES_UART, FALSE, FALSE)) {
    uart_put_string("count:");
    uart_put_int(count);
    uart_new_line();

    release_resource(SYS_RES_UART);
  }

  wait_on_event(counter_event);

  COROUTINE_END();
}

DEFINE_INTERNAL_INTERFACE(
    tasks,
    counter_app_api,
    .fn = counter_app,
    .data = { .ptr = NULL },
    .is_service = FALSE
);
