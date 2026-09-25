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
 * @file lcd_stub.c
 * @author notweerdmonk
 * @brief Weak linkage stubs for HD44780 LCD module driver
 */

#include <common.h>
#include <lcd.h>

WEAK
void lcd_set_pins(UNUSED_VARIABLE lcd_data_t *p_data) { }

WEAK
void lcd_set_brightness(UNUSED_VARIABLE uint8_t value) { }

WEAK
boolean is_lcd_ready();

WEAK
event_id_t get_lcd_ready_event();

WEAK
cursor_t lcd_get_cursor() { return (cursor_t){ 0, 0 }; }

WEAK
void lcd_set_cursor(
        UNUSED_VARIABLE uint8_t row,
        UNUSED_VARIABLE uint8_t col
) { }

WEAK
void lcd_clear() { }

WEAK
void lcd_clear_till(UNUSED_VARIABLE uint8_t n) { }

#ifndef LCD_CLEAR_LINE
#define LCD_CLEAR_LINE()
#endif

WEAK
void lcd_put_char(UNUSED_VARIABLE char c) { }

WEAK
void lcd_put_string(UNUSED_VARIABLE char *str) { }

WEAK
void lcd_put_pgm_string(UNUSED_VARIABLE PGM_P str) { }

WEAK
void lcd_put_uint(UNUSED_VARIABLE unsigned int n) { }

WEAK
void lcd_put_int(UNUSED_VARIABLE int num) { }

WEAK
void lcd_put_float(UNUSED_VARIABLE float f, UNUSED_VARIABLE uint8_t m) { }

WEAK
void lcd_force_display() { }

WEAK
void* lcd_task(UNUSED_VARIABLE task_data_t data) { return NULL; }
