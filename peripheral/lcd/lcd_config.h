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
 * @file lcd_config.h
 * @author notweerdmonk
 * @brief configuration for lcd module
 */

#ifndef _LCD_CONFIG_H_
#define _LCD_CONFIG_H_

#define LCD_ROWS 2

#define LCD_COLS 16

#define LCD_PWM_CHANNEL PWM_CHANNEL_A

#ifndef LCD_BL_VALUE
#define LCD_BL_VALUE 150
#endif

#endif /* _LCD_CONFIG_H_ */
