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
 * @file utility.c
 * @author notweerdmonk
 * @brief implements utility functions
 */

#include <utility.h>

static const char PROGMEM hex_lut[] = \
  {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void byte_to_hex(uint8_t byte, char *str) {
  *str++ = pgm_read_byte(&hex_lut[(byte >> 4) & 0xf]);
  *str++ = pgm_read_byte(&hex_lut[byte & 0xf]);
  *str = 0;
}

void word_to_hex(uint16_t word, char *str) {
  *str++ = pgm_read_byte(&hex_lut[(word >> 12) & 0xf]);
  *str++ = pgm_read_byte(&hex_lut[(word >> 8) & 0xf]);
  *str++ = pgm_read_byte(&hex_lut[(word >> 4) & 0xf]);
  *str++ = pgm_read_byte(&hex_lut[word & 0xf]);
  *str = 0;
}

uint8_t hex_to_nibble(const char *hex) {
  uint8_t byte;

  byte = *hex;
  if (byte >= 'a') byte = (byte - 'a') + 10;
  else if (byte >= 'A') byte = (byte - 'A') + 10;
  else byte -= '0';
  return byte;
}
