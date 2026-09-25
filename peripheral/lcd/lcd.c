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
 * @file lcd.c
 * @author notweerdmonk
 * @brief HD44780 LCD module driver
 */

#include <config.h>
#include <common.h>
#include <utility.h>
#include <coroutine/coroutine.h>
#include <lcd_commands.h>
#include <lcd.h>

/*
 * works using busy wait method
 * 
 * TODO: add busy poll method
 * TODO: supports HD44780 controller only, add a way to determine controller
 */

/*****************************************************************************/
typedef struct _char_buffer {
  char c;
  boolean is_dirty;
} char_buffer_t;

typedef struct {
  char_buffer_t buffer[LCD_ROWS][LCD_COLS];
} screen_buffer_t;

typedef struct _lcd {
  screen_buffer_t screen;
  cursor_t cursor;

  event_id_t rdy_ev;

  uint8_t bus_bitmask[8];
  uint8_t data_port : 2;
  uint8_t rs_pin : 5;
  uint8_t en_pin : 5;
  uint8_t rw_pin : 5;
  uint8_t bl_pin : 5;

  boolean clr : 1;
  boolean upt : 1;
  boolean rdy : 1;

} lcd_t;

/*****************************************************************************/
#ifdef __ENABLE_LCD

static lcd_t lcd;

/*****************************************************************************/

#define SCREEN(x, y) screen.buffer[y][x]

#define CURX cursor.col
#define CURY cursor.row

static
void lcd_toggle_en() {
  /* Set EN pin as LOW */
  CLR_PIN_NUMBER(lcd.en_pin);
  DELAY_US(1);

  /* Set EN pin as HIGH */
  SET_PIN_NUMBER(lcd.en_pin);
  DELAY_US(2000);

  /* Set EN pin as LOW */
  CLR_PIN_NUMBER(lcd.en_pin);
  DELAY_US(100);
}

#if defined LCD_4BIT

static
void lcd_write_nibble(char nibble) {

  if (nibble & 0x01)
    SET_PORT_NUMBER(lcd.data_port, lcd.bus_bitmask[4]);
  else
    CLR_PORT_NUMBER(lcd.data_port, lcd.bus_bitmask[4]);
  if (nibble & 0x02)
    SET_PORT_NUMBER(lcd.data_port, lcd.bus_bitmask[5]);
  else
    CLR_PORT_NUMBER(lcd.data_port, lcd.bus_bitmask[5]);
  if (nibble & 0x04)
    SET_PORT_NUMBER(lcd.data_port, lcd.bus_bitmask[6]);
  else
    CLR_PORT_NUMBER(lcd.data_port, lcd.bus_bitmask[6]);
  if (nibble & 0x08)
    SET_PORT_NUMBER(lcd.data_port, lcd.bus_bitmask[7]);
  else
    CLR_PORT_NUMBER(lcd.data_port, lcd.bus_bitmask[7]);

  lcd_toggle_en();
}

#elif defined LCD_8BIT

static
void lcd_write_byte(char byte) {
  for (uint8_t i = 0; i < 8; i++) {
    if ((byte & 0x1) == 1) {
      SET_PORT_NUMBER(lcd.data_port, lcd.bus_bitmask[i]);
    }
    else {
      CLR_PORT_NUMBER(lcd.data_port, lcd.bus_bitmask[i]);
    }
    byte = byte >> 1;
  }

  lcd_toggle_en();
}

#endif

static
void lcd_data(unsigned char data) {
  /* set RS pin as HIGH */
  SET_PIN_NUMBER(lcd.rs_pin);

#ifdef LCD_4BIT
  /* write high nibble */
  lcd_write_nibble(data >> 4);
  /* write high nibble */
  lcd_write_nibble(data & 0x0F);

#elif defined LCD_8BIT
  /* write byte */
  lcd_write_byte(data);
#endif
}

static
void lcd_cmd(unsigned char cmd) {
  /* Set RS pin as LOW */
  CLR_PIN_NUMBER(lcd.rs_pin);

#ifdef LCD_4BIT
  /* write high nibble */
  lcd_write_nibble(cmd >> 4);
  /* write low nibble */
  lcd_write_nibble(cmd & 0x0F);

#elif defined LCD_8BIT
  /* write byte */
  lcd_write_byte(cmd);
#endif
}

/* TODO: 8-bit mode reset routine */
static
void* lcd_reset() {

  COROUTINE_BEGIN();

  CLR_PIN_NUMBER(lcd.rs_pin);
  CLR_PIN_NUMBER(lcd.en_pin);
  sleep(20);
  COROUTINE_YIELD();

#ifdef LCD_4BIT
  lcd_write_nibble(0x03);
  sleep(5);
  COROUTINE_YIELD();

  lcd_write_nibble(0x03);
  sleep(5);
  COROUTINE_YIELD();

  lcd_write_nibble(0x03);
  sleep(1);
  COROUTINE_YIELD();

  lcd_write_nibble(0x02);
#endif

  COROUTINE_STOP();
}

/* TODO: 8-bit mode setup routine */
static
void lcd_setup() {
#ifdef LCD_4BIT
  /* 4-bit mode - 2 line - 5x8 font */
  lcd_cmd(LCD_FUNCTION_SET | LCD_4BIT_MODE | LCD_2LINE | LCD_5x8DOTS);
#endif
  /* Display on, cusror on, blink on */
  lcd_cmd(LCD_DISPLAY_CONTROL | LCD_DISPLAY_ON | LCD_CURSOR_OFF |
          LCD_BLINK_OFF);
  /* Clear display */
  lcd_cmd(LCD_CLEAR_DISPLAY);
}

UNUSED_FUNCTION
static
void lcd_set_ddram(uint8_t row, uint8_t col) {
  if ((row < LCD_ROWS) && (col < LCD_COLS)) {
    switch (row) {
      case 0:
        lcd_cmd(LCD_SET_DDRAMADDR | LCD_DDRAMADDR_LINE1 | col);
        break;

      case 1:
        lcd_cmd(LCD_SET_DDRAMADDR | LCD_DDRAMADDR_LINE2 | col);
        break;

      default:
        ;
    }
  }
}

static
void lcd_display() {
  uint8_t col, row, ddram_addr = LCD_SET_DDRAMADDR;
  boolean set_ddram = FALSE;

  if (lcd.clr) {
    lcd_cmd(LCD_CLEAR_DISPLAY);
  }

  if (lcd.upt) {
    lcd_cmd(ddram_addr);

    for (row = 0; row < LCD_ROWS; row++) {
      if (row > 0) {
        ddram_addr |= LCD_DDRAMADDR_LINE2;
        lcd_cmd(ddram_addr);
      }

      alive();

      for (col = 0; col < LCD_COLS; col++) {
        if (lcd.SCREEN(col, row).is_dirty) {
          if (set_ddram) {
            lcd_cmd(ddram_addr | col);
            set_ddram = FALSE;
          }

          lcd_data(lcd.SCREEN(col, row).c);
          lcd.SCREEN(col, row).is_dirty = FALSE;
        }
        else {
          if (lcd.clr) {
            lcd.SCREEN(col, row).c = 0;
          }

          set_ddram = TRUE;
        }
      }
    }

    lcd.clr = lcd.upt = FALSE;
  }
}

/*****************************************************************************/

void lcd_set_pins(lcd_data_t *p) {
  if (!p) return;

  lcd.data_port = p->data_port;

  if (p->rs != PIN_NC) {
    lcd.rs_pin = p->rs;
    OUTPUT_PIN_NUMBER(p->rs);
  }
  if (p->en != PIN_NC) {
    lcd.en_pin = p->en;
    OUTPUT_PIN_NUMBER(p->en);
  }
  if (p->rw != PIN_NC) {
    lcd.rw_pin = p->rw;
    OUTPUT_PIN_NUMBER(p->rw);
  }
  if (p->bl != PIN_NC) {
    lcd.bl_pin = p->bl;
    OUTPUT_PIN_NUMBER(p->bl);
  }

#ifdef LCD_8BIT

  SET_DDR_NUMBER(p->data_port, 0xf);
  if (p->D0 != PIN_NC) {
    lcd.bus_bitmask[0] = BITMASK(CONVERT_PIN_NUMBER(p->D0));
  }
  if (p->D1 != PIN_NC) {
    lcd.bus_bitmask[1] = BITMASK(CONVERT_PIN_NUMBER(p->D1));
  }
  if (p->D2 != PIN_NC) {
    lcd.bus_bitmask[2] = BITMASK(CONVERT_PIN_NUMBER(p->D2));
  }
  if (p->D3 != PIN_NC) {
    lcd.bus_bitmask[3] = BITMASK(CONVERT_PIN_NUMBER(p->D3));
  }
#endif

  SET_DDR_NUMBER(p->data_port, 0xf0);
  if (p->D4 != PIN_NC) {
    lcd.bus_bitmask[4] = BITMASK(CONVERT_PIN_NUMBER(p->D4));
  }
  if (p->D5 != PIN_NC) {
    lcd.bus_bitmask[5] = BITMASK(CONVERT_PIN_NUMBER(p->D5));
  }
  if (p->D6 != PIN_NC) {
    lcd.bus_bitmask[6] = BITMASK(CONVERT_PIN_NUMBER(p->D6));
  }
  if (p->D7 != PIN_NC) {
    lcd.bus_bitmask[7] = BITMASK(CONVERT_PIN_NUMBER(p->D7));
  }
}

/**
 * Calling lcd_set_brightness before registering lcd_task will result in
 * overriding of the brightness value to LCD_BL_VALUE.
 */
void lcd_set_brightness(uint8_t value) {
  set_software_pwm_value(LCD_PWM_CHANNEL, value);
}

boolean is_lcd_ready() {
  return (boolean)lcd.rdy;
}

event_id_t get_lcd_ready_event() {
  return lcd.rdy_ev;
}

cursor_t lcd_get_cursor() {
  return lcd.cursor;
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
  if ((row < LCD_ROWS) && (col < LCD_COLS)) {
    lcd.CURY = row;
    lcd.CURX = col;
  }
}

void lcd_clear() {
  lcd.CURX = lcd.CURY = 0;
  lcd.clr = TRUE;

  for (uint8_t row = 0; row < LCD_ROWS; row++) {
    for (uint8_t col = 0; col < LCD_COLS; col++) {
      lcd.SCREEN(col, row).is_dirty = FALSE;
    }
  }
}

void lcd_clear_till(uint8_t n) {
  cursor_t prev_cursor = lcd.cursor;

  while (n--) lcd_put_char(' ');
  lcd_set_cursor(prev_cursor.row, prev_cursor.col);
}

void lcd_put_char(char c) {
  if ((lcd.CURY == LCD_ROWS) || (lcd.CURX == LCD_COLS)) {
    return;
  }

  char_buffer_t *p_pixel = &(lcd.SCREEN(lcd.CURX, lcd.CURY));
  if ((p_pixel->c != c) || lcd.clr){
    p_pixel->c = c;
    p_pixel->is_dirty = TRUE;
  }

  ++lcd.CURX;
  if (lcd.CURX == LCD_COLS) {
    lcd.CURY = (lcd.CURY + 1) % 2;
    lcd.CURX = 0;
  }

  lcd.upt = TRUE;
}

void lcd_put_string(char *str) {
  while(*str) lcd_put_char(*str++);
}

void lcd_put_pgm_string(PGM_P s) {
  for (char c = pgm_read_byte(s); c != 0; c = pgm_read_byte(++s)) {
    lcd_put_char(c);
  }
}

void lcd_put_uint(unsigned int u) {
#if (__SIZEOF_INT__ == 1)
  uint8_t digits[4];
#elif (__SIZEOF_INT__ == 2)
  uint8_t digits[6];
#endif
  uint8_t idx = 0;

  if (u == 0) {
    digits[idx++] = 0;
  } else {
    while (u > 0) {
      digits[idx++] = u % 10;
      u /= 10;
    }
  }

  while (idx > 0) {
    lcd_put_char(ASCII(digits[--idx]));
  }
}

void lcd_put_int(int n) {
  if(n < 0) {
    lcd_put_char('-');
    n = -n;
  }

  lcd_put_uint(n);
}

/* TODO: sprintf with -lprintf_flt or dtostrf, but bigger size */
void lcd_put_float(float f, uint8_t m) {
  /* fractional part greater than 4 does not work */
  if (m > 4) m = 4;

  lcd_put_int(f);
  lcd_put_char('.');

  f = f - (int)f;
  for (uint8_t i = 0; i < m; i++) {
    f = f * 10;
  }

  lcd_put_int(f);
}

void lcd_force_display() {
  lcd_display();
}

void* lcd_task(UNUSED_VARIABLE task_data_t data) {
  COROUTINE_BEGIN();

  lcd.rdy = FALSE;
  lcd.rdy_ev = register_event();

  COROUTINE_DELEGATE(lcd_reset);

  lcd_setup();

  REGISTER_SOFTWARE_PWM(LCD_PWM_CHANNEL, lcd.bl_pin, LCD_BL_VALUE);

  lcd.rdy = TRUE;
  trigger_event(lcd.rdy_ev, EV_NOW);

  COROUTINE_YIELD();

  lcd_display();

  COROUTINE_END();
}

#endif /* __ENABLE_LCD */
