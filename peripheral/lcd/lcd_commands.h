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
 * @file lcd_commands.h
 * @author notweerdmonk
 * @brief commands for HD44780 lcd driver
 */

#ifndef _LCD_COMMANDS_H_
#define _LCD_COMMANDS_H_

/*****************************************************************************/
/* commands */
#define LCD_CLEAR_DISPLAY        0x01
#define LCD_RETURN_HOME          0x02
#define LCD_ENTRY_MODE_SET       0x04
#define LCD_DISPLAY_CONTROL      0x08
#define LCD_CURSOR_SHIFT         0x10
#define LCD_FUNCTION_SET         0x20
#define LCD_SET_CGRAMADDR        0x40
#define LCD_SET_DDRAMADDR        0x80

/* addresses of display RAM */
#define LCD_DDRAMADDR_LINE1      0x00
#define LCD_DDRAMADDR_LINE2      0x40

/* flags for display entry mode */
#define LCD_ENTRY_INCR           0x02
#define LCD_ENTRY_DECR           0x00
#define LCD_ENTRY_SHIFT_ON       0x01
#define LCD_ENTRY_SHIFT_OFF      0x00

/* flags for display and cursor control */
#define LCD_DISPLAY_ON           0x04
#define LCD_DISPLAY_OFF          0x00
#define LCD_CURSOR_ON            0x02
#define LCD_CURSOR_OFF           0x00
#define LCD_BLINK_ON             0x01
#define LCD_BLINK_OFF            0x00

/* flags for display/cursor shift */
#define LCD_DISPLAY_MOVE         0x08
#define LCD_CURSOR_MOVE          0x00
#define LCD_MOVE_RIGHT           0x04
#define LCD_MOVE_LEFT            0x00

/* flags for setting function */
#define LCD_8BIT_MODE            0x10
#define LCD_4BIT_MODE            0x00
#define LCD_2LINE                0x08
#define LCD_1LINE                0x00
#define LCD_5x10DOTS             0x04
#define LCD_5x8DOTS              0x00
/*****************************************************************************/

#endif /* _LCD_COMMANDS_H_ */
