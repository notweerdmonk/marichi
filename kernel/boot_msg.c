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
 * @file boot_msg.c
 * #author notweerdmonk
 * @brief boot message sent over UART
 */

#ifdef __ENABLE_UART

#include <config.h>

#if VERBOSITY >= VERBOSE1

#include <utility.h>
#include <uart.h>

#define VERSION_CONCAT(major, minor, revision) major ## . ## minor ## . ## revision
#define VERSION(major, minor, revision) STRINGIFY(VERSION_CONCAT(major, minor, revision))
#define VERSION_STRING VERSION(MAJOR_VERSION, MINOR_VERSION, REVISION)

static const char string01[] PROGMEM = "\x1b[37m              SSS   SSS       dSS SSSSSSb.  SSSSSSSSS  .dSSSSb. SSS   SSS SSSSSSSSS \x1b[0m"NEWLINE_STRING;
static const char string02[] PROGMEM = "\x1b[90m              SSSb.dSSS      dSSS SS    YSb    SSS    dSP   YSb SSS   SSS    SSS    \x1b[0m"NEWLINE_STRING;
static const char string03[] PROGMEM = "\x1b[36m       |      SSYSSSYSS     dSPSS SS    dSP    SSS    SS        SSSSSSSSS    SSS    \x1b[0m"NEWLINE_STRING;
static const char string04[] PROGMEM = "\x1b[36m    \\  |  /   SS dSb SS   dSP  SS SSSSSSP'     SSS    SS        SSS   SSS    SSS    \x1b[0m"NEWLINE_STRING;
static const char string05[] PROGMEM = "\x1b[96m     \\ | /    SS  '  SS  dSSSSSSS SS   TSb     SSS    YSb   zSP SSS   SSS    SSS    \x1b[0m"NEWLINE_STRING;
static const char string06[] PROGMEM = "\x1b[96m      \\|/     SS     SS dSP    SS SS    TSb SSSSSSSSS  'YSSSSP' SSS   SSS SSSSSSSSS \x1b[0m"NEWLINE_STRING;
static const char string07[] PROGMEM = "\x1b[37m ----- * ---------------------------------------------------------------------------\x1b[0m"NEWLINE_STRING;
static const char string08[] PROGMEM = "\x1b[96m      /|\\                                                            A RAY OF LIGHT \x1b[0m"NEWLINE_STRING;
static const char string09[] PROGMEM = "\x1b[36m     / | \\\x1b[0m"NEWLINE_STRING;
static const char string10[] PROGMEM = "\x1b[90m    /  |  \\\x1b[0m"NEWLINE_STRING;
static const char string11[] PROGMEM = "\x1b[37m       |\x1b[0m"NEWLINE_STRING;

static const char string12[] PROGMEM = NEWLINE_STRING "v" VERSION_STRING NEWLINE_STRING;
static const char string13[] PROGMEM = "Co-operative kernel for AVR family"NEWLINE_STRING;
static const char string14[] PROGMEM = NEWLINE_STRING"Copyright (C) 2022 notweerdmonk"NEWLINE_STRING;
static const char string15[] PROGMEM = NEWLINE_STRING;

static PGM_P const string_tbl1[] PROGMEM = {
  string01,
  string02,
  string03,
  string04,
  string05,
  string06,
  string07,
  string08,
  string09,
  string10,
  string11,
  string12,
  string13,
  string14,
  string15
};

#define STRING_TABLE1_SIZE sizeof(string_tbl1) / sizeof(char*)

#if VERBOSITY >= VERBOSE2

static const char string16[] PROGMEM = "This program is free software: you can redistribute it and/or modify"NEWLINE_STRING;
static const char string17[] PROGMEM = "it under the terms of the GNU General Public License as published by"NEWLINE_STRING;
static const char string18[] PROGMEM = "the Free Software Foundation, either version 3 of the License, or"NEWLINE_STRING;
static const char string19[] PROGMEM = "(at your option) any later version."NEWLINE_STRING NEWLINE_STRING;
static const char string20[] PROGMEM = "This program is distributed in the hope that it will be useful,"NEWLINE_STRING;
static const char string21[] PROGMEM = "but WITHOUT ANY WARRANTY; without even the implied warranty of"NEWLINE_STRING;
static const char string22[] PROGMEM = "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"NEWLINE_STRING;
static const char string23[] PROGMEM = "GNU General Public License for more details."NEWLINE_STRING NEWLINE_STRING;
static const char string24[] PROGMEM = "You should have received a copy of the GNU General Public License"NEWLINE_STRING;
static const char string25[] PROGMEM = "along with this program.  If not, see <http://www.gnu.org/licenses/>."NEWLINE_STRING;
static const char string26[] PROGMEM = NEWLINE_STRING;

static PGM_P const string_tbl2[] PROGMEM = {
  string16,
  string17,
  string18,
  string19,
  string20,
  string21,
  string22,
  string23,
  string24,
  string25,
  string26
};

#define STRING_TABLE2_SIZE sizeof(string_tbl2) / sizeof(char*)

#endif /* VERBOSITY >= VERBOSE2 */

void print_boot_msg() {
  for (uint8_t i = 0; i < STRING_TABLE1_SIZE; i++) {
    uart_put_string(GET_PROGMEM_STR(GET_PROGMEM_ADDR(string_tbl1, i)));
  }

#if VERBOSITY >= VERBOSE2
  for (uint8_t i = 0; i < STRING_TABLE2_SIZE; i++) {
    uart_put_string(GET_PROGMEM_STR(GET_PROGMEM_ADDR(string_tbl2, i)));
  }
#endif
}

#endif /* VERBOSITY >= VERBOSE1 */

#else

void print_boot_msg() { }

#endif /* __ENABLE_UART */
