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

#ifndef PORT_MEGA328P_H
#define PORT_MEGA328P_H

#ifdef __AVR_ATmega328P__

/**
 * @file port_mega328p.h
 * @author notweerdmonk
 * @brief Portability layer macros fro ATmega328p
 */

/* TODO: Use BITMASK */

/* PORT and DDR register manipulation */

/*
 * port numbering:
 * 0 : DDRB/PORTB/PINB
 * 1 : DDRC/PORTC/PINC
 * 2 : DDRD/PORTD/PIND
 */

/* TODO: use ternary operator */
#define SET_DDR_NUMBER(ddr, mask) \
  do {                  \
    if (ddr == 0)       \
      SET_DDR(B, mask); \
    else if (ddr == 1)  \
      SET_DDR(C, mask); \
    else if (ddr == 2)  \
      SET_DDR(D, mask); \
  } while (0)

#define CLR_DDR_NUMBER(ddr, mask) \
  do {                  \
    if (ddr == 0)       \
      CLR_DDR(B, mask); \
    else if (ddr == 1)  \
      CLR_DDR(C, mask); \
    else if (ddr == 2)  \
      CLR_DDR(D, mask); \
  } while (0)

#define SET_PORT_NUMBER(port, mask) \
  do {                   \
    if (port == 0)       \
      SET_PORT(B, mask); \
    else if (port == 1)  \
      SET_PORT(C, mask); \
    else if (port == 2)  \
      SET_PORT(D, mask); \
  } while (0)

#define CLR_PORT_NUMBER(port, mask) \
  do {                   \
    if (port == 0)       \
      CLR_PORT(B, mask); \
    else if (port == 1)  \
      CLR_PORT(C, mask); \
    else if (port == 2)  \
      CLR_PORT(D, mask); \
  } while (0)

/* pin manipulation and PIN register read */
/*
 * pin numbering:
 * 0..7   : PB0..PB7
 * 8..14  : PC0..PC6
 * 15..22 : PD0..PD7
 */

#define CONVERT_PIN_NUMBER(p) \
  ({                             \
    uint8_t pin = p;             \
    if (8 <= p && p <= 14)       \
      pin = p - 8;               \
    else if (15 <= p && p <= 22) \
      pin = p - 15;              \
    pin;                         \
  })

#define PIN_NUMBER_TO_PORT(p) \
  ({                                 \
    volatile uint8_t *port = &PORTB; \
    if (8 <= p && p <= 14)           \
      port = &PORTC;                 \
    else if (15 <= p && p <= 22)     \
      port = &PORTD;                 \
    port;                            \
  })

#define PIN_NUMBER_TO_DDR(p) \
  ({                               \
    volatile uint8_t *ddr = &DDRB; \
    if (8 <= p && p <= 14)         \
      ddr = &DDRC;                 \
    else if (15 <= p && p <= 22)   \
      ddr = &DDRD;                 \
    port;                          \
  })

#define OUTPUT_PIN_NUMBER(p) \
  do {                           \
    if (0 <= p && p <= 7)        \
      OUTPUT_PIN(B, p);          \
    else if (8 <= p && p <= 14)  \
      OUTPUT_PIN(C, p - 8);      \
    else if (15 <= p && p <= 22) \
      OUTPUT_PIN(D, p - 15);     \
  } while (0)

#define OUTPUT_PIN_NUMBER_IMM(p) \
  do {                           \
    if (0 <= p && p <= 7)        \
      OUTPUT_PIN_IMM(B, p);      \
    else if (8 <= p && p <= 14)  \
      OUTPUT_PIN_IMM(C, p - 8);  \
    else if (15 <= p && p <= 22) \
      OUTPUT_PIN_IMM(D, p - 15); \
  } while (0)

#define INPUT_PIN_NUMBER(p) \
  do {                           \
    if (0 <= p && p <= 7)        \
      INPUT_PIN(B, p);           \
    else if (8 <= p && p <= 14)  \
      INPUT_PIN(C, p - 8);       \
    else if (15 <= p && p <= 22) \
      INPUT_PIN(D, p - 15);      \
  } while (0)

#define INPUT_PIN_NUMBER_IMM(p) \
  do {                           \
    if (0 <= p && p <= 7)        \
      INPUT_PIN_IMM(B, p);       \
    else if (8 <= p && p <= 14)  \
      INPUT_PIN_IMM(C, p - 8);   \
    else if (15 <= p && p <= 22) \
      INPUT_PIN_IMM(D, p - 15);  \
  } while (0)

#define SET_PIN_NUMBER(p) \
  do {                           \
    if (0 <= p && p <= 7)        \
      SET_PIN(B, p);             \
    else if (8 <= p && p <= 14)  \
      SET_PIN(C, p - 8);         \
    else if (15 <= p && p <= 22) \
      SET_PIN(D, p - 15);        \
  } while (0)

#define SET_PIN_NUMBER_IMM(p) \
  do {                           \
    if (0 <= p && p <= 7)        \
      SET_PIN_IMM(B, p);         \
    else if (8 <= p && p <= 14)  \
      SET_PIN_IMM(C, p - 8);     \
    else if (15 <= p && p <= 22) \
      SET_PIN_IMM(D, p - 15);    \
  } while (0)

#define CLR_PIN_NUMBER(p) \
  do {                           \
    if (0 <= p && p <= 7)        \
      CLR_PIN(B, p);             \
    else if (8 <= p && p <= 14)  \
      CLR_PIN(C, p - 8);         \
    else if (15 <= p && p <= 22) \
      CLR_PIN(D, p - 15);        \
  } while (0)

#define CLR_PIN_NUMBER_IMM(p) \
  do {                           \
    if (0 <= p && p <= 7)        \
      CLR_PIN_IMM(B, p);         \
    else if (8 <= p && p <= 14)  \
      CLR_PIN_IMM(C, p - 8);     \
    else if (15 <= p && p <= 22) \
      CLR_PIN_IMM(D, p - 15);    \
  } while (0)

#define TOGGLE_PIN_NUMBER(p) \
  do {                           \
    if (0 <= p && p <= 7)        \
      TOGGLE_PIN(B, p);          \
    else if (8 <= p && p <= 14)  \
      TOGGLE_PIN(C, p - 8);      \
    else if (15 <= p && p <= 22) \
      TOGGLE_PIN(D, p - 15);     \
  } while (0)

#define TOGGLE_PIN_NUMBER_IMM(p) \
  do {                           \
    if (0 <= p && p <= 7)        \
      TOGGLE_PIN_IMM(B, p);      \
    else if (8 <= p && p <= 14)  \
      TOGGLE_PIN_IMM(C, p - 8);  \
    else if (15 <= p && p <= 22) \
      TOGGLE_PIN_IMM(D, p - 15); \
  } while (0)

#define GET_PIN_NUMBER(p) \
  ({                                \
    register uint8_t pin_val = 0;   \
    if (0 <= p && p <= 7)           \
      pin_val = GET_PIN(B, p);      \
    else if (8 <= p && p <= 14)     \
      pin_val = GET_PIN(C, p - 8);  \
    else if (15 <= p && p <= 22)    \
      pin_val = GET_PIN(D, p - 15); \
    pin_val;                        \
  })

/******************************************************************************/
/* Status Register Manipulation                                               */
/******************************************************************************/

/**
 * @name AVR Status Register
 * Status registers stores information about the resutl of the most recently
 * executed arithmetic instruction and the state of global interrupt gating.
 */
/*@{*/

/** @brief Enable Timer/Counter 0 */
#define PORT_ENABLE_GLOBAL_INTERRUPTS() \
  sei()

/*@}*/

/* Timers */

/* Timer/Counter 0 */

#define TIMER0

#define PORT_TIMER0_COUNTER_REGISTER \
  TCNT0

#define PORT_TIMER0_CONTROL_REGISTER_A \
  TCCR0A

#define PORT_TIMER0_CONTROL_REGISTER_B \
  TCCR0B

/* waveform generation mode */
enum _port_timer0_wgm {
  PORT_TIMER0_WGM_NORMAL = 0,
  PORT_TIMER0_WGM_PWM_PHASE_CORRECT_1,
  PORT_TIMER0_WGM_CTC,
  PORT_TIMER0_WGM_FAST_PWM_1,
  PORT_TIMER0_WGM_PWM_PHASE_CORRECT_2,
  PORT_TIMER0_WGM_FAST_PWM_2
};

#define PORT_TIMER0_PWM_PHASE_CORRECT_1() \
  PORT_TIMER0_CONTROL_REGISTER_A |= (1<<WGM00)

#define PORT_TIMER0_CTC() \
  PORT_TIMER0_CONTROL_REGISTER_A |= (1<<WGM01)

#define PORT_TIMER0_FAST_PWM_1() \
  PORT_TIMER0_CONTROL_REGISTER_A |= (1<<WGM00) | (1<<WGM01)

#define PORT_TIMER0_PWM_PHASE_CORRECT_2() \
  do { \
    PORT_TIMER0_CONTROL_REGISTER_A |= (1<<WGM00); \
    PORT_TIMER0_CONTROL_REGISTER_B |= (1<<WGM02); \
  } while (0)

#define PORT_TIMER0_FAST_PWM_2() \
  do { \
    PORT_TIMER0_CONTROL_REGISTER_A |= (1<<WGM00) | (1<<WGM01); \
    PORT_TIMER0_CONTROL_REGISTER_B |= (1<<WGM02); \
  } while (0)

/* compare output mode */
enum _port_timer0_com_a {
  PORT_TIMER0_COMA_NORMAL = 0,
  PORT_TIMER0_COMA_TOGGLE_OC0A,
  PORT_TIMER0_COMA_CLEAR_OC0A,
  PORT_TIMER0_COMA_SET_OC0A
};

#define PORT_TIMER0_TOGGLE_OC0A() \
  PORT_TIMER0_CONTROL_REGISTER_A |= (1<<COM0A0)

#define PORT_TIMER0_CLEAR_OC0A() \
  PORT_TIMER0_CONTROL_REGISTER_A |= (1<<COM0A1)

#define PORT_TIMER0_SET_OC0A() \
  PORT_TIMER0_CONTROL_REGISTER_A |= (1<<COM0A0) | (1<<COM0A1)

/* compare output mode */
enum _port_timer0_com_b {
  PORT_TIMER0_COMB_NORMAL = 0,
  PORT_TIMER0_COMB_TOGGLE_OC0B,
  PORT_TIMER0_COMB_CLEAR_OC0B,
  PORT_TIMER0_COMB_SET_OC0B
};

#define PORT_TIMER0_TOGGLE_OC0B() \
  PORT_TIMER0_CONTROL_REGISTER_A |= (1<<COM0B0)

#define PORT_TIMER0_CLEAR_OC0B() \
  PORT_TIMER0_CONTROL_REGISTER_A |= (1<<COM0B1)

#define PORT_TIMER0_SET_OC0B() \
  PORT_TIMER0_CONTROL_REGISTER_A |= (1<<COM0B0) | (1<<COM0B1)

#define PORT_TIMER0_CS_1_MASK \
  (1<<CS00)

#define PORT_TIMER0_CS_8_MASK \
  (1<<CS01)

#define PORT_TIMER0_CS_64_MASK \
  (1<<CS00) | (1<<CS01)

#define PORT_TIMER0_CS_256_MASK \
  (1<<CS02)

#define PORT_TIMER0_CS_1024_MASK \
  (1<<CS00) | (1<<CS02)

#define PORT_TIMER0_CS_EXT_FALLING_MASK \
  (1<<CS01) | (1<<CS02)

#define PORT_TIMER0_CS_EXT_RISING_MASK \
  (1<<CS00) | (1<<CS01) | (1<<CS02)

#define PORT_TIMER0_SET_CS(cs) \
  PORT_TIMER0_CONTROL_REGISTER_B |= \
    PORT_TIMER0_CS_##cs##_MASK

#define PORT_TIMER0_SET_COUNT(value) \
  PORT_TIMER0_COUNTER_REGISTER = (value)

#define PORT_TIMER0_SET_OCR(channel, value) \
  OCR0##channel = (value)

#define PORT_TIMER0_FORCE_OCRA_MASK \
  (1<<FOC0A)

#define PORT_TIMER0_FORCE_OCRB_MASK \
  (1<<FOC0B)

#define PORT_TIMER0_FORCE_OUTPUT_COMPARE(channel) \
  PORT_TIMER0_CONTROL_REGISTER_B |= \
    PORT_TIMER0_FORCE_OCR##channel##_MASK

#define PORT_TIMER0_COMPA_VECT \
  TIMER0_COMPA_vect

#define PORT_TIMER0_COMPB_VECT \
  TIMER0_COMPB_vect

#define PORT_TIMER0_OVF_VECT \
  TIMER0_OVF_vect

#define PORT_TIMER0_INTERRUPT_MASK_REGISTER \
  TIMSK0

#define PORT_TIMER0_ENABLE_OCRA_INTERRUPT_MASK \
  (1<<OCIE0A)

#define PORT_TIMER0_ENABLE_OCRB_INTERRUPT_MASK \
  (1<<OCIE0B)

#define PORT_TIMER0_ENABLE_OCR_INTERRUPT(channel) \
  PORT_TIMER0_INTERRUPT_MASK_REGISTER |= \
    PORT_TIMER0_ENABLE_OCR##channel##_INTERRUPT_MASK

#define PORT_TIMER0_ENABLE_OVERFLOW_INTERRUPT_MASK \
  (1<<TOIE0)

#define PORT_TIMER0_ENABLE_OVERFLOW_INTERRUPT() \
  PORT_TIMER0_INTERRUPT_MASK_REGISTER |= \
    PORT_TIMER0_ENABLE_OVERFLOW_INTERRUPT_MASK

#define PORT_TIMER0_INTERRUPT_FLAG_REGISTER \
  TIFR0

#define PORT_TIMER0_0CRA_INTERRUPT_MASK \
  (1<<OCF0A)

#define PORT_TIMER0_0CRB_INTERRUPT_MASK \
  (1<<OCF0B)

#define PORT_TIMER0_TOV_INTERRUPT_MASK \
  (1<<TOV0)

/* Timer/Counter 1 */

#define TIMER1

#define PORT_TIMER1_COUNTER_REGISTER \
  TCNT1

#define PORT_TIMER1_CONTROL_REGISTER_A \
  TCCR1A

#define PORT_TIMER1_CONTROL_REGISTER_B \
  TCCR1B

#define PORT_TIMER1_CONTROL_REGISTER_C \
  TCCR1C

/* waveform generation mode */
enum _port_timer1_wgm {
  PORT_TIMER1_WGM_NORMAL = 0,
  PORT_TIMER1_WGM_PWM_PHASE_CORRECT_8BIT,
  PORT_TIMER1_WGM_PWM_PHASE_CORRECT_9BIT,
  PORT_TIMER1_WGM_PWM_PHASE_CORRECT_10BIT,
  PORT_TIMER1_WGM_CTC,
  PORT_TIMER1_WGM_FAST_PWM_8BIT,
  PORT_TIMER1_WGM_FAST_PWM_9BIT,
  PORT_TIMER1_WGM_FAST_PWM_10BIT,
  PORT_TIMER1_WGM_PWM_PHASE_FREQ_CORRECT_ICP,
  PORT_TIMER1_WGM_PWM_PHASE_FREQ_CORRECT,
  PORT_TIMER1_WGM_PWM_PHASE_CORRECT_ICP,
  PORT_TIMER1_WGM_PWM_PHASE_CORRECT,
  PORT_TIMER1_WGM_CTC_ICP,
  PORT_TIMER1_WGM_FAST_PWM_ICP,
  PORT_TIMER1_WGM_FAST_PWM
};

#define PORT_TIMER1_PWM_PHASE_CORRECT_8BIT() \
  PORT_TIMER1_CONTROL_REGISTER_A |= (1<<WGM10)

#define PORT_TIMER1_PWM_PHASE_CORRECT_9BIT() \
  PORT_TIMER1_CONTROL_REGISTER_A |= (1<<WGM11)

#define PORT_TIMER1_PWM_PHASE_CORRECT_10BIT() \
  PORT_TIMER1_CONTROL_REGISTER_A |= (1<<WGM10) | (1<<WGM11)

#define PORT_TIMER1_CTC() \
  PORT_TIMER1_CONTROL_REGISTER_B |= (1<<WGM12)

#define PORT_TIMER1_FAST_PWM_8BIT() \
  do { \
    PORT_TIMER1_CONTROL_REGISTER_A |= (1<<WGM10); \
    PORT_TIMER1_CONTROL_REGISTER_B |= (1<<WGM12); \
  } while(0)

#define PORT_TIMER1_FAST_PWM_9BIT() \
  do { \
    PORT_TIMER1_CONTROL_REGISTER_A |= (1<<WGM11); \
    PORT_TIMER1_CONTROL_REGISTER_B |= (1<<WGM12); \
  } while(0)

#define PORT_TIMER1_FAST_PWM_10BIT() \
  do { \
    PORT_TIMER1_CONTROL_REGISTER_A |= (1<<WGM10) | (1<<WGM11); \
    PORT_TIMER1_CONTROL_REGISTER_B |= (1<<WGM12); \
  } while(0)

#define PORT_TIMER1_PWM_PHASE_FREQ_CORRECT_ICP() \
  PORT_TIMER1_CONTROL_REGISTER_B |= (1<<WGM13)

#define PORT_TIMER1_PWM_PHASE_FREQ_CORRECT() \
  do { \
    PORT_TIMER1_CONTROL_REGISTER_A |= (1<<WGM10); \
    PORT_TIMER1_CONTROL_REGISTER_B |= (1<<WGM13); \
  } while(0)

#define PORT_TIMER1_PWM_PHASE_CORRECT_ICP() \
  do { \
    PORT_TIMER1_CONTROL_REGISTER_A |= (1<<WGM11); \
    PORT_TIMER1_CONTROL_REGISTER_B |= (1<<WGM13); \
  } while(0)

#define PORT_TIMER1_PWM_PHASE_CORRECT() \
  do { \
    PORT_TIMER1_CONTROL_REGISTER_A |= (1<<WGM10) | (1<<WGM11); \
    PORT_TIMER1_CONTROL_REGISTER_B |= (1<<WGM13); \
  } while(0)

#define PORT_TIMER1_CTC_ICP() \
  PORT_TIMER1_CONTROL_REGISTER_B |= (1<<WGM12) | (1<<WGM13)

#define PORT_TIMER1_FAST_PWM_ICP() \
  do { \
    PORT_TIMER1_CONTROL_REGISTER_A |= (1<<WGM11); \
    PORT_TIMER1_CONTROL_REGISTER_B |= (1<<WGM12) | (1<<WGM13); \
  } while(0)

#define PORT_TIMER1_FAST_PWM() \
  do { \
    PORT_TIMER1_CONTROL_REGISTER_A |= (1<<WGM10) | (1<<WGM11); \
    PORT_TIMER1_CONTROL_REGISTER_B |= (1<<WGM12) | (1<<WGM13); \
  } while(0)

/* compare output mode */
enum _port_timer1_com_a {
  PORT_TIMER1_COMA_NORMAL = 0,
  PORT_TIMER1_COMA_TOGGLE_OC1A,
  PORT_TIMER1_COMA_CLEAR_OC1A,
  PORT_TIMER1_COMA_SET_OC1A
};

#define PORT_TIMER1_TOGGLE_OC1A() \
  PORT_TIMER1_CONTROL_REGISTER_A |= (1<<COM1A0)

#define PORT_TIMER1_CLEAR_OC1A() \
  PORT_TIMER1_CONTROL_REGISTER_A |= (1<<COM1A1)

#define PORT_TIMER1_SET_OC1A() \
  PORT_TIMER1_CONTROL_REGISTER_A |= (1<<COM1A0) | (1<<COM1A1)

/* compare output mode */
enum _port_timer1_com_b {
  PORT_TIMER1_COMB_NORMAL = 0,
  PORT_TIMER1_COMB_TOGGLE_OC1B,
  PORT_TIMER1_COMB_CLEAR_OC1B,
  PORT_TIMER1_COMB_SET_OC1B
};

#define PORT_TIMER1_TOGGLE_OC1B() \
  PORT_TIMER1_CONTROL_REGISTER_A |= (1<<COM1B0)

#define PORT_TIMER1_CLEAR_OC1B() \
  PORT_TIMER1_CONTROL_REGISTER_A |= (1<<COM1B1)

#define PORT_TIMER1_SET_OC1B() \
  PORT_TIMER1_CONTROL_REGISTER_A |= (1<<COM1B0) | (1<<COM1B1)

#define PORT_TIMER1_CS_1_MASK \
  (1<<CS10)

#define PORT_TIMER1_CS_8_MASK \
  (1<<CS11)

#define PORT_TIMER1_CS_64_MASK \
  (1<<CS10) | (1<<CS11)

#define PORT_TIMER1_CS_256_MASK \
  (1<<CS12)

#define PORT_TIMER1_CS_1024_MASK \
  (1<<CS10) | (1<<CS12)

#define PORT_TIMER1_CS_EXT_FALLING_MASK \
  (1<<CS11) | (1<<CS12)

#define PORT_TIMER1_CS_EXT_RISING_MASK \
  (1<<CS10) | (1<<CS11) | (1<<CS12)

#define PORT_TIMER1_SET_CS(cs) \
  PORT_TIMER1_CONTROL_REGISTER_B |= \
    PORT_TIMER1_CS_##cs##_MASK 

#define PORT_TIMER1_SET_COUNT(value) \
  PORT_TIMER1_COUNTER_REGISTER = (value)

#define PORT_TIMER1_SET_OCR(channel, value) \
  OCR1##channel = (value)

#define PORT_TIMER1_FORCE_OCRA_MASK \
  (1<<FOC1A)

#define PORT_TIMER1_FORCE_OCRB_MASK \
  (1<<FOC1B)

#define PORT_TIMER1_FORCE_OUTPUT_COMPARE(channel) \
  PORT_TIMER1_CONTROL_REGISTER_C |= \
    PORT_TIMER1_FORCE_OCR##channel##_MASK

#define PORT_TIMER1_CAPT_VECT \
  TIMER1_CAPT_vect

#define PORT_TIMER1_COMPA_VECT \
  TIMER1_COMPA_vect

#define PORT_TIMER1_COMPB_VECT \
  TIMER1_COMPB_vect

#define PORT_TIMER1_OVF_VECT \
  TIMER1_OVF_vect

#define PORT_TIMER1_INTERRUPT_MASK_REGISTER \
  TIMSK1

#define PORT_TIMER1_ENABLE_ICP_INTERRUPT_MASK \
  (1<<ICIE1)

#define PORT_TIMER1_ENABLE_ICP_INTERRUPT() \
  PORT_TIMER1_INTERRUPT_MASK_REGISTER |= \
    PORT_TIMER1_ENABLE_ICP_INTERRUPT_MASK

#define PORT_TIMER1_ENABLE_OCRA_INTERRUPT_MASK \
  (1<<OCIE1A);

#define PORT_TIMER1_ENABLE_OCRB_INTERRUPT_MASK \
  (1<<OCIE1B);

#define PORT_TIMER1_ENABLE_OCR_INTERRUPT(channel) \
  PORT_TIMER1_INTERRUPT_MASK_REGISTER |= \
    PORT_TIMER1_ENABLE_OCR##channel##_INTERRUPT_MASK

#define PORT_TIMER1_ENABLE_OVERFLOW_INTERRUPT_MASK \
  (1<<TOIE1)

#define PORT_TIMER1_ENABLE_OVERFLOW_INTERRUPT() \
  PORT_TIMER1_INTERRUPT_MASK_REGISTER |= \
    PORT_TIMER1_ENABLE_OVERFLOW_INTERRUPT_MASK

#define PORT_TIMER1_INTERRUPT_FLAG_REGISTER \
  TIFR1

#define PORT_TIMER1_ICP_INTERRUPT_MASK \
  (1<<ICF1)

#define PORT_TIMER1_0CRA_INTERRUPT_MASK \
  (1<<OCF1A)

#define PORT_TIMER1_0CRB_INTERRUPT_MASK \
  (1<<OCF1B)

#define PORT_TIMER1_TOV_INTERRUPT_MASK \
  (1<<TOV1)

/* Timer/Counter 2 */

#define TIMER2

#define PORT_TIMER2_COUNTER_REGISTER \
  TCNT2

#define PORT_TIMER2_CONTROL_REGISTER_A \
  TCCR2A

#define PORT_TIMER2_CONTROL_REGISTER_B \
  TCCR2B

/* waveform generation mode */
enum _port_timer2_wgm {
  PORT_TIMER2_WGM_NORMAL = 0,
  PORT_TIMER2_WGM_PWM_PHASE_CORRECT_1,
  PORT_TIMER2_WGM_CTC,
  PORT_TIMER2_WGM_FAST_PWM_1,
  PORT_TIMER2_WGM_PWM_PHASE_CORRECT_2,
  PORT_TIMER2_WGM_FAST_PWM_2
};

#define PORT_TIMER2_PWM_PHASE_CORRECT_1() \
  PORT_TIMER2_CONTROL_REGISTER_A |= (1<<WGM20)

#define PORT_TIMER2_CTC() \
  PORT_TIMER2_CONTROL_REGISTER_A |= (1<<WGM21)

#define PORT_TIMER2_FAST_PWM_1() \
  PORT_TIMER2_CONTROL_REGISTER_A |= (1<<WGM20) | (1<<WGM21)

#define PORT_TIMER2_PWM_PHASE_CORRECT_2() \
  do { \
    PORT_TIMER2_CONTROL_REGISTER_A |= (1<<WGM20); \
    PORT_TIMER2_CONTROL_REGISTER_B |= (1<<WGM22); \
  } while (0)

#define PORT_TIMER2_FAST_PWM_2() \
  do { \
    PORT_TIMER2_CONTROL_REGISTER_A |= (1<<WGM20) | (1<<WGM21); \
    PORT_TIMER2_CONTROL_REGISTER_B |= (1<<WGM22); \
  } while (0)

/* compare output mode */
enum _port_timer2_com_a {
  PORT_TIMER2_COMA_NORMAL = 0,
  PORT_TIMER2_COMA_TOGGLE_OC2A,
  PORT_TIMER2_COMA_CLEAR_OC2A,
  PORT_TIMER2_COMA_SET_OC2A
};

#define PORT_TIMER2_TOGGLE_OC2A() \
  PORT_TIMER2_CONTROL_REGISTER_A |= (1<<COM2A0)

#define PORT_TIMER2_CLEAR_OC2A() \
  PORT_TIMER2_CONTROL_REGISTER_A |= (1<<COM2A1)

#define PORT_TIMER2_SET_OC2A() \
  PORT_TIMER2_CONTROL_REGISTER_A |= (1<<COM2A0) | (1<<COM2A1)

/* compare output mode */
enum _port_timer2_com_b {
  PORT_TIMER2_COMB_NORMAL = 0,
  PORT_TIMER2_COMB_TOGGLE_OC2B,
  PORT_TIMER2_COMB_CLEAR_OC2B,
  PORT_TIMER2_COMB_SET_OC2B
};

#define PORT_TIMER2_TOGGLE_OC2B() \
  PORT_TIMER2_CONTROL_REGISTER_A |= (1<<COM2B0)

#define PORT_TIMER2_CLEAR_OC2B() \
  PORT_TIMER2_CONTROL_REGISTER_A |= (1<<COM2B1)

#define PORT_TIMER2_SET_OC2B() \
  PORT_TIMER2_CONTROL_REGISTER_A |= (1<<COM2B0) | (1<<COM2B1)

#define PORT_TIMER2_CS_1_MASK \
  (1<<CS20)

#define PORT_TIMER2_CS_8_MASK \
  (1<<CS21)

#define PORT_TIMER2_CS_32_MASK \
  (1<<CS20) | (1<<CS21)

#define PORT_TIMER2_CS_64_MASK \
  (1<<CS22)

#define PORT_TIMER2_CS_128_MASK \
  (1<<CS20) | (1<<CS22)

#define PORT_TIMER2_CS_256_MASK \
  (1<<CS21) | (1<<CS22)

#define PORT_TIMER2_CS_1024_MASK \
  (1<<CS20) | (1<<CS21) | (1<<CS22)

#define PORT_TIMER2_SET_CS(cs) \
  PORT_TIMER2_CONTROL_REGISTER_B |= \
    PORT_TIMER2_CS_##cs##_MASK

#define PORT_TIMER2_SET_COUNT(value) \
  PORT_TIMER2_COUNTER_REGISTER = (value)

#define PORT_TIMER2_SET_OCR(channel, value) \
  OCR2##channel = (value)

#define PORT_TIMER2_FORCE_OCRA_MASK \
  (1<<FOC2A)

#define PORT_TIMER2_FORCE_OCRB_MASK \
  (1<<FOC2B)

#define PORT_TIMER2_FORCE_OUTPUT_COMPARE(channel) \
  PORT_TIMER2_CONTROL_REGISTER_B |= \
    PORT_TIMER2_FORCE_OCR##channel##_MASK

#define PORT_TIMER2_COMPA_VECT \
  TIMER2_COMPA_vect

#define PORT_TIMER2_COMPB_VECT \
  TIMER2_COMPB_vect

#define PORT_TIMER2_OVF_VECT \
  TIMER2_OVF_vect

#define PORT_TIMER2_INTERRUPT_MASK_REGISTER \
  TIMSK2

#define PORT_TIMER2_ENABLE_OCRA_INTERRUPT_MASK \
  (1<<OCIE2A);

#define PORT_TIMER2_ENABLE_OCRB_INTERRUPT_MASK \
  (1<<OCIE2B);

#define PORT_TIMER2_ENABLE_OCR_INTERRUPT(channel) \
  PORT_TIMER2_INTERRUPT_MASK_REGISTER |= \
    PORT_TIMER2_ENABLE_OCR##channel##_INTERRUPT_MASK

#define PORT_TIMER2_ENABLE_OVERFLOW_INTERRUPT_MASK \
  (1<<TOIE2)

#define PORT_TIMER2_ENABLE_OVERFLOW_INTERRUPT() \
  PORT_TIMER2_INTERRUPT_MASK_REGISTER |= \
    PORT_TIMER2_ENABLE_OVERFLOW_INTERRUPT_MASK

#define PORT_TIMER2_INTERRUPT_FLAG_REGISTER \
  TIFR2

#define PORT_TIMER2_0CRA_INTERRUPT_MASK \
  (1<<OCF2A)

#define PORT_TIMER2_0CRB_INTERRUPT_MASK \
  (1<<OCF2B)

#define PORT_TIMER2_TOV_INTERRUPT_MASK \
  (1<<TOV2)

#define PORT_TIMER2_ASYNC_STATUS_REGISTER \
  ASSR

#define PORT_TIMER2_ENABLE_EXTERNAL_CLK_INPUT() \
  PORT_TIMER2_ASYNC_STATUS_REGISTER |= (1<<EXCLK)

#define PORT_TIMER2_ASYNC_OPERATION() \
  PORT_TIMER2_ASYNC_STATUS_REGISTER |= (1<<AS2)

#define PORT_TIMER2_TCNT_UPDATE_BUSY() \
  (PORT_TIMER2_ASYNC_STATUS_REGISTER & (1<<TCN2UB))

#define PORT_TIMER2_OCRA_UPDATE_BUSY() \
  (PORT_TIMER2_ASYNC_STATUS_REGISTER & (1<<OCR2AUB))

#define PORT_TIMER2_OCRB_UPDATE_BUSY() \
  (PORT_TIMER2_ASYNC_STATUS_REGISTER & (1<<OCR2BUB))

#define PORT_TIMER2_TCCRA_UPDATE_BUSY() \
  (PORT_TIMER2_ASYNC_STATUS_REGISTER & (1<<TCR2AUB))

#define PORT_TIMER2_TCCRB_UPDATE_BUSY() \
  (PORT_TIMER2_ASYNC_STATUS_REGISTER & (1<<TCR2BUB))

/* General timer control register */

#define PORT_GENERAL_TIMER_CONTROL_REGISTER \
  GTCCR

#define PORT_TIMER_SYNCHRONIZATION_MODE_START() \
  PORT_GENERAL_TIMER_CONTROL_REGISTER |= (1<<TSM)

#define PORT_TIMER_SYNCHRONIZATION_MODE_END() \
  PORT_GENERAL_TIMER_CONTROL_REGISTER &= ~(1<<TSM)

#define PORT_SYNC_TIMER_PRESCALER_RESET() \
  PORT_GENERAL_TIMER_CONTROL_REGISTER |= (1<<PSRSYNC)

#define PORT_ASYNC_TIMER_PRESCALER_RESET() \
  PORT_GENERAL_TIMER_CONTROL_REGISTER |= (1<<PSRASY)

/* system clock tick */

#define PORT_SYS_TICK_VECT \
  PORT_TIMER1_COMPA_VECT

#define PORT_INIT_SYSTICK_TIMER(duration) \
  do {                                           \
    /* CTC mode */                               \
    PORT_TIMER1_CTC();                           \
    /* output compare interrupt for channel A */ \
    PORT_TIMER1_ENABLE_OCR_INTERRUPT(A);         \
    /* reset timer counter */                    \
    PORT_TIMER1_SET_COUNT(0);                     \
    /* set the top of CTC on channel A */        \
    PORT_TIMER1_SET_OCR(A, duration);            \
    /* timer activated, 1:1 pre-scaler */        \
    PORT_TIMER1_SET_CS(1);                       \
  } while (0)

/* USART */

#define PORT_UDRE_VECT \
  USART_UDRE_vect

#define PORT_RXC_VECT \
  USART_RX_vect

#define PORT_UDR \
  UDR0

#define PORT_UART_BAUDREG_CALC(baud_rate) \
  (uint16_t)((F_CPU / (16L * baud_rate)) - 1)

#define PORT_UART_SET_BAUD_RATE(baud_rate) \
  UBRR0 = PORT_UART_BAUDREG_CALC(baud_rate)

#define PORT_UART_SET_CHAR_SIZE(n) \
  if (n > 5) { \
    if (n == 9) UCSR0C |= (3<<UCSZ00); \
    else UCSR0C |= ((n-5)<<UCSZ00); \
  }

#define PORT_UART_SET_STOP_BITS(n) \
  if (n == 2) { \
    UCSR0C |= (1<<USBS0); \
  }

#define PORT_UART_SET_PARITY(parity) \
  if (parity == UART_PARITY_EVEN) { \
    UCSR0C |= (1<<UPM01); \
  } else if (parity == UART_PARITY_ODD) { \
    UCSR0C |= (1<<UPM00) | (1<<UPM01); \
  }

#define PORT_UART_INIT() \
  do { \
    UCSR0B = (1<<RXCIE0) | (1<<RXEN0) | (1<<TXEN0); \
  } while (0)

#define PORT_ENABLE_UDRE_INTERRUPT() \
  UCSR0B |= (1<<UDRIE0)

#define PORT_DISABLE_UDRE_INTERRUPT() \
  UCSR0B &= ~(1<<UDRIE0)

#define PORT_ENABLE_RXC_INTERRUPT() \
  UCSR0B |= (1<<RXCIE0)

#define PORT_DISABLE_RXC_INTERRUPT() \
  UCSR0B &= ~(1<<RXCIE0)

/* ADC */

#define PORT_ADC_CONTROL_STATUS_REGISTER_A \
  ADCSRA

#define PORT_ADC_CONTROL_STATUS_REGISTER_B \
  ADCSRB

#define PORT_ADC_MUX_CONTROL_REGISTER \
  ADMUX

#define PORT_ADC_DATA_REGISTER \
  ADC

#define PORT_ADC_ENABLE() \
  PORT_ADC_CONTROL_STATUS_REGISTER_A |= (1<<ADEN)

#define PORT_ADC_DISABLE() \
  PORT_ADC_CONTROL_STATUS_REGISTER_A &= ~(1<<ADEN)

#define PORT_ADC_IS_ENABLED() \
  (PORT_ADC_CONTROL_STATUS_REGISTER_A & (1<<ADEN))

enum _port_adc_prescaler {
  PORT_ADC_PS_2 = 1,
  PORT_ADC_PS_4,
  PORT_ADC_PS_8,
  PORT_ADC_PS_16,
  PORT_ADC_PS_32,
  PORT_ADC_PS_64,
  PORT_ADC_PS_128 
};

#define PORT_ADC_PS_2_MASK \
  (1<<ADPS0)

#define PORT_ADC_PS_4_MASK \
  (1<<ADPS1)

#define PORT_ADC_PS_8_MASK \
  (1<<ADPS0) | (1<<ADPS1)

#define PORT_ADC_PS_16_MASK \
  (1<<ADPS2)

#define PORT_ADC_PS_32_MASK \
  (1<<ADPS0) | (1<<ADPS2)

#define PORT_ADC_PS_64_MASK \
  (1<<ADPS1) | (1<<ADPS2)

#define PORT_ADC_PS_128_MASK \
  (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2)

#define PORT_ADC_SET_PS(ps) \
  PORT_ADC_CONTROL_STATUS_REGISTER_A |= PORT_ADC_PS_##ps##_MASK

enum _port_adc_reference {
  PORT_ADC_REF_AREF = 0,
  PORT_ADC_REF_AVCC = 1,
  PORT_ADC_REF_INTERNAL = 3
};

#define PORT_ADC_SET_REF_AREF() \
  PORT_ADC_MUX_CONTROL_REGISTER &= ~((1<<REFS0) | (1<<REFS1))

#define PORT_ADC_SET_REF_AVCC() \
  PORT_ADC_MUX_CONTROL_REGISTER |= (1<<REFS0), \
  PORT_ADC_MUX_CONTROL_REGISTER &= ~(1<<REFS1)

#define PORT_ADC_SET_REF_INTERNAL() \
  PORT_ADC_MUX_CONTROL_REGISTER |= (1<<REFS0) | (1<<REFS1)

#define PORT_ADC_SET_REF(ref) \
  PORT_ADC_SET_REF_##ref()

#define PORT_ADC_SET_CHAN_0() \
  PORT_ADC_MUX_CONTROL_REGISTER &= 0Xf0

#define PORT_ADC_SET_CHAN_1() \
  PORT_ADC_MUX_CONTROL_REGISTER |= (1<<MUX0), \
  PORT_ADC_MUX_CONTROL_REGISTER &= ~((1<<MUX1) | (1<<MUX2) | (1<<MUX3)) 

#define PORT_ADC_SET_CHAN_2() \
  PORT_ADC_MUX_CONTROL_REGISTER |= (1<<MUX1), \
  PORT_ADC_MUX_CONTROL_REGISTER &= ~((1<<MUX0) | (1<<MUX2) | (1<<MUX3)) 

#define PORT_ADC_SET_CHAN_3() \
  PORT_ADC_MUX_CONTROL_REGISTER |= (1<<MUX0) | (1<<MUX1), \
  PORT_ADC_MUX_CONTROL_REGISTER &= ~((1<<MUX2) | (1<<MUX3)) 

#define PORT_ADC_SET_CHAN_4() \
  PORT_ADC_MUX_CONTROL_REGISTER |= (1<<MUX2), \
  PORT_ADC_MUX_CONTROL_REGISTER &= ~((1<<MUX0) | (1<<MUX1) | (1<<MUX3)) 

#define PORT_ADC_SET_CHAN_5() \
  PORT_ADC_MUX_CONTROL_REGISTER |= (1<<MUX0) | (1<<MUX2), \
  PORT_ADC_MUX_CONTROL_REGISTER &= ~((1<<MUX1) | (1<<MUX3)) 

#define PORT_ADC_SET_CHAN_6() \
  PORT_ADC_MUX_CONTROL_REGISTER |= (1<<MUX1) | (1<<MUX2), \
  PORT_ADC_MUX_CONTROL_REGISTER &= ~((1<<MUX0) | (1<<MUX3)) 

#define PORT_ADC_SET_CHAN_7() \
  PORT_ADC_MUX_CONTROL_REGISTER |= (1<<MUX0) | (1<<MUX1) | (1<<MUX2), \
  PORT_ADC_MUX_CONTROL_REGISTER &= ~(1<<MUX3) 

#define PORT_ADC_SET_CHAN_8() \
  PORT_ADC_MUX_CONTROL_REGISTER |= (1<<MUX3), \
  PORT_ADC_MUX_CONTROL_REGISTER &= ~((1<<MUX0) | (1<<MUX1) | (1<<MUX2)) 

#define PORT_ADC_SET_CHAN_INTERNAL() \
  PORT_ADC_MUX_CONTROL_REGISTER |= (1<<MUX1) | (1<<MUX2) | (1<<MUX3), \
  PORT_ADC_MUX_CONTROL_REGISTER &= ~(1<<MUX0) 

#define PORT_ADC_SET_CHAN_GND() \
  PORT_ADC_MUX_CONTROL_REGISTER |= 0X0f

#define PORT_ADC_SET_CHAN(ch) \
  if ((ch < 9) || (ch == 14) || (ch == 15)) { \
    PORT_ADC_MUX_CONTROL_REGISTER = \
    (PORT_ADC_MUX_CONTROL_REGISTER & 0xf0) | ch; \
  }

#define PORT_ADC_SET_LAR() \
  PORT_ADC_MUX_CONTROL_REGISTER |= (1<<ADLAR)

#define PORT_ADC_SET_NO_LAR() \
  PORT_ADC_MUX_CONTROL_REGISTER &= ~(1<<ADLAR)

#define PORT_ADC_START_CONVERSION() \
  PORT_ADC_CONTROL_STATUS_REGISTER_A |= (1<<ADSC)

#define PORT_ADC_AUTO_TRIGGER_ENABLE_MASK \
  (1<<ADATE)

#define PORT_ADC_ENABLE_AUTO_TRIGGER() \
  PORT_ADC_CONTROL_STATUS_REGISTER_A |= PORT_ADC_AUTO_TRIGGER_ENABLE_MASK

#define PORT_ADC_DISABLE_AUTO_TRIGGER() \
  PORT_ADC_CONTROL_STATUS_REGISTER_A &= ~PORT_ADC_AUTO_TRIGGER_ENABLE_MASK

#define PORT_ADC_IS_AUTO_TRIGGER_ENABLED() \
  (PORT_ADC_CONTROL_STATUS_REGISTER_A & PORT_ADC_AUTO_TRIGGER_ENABLE_MASK)

#define PORT_ADC_SET_FREE_RUNNING() \
  PORT_ADC_CONTROL_STATUS_REGISTER_B &= 0xf8

#define PORT_ADC_SET_AT_ANALOG_COMPARATOR() \
  PORT_ADC_CONTROL_STATUS_REGISTER_B |= (1<<ADTS0)

#define PORT_ADC_SET_AT_EXTERNAL_IRQ0() \
  PORT_ADC_CONTROL_STATUS_REGISTER_B |= (1<<ADTS1)

#define PORT_ADC_SET_AT_TIMER0_COMPARE_MATCH_A() \
  PORT_ADC_CONTROL_STATUS_REGISTER_B |= (1<<ADTS0) | (1<<ADTS1)

#define PORT_ADC_SET_AT_TIMER0_OVERFLOW() \
  PORT_ADC_CONTROL_STATUS_REGISTER_B |= (1<<ADTS2)

#define PORT_ADC_SET_AT_TIMER1_COMPARE_MATCH_B() \
  PORT_ADC_CONTROL_STATUS_REGISTER_B |= (1<<ADTS0) | (1<<ADTS2)

#define PORT_ADC_SET_AT_TIMER1_OVERFLOW() \
  PORT_ADC_CONTROL_STATUS_REGISTER_B |= (1<<ADTS1) | (1<<ADTS2)

#define PORT_ADC_SET_AT_TIMER1_CAPTURE_EVENT() \
  PORT_ADC_CONTROL_STATUS_REGISTER_B |= (1<<ADTS0) | (1<<ADTS1) | (1<<ADTS2)

#define PORT_ADC_VECT \
  ADC_vect

#define PORT_ADC_INTERRUPT_ENABLE_MASK \
  (1<<ADIE)

#define PORT_ADC_ENABLE_INTERRUPT() \
  PORT_ADC_CONTROL_STATUS_REGISTER_A |= PORT_ADC_INTERRUPT_ENABLE_MASK

#define PORT_ADC_DISABLE_INTERRUPT() \
  PORT_ADC_CONTROL_STATUS_REGISTER_A &= ~PORT_ADC_INTERRUPT_ENABLE_MASK

#define PORT_ADC_IS_INTERRUPT_ENABLED() \
  (PORT_ADC_CONTROL_STATUS_REGISTER_A & PORT_ADC_INTERRUPT_ENABLE_MASK)

#define PORT_ADC_INTERRUPT_MASK \
  (1<<ADIF)

#define PORT_ADC_IS_INTERRUPT_FLAG_SET() \
  (PORT_ADC_CONTROL_STATUS_REGISTER_A & PORT_ADC_INTERRUPT_MASK)

#define PORT_ADC_CLEAR_INTERRUPT_FLAG() \
  PORT_ADC_CONTROL_STATUS_REGISTER_A |= PORT_ADC_INTERRUPT_MASK

#define PORT_DIGITAL_INPUT_DISABLE_REGISTER \
  DIDR0

#define PORT_DIGITAL_INPUT_DISABLE_PIN_0_MASK \
  (1<<ADC0D)

#define PORT_DIGITAL_INPUT_DISABLE_PIN_1_MASK \
  (1<<ADC1D)

#define PORT_DIGITAL_INPUT_DISABLE_PIN_2_MASK \
  (1<<ADC2D)

#define PORT_DIGITAL_INPUT_DISABLE_PIN_3_MASK \
  (1<<ADC3D)

#define PORT_DIGITAL_INPUT_DISABLE_PIN_4_MASK \
  (1<<ADC4D)

#define PORT_DIGITAL_INPUT_DISABLE_PIN_5_MASK \
  (1<<ADC5D)

#define PORT_DIGITAL_INPUT_DISABLE_PIN(pin) \
  PORT_DIGITAL_INPUT_DISABLE_REGISTER |= \
    PORT_DIGITAL_INPUT_DISABLE_PIN_##pin##_MASK

/* watchdog */

#define PORT_WDT_CONTROL_REGISTER \
  WDTCSR

#define PORT_WDT_CHANGE_ENABLE_MASK \
  (1<<WDCE)

#define PORT_WDT_ENABLE_SYSTEM_RESET_MASK \
  (1<<WDE)

#define PORT_WDT_ENABLE_INTERRUPT_MASK \
  (1<<WDIE)

#define PORT_WDT_TIMEOUT_15MS   WDTO_15MS

#define PORT_WDT_TIMEOUT_30MS   WDTO_30MS

#define PORT_WDT_TIMEOUT_60MS   WDTO_60MS

#define PORT_WDT_TIMEOUT_120MS  WDTO_120MS

#define PORT_WDT_TIMEOUT_250MS  WDTO_250MS

#define PORT_WDT_TIMEOUT_500MS  WDTO_500MS

#define PORT_WDT_TIMEOUT_1S     WDTO_1S

#define PORT_WDT_TIMEOUT_2S     WDTO_2S

#define PORT_WDT_TIMEOUT_4S     WDTO_4S

#define PORT_WDT_TIMEOUT_8s     WDTO_8S

#define PORT_WDT_VECT \
  WDT_vect

#define PORT_WDT_ENABLE_INTERRUPT(timeout) \
  asm volatile (                                               \
      "in __tmp_reg__, __SREG__"  "\n\t"                       \
      "cli"                       "\n\t"                       \
      "wdr"                       "\n\t"                       \
      "sts %0, %1"                "\n\t"                       \
      "out __SREG__, __tmp_reg__" "\n\t"                       \
      "sts %0, %2"                "\n\t"                       \
      : /* no outputs */                                       \
      : "n" (_SFR_MEM_ADDR(PORT_WDT_CONTROL_REGISTER)),        \
      "r" ((uint8_t)(PORT_WDT_CHANGE_ENABLE_MASK |             \
          PORT_WDT_ENABLE_SYSTEM_RESET_MASK)),                 \
      "r" ((uint8_t) ((timeout & 0x08 ? (1<<WDP3) : 0x00) |    \
          PORT_WDT_ENABLE_INTERRUPT_MASK | (timeout & 0x07)) ) \
      : "r0"                                                   \
  );

#define PORT_GET_TASK_TIMEOUT(wdt_timeout) \
  ({                           \
    uint16_t timeout = 0;      \
    switch(wdt_timeout) {      \
      case WDTO_15MS:          \
        timeout = 15; break;   \
      case WDTO_30MS:          \
        timeout = 30; break;   \
      case WDTO_60MS:          \
        timeout = 60; break;   \
      case WDTO_120MS:         \
        timeout = 120; break;  \
      case WDTO_250MS:         \
        timeout = 250; break;  \
      case WDTO_500MS:         \
        timeout = 500; break;  \
      case WDTO_1S:            \
        timeout = 1000; break; \
      case WDTO_2S:            \
        timeout = 2000; break; \
    }; timeout;                \
  })

/* sleep */

#define PORT_SLEEP_MODE_CONTROL_REGISTER \
  SMCR

#define PORT_SLEEP_MODE_IS_IDLE() \
  ((PORT_SLEEP_MODE_CONTROL_REGISTER & 0xe) == 0)

#define PORT_SLEEP_MODE_IS_ADC_NOISE_RED() \
  ((PORT_SLEEP_MODE_CONTROL_REGISTER & 0xe) == 0x1)

#define PORT_SLEEP_MODE_IS_POWER_DOWN() \
  ((PORT_SLEEP_MODE_CONTROL_REGISTER & 0xe) == 0x2)

#define PORT_SLEEP_MODE_IS_POWER_SAVE() \
  ((PORT_SLEEP_MODE_CONTROL_REGISTER & 0xe) == 0x3)

#define PORT_SLEEP_MODE_IS_STANDBY() \
  ((PORT_SLEEP_MODE_CONTROL_REGISTER & 0xe) == 0x6)

#define PORT_SLEEP_MODE_IS_EXTERNAL_STANDBY() \
  ((PORT_SLEEP_MODE_CONTROL_REGISTER & 0xe) == 0x7)

/* power reduction */

#define PORT_SET_PWR_REDUCTION_TWI() \
  SET_BIT(PRR, PRTWI)

#define PORT_SET_PWR_REDUCTION_TIMER0() \
  SET_BIT(PRR, PRTIM0)

#define PORT_SET_PWR_REDUCTION_TIMER1() \
  SET_BIT(PRR, PRTIM1)

#define PORT_SET_PWR_REDUCTION_TIMER2() \
  SET_BIT(PRR, PRTIM2)

#define PORT_SET_PWR_REDUCTION_SPI() \
  SET_BIT(PRR, PRSPI)

#define PORT_SET_PWR_REDUCTION_USART0() \
  SET_BIT(PRR, PRUSART0)

#define PORT_SET_PWR_REDUCTION_ADC() \
  SET_BIT(PRR, PRADC)

/* trap */

#define PORT_TRAP_INTERRUPT_MASK_REGISTER \
  EIMSK

#define PORT_TRAP_INTERRUPT_MASK \
  (1<<INT1)

#define PORT_TRAP_VECT \
  INT1_vect

#define PORT_TRAP_PORT \
  D

#define PORT_TRAP_PIN \
  3

/* error handling */

#define PORT_EEPROM_ERROR_FLAG_ADDR 0x3ff

#define PORT_EEPROM_ERROR_VALUE_ADDR 0x3fe

#define PORT_EEPROM_PC_VALUE_HIGH_ADDR 0x3fd

#define PORT_EEPROM_PC_VALUE_LOW_ADDR 0x3fc

#endif /* __AVR_ATmega328P__ */

#endif /* PORT_MEGA328P_H */
