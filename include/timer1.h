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
 * @file timer1.h
 * @author notweerdmonk
 * @brief header file for Timer/Counter 1
 */

#ifndef _TIMER1_H_
#define _TIMER1_H_

#include <port.h>
#include <kernel.h>

#ifdef TIMER1

typedef enum _port_timer1_wgm timer1_wgm_t;

typedef enum _port_timer1_com_a timer1_com_a_t;

typedef enum _port_timer1_com_b timer1_com_b_t;

#define TIMER1_SET_WAVEFORM_GENERATION_MODE(mode) \
  do { \
    RESTRICT_SYS_TICK_TIMER(); \
    \
    if (check_resource_owner(SYS_RES_TIMER1)) { \
      \
      PORT_TIMER1_CONTROL_REGISTER_A &= ~( (1<<WGM10) | (1<<WGM11) ); \
      PORT_TIMER1_CONTROL_REGISTER_B &= ~( (1<<WGM12) | (1<<WGM13) ); \
      \
      switch (mode) { \
        case PORT_TIMER1_WGM_PWM_PHASE_CORRECT_8BIT: \
          PORT_TIMER1_PWM_PHASE_CORRECT_8BIT(); \
          break; \
        \
        case PORT_TIMER1_WGM_PWM_PHASE_CORRECT_9BIT: \
          PORT_TIMER1_PWM_PHASE_CORRECT_9BIT(); \
          break; \
        \
        case PORT_TIMER1_WGM_PWM_PHASE_CORRECT_10BIT: \
          PORT_TIMER1_PWM_PHASE_CORRECT_10BIT(); \
          break; \
        \
        case PORT_TIMER1_WGM_CTC: \
          PORT_TIMER1_CTC(); \
          break; \
        \
        case PORT_TIMER1_WGM_FAST_PWM_8BIT: \
          PORT_TIMER1_FAST_PWM_8BIT(); \
          break; \
        \
        case PORT_TIMER1_WGM_FAST_PWM_9BIT: \
          PORT_TIMER1_FAST_PWM_9BIT(); \
          break; \
        \
        case PORT_TIMER1_WGM_FAST_PWM_10BIT: \
          PORT_TIMER1_FAST_PWM_10BIT(); \
          break; \
        \
        case PORT_TIMER1_WGM_PWM_PHASE_FREQ_CORRECT_ICP: \
          PORT_TIMER1_PWM_PHASE_FREQ_CORRECT_ICP(); \
          break; \
        \
        case PORT_TIMER1_WGM_PWM_PHASE_FREQ_CORRECT: \
          PORT_TIMER1_PWM_PHASE_FREQ_CORRECT(); \
          break; \
        \
        case PORT_TIMER1_WGM_PWM_PHASE_CORRECT_ICP: \
          PORT_TIMER1_PWM_PHASE_CORRECT_ICP(); \
          break; \
        \
        case PORT_TIMER1_WGM_PWM_PHASE_CORRECT: \
          PORT_TIMER1_PWM_PHASE_CORRECT(); \
          break; \
        \
        case PORT_TIMER1_WGM_CTC_ICP: \
          PORT_TIMER1_CTC_ICP(); \
          break; \
        \
        case PORT_TIMER1_WGM_FAST_PWM_ICP: \
          PORT_TIMER1_FAST_PWM_ICP(); \
          break; \
        \
        case PORT_TIMER1_WGM_FAST_PWM: \
          PORT_TIMER1_FAST_PWM(); \
          break; \
        \
        case PORT_TIMER1_WGM_NORMAL: \
          ; \
      } \
    } \
  } while (0)

#define TIMER1_SET_COMPARE_OUTPUT_MODE_A(mode) \
  do { \
    RESTRICT_SYS_TICK_TIMER(); \
    \
    if (check_resource_owner(SYS_RES_TIMER1)) { \
      \
      PORT_TIMER0_CONTROL_REGISTER_A &= ~( (1<<COM1A0) | (1<<COM1A1) ); \
      \
      switch (mode) { \
        case PORT_TIMER1_COMA_TOGGLE_OC1A: \
          PORT_TIMER1_TOGGLE_OC1A(); \
          break; \
        \
        case PORT_TIMER1_COMA_CLEAR_OC1A: \
          PORT_TIMER1_CLEAR_OC1A(); \
          break; \
        \
        case PORT_TIMER1_COMA_SET_OC1A: \
          PORT_TIMER1_SET_OC1A(); \
          break; \
        \
        case PORT_TIMER1_COMA_NORMAL: \
          ; \
      } \
    } \
  } while (0)

#define TIMER1_SET_COMPARE_OUTPUT_MODE_B(mode) \
  do { \
    RESTRICT_SYS_TICK_TIMER(); \
    \
    if (check_resource_owner(SYS_RES_TIMER1)) { \
      \
      PORT_TIMER1_CONTROL_REGISTER_A &= ~( (1<<COM1B0) | (1<<COM1B1) ); \
      \
      switch (mode) { \
        case PORT_TIMER1_COMB_TOGGLE_OC1B: \
          PORT_TIMER1_TOGGLE_OC1B(); \
          break; \
        \
        case PORT_TIMER1_COMB_CLEAR_OC1B: \
          PORT_TIMER1_CLEAR_OC1B(); \
          break; \
        \
        case PORT_TIMER1_COMB_SET_OC1B: \
          PORT_TIMER1_SET_OC1B(); \
          break; \
        \
        case PORT_TIMER1_COMB_NORMAL: \
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
#define TIMER1_SET_CS(cs) \
  do { \
    RESTRICT_SYS_TICK_TIMER(); \
    if (check_resource_owner(SYS_RES_TIMER1)) \
      PORT_TIMER1_SET_CS(cs); \
  } while (0)

#define TIMER1_GET_COUNT() PORT_TIMER1_COUNTER_REGISTER

#define TIMER1_SET_COUNT(value) \
  do { \
    if (check_resource_owner(SYS_RES_TIMER1)) \
      PORT_TIMER1_SET_COUNT(value); \
  } while (0)

#define TIMER1_SET_OCR(channel, value) \
  do { \
    if (check_resource_owner(SYS_RES_TIMER1)) \
      PORT_TIMER1_SET_OCR(channel, value); \
  } while (0)

/**
 * Values for channel:
 * A
 * B
 */
#define TIMER1_FORCE_OUTPUT_COMPARE(channel) \
  do { \
    if (check_resource_owner(SYS_RES_TIMER1)) \
      PORT_TIMER1_FORCE_OUTPUT_COMPARE(channel); \
  } while (0)

#define TIMER1_ENABLE_ICP_INTERRUPT() \
  do { \
    if (check_resource_owner(SYS_RES_TIMER1)) \
      PORT_TIMER1_ENABLE_ICP_INTERRUPT(); \
  } while (0)

/**
 * Values for channel:
 * A
 * B
 */
#define TIMER1_ENABLE_OCR_INTERRUPT(channel) \
  do { \
    if (check_resource_owner(SYS_RES_TIMER1)) \
      PORT_TIMER1_ENABLE_OCR_INTERRUPT(channel); \
  } while (0)

#define TIMER1_ENABLE_OVERFLOW_INTERRUPT() \
  do { \
    if (check_resource_owner(SYS_RES_TIMER1)) \
      PORT_TIMER1_ENABLE_OVERFLOW_INTERRUPT(); \
  } while (0)

#define TIMER1_ICP_INTERRUPT_FLAG_IS_SET() \
  (PORT_TIMER1_INTERRUPT_FLAG_REGISTER & PORT_TIMER1_ICP_INTERRUPT_MASK)

#define TIMER1_OCRA_INTERRUPT_FLAG_IS_SET() \
  (PORT_TIMER1_INTERRUPT_FLAG_REGISTER & PORT_TIMER1_0CRA_INTERRUPT_MASK)

#define TIMER1_OCRB_INTERRUPT_FLAG_IS_SET() \
  (PORT_TIMER1_INTERRUPT_FLAG_REGISTER & PORT_TIMER1_0CRB_INTERRUPT_MASK)

#define TIMER1_TOV_INTERRUPT_FLAG_IS_SET() \
  (PORT_TIMER1_INTERRUPT_FLAG_REGISTER & PORT_TIMER1_TOV_INTERRUPT_MASK)

#endif /* TIMER1 */

#endif /* _TIMER1_H_ */
