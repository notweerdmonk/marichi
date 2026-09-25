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
 * @file debugger.c
 * @author notweerdmonk
 * @brief implements debugger module
 */

/**
 * This project is inspired and in parts a derivative of AVRILOS
 * https://sourceforge.net/projects/avrilos/
 */

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/

#include <config.h>
#include <common.h>
#include <utility.h>
#include <debugger.h>
#include <coroutine/coroutine.h>
#include <uart.h>
#include <lcd.h>
#include <adc.h>

/* TODO: verbose outputs */
/* TODO: find whitespaces to parse tokens instead of hard-coded indexes */
/* TODO: add command history buffer */

#ifdef __ENABLE_DEBUGGER

/*****************************************************************************/
/* Declarations                                                              */
/*****************************************************************************/

#define BIT(x, y) ((x) >> (y)) & 1

#define c_DEBUG_BUFLEN 16

static debugger_params_t params;

static uint8_t entry_count;

static uint8_t entry[c_DEBUG_BUFLEN];

/*****************************************************************************/
/* Forward declarations                                                      */
/*****************************************************************************/

static void read_byte();
static void read_word();
static void write_byte();
static void write_word();
static void read_eeprom_byte();
static void write_eeprom_byte();
static void read_port();
static void write_port();
static void write_port_bit();
static void read_pin();
static void read_pin_bit();
static void write_pin_bit();
static void read_adc();
static void inspect_crash();
static void suspend_apps();
static void resume_apps();
static void restart_apps();
static void clearscreen();
static void print_usage();

/*****************************************************************************/
/* Data                                                                      */
/*****************************************************************************/

static const char __attribute__((progmem)) lut_commands[] = {
  'A', 'E', 'I', 'O', 'P', 'R', 'W', 'c', 'e', 'h', 'i', 'l', 'n', 'p', 'r',
  's', 't', 'u', 'w',
};

#define c_DEBUG_AVAILCMD sizeof lut_commands / sizeof lut_commands[0]

static
void (*lut_functions[])(void) = {
  read_adc,          //'A'
  write_eeprom_byte, //'E'
  write_pin_bit,     //'I'
  write_port_bit,    //'O'
  write_port,        //'P'
  read_word,         //'R'
  write_word,        //'W'
  inspect_crash,     //'c'
  read_eeprom_byte,  //'e'
  print_usage,       //'h'
  read_pin_bit,      //'i'
  clearscreen,       //'l'
  read_pin,          //'n'
  read_port,         //'p'
  read_byte,         //'r'
  suspend_apps,      //'s'
  restart_apps,      //'t'
  resume_apps,       //'u'
  write_byte,        //'w'
};

/*
 * r   XXXX       Read byte at address XXXX
 * R   XXXX       Read word at address XXXX
 * w   XXXX YY    Write byte at address XXXX
 * W   XXXX YYYY  Write word at address XXXX
 * e   XXXX       Inspect data at EEPROM address XXXX
 * E   XXXX YY    Write byte YY at EEPROM address XXXX
 * p   XX         Read PORT XX(01-04) or DDR XX(11-14)
 * P   XX   YY    Write YY to PORT XX(01-04) or DDR XX(11-14)
 * O   XX   Y  Z  Write Z(0-1) to bit Y(0-7) of PORT(01-04) or DDR(11-14)
 * n   XX         Read PIN XX(01-04)
 * i   XX   Y     Read bit Y(0-7) of PIN XX(01-04)
 * I   XX   Y  Z  Write Z(0-1) to bit Y(0-7) of PIN XX(01-04)
 * A   XX         Read analog channel XX(00-07)
 * c              Inspect error data from EEPROM
 * s              Suspend all apps
 * u              Resume all apps
 * t              Restart all apps
 * l              Clear screen
 * h              Display usage
 * F6             Start/Exit debugger
 */

static const char uart_string01[] PROGMEM = "Cmd A1   A2 A3 Description";
static const char uart_string02[] PROGMEM = "--------------------------";
static const char uart_string03[] PROGMEM = "r   XXXX       Read byte at address XXXX";
static const char uart_string04[] PROGMEM = "R   XXXX       Read word at address XXXX";
static const char uart_string05[] PROGMEM = "w   XXXX YY    Write byte at address XXXX";
static const char uart_string06[] PROGMEM = "W   XXXX YYYY  Write word at address XXXX";
static const char uart_string07[] PROGMEM = "e   XXXX       Inspect data at EEPROM address XXXX";
static const char uart_string08[] PROGMEM = "E   XXXX YY    Write byte YY at EEPROM address XXXX";
static const char uart_string09[] PROGMEM = "p   XX         Read PORT XX(01-04) or DDR XX(11-14)";
static const char uart_string10[] PROGMEM = "P   XX   YY    Write YY to PORT XX(01-04) or DDR XX(11-14)";
static const char uart_string11[] PROGMEM = "O   XX   Y  Z  Write Z(0-1) to bit Y(0-7) of PORT XX(01-04) or DDR XX(11-14)";
static const char uart_string12[] PROGMEM = "n   XX         Read PIN XX(01-04)";
static const char uart_string13[] PROGMEM = "i   XX   Y     Read bit Y(0-7) of PIN XX(01-04)";
static const char uart_string14[] PROGMEM = "I   XX   Y  Z  Write Z(0-1) to bit Y(0-7) of PIN XX(01-04)";
static const char uart_string15[] PROGMEM = "A   XX         Read Analog channel XX(00-07)";
static const char uart_string16[] PROGMEM = "c              Inspect Crash error data from EEPROM";
static const char uart_string17[] PROGMEM = "s              Suspend all apps";
static const char uart_string18[] PROGMEM = "u              ReSume all apps";
static const char uart_string19[] PROGMEM = "t              ResTart all apps";
static const char uart_string20[] PROGMEM = "l              cLear screen";
static const char uart_string21[] PROGMEM = "h              Display usage";
static const char uart_string22[] PROGMEM = "F6             Start/Exit debugger";

static PGM_P const uart_string_table[] PROGMEM = {
  uart_string01, uart_string02, uart_string03, uart_string04, uart_string05,
  uart_string06, uart_string07, uart_string08, uart_string09, uart_string10,
  uart_string11, uart_string12, uart_string13, uart_string14, uart_string15,
  uart_string16, uart_string17, uart_string18, uart_string19, uart_string20,
  uart_string21, uart_string22
};

#define UART_STRING_TBL_SIZE sizeof(uart_string_table) / sizeof(char*)

/*****************************************************************************/

static const char config_heading[] PROGMEM = "Debugger configuration:"NEWLINE_STRING;
static const char verbosity_label[] PROGMEM = "verbosity:\t";

typedef enum debugger_config {
  DEBUGGER_CONFIG_VERBOSITY = 0,
  DEBUGGER_CONFIG_MAX
} debugger_config_t;

static PGM_P const config_string_table[] PROGMEM = {
  verbosity_label
};

static const char debugger_help_prompt[] PROGMEM = "Use 'h' for help."NEWLINE_STRING;

static const char error_string[] PROGMEM = NEWLINE_STRING"Error"NEWLINE_STRING;

static const char inspect_crash_string1[] PROGMEM = "Err:";
static const char inspect_crash_string2[] PROGMEM = " PC:0x";

/*****************************************************************************/

static
void print_byte(uint8_t data) {
  char hex[3];

  byte_to_hex(data, hex);

  uart_put_string("0x");
  uart_put_string(hex);
  uart_new_line();
}

static
void print_word(uint16_t data) {
  char hex[5];

  word_to_hex(data, hex);

  uart_put_string("0x");
  uart_put_string(hex);
  uart_new_line();
}

static
void print_int(int n) {
  uart_put_int(n);
  uart_new_line();
}

/*****************************************************************************/

static
void read_byte(void) {
  uint8_t data;
  uint16_t addr;
  char *str;

  str = (char*)&entry[2];
  addr = HEX_TO_WORD(str);
  data = *(uint8_t*)addr;

  print_byte(data);
}

static
void read_word(void) {
  uint16_t data;
  uint16_t addr;
  char *str;

  str = (char*)&entry[2];
  addr = HEX_TO_WORD(str);
  data = *(volatile uint8_t*)addr;
  data |= (*(volatile uint8_t*)(addr + 1)) << 8;

  print_word(data);
}


static
void write_byte(void) {
  uint8_t data;
  uint16_t addr;
  char *str;

  str = (char*)&entry[2];
  addr = HEX_TO_WORD(str);
  str = (char*)&entry[2+4+1];
  data = HEX_TO_BYTE(str);

  *(uint8_t*)addr = data;
  /* Readback */
  data = *(uint8_t*)addr;

  print_byte(data);
}

static
void write_word(void) {
  uint16_t data;
  uint16_t addr;
  char *str;

  str = (char*)&entry[2];
  addr = HEX_TO_WORD(str);
  str = (char*)&entry[2+4+1];
  data = HEX_TO_WORD(str);

  *(uint16_t*)addr = data;
  /* Readback */
  data = *(uint16_t*)addr;

  print_word(data);
}

static
void read_eeprom_byte(void) {
  uint8_t data;
  uint8_t *addr;
  char *str;

  str = (char*)&entry[2];
  addr = (uint8_t*)HEX_TO_WORD(str);

  data = eeprom_read_byte(addr);

  print_byte(data);
}

static
void write_eeprom_byte(void) {
  uint8_t data;
  uint8_t *addr;
  char *str;

  str = (char*)&entry[2];
  addr = (uint8_t*)HEX_TO_WORD(str);
  str = (char*)&entry[2+4+1];
  data = HEX_TO_BYTE(str);

  eeprom_write_byte(addr, data);
  /* Readback */
  data = eeprom_read_byte(addr);

  print_byte(data);
}

static
void read_port(void) {
  uint8_t data = 0;
  uint16_t target;
  char *str;

  str = (char*)&entry[2];
  target = HEX_TO_BYTE(str);

  switch (target) {
    case 0x01:
#ifdef PORTA
      data = PORTA;
#endif
      break;
    case 0x02:
      data = PORTB;
      break;
    case 0x03:
      data = PORTC;
      break;
    case 0x04:
      data = PORTD;
      break;
    case 0x11:
#ifdef DDRA
      data = DDRA;
#endif
      break;
    case 0x12:
      data = DDRB;
      break;
    case 0x13:
      data = DDRC;
      break;
    case 0x14:
      data = DDRD;
      break;
    default:
      ;
  }

  print_byte(data);
}

static
void write_port(void) {
  uint8_t data;
  uint16_t target;
  char *str;

  str = (char*)&entry[2];
  target = HEX_TO_BYTE(str);
  str = (char*)&entry[2+2+1];
  data = HEX_TO_BYTE(str);

  switch (target) {
    case 0x01:
#ifdef PORTA
      PORTA = data;
#endif
      break;
    case 0x02:
      PORTB = data;
      break;
    case 0x03:
      PORTC = data;
      break;
    case 0x04:
      PORTD = data;
      break;
    case 0x11:
#ifdef DDRA
      DDRA = data;
#endif
      break;
    case 0x12:
      DDRB = data;
      break;
    case 0x13:
      DDRC = data;
      break;
    case 0x14:
      DDRD = data;
      break;
    default:
      ;
  }
}

static
void write_port_bit(void) {
  uint16_t target;
  uint8_t bit;
  uint8_t data;
  char *str;

  str = (char*)&entry[2];
  target = HEX_TO_BYTE(str);
  str = (char*)&entry[2+2+1];
  bit = hex_to_nibble(str);
  str = (char*)&entry[2+2+1+2];
  data = hex_to_nibble(str);

  switch (target) {
    case 0x01:
#ifdef PORTA
      if (data) SET_PORT(A, bit);
      else CLR_PORT(A, bit);
#endif
      break;
    case 0x02:
      if (data) SET_PORT(B, bit);
      else CLR_PORT(B, bit);
      break;
    case 0x03:
      if (data) SET_PORT(C, bit);
      else CLR_PORT(C, bit);
      break;
    case 0x04:
      if (data) SET_PORT(D, bit);
      else CLR_PORT(D, bit);
      break;
    case 0x11:
#ifdef DDRA
      if (data) OUTPUT_PIN(A, bit);
      else INPUT_PIN(A, bit);
#endif
      break;
    case 0x12:
      if (data) OUTPUT_PIN(B, bit);
      else INPUT_PIN(B, bit);
      break;
    case 0x13:
      if (data) OUTPUT_PIN(C, bit);
      else INPUT_PIN(C, bit);
      break;
    case 0x14:
      if (data) OUTPUT_PIN(D, bit);
      else INPUT_PIN(D, bit);
      break;
    default:
      ;
  }
}

static
void read_pin() {
  uint8_t pin;
  uint8_t data = 0;
  char *str;

  str = (char*)&entry[2];
  pin = HEX_TO_BYTE(str);

  switch (pin) {
    case 1:
#ifdef PINA
      data = PINA;
#endif
      break;
    case 2:
      data = PINB;
      break;
    case 3:
      data = PINC;
      break;
    case 4:
      data = PIND;
      break;
    default:
      ;
  }

  print_byte(data);
}

static
void read_pin_bit() {
  unsigned char pin;
  unsigned char bit;
  unsigned char data = 0;
  char *str;

  str = (char*)&entry[2];
  pin = HEX_TO_BYTE(str);
  str = (char*)&entry[2+2+1];
  bit = hex_to_nibble(str);

  switch (pin) {
    case 1:
#ifdef PINA
      data = BIT(PINA, bit);
#endif
      break;
    case 2:
      data = BIT(PINB, bit);
      break;
    case 3:
      data = BIT(PINC, bit);
      break;
    case 4:
      data = BIT(PIND, bit);
      break;
    default:
      ;
  }

  print_byte(data);
}

static
void write_pin_bit() {
  unsigned char pin;
  unsigned char bit;
  unsigned char data = 0;
  char *str;

  str = (char*)&entry[2];
  pin = HEX_TO_BYTE(str);
  str = (char*)&entry[2+2+1];
  bit = hex_to_nibble(str);
  str = (char*)&entry[2+2+1+2];
  data = hex_to_nibble(str);

  switch (pin) {
    case 1:
#ifdef PINA
      if (data) SET_PIN(A, bit);
      else CLR_PIN(A, bit);
#endif
      break;
    case 2:
      if (data) SET_PIN(B, bit);
      else CLR_PIN(B, bit);
      break;
    case 3:
      if (data) SET_PIN(C, bit);
      else CLR_PIN(C, bit);
      break;
    case 4:
      if (data) SET_PIN(D, bit);
      else CLR_PIN(D, bit);
      break;
    default:
      ;
  }
}

static
void read_adc(void) {
  uint16_t data = -1;
  char *str = (char*)&entry[2];
  uint8_t channel = HEX_TO_BYTE(str);

  if (ADC_IS_ENABLED()) {
    if (!acquire_resource(SYS_RES_ADC, FALSE, TRUE)) {
      uint8_t adate = ADC_IS_AUTO_TRIGGER_ENABLED();
      uint8_t adie = ADC_IS_INTERRUPT_ENABLED();

      if (adie) {
        ADC_DISABLE_INTERRUPT();
      }
      if (adate) {
        ADC_DISABLE_AUTO_TRIGGER();
      }

      data = adc_single_conversion(channel);

      if (adate) {
        ADC_ENABLE_AUTO_TRIGGER();
      }
      if (adie) {
        ADC_ENABLE_INTERRUPT();
      }
      release_resource(SYS_RES_ADC);
    }
  }

  print_int(data);
}

static void inspect_crash() {
  uint8_t flag = eeprom_read_byte(EEPROM_ERROR_FLAG_ADDR);
  if (flag) {
    uint8_t error = eeprom_read_byte(EEPROM_ERROR_VALUE_ADDR);

    uint16_t pc = eeprom_read_byte(EEPROM_PC_VALUE_LOW_ADDR);
    pc |= eeprom_read_byte(EEPROM_PC_VALUE_HIGH_ADDR) << 8;

    char hex[5];
    word_to_hex(pc << 1, hex);

    uart_put_pgm_string(inspect_crash_string1);
    uart_put_int(error);
    uart_put_pgm_string(inspect_crash_string2);
    uart_put_string(hex);
    uart_new_line();
  }
}

extern void suspend_all_apps();

static
void suspend_apps() {
  suspend_all_apps();
}

extern void resume_all_apps();

static
void resume_apps() {
  resume_all_apps();
}

extern void restart_all_apps();

static
void restart_apps() {
  restart_all_apps();
}

static
void print_usage(void) {
  for (uint8_t i = 0; i < UART_STRING_TBL_SIZE; i++) {
    uart_put_pgm_string(GET_PROGMEM_ADDR(uart_string_table, i));
    uart_new_line();

    if (i % 10) alive();
  }
}

static
void clearscreen(void) {
  uart_clear();
}

/*****************************************************************************/

static
void process_cmd(void) {
  uint8_t i = 0;

  for (uint8_t l = 0, h = c_DEBUG_AVAILCMD; l < h;) {
    uint8_t m = l + ((h - l) >> 1);
    uint8_t c = pgm_read_byte(&lut_commands[m]);

    if (c == entry[0]) {
      i = m;
      break;
    } else if (c < entry[0]) {
      l = m + 1;
    } else if (c > entry[0]) {
      h = m;
    }
  }

  if(i != c_DEBUG_AVAILCMD) {
    (*lut_functions[i])();
  }
}

void* debugger_task(task_data_t data) {
  void debugger_put_prompt() {
    uart_put_char('?');
    uart_put_char(' ');
  }

  COROUTINE_BEGIN();

  debugger_params_t *p_params = data.ptr;
  if (p_params) {
    params = *p_params;
  }

  acquire_resource(SYS_RES_UART, FALSE, TRUE);
  /* Do not release UART */

  UART_FLUSH();

  if (params.verbosity > 0) {
    uart_put_pgm_string(config_heading);
    for (uint8_t i = 0; i < DEBUGGER_CONFIG_MAX; ++i) {
      uart_put_pgm_string(GET_PROGMEM_ADDR(config_string_table, i));

      switch (i) {
        case DEBUGGER_CONFIG_VERBOSITY:
          uart_put_int(params.verbosity);
          break;

        default:
          ;
      }

      uart_new_line();
    }
  }

  uart_put_pgm_string(debugger_help_prompt);

  debugger_put_prompt();

  entry_count = 0;

  COROUTINE_YIELD();

  unsigned char c;

  /* REPL starts here */
  if ((c = uart_get_char())) {
    if (c == c_DEL || c == c_BKSPACE) {
      if (entry_count > 0) {
        --entry_count;

        uart_put_char(c);
      }
    } else if (c == c_RETURN) {
      uart_new_line();

      process_cmd();
      entry_count = 0;

      debugger_put_prompt();

    } else {
      entry[entry_count++] = c;

      if (entry_count == c_DEBUG_BUFLEN) {
        entry_count = 0;

        uart_put_pgm_string(error_string);
        debugger_put_prompt();

      } else {
        uart_put_char(c);
      }
    }
  }

  COROUTINE_END();
}

#endif /* __ENABLE_DEBUGGER */
