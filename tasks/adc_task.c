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
 * @file adc_task.c
 * @author notweerdmonk
 * @brief Use ADC to print button pressed on the Arduino LCD-Keypad shield.
 */

#include <common.h>
#include <coroutine/coroutine.h>
#include <tasks_interface.h>
#include <adc.h>
#include <uart.h>
#include <lcd.h>

event_id_t adc_app_event = INVALID_ID;

void* adc_app(UNUSED_VARIABLE task_data_t data) {

  static const char adc_app_string1[] PROGMEM = "ADC demo app is running";
  static const char adc_app_string2[] PROGMEM = "ADC value: ";

  static const char adc_app_string_btn[] PROGMEM = "button:";
  static const char adc_app_string_left[] PROGMEM = "left";
  static const char adc_app_string_right[] PROGMEM = "right";
  static const char adc_app_string_up[] PROGMEM = "up";
  static const char adc_app_string_down[] PROGMEM = "down";
  static const char adc_app_string_select[] PROGMEM = "select";
  static const char adc_app_string_none[] PROGMEM = "none";

  COROUTINE_BEGIN();

  if (adc_app_event == INVALID_ID) {
    if ((adc_app_event = register_event()) == INVALID_ID) {
      crash(ERR_INVALID_RETURN);
    }
  }

  if (!is_lcd_ready()) {
    wait_on_event(get_lcd_ready_event());
    COROUTINE_YIELD();
  }

  if (!acquire_resource(SYS_RES_ADC, FALSE, FALSE)) {
    ADC_INIT(128, AVCC);
    DIGITAL_INPUT_DISABLE_PIN(0);

    release_resource(SYS_RES_ADC);
  }

  if (!acquire_resource(SYS_RES_LCD, FALSE, FALSE)) {

    lcd_set_cursor(1, 0);
    lcd_put_pgm_string(adc_app_string_btn);

    release_resource(SYS_RES_LCD);
  }

  if (!acquire_resource(SYS_RES_UART, FALSE, FALSE)) {
#ifdef UART_IOSTREAM
    printf("%s\n", GET_PROGMEM_STR(adc_app_string1));
#else
    uart_new_line();
    uart_put_pgm_string(adc_app_string1);
    uart_new_line();
#endif

    release_resource(SYS_RES_UART);
  }

  COROUTINE_YIELD();

  if (resumed()) {
    COROUTINE_RESET();
  }

  if (!acquire_resource(SYS_RES_ADC, FALSE, FALSE)) {
    uint16_t cur_adc_value = adc_single_conversion(0);

    if (!acquire_resource(SYS_RES_UART, TRUE, FALSE)) {

      static uint16_t prev_adc_value = 0;

      if (
          cur_adc_value > (prev_adc_value + 20) ||
          (
            prev_adc_value > 0 &&
            cur_adc_value < (prev_adc_value - 20)
          )
      ) {
#ifdef UART_IOSTREAM
        printf("%s%d\n", GET_PROGMEM_STR(adc_app_string2), cur_adc_value);
#else
        uart_put_pgm_string(adc_app_string2);
        uart_put_uint(cur_adc_value);
        uart_new_line();
#endif

        if (adc_app_event != INVALID_ID) {
          trigger_event(adc_app_event, EV_NOW);
          deregister_event(adc_app_event);
          adc_app_event = INVALID_ID;
        }

        prev_adc_value = cur_adc_value;
      }

      /* Do not release UART */
    }

    if (!acquire_resource(SYS_RES_LCD, TRUE, FALSE)) {

      lcd_set_cursor(1, 7);
      lcd_clear_till(6);

      if ( cur_adc_value < 100 ) {
        lcd_put_pgm_string(adc_app_string_right);
      }
      else if ( ( cur_adc_value > 100 ) && ( cur_adc_value < 300 ) ) {
        lcd_put_pgm_string(adc_app_string_up);
      }
      else if ( ( cur_adc_value > 300 ) && ( cur_adc_value < 400 ) ) {
        lcd_put_pgm_string(adc_app_string_down);
      }
      else if ( ( cur_adc_value > 400 ) && ( cur_adc_value < 700 ) ) {
        lcd_put_pgm_string(adc_app_string_left);
      }
      else if ( ( cur_adc_value > 700 ) && ( cur_adc_value < 900 ) ) {
        lcd_put_pgm_string(adc_app_string_select);
      }
      else {
        lcd_put_pgm_string(adc_app_string_none);
      }

    /* Do not release LCD */
    }

    release_resource(SYS_RES_ADC);
  }

  COROUTINE_END();
}

DEFINE_INTERNAL_INTERFACE(
    tasks,
    adc_app_api,
    .fn = adc_app,
    .data = { .ptr = NULL },
    .is_service = FALSE
);
