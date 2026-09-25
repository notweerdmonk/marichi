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
 * @file timer2.h
 * @author notweerdmonk
 * @brief header file for Timer/Counter 2
 */

#ifndef _TIMER2_H_
#define _TIMER2_H_

#include <port.h>
#include <kernel.h>

#ifdef TIMER2

typedef enum _port_timer2_wgm timer2_wgm_t;

typedef enum _port_timer2_com_a timer2_com_a_t;

typedef enum _port_timer2_com_b timer2_com_b_t;

#define TIMER2_SET_WAVEFORM_GENERATION_MODE(mode) \
  do { \
    if (check_resource_owner(SYS_RES_TIMER2)) { \
      \
      PORT_TIMER2_CONTROL_REGISTER_A &= ~( (1<<WGM20) | (1<<WGM21) ); \
      PORT_TIMER2_CONTROL_REGISTER_B &= ~(1<<WGM22); \
      \
      switch (mode) { \
        case PORT_TIMER2_WGM_PWM_PHASE_CORRECT_1: \
          PORT_TIMER2_PWM_PHASE_CORRECT_1(); \
          break; \
        \
        case PORT_TIMER2_WGM_CTC: \
          PORT_TIMER2_CTC(); \
          break; \
        \
        case PORT_TIMER2_WGM_FAST_PWM_1: \
          PORT_TIMER2_FAST_PWM_1(); \
          break; \
        \
        case PORT_TIMER2_WGM_PWM_PHASE_CORRECT_2: \
          PORT_TIMER2_PWM_PHASE_CORRECT_2(); \
          break; \
        \
        case PORT_TIMER2_WGM_FAST_PWM_2: \
          PORT_TIMER2_FAST_PWM_2(); \
          break; \
        \
        case PORT_TIMER2_WGM_NORMAL: \
          ; \
      } \
    } \
  } while (0)

#define TIMER2_SET_COMPARE_OUTPUT_MODE_A(mode) \
  do { \
    if (check_resource_owner(SYS_RES_TIMER2)) { \
      \
      PORT_TIMER2_CONTROL_REGISTER_A &= ~( (1<<COM2A0) | (1<<COM2A1) ); \
      \
      switch (mode) { \
        case PORT_TIMER2_COMA_TOGGLE_OC2A: \
          PORT_TIMER2_TOGGLE_OC2A(); \
          break; \
        \
        case PORT_TIMER2_COMA_CLEAR_OC2A: \
          PORT_TIMER2_CLEAR_OC2A(); \
          break; \
        \
        case PORT_TIMER2_COMA_SET_OC2A: \
          PORT_TIMER2_SET_OC2A(); \
          break; \
        \
        case PORT_TIMER2_COMA_NORMAL: \
          ; \
      } \
    } \
  } while (0)

#define TIMER2_SET_COMPARE_OUTPUT_MODE_B(mode) \
  do { \
    if (check_resource_owner(SYS_RES_TIMER2)) { \
      \
      PORT_TIMER2_CONTROL_REGISTER_A &= ~( (1<<COM2B0) | (1<<COM2B1) ); \
      \
      switch (mode) { \
        case PORT_TIMER2_COMB_TOGGLE_OC2B: \
          PORT_TIMER2_TOGGLE_OC2B(); \
          break; \
        \
        case PORT_TIMER2_COMB_CLEAR_OC2B: \
          PORT_TIMER2_CLEAR_OC2B(); \
          break; \
        \
        case PORT_TIMER2_COMB_SET_OC2B: \
          PORT_TIMER2_SET_OC2B(); \
          break; \
        \
        case PORT_TIMER2_COMB_NORMAL: \
          ; \
      } \
    } \
  } while (0)

/**
 * Values for cs:
 * 1
 * 8
 * 32
 * 64
 * 128
 * 256
 * 1024
 */
#define TIMER2_SET_CS(cs) \
  do { \
    if (check_resource_owner(SYS_RES_TIMER2)) \
      PORT_TIMER2_SET_CS(cs); \
  } while (0)

#define TIMER2_GET_COUNT() PORT_TIMER2_COUNTER_REGISTER

#define TIMER2_SET_COUNT(value) \
  do { \
    if (check_resource_owner(SYS_RES_TIMER2)) \
      PORT_TIMER2_SET_COUNT(value); \
  } while (0)

#define TIMER2_SET_OCR(channel, value) \
  do { \
    if (check_resource_owner(SYS_RES_TIMER2)) \
      PORT_TIMER2_SET_OCR(channel, value); \
  } while (0)

/**
 * Values for channel:
 * A
 * B
 */
#define TIMER2_FORCE_OUTPUT_COMPARE(channel) \
  do { \
    if (check_resource_owner(SYS_RES_TIMER2)) \
      PORT_TIMER2_FORCE_OUTPUT_COMPARE(channel); \
  } while (0)

/**
 * Values for channel:
 * A
 * B
 */
#define TIMER2_ENABLE_OCR_INTERRUPT(channel) \
  do { \
    if (check_resource_owner(SYS_RES_TIMER2)) \
      PORT_TIMER2_ENABLE_OCR_INTERRUPT(channel); \
  } while (0)

#define TIMER2_ENABLE_OVERFLOW_INTERRUPT() \
  do { \
    if (check_resource_owner(SYS_RES_TIMER2)) \
      PORT_TIMER2_ENABLE_OVERFLOW_INTERRUPT(); \
  } while (0)

#define TIMER2_OCRA_INTERRUPT_FLAG_IS_SET() \
  (PORT_TIMER2_INTERRUPT_FLAG_REGISTER & PORT_TIMER2_0CRA_INTERRUPT_MASK)

#define TIMER2_OCRB_INTERRUPT_FLAG_IS_SET() \
  (PORT_TIMER2_INTERRUPT_FLAG_REGISTER & PORT_TIMER2_0CRB_INTERRUPT_MASK)

#define TIMER2_TOV_INTERRUPT_FLAG_IS_SET() \
  (PORT_TIMER2_INTERRUPT_FLAG_REGISTER & PORT_TIMER2_TOV_INTERRUPT_MASK)

#define TIMER2_ENABLE_EXTERNAL_CLK_INPUT() \
  do { \
    if (check_resource_owner(SYS_RES_TIMER2)) \
      PORT_TIMER2_ENABLE_EXTERNAL_CLK_INPUT(); \
  } while (0)

#define TIMER2_ENABLE_EXTERNAL_CLK_INPUT() \
  do { \
    if (check_resource_owner(SYS_RES_TIMER2)) \
      PORT_TIMER2_ENABLE_EXTERNAL_CLK_INPUT(); \
  } while (0)

#define TIMER2_ASYNC_OPERATION() \
  do { \
    if (check_resource_owner(SYS_RES_TIMER2)) \
      PORT_TIMER2_ASYNC_OPERATION(); \
  } while (0)

#define TIMER2_TCNT_UPDATE_BUSY() \
  PORT_TIMER2_TCNT_UPDATE_BUSY()

#define TIMER2_OCRA_UPDATE_BUSY() \
  PORT_TIMER2_OCRA_UPDATE_BUSY()

#define TIMER2_OCRB_UPDATE_BUSY() \
  PORT_TIMER2_OCRB_UPDATE_BUSY()

#define TIMER2_TCCRA_UPDATE_BUSY() \
  PORT_TIMER2_TCCRA_UPDATE_BUSY()

#define TIMER2_TCCRB_UPDATE_BUSY() \
  PORT_TIMER2_TCCRB_UPDATE_BUSY()

#endif /* TIMER2 */

#endif /* _TIMER2_H_ */
