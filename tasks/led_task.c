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
 * @file led_task.c
 * @author notweerdmonk
 * @brief Blink an LED.
 */

#include <common.h>
#include <coroutine/coroutine.h>
#include <tasks_interface.h>

#define LED_TOGGLE_DURATION_MS 500

void toggle_led(UNUSED_VARIABLE timer_id_t t_id, UNUSED_VARIABLE task_data_t data) {
  TOGGLE_PIN_NUMBER_IMM(5);
}

void* led_app(UNUSED_VARIABLE task_data_t data) {

  COROUTINE_BEGIN();

  static timer_id_t led_timer = INVALID_ID;
  if (led_timer == INVALID_ID) {

    OUTPUT_PIN_NUMBER(5);
    CLR_PIN_NUMBER_IMM(5);

    led_timer = get_timer();
    SET_TIMER(led_timer, LED_TOGGLE_DURATION_MS, TRUE, toggle_led, NULL);
    start_timer(led_timer);
  }

  suspend();

  COROUTINE_END();
}

DEFINE_INTERNAL_INTERFACE(
    tasks,
    led_app_api,
    .fn = led_app,
    .data = { .ptr = NULL },
    .is_service = FALSE
);
