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
 * @file timer0.h
 * @author notweerdmonk
 * @brief header file for Timer/Counter 0
 */

#ifndef _TIMER0_H_
#define _TIMER0_H_

#include <port.h>
#include <kernel.h>

#ifdef TIMER0

typedef enum _port_timer0_wgm timer0_wgm_t;

typedef enum _port_timer0_com_a timer0_com_a_t;

typedef enum _port_timer0_com_b timer0_com_b_t;

#define TIMER0_SET_WAVEFORM_GENERATION_MODE(mode) \
  do { \
    if (check_resource_owner(SYS_RES_TIMER0)) {\
      \
      PORT_TIMER0_CONTROL_REGISTER_A &= ~( (1<<WGM00) | (1<<WGM01) ); \
      PORT_TIMER0_CONTROL_REGISTER_B &= ~(1<<WGM02); \
      \
      switch (mode) { \
        case PORT_TIMER0_WGM_PWM_PHASE_CORRECT_1: \
          PORT_TIMER0_PWM_PHASE_CORRECT_1(); \
          break; \
        \
        case PORT_TIMER0_WGM_CTC: \
          PORT_TIMER0_CTC(); \
          break; \
        \
        case PORT_TIMER0_WGM_FAST_PWM_1: \
          PORT_TIMER0_FAST_PWM_1(); \
          break; \
        \
        case PORT_TIMER0_WGM_PWM_PHASE_CORRECT_2: \
          PORT_TIMER0_PWM_PHASE_CORRECT_2(); \
          break; \
        \
        case PORT_TIMER0_WGM_FAST_PWM_2: \
          PORT_TIMER0_FAST_PWM_2(); \
          break; \
        \
        case PORT_TIMER0_WGM_NORMAL: \
          ; \
      } \
    } \
  } while (0)

#define TIMER0_SET_COMPARE_OUTPUT_MODE_A(mode) \
  do { \
    if (check_resource_owner(SYS_RES_TIMER0)) {\
      \
      PORT_TIMER0_CONTROL_REGISTER_A &= ~( (1<<COM0A0) | (1<<COM0A1) ); \
      \
      switch (mode) { \
        case PORT_TIMER0_COMA_TOGGLE_OC0A: \
          PORT_TIMER0_TOGGLE_OC0A(); \
          break; \
        \
        case PORT_TIMER0_COMA_CLEAR_OC0A: \
          PORT_TIMER0_CLEAR_OC0A(); \
          break; \
        \
        case PORT_TIMER0_COMA_SET_OC0A: \
          PORT_TIMER0_SET_OC0A(); \
          break; \
        \
        case PORT_TIMER0_COMA_NORMAL: \
          ; \
      } \
    } \
  } while (0)

#define TIMER0_SET_COMPARE_OUTPUT_MODE_B(mode) \
  do { \
    if (check_resource_owner(SYS_RES_TIMER0)) {\
      \
      PORT_TIMER0_CONTROL_REGISTER_A &= ~( (1<<COM0B0) | (1<<COM0B1) ); \
      \
      switch (mode) { \
        case PORT_TIMER0_COMB_TOGGLE_OC0B: \
          PORT_TIMER0_TOGGLE_OC0B(); \
          break; \
        \
        case PORT_TIMER0_COMB_CLEAR_OC0B: \
          PORT_TIMER0_CLEAR_OC0B(); \
          break; \
        \
        case PORT_TIMER0_COMB_SET_OC0B: \
          PORT_TIMER0_SET_OC0B(); \
          break; \
        \
        case PORT_TIMER0_COMB_NORMAL: \
          ; \
      } \
    } \
  } while (0)

/**
 * Values for cs:
 * 1
 * 8
 * 64
 * 256
 * 1024
 * EXT_FALLING
 * EXT_RISING
 */
#define TIMER0_SET_CS(cs) \
  do { \
    if (check_resource_owner(SYS_RES_TIMER0)) \
      PORT_TIMER0_SET_CS(cs); \
  } while (0)

#define TIMER0_GET_COUNT() PORT_TIMER0_COUNTER_REGISTER

#define TIMER0_SET_COUNT(value) \
  do { \
    if (check_resource_owner(SYS_RES_TIMER0)) \
      PORT_TIMER0_SET_COUNT(value); \
  } while (0)

#define TIMER0_SET_OCR(channel, value) \
  do { \
    if (check_resource_owner(SYS_RES_TIMER0)) \
      PORT_TIMER0_SET_OCR(channel, value); \
  } while (0)

/**
 * Values for channel:
 * A
 * B
 */
#define TIMER0_FORCE_OUTPUT_COMPARE(channel) \
  do { \
    if (check_resource_owner(SYS_RES_TIMER0)) \
      PORT_TIMER0_FORCE_OUTPUT_COMPARE(channel); \
  } while (0)

/**
 * Values for channel:
 * A
 * B
 */
#define TIMER0_ENABLE_OCR_INTERRUPT(channel) \
  do { \
    if (check_resource_owner(SYS_RES_TIMER0)) \
      PORT_TIMER0_ENABLE_OCR_INTERRUPT(channel); \
  } while (0)

#define TIMER0_ENABLE_OVERFLOW_INTERRUPT() \
  do { \
    if (check_resource_owner(SYS_RES_TIMER0)) \
      PORT_TIMER0_ENABLE_OVERFLOW_INTERRUPT(); \
  } while (0)

#define TIMER0_OCRA_INTERRUPT_FLAG_IS_SET() \
  (PORT_TIMER0_INTERRUPT_FLAG_REGISTER & PORT_TIMER0_0CRA_INTERRUPT_MASK)

#define TIMER0_OCRB_INTERRUPT_FLAG_IS_SET() \
  (PORT_TIMER0_INTERRUPT_FLAG_REGISTER & PORT_TIMER0_0CRB_INTERRUPT_MASK)

#define TIMER0_TOV_INTERRUPT_FLAG_IS_SET() \
  (PORT_TIMER0_INTERRUPT_FLAG_REGISTER & PORT_TIMER0_TOV_INTERRUPT_MASK)

#endif /* TIMER0 */

#endif /* _TIMER0_H_ */
