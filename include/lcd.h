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
 * @file lcd.h
 * @author notweerdmonk
 * @brief header file for lcd module
 */

#ifndef _LCD_H_
#define _LCD_H_

#include <common.h>
#include <kernel.h>
#include <lcd_config.h>

typedef struct _lcd_data
{
  uint8_t rs : 5;
  uint8_t en : 5;
  uint8_t rw : 5;
  uint8_t bl : 5;
  uint8_t D0 : 5;
  uint8_t D1 : 5;
  uint8_t D2 : 5;
  uint8_t D3 : 5;
  uint8_t D4 : 5;
  uint8_t D5 : 5;
  uint8_t D6 : 5;
  uint8_t D7 : 5;
  uint8_t data_port : 2;
} lcd_data_t;

typedef struct _cursor {
  uint8_t row;
  uint8_t col;
} cursor_t;

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN(SYS_RES_LCD);
 */
WEAK
void lcd_set_pins(lcd_data_t *p_data);

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN(SYS_RES_LCD);
 */
WEAK
void lcd_set_brightness(uint8_t value);

WEAK
boolean is_lcd_ready();

WEAK
event_id_t get_lcd_ready_event();

WEAK
cursor_t lcd_get_cursor();

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN(SYS_RES_LCD);
 */
WEAK
void lcd_set_cursor(uint8_t row, uint8_t col);

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN(SYS_RES_LCD);
 */
WEAK
void lcd_clear();

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN(SYS_RES_LCD);
 */
WEAK
void lcd_clear_till(uint8_t n);

#define LCD_CLEAR_LINE() lcd_clear_till(LCD_COLS - lcd_get_cursor().row)

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN(SYS_RES_LCD);
 */
WEAK
void lcd_put_char(char c);

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN(SYS_RES_LCD);
 */
WEAK
void lcd_put_string(char *str);

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN(SYS_RES_LCD);
 */
WEAK
void lcd_put_pgm_string(PGM_P str);

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN(SYS_RES_LCD);
 */
WEAK
void lcd_put_uint(unsigned int u);

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN(SYS_RES_LCD);
 */
WEAK
void lcd_put_int(int n);

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN(SYS_RES_LCD);
 */
WEAK
void lcd_put_float(float f, uint8_t m);

/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN(SYS_RES_LCD);
 */
WEAK
void lcd_force_display();

WEAK
void* lcd_task(task_data_t data);

#endif /* _LCD_H_ */
